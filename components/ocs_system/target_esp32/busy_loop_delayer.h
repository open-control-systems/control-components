/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ocs_core/noncopyable.h"
#include "ocs_system/target_esp32/basic_delayer.h"

namespace ocs {
namespace system {

class BusyLoopDelayer : public BasicDelayer, public core::NonCopyable<> {
public:
    //! Initialize.
    //!
    //! @params
    //!  - @p resolution - single tick duration equals to 1/resolution.
    explicit BusyLoopDelayer(core::Time resolution);

    //! Highly-accurate delay based on the hardware timers and busy loop.
    //!
    //! @notes
    //!  Busy loop approach provides high precision but is inefficient for power usage
    //!  and CPU utilization.
    status::StatusCode delay(core::Time delay) override;
};

} // namespace system
} // namespace ocs
