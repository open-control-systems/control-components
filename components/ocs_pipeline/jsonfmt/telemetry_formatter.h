/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>

#include "ocs_core/noncopyable.h"
#include "ocs_fmt/json/fanout_formatter.h"
#include "ocs_fmt/json/iformatter.h"

namespace ocs {
namespace pipeline {
namespace jsonfmt {

class TelemetryFormatter : public fmt::json::IFormatter, private core::NonCopyable<> {
public:
    //! Initialize.
    TelemetryFormatter();

    //! Format all telemetry data into @p json.
    status::StatusCode format(cJSON* json);

    fmt::json::FanoutFormatter& get_fanout_formatter();

private:
    std::unique_ptr<fmt::json::FanoutFormatter> fanout_formatter_;
    std::unique_ptr<fmt::json::IFormatter> system_formatter_;
};

} // namespace jsonfmt
} // namespace pipeline
} // namespace ocs
