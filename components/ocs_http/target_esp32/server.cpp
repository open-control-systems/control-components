/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "ocs_algo/uri_ops.h"
#include "ocs_core/log.h"
#include "ocs_http/target_esp32/request.h"
#include "ocs_http/target_esp32/response_writer.h"
#include "ocs_http/target_esp32/server.h"
#include "ocs_status/code_to_str.h"

namespace ocs {
namespace http {

namespace {

const char* log_tag = "http_server";

httpd_err_code_t status_code_to_http_code(status::StatusCode code) {
    switch (code) {
    case status::StatusCode::InvalidArg:
        return HTTPD_400_BAD_REQUEST;

    default:
        break;
    }

    return HTTPD_500_INTERNAL_SERVER_ERROR;
}

} // namespace

Server::Server(const Params& params) {
    config_ = HTTPD_DEFAULT_CONFIG();
    config_.server_port = params.server_port;
    config_.max_uri_handlers = params.max_uri_handlers;
    config_.uri_match_fn = httpd_uri_match_wildcard;
}

Server::~Server() {
    if (handle_) {
        stop();
    }
}

void Server::add_GET(const char* path, Server::HandlerFunc func) {
    endpoints_get_.push_back(std::make_pair(path, func));
}

status::StatusCode Server::start() {
    const auto err = httpd_start(&handle_, &config_);
    if (err != ESP_OK) {
        ocs_loge(log_tag, "httpd_start(): %s", esp_err_to_name(err));
        return status::StatusCode::Error;
    }

    return register_uris_();
}

status::StatusCode Server::register_uris_() {
    for (const auto& [path, _] : endpoints_get_) {
        httpd_uri_t uri;
        memset(&uri, 0, sizeof(uri));

        uri.method = HTTP_GET;
        uri.handler = handle_request_;
        uri.user_ctx = this;
        uri.uri = path.c_str();

        ESP_ERROR_CHECK(httpd_register_uri_handler(handle_, &uri));
    }

    return status::StatusCode::OK;
}

status::StatusCode Server::stop() {
    if (handle_) {
        ESP_ERROR_CHECK(httpd_stop(handle_));
        handle_ = nullptr;
    }

    return status::StatusCode::OK;
}

esp_err_t Server::handle_request_(httpd_req_t* req) {
    Server& self = *static_cast<Server*>(req->user_ctx);
    self.handle_request_get_(req);

    return ESP_OK;
}

void Server::handle_request_get_(httpd_req_t* req) {
    const auto path = algo::UriOps::parse_path(req->uri);

    const auto endpoint =
        std::find_if(endpoints_get_.begin(), endpoints_get_.end(),
                     [&path, this](const auto& endpoint) {
                         if (config_.uri_match_fn) {
                             return config_.uri_match_fn(endpoint.first.c_str(),
                                                         path.data(), path.size());
                         }

                         return endpoint.first == path;
                     });
    if (endpoint == endpoints_get_.end()) {
        ocs_loge(log_tag, "unknown URI: %s", req->uri);

        const auto ret = httpd_resp_send_err(
            req, status_code_to_http_code(status::StatusCode::InvalidArg),
            status::code_to_str(status::StatusCode::InvalidArg));
        if (ret != ESP_OK) {
            ocs_loge(log_tag, "httpd_resp_send_err(): URI=%s err=%s", req->uri,
                     esp_err_to_name(ret));
        }

        return;
    }

    Request r(*req);
    ResponseWriter w(*req);

    const auto code = endpoint->second(w, r);
    if (code != status::StatusCode::OK) {
        ocs_loge(log_tag, "failed to handle request: URI=%s code=%s", req->uri,
                 status::code_to_str(code));

        const auto http_code = status_code_to_http_code(code);
        const auto ret = httpd_resp_send_err(req, http_code, status::code_to_str(code));
        if (ret != ESP_OK) {
            ocs_loge(log_tag, "httpd_resp_send_err(): URI=%s err=%s", req->uri,
                     esp_err_to_name(ret));
        }
    }
}

} // namespace http
} // namespace ocs
