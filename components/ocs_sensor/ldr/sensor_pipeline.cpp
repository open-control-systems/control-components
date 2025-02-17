/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "freertos/FreeRTOSConfig.h"

#include "ocs_sensor/ldr/sensor_pipeline.h"

namespace ocs {
namespace sensor {
namespace ldr {

SensorPipeline::SensorPipeline(io::adc::IStore& adc_store,
                               scheduler::ITaskScheduler& task_scheduler,
                               const char* id,
                               SensorPipeline::Params params)
    : task_id_(std::string(id) + "_task") {
    adc_ = adc_store.add(params.adc_channel);
    configASSERT(adc_);

    sensor_.reset(new (std::nothrow) Sensor(*adc_, params.sensor));
    configASSERT(sensor_);

    configASSERT(task_scheduler.add(*sensor_, task_id_.c_str(), params.read_interval)
                 == status::StatusCode::OK);
}

Sensor& SensorPipeline::get_sensor() {
    return *sensor_;
}

} // namespace ldr
} // namespace sensor
} // namespace ocs
