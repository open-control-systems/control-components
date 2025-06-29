/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ocs_core/iclock.h"
#include "ocs_core/noncopyable.h"
#include "ocs_core/time.h"
#include "ocs_diagnostic/basic_counter.h"

namespace ocs {
namespace diagnostic {

class TimeCounter : public BasicCounter, private core::NonCopyable<> {
public:
    //! Initialize.
    //!
    //! @params
    //!  - @p clock to read a time since boot.
    //!  - @p id - counter identifier.
    //!  - @p resolution - time resolution used to count the value.
    TimeCounter(core::IClock& clock, const char* id, core::Time resolution);

    //! Return time since boot with the configured resolution.
    ICounter::Value get() const override;

    //! Restart time counting from now.
    void reset();

private:
    const core::Time resolution_ = core::Duration::microsecond;

    core::IClock& clock_;

    core::Time offset_ { 0 };
};

} // namespace diagnostic
} // namespace ocs
