/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ocs_diagnostic/basic_counter_holder.h"

namespace ocs {
namespace diagnostic {

void BasicCounterHolder::add(ICounter& counter) {
    counters_.emplace_back(&counter);
}

const BasicCounterHolder::CounterList& BasicCounterHolder::get_counters_() const {
    return counters_;
}

} // namespace diagnostic
} // namespace ocs
