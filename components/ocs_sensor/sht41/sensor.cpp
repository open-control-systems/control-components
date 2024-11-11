/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <cstring>

#include "freertos/FreeRTOSConfig.h"

#include "ocs_algo/bit_ops.h"
#include "ocs_algo/crc_ops.h"
#include "ocs_algo/math_ops.h"
#include "ocs_core/log.h"
#include "ocs_sensor/sht41/sensor.h"
#include "ocs_status/code_to_str.h"
#include "ocs_status/macros.h"

namespace ocs {
namespace sensor {
namespace sht41 {

namespace {

uint8_t calculate_crc(uint8_t hi, uint8_t lo) {
    const uint8_t buf[2] { hi, lo };
    return algo::CrcOps::crc8(buf, sizeof(buf), 0xFF, 0x31, algo::CrcOps::BitOrder::MSB);
}

const char* log_tag = "sht41_sensor";

} // namespace

Sensor::Sensor(io::i2c::ITransceiver& transceiver, Sensor::Params params)
    : params_(params)
    , transceiver_(transceiver) {
    configASSERT(params_.send_wait_interval);
    configASSERT(params_.bus_wait_interval);

    if (const auto code = reset_(); code != status::StatusCode::OK) {
        ocs_loge(log_tag, "failed to reset sensor: code=%s", status::code_to_str(code));
    }
}

status::StatusCode Sensor::run() {
    OCS_STATUS_RETURN_ON_ERROR(send_command_(params_.measure_command));

    uint8_t buf[6];
    memset(buf, 0, sizeof(buf));
    OCS_STATUS_RETURN_ON_ERROR(
        transceiver_.receive(buf, sizeof(buf), params_.bus_wait_interval));

    Data data;

    const uint16_t temperature_ticks = algo::BitOps::pack_u8(buf[0], buf[1]);
    const uint8_t temperature_checksum = buf[2];
    if (temperature_checksum == calculate_crc(buf[0], buf[1])) {
        data.temperature = -45 + (175.0 * temperature_ticks / UINT16_MAX);
        data.temperature = algo::MathOps::round_floor(data.temperature, 2);
    }

    const uint16_t humidity_ticks = algo::BitOps::pack_u8(buf[3], buf[4]);
    const uint8_t humidity_checksum = buf[5];

    if (humidity_checksum == calculate_crc(buf[3], buf[4])) {
        data.humidity = -6 + (125.0 * humidity_ticks / UINT16_MAX);
        data.humidity = algo::MathOps::round_floor(data.humidity, 2);
        data.humidity = std::clamp(data.humidity, 0.0, 100.0);
    }

    data_.set(data);

    return status::StatusCode::OK;
}

Sensor::Data Sensor::get_data() const {
    return data_.get();
}

status::StatusCode Sensor::reset_() {
    OCS_STATUS_RETURN_ON_ERROR(send_command_(Command::SoftReset));

    return status::StatusCode::OK;
}
status::StatusCode Sensor::send_command_(Sensor::Command command) {
    OCS_STATUS_RETURN_ON_ERROR(
        transceiver_.send(reinterpret_cast<const uint8_t*>(&command), sizeof(command),
                          params_.bus_wait_interval));

    vTaskDelay(params_.send_wait_interval);

    return status::StatusCode::OK;
}

} // namespace sht41
} // namespace sensor
} // namespace ocs
