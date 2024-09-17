/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "driver/gpio.h"

#include "ocs_onewire/bus.h"
#include "ocs_scheduler/async_func_scheduler.h"
#include "ocs_scheduler/async_task_scheduler.h"
#include "ocs_sensor/ds18b20/sensor.h"
#include "ocs_status/code.h"

namespace ocs {
namespace sensor {
namespace ds18b20 {

class Store : public scheduler::ITask, public core::NonCopyable<> {
public:
    using SensorList = std::vector<Sensor*>;

    using Func =
        std::function<status::StatusCode(onewire::Bus& bus, SensorList& sensors)>;

    //! Initialize.
    //!
    //! @params
    //!  - @p max_event_count - maximum number of asynchronous events that can be
    //!    scheduled per 1-Wire bus. If the value is too small and the run() is called
    //!    rarely, it's possible to miss some events.
    explicit Store(unsigned max_event_count);

    //! Handle asynchronous events on the 1-wire buses.
    status::StatusCode run() override;

    //! Add sensor to the bus.
    //!
    //! @notes
    //!  If no bus exists, it will be created automatically. Single bus can contain
    //!  several sensors.
    scheduler::ITask* add(Sensor& sensor, gpio_num_t gpio, const char* gpio_id);

    //! Schedule an asynchronous event to the bus.
    scheduler::AsyncFuncScheduler::FuturePtr schedule(gpio_num_t gpio, Func func);

private:
    class Node : public scheduler::ITask, public core::NonCopyable<> {
    public:
        //! Initialize.
        Node(gpio_num_t gpio, const char* gpio_id, unsigned max_event_count);

        //! Handle operations on the 1-Wire bus.
        status::StatusCode run() override;

        //! Associate @p sensor with the bus.
        scheduler::ITask* add(Sensor& sensor);

        //! Schedule an asynchronous event to the bus.
        scheduler::AsyncFuncScheduler::FuturePtr schedule(Func func);

    private:
        scheduler::AsyncTaskScheduler task_scheduler_;
        scheduler::AsyncFuncScheduler func_scheduler_;

        std::unique_ptr<io::IGpio> gpio_;
        std::unique_ptr<system::IDelayer> delayer_;
        std::unique_ptr<onewire::Bus> bus_;
        SensorList sensors_;
    };

    using NodePtr = std::shared_ptr<Node>;
    using NodeListItem = std::pair<gpio_num_t, NodePtr>;
    using NodeList = std::vector<NodeListItem>;

    // Keep the interval short, since the store itself actively poll the underlying async
    // task schedulers.
    static const TickType_t wait_interval_ { pdMS_TO_TICKS(5) };

    const unsigned max_event_count_ { 0 };

    NodePtr get_node_(gpio_num_t gpio);
    NodePtr add_node_(gpio_num_t gpio, const char* gpio_id);

    NodeList nodes_;
};

} // namespace ds18b20
} // namespace sensor
} // namespace ocs