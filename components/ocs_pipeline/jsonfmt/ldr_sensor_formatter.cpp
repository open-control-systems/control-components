/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ocs_pipeline/jsonfmt/ldr_sensor_formatter.h"
#include "ocs_fmt/json/cjson_object_formatter.h"

namespace ocs {
namespace pipeline {
namespace jsonfmt {

LdrSensorFormatter::LdrSensorFormatter(sensor::ldr::Sensor& sensor)
    : sensor_(sensor) {
}

status::StatusCode LdrSensorFormatter::format(cJSON* json) {
    fmt::json::CjsonObjectFormatter formatter(json);

    const auto data = sensor_.get_data();

    if (!formatter.add_number_cs("sensor_ldr_raw", data.raw)) {
        return status::StatusCode::NoMem;
    }

    if (!formatter.add_number_cs("sensor_ldr_voltage", data.voltage)) {
        return status::StatusCode::NoMem;
    }

    if (!formatter.add_number_cs("sensor_ldr_lightness", data.lightness)) {
        return status::StatusCode::NoMem;
    }

    return status::StatusCode::OK;
}

} // namespace jsonfmt
} // namespace pipeline
} // namespace ocs
