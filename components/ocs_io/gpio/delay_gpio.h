/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "freertos/FreeRTOS.h"

#include "ocs_core/noncopyable.h"
#include "ocs_io/gpio/igpio.h"

namespace ocs {
namespace io {
namespace gpio {

class DelayGpio : public IGpio, private core::NonCopyable<> {
public:
    struct Params {
        //! Interval to wait after the GPIO is flipped.
        TickType_t flip_delay_interval { pdMS_TO_TICKS(0) };

        //! Interval to wait after the GPIO is turned on.
        TickType_t turn_on_delay_interval { pdMS_TO_TICKS(0) };

        //! Interval to wait after the GPIO is turned off.
        TickType_t turn_off_delay_interval { pdMS_TO_TICKS(0) };
    };

    //! Initialize.
    DelayGpio(IGpio& gpio, Params params);

    //! Get the GPIO level.
    int get() override;

    //! Add a delay after the GPIO is flipped.
    status::StatusCode flip() override;

    //! Add a delay after the GPIO is turned on.
    status::StatusCode turn_on() override;

    //! Add a delay after the GPIO is turned off.
    status::StatusCode turn_off() override;

    //! Set the GPIO direction.
    status::StatusCode set_direction(IGpio::Direction direction) override;

private:
    const Params params_;

    IGpio& gpio_;
};

} // namespace gpio
} // namespace io
} // namespace ocs
