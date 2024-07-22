/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "test_counter.h"

namespace ocs {
namespace diagnostic {

TestCounter::TestCounter(const char* id)
    : BasicCounter(id) {
}

ICounter::Value TestCounter::get() {
    return value;
}

} // namespace diagnostic
} // namespace ocs
