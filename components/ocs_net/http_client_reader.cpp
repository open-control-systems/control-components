/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "esp_log.h"

#include "ocs_core/lock_guard.h"
#include "ocs_net/http_client_reader.h"
#include "ocs_status/macros.h"

namespace ocs {
namespace net {

namespace {

const char* log_tag = "http-reader";

} // namespace

HttpClientReader::HttpClientReader(const Params& params)
    : params_(params)
    , cond_(mu_) {
    memset(&config_, 0, sizeof(config_));
    config_.host = params_.host.c_str();
    config_.path = params_.path.c_str();
    config_.transport_type = HTTP_TRANSPORT_OVER_TCP;
    config_.event_handler = handle_event_;
    config_.user_data = this;

    client_ = make_http_client_shared(config_);

    buf_.reset(new (std::nothrow) char[params_.bufsize]);
    configASSERT(buf_);
}

esp_http_client_handle_t HttpClientReader::client() const {
    return client_.get();
}

status::StatusCode HttpClientReader::wait(TickType_t wait) {
    core::LockGuard lock(mu_);

    while (!data_received_) {
        OCS_STATUS_RETURN_ON_ERROR(cond_.wait(wait));
    }

    return status::StatusCode::OK;
}

unsigned HttpClientReader::read(char* buf, unsigned size) {
    core::LockGuard lock(mu_);

    const unsigned len = std::min(size, strlen(buf_.get()));
    memcpy(buf, buf_.get(), len);

    return len;
}

esp_err_t HttpClientReader::handle_event_(esp_http_client_event_t* event) {
    configASSERT(event->user_data);
    HttpClientReader& self = *static_cast<HttpClientReader*>(event->user_data);

    switch (event->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(log_tag, "HTTP_EVENT_ERROR");
        break;

    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(log_tag, "HTTP_EVENT_ON_CONNECTED");
        break;

    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(log_tag, "HTTP_EVENT_HEADER_SENT");
        break;

    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(log_tag, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", event->header_key,
                 event->header_value);
        break;

    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(log_tag, "HTTP_EVENT_ON_DATA, len=%d", event->data_len);
        self.handle_event_on_data_(event);
        break;

    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(log_tag, "HTTP_EVENT_ON_FINISH");
        self.handle_event_on_finish_();
        break;

    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(log_tag, "HTTP_EVENT_DISCONNECTED");
        break;

    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(log_tag, "HTTP_EVENT_REDIRECT");
        break;
    }

    return ESP_OK;
}

void HttpClientReader::handle_event_on_data_(esp_http_client_event_t* event) {
    core::LockGuard lock(mu_);

    memcpy(buf_.get(), event->data,
           std::min(event->data_len, static_cast<int>(params_.bufsize)));
}

void HttpClientReader::handle_event_on_finish_() {
    core::LockGuard lock(mu_);

    data_received_ = true;

    cond_.broadcast();
}

} // namespace net
} // namespace ocs
