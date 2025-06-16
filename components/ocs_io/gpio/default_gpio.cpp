/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ocs_io/gpio/default_gpio.h"

namespace ocs {
namespace io {
namespace gpio {

DefaultGpio::DefaultGpio(const char* id, Gpio gpio)
    : BasicGpio(id, gpio, true) {
}

} // namespace gpio
} // namespace io
} // namespace ocs
