/*
 * Copyright (c) 2025, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <charconv>
#include <cstring>
#include <string>

#include "ocs_core/log.h"
#include "ocs_fmt/json/cjson_builder.h"
#include "ocs_fmt/json/cjson_object_formatter.h"
#include "ocs_fmt/json/dynamic_formatter.h"
#include "ocs_pipeline/httpserver/analog_config_store_handler.h"
#include "ocs_status/code_to_str.h"

namespace ocs {
namespace pipeline {
namespace httpserver {

namespace {

status::StatusCode send_ok(httpd_req_t* req) {
    auto err = httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    if (err != ESP_OK) {
        return status::StatusCode::Error;
    }

    err = httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    if (err != ESP_OK) {
        return status::StatusCode::Error;
    }

    return status::StatusCode::OK;
}

status::StatusCode send_json(httpd_req_t* req, cJSON* json, unsigned size) {
    fmt::json::DynamicFormatter json_formatter(size);

    const auto code = json_formatter.format(json);
    if (code != status::StatusCode::OK) {
        return code;
    }

    auto err = httpd_resp_set_type(req, HTTPD_TYPE_JSON);
    if (err != ESP_OK) {
        return status::StatusCode::Error;
    }

    err = httpd_resp_send(req, json_formatter.c_str(), HTTPD_RESP_USE_STRLEN);
    if (err != ESP_OK) {
        return status::StatusCode::Error;
    }

    return status::StatusCode::OK;
}

bool format_config(fmt::json::CjsonObjectFormatter& formatter,
                   const sensor::AnalogConfig& config) {
    if (!formatter.add_number_cs("min", config.get_min())) {
        return false;
    }
    if (!formatter.add_number_cs("max", config.get_max())) {
        return false;
    }
    if (!formatter.add_number_cs("oversampling", config.get_sample_count())) {
        return false;
    }
    if (!formatter.add_number_cs("bitwidth", config.get_bitwidth())) {
        return false;
    }

    return true;
}

const char* log_tag = "analog_config_store_handler";

} // namespace

AnalogConfigStoreHandler::AnalogConfigStoreHandler(
    scheduler::AsyncFuncScheduler& func_scheduler,
    http::Server& server,
    sensor::AnalogConfigStore& store)
    : store_(store) {
    server.add_GET("/api/v1/config/sensor/analog",
                   [this, &func_scheduler](httpd_req_t* req) {
                       auto future = func_scheduler.add([this, req]() {
                           const auto values = algo::UriOps::parse_query(req->uri);
                           if (!values.size()) {
                               return handle_all_(req);
                           }

                           return handle_single_(req, values);
                       });
                       if (!future) {
                           return status::StatusCode::InvalidState;
                       }
                       if (future->wait(wait_timeout_) != status::StatusCode::OK) {
                           return status::StatusCode::Timeout;
                       }
                       if (future->code() != status::StatusCode::OK) {
                           return future->code();
                       }

                       return status::StatusCode::OK;
                   });
}

status::StatusCode AnalogConfigStoreHandler::handle_all_(httpd_req_t* req) {
    fmt::json::CjsonUniqueBuilder builder;

    auto array = builder.make_array();
    if (!array) {
        return status::StatusCode::NoMem;
    }

    for (auto& config : store_.get_all()) {
        auto json = builder.make_object();
        if (!json) {
            return status::StatusCode::NoMem;
        }

        fmt::json::CjsonObjectFormatter object_formatter(json.get());

        if (!object_formatter.add_string_ref_cs("id", config->get_id())) {
            return status::StatusCode::NoMem;
        }
        if (!format_config(object_formatter, *config)) {
            return status::StatusCode::NoMem;
        }

        if (!cJSON_AddItemToArray(array.get(), json.get())) {
            return status::StatusCode::NoMem;
        }
        json.release();
    }

    return send_json(req, array.get(), 256);
}

status::StatusCode
AnalogConfigStoreHandler::handle_single_(httpd_req_t* req,
                                         const algo::UriOps::Values& values) {
    const auto id = values.find("id");
    if (id == values.end()) {
        ocs_logw(log_tag, "id field is required");

        return status::StatusCode::InvalidArg;
    }

    const std::string id_buf(id->second);
    auto config = store_.get(id_buf.c_str());
    if (!config) {
        ocs_logw(log_tag, "config doesn't exist for id=%s", id_buf.c_str());

        return status::StatusCode::InvalidArg;
    }

    const auto reset = values.find("reset");
    if (reset != values.end()) {
        if (reset->second != "1") {
            ocs_logw(log_tag, "reset parameter contains invalid value");

            return status::StatusCode::InvalidArg;
        }

        const auto code = config->reset();
        if (code != status::StatusCode::OK && code != status::StatusCode::NotModified) {
            ocs_loge(log_tag, "failed to reset config: %s", status::code_to_str(code));

            return code;
        }

        return send_ok(req);
    }

    const auto min = values.find("min");
    const auto max = values.find("max");
    const auto oversampling = values.find("oversampling");

    if (min == values.end() && max == values.end() && oversampling == values.end()) {
        return handle_single_get_(req, *config);
    }

    uint16_t parsed_min = config->get_min();
    if (min != values.end()) {
        const auto result = std::from_chars(
            min->second.data(), min->second.data() + min->second.size(), parsed_min);
        if (result.ec != std::errc()) {
            ocs_loge(log_tag, "failed to parse min parameter");

            return status::StatusCode::InvalidArg;
        }
    }

    uint16_t parsed_max = config->get_max();
    if (max != values.end()) {
        const auto result = std::from_chars(
            max->second.data(), max->second.data() + max->second.size(), parsed_max);
        if (result.ec != std::errc()) {
            ocs_loge(log_tag, "failed to parse max parameter");

            return status::StatusCode::InvalidArg;
        }
    }

    uint8_t parsed_sample_count = config->get_sample_count();
    if (oversampling != values.end()) {
        const auto result =
            std::from_chars(oversampling->second.data(),
                            oversampling->second.data() + oversampling->second.size(),
                            parsed_sample_count);
        if (result.ec != std::errc()) {
            ocs_loge(log_tag, "failed to parse oversampling parameter");

            return status::StatusCode::InvalidArg;
        }
    }

    const auto code = config->configure(parsed_min, parsed_max, parsed_sample_count);
    if (code != status::StatusCode::OK && code != status::StatusCode::NotModified) {
        ocs_loge(log_tag, "failed to update config: %s", status::code_to_str(code));

        return code;
    }

    return send_ok(req);
}

status::StatusCode
AnalogConfigStoreHandler::handle_single_get_(httpd_req_t* req,
                                             const sensor::AnalogConfig& config) {
    if (!config.valid()) {
        return status::StatusCode::InvalidState;
    }

    fmt::json::CjsonUniqueBuilder builder;

    auto json = builder.make_object();
    if (!json) {
        return status::StatusCode::NoMem;
    }

    fmt::json::CjsonObjectFormatter object_formatter(json.get());
    if (!format_config(object_formatter, config)) {
        return status::StatusCode::NoMem;
    }

    return send_json(req, json.get(), 64);
}

} // namespace httpserver
} // namespace pipeline
} // namespace ocs
