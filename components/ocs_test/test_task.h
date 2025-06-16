/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ocs_core/cond.h"
#include "ocs_core/noncopyable.h"
#include "ocs_core/static_mutex.h"
#include "ocs_scheduler/itask.h"

namespace ocs {
namespace test {

class TestTask : public scheduler::ITask, private core::NonCopyable<> {
public:
    explicit TestTask(status::StatusCode code);

    status::StatusCode run() override;

    bool was_run_called() const;
    unsigned run_call_count() const;

    //! Reset to initial state.
    void reset(status::StatusCode code);

    //! Wait for the task to be called.
    status::StatusCode wait(TickType_t wait = portMAX_DELAY);

private:
    mutable core::StaticMutex mu_;
    core::Cond cond_;

    status::StatusCode code_ { status::StatusCode::OK };
    bool run_called_ { false };
    unsigned run_call_count_ { 0 };
};

} // namespace test
} // namespace ocs
