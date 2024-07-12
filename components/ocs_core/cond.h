/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <deque>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ocs_core/noncopyable.h"
#include "ocs_core/static_mutex.h"

namespace ocs {
namespace core {

//! FreeRTOS condition variable.
class Cond : public NonCopyable<> {
public:
    //! Initialize.
    explicit Cond(StaticMutex& mutex);

    //! Atomically unlocks mutex and suspends execution of the calling task.
    //! After later resuming execution, wait() locks mutex before returning.
    //!
    //! @remarks
    //!  Because mutex is not locked when wait() first resumes, the caller
    //!  typically cannot assume that the condition is true when
    //!  wait() returns. Instead, the caller should wait() in a loop.
    //!
    //! @example
    //!    Mutex mutex;
    //!    Cond cond(mutex);
    //!
    //!    Mutex::Lock lock(mutex);
    //!
    //!    while (!condition()) {
    //!        cond.wait()
    //!    }
    [[nodiscard]] bool wait(TickType_t wait = portMAX_DELAY);

    //! Signal wakes a task waiting on a condition variable, if there is one.
    //!
    //! @remarks
    //!  Mutex should be held.
    void signal();

    //! Signal wakes any tasks waiting on the condition variable, if there are any.
    //!
    //! @remarks
    //!  Mutex should be held.
    void broadcast();

private:
    StaticMutex& mu_;
    std::deque<TaskHandle_t> tasks_;
};

} // namespace core
} // namespace ocs