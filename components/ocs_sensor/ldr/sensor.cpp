/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "freertos/FreeRTOSConfig.h"

#include "ocs_sensor/ldr/sensor.h"

namespace ocs {
namespace sensor {
namespace ldr {

Sensor::Sensor(io::AdcStore& adc_store, const char* id, Sensor::Params params)
    : BasicSensor(id)
    , params_(params) {
    configASSERT(params_.value_min < params_.value_max);

    adc_ = adc_store.add(params_.adc_channel);
    configASSERT(adc_);
}

status::StatusCode Sensor::run() {
    const auto read_result = adc_->read();
    if (read_result.code != status::StatusCode::OK) {
        return read_result.code;
    }

    const auto conv_result = adc_->convert(read_result.value);
    if (conv_result.code != status::StatusCode::OK) {
        return conv_result.code;
    }

    update_data_(read_result.value, conv_result.value);

    return status::StatusCode::OK;
}

int Sensor::calculate_lightness_(int raw) const {
    if (raw >= params_.value_max) {
        return 100;
    }

    if (raw <= params_.value_min) {
        return 0;
    }

    const int range = params_.value_max - params_.value_min;
    const int offset = raw - params_.value_min;
    const float lightness = static_cast<float>(offset) / range;

    return 100 * lightness;
}

void Sensor::update_data_(int raw, int voltage) {
    SensorData data;

    data.raw = raw;
    data.voltage = voltage;
    data.lightness = calculate_lightness_(raw);

    set_data_(data);
}

} // namespace ldr
} // namespace sensor
} // namespace ocs