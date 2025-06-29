/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>

#include "ocs_system/irandomizer.h"
#include "ocs_system/ireboot_handler.h"
#include "ocs_system/irebooter.h"
#include "ocs_system/irt_delayer.h"

namespace ocs {
namespace system {

struct PlatformBuilder {
    using IRtDelayerPtr = std::unique_ptr<IRtDelayer>;
    using IRebooterPtr = std::unique_ptr<IRebooter>;
    using IRandomizerPtr = std::unique_ptr<IRandomizer>;

    //! Create real-time delayer for highly-accurate delays.
    static IRtDelayerPtr make_rt_delayer();

    //! Create a basic system rebooter.
    static IRebooterPtr make_rebooter(IRebootHandler& handler);

    //! Create a basic system randomizer.
    static IRandomizerPtr make_randomizer();
};

} // namespace system
} // namespace ocs
