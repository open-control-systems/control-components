/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ocs_core/noncopyable.h"
#include "ocs_http/irouter.h"
#include "ocs_scheduler/itask.h"

namespace ocs {
namespace pipeline {
namespace httpserver {

class SystemHandler : public core::NonCopyable<> {
public:
    //! Initialize.
    //!
    //! @params
    //!  - @p router to register HTTP endpoints.
    //!  - @p reboot_task to initiate the reboot process.
    SystemHandler(http::IRouter& router, scheduler::ITask& reboot_task);
};

} // namespace httpserver
} // namespace pipeline
} // namespace ocs
