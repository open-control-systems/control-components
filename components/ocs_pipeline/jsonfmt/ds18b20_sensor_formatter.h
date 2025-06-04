/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string>

#include "ocs_core/noncopyable.h"
#include "ocs_fmt/json/iformatter.h"
#include "ocs_sensor/ds18b20/sensor.h"

namespace ocs {
namespace pipeline {
namespace jsonfmt {

class DS18B20SensorFormatter : public fmt::json::IFormatter, private core::NonCopyable<> {
public:
    //! Initialize.
    //!
    //! @params
    //!  - @p sensor to read temperature.
    explicit DS18B20SensorFormatter(sensor::ds18b20::Sensor& sensor);

    //! Format DS18B20 sensor data into @p json.
    status::StatusCode format(cJSON* json) override;

private:
    sensor::ds18b20::Sensor& sensor_;
};

} // namespace jsonfmt
} // namespace pipeline
} // namespace ocs
