/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ocs_core/time.h"

namespace ocs {
namespace core {

class IClock {
public:
    //! Destroy.
    virtual ~IClock() = default;

    //! Get time in microseconds since boot.
    virtual Time now() = 0;
};

} // namespace core
} // namespace ocs
