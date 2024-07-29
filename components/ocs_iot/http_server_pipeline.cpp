/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "esp_log.h"

#include "ocs_iot/http_server_pipeline.h"
#include "ocs_net/wifi_network.h"
#include "ocs_status/code_to_str.h"

namespace ocs {
namespace iot {

namespace {

const char* log_tag = "http-server-pipeline";

} // namespace

HTTPServerPipeline::HTTPServerPipeline() {
    wifi_network_.reset(new (std::nothrow) net::WiFiNetwork(net::WiFiNetwork::Params {
        .max_retry_count = CONFIG_OCS_NETWORK_WIFI_STA_RETRY_COUNT,
        .ssid = CONFIG_OCS_NETWORK_WIFI_STA_SSID,
        .password = CONFIG_OCS_NETWORK_WIFI_STA_PASSWORD,
    }));
    wifi_network_->add(*this);

    http_server_.reset(new (std::nothrow) net::HTTPServer(net::HTTPServer::Params {
        .server_port = CONFIG_OCS_NETWORK_HTTP_SERVER_PORT,
    }));

    mdns_provider_.reset(new (std::nothrow) net::MDNSProvider(net::MDNSProvider::Params {
        .hostname = CONFIG_OCS_NETWORK_MDNS_HOSTNAME,
        .instance_name = CONFIG_OCS_NETWORK_MDNS_INSTANCE_NAME,
    }));
}

void HTTPServerPipeline::handle_connected() {
    http_server_->start();
}

void HTTPServerPipeline::handle_disconnected() {
    http_server_->stop();
}

status::StatusCode HTTPServerPipeline::start() {
    auto code = try_start_wifi_();
    if (code != status::StatusCode::OK) {
        stop_wifi_();
        return code;
    }

    code = try_start_mdns_();
    if (code != status::StatusCode::OK) {
        stop_mdns_();
        return code;
    }

    return status::StatusCode::OK;
}

net::BasicNetwork& HTTPServerPipeline::network() {
    return *wifi_network_;
}

net::HTTPServer& HTTPServerPipeline::server() {
    return *http_server_;
}

net::MDNSProvider& HTTPServerPipeline::mdns() {
    return *mdns_provider_;
}

status::StatusCode HTTPServerPipeline::try_start_wifi_() {
    auto code = wifi_network_->start();
    if (code != status::StatusCode::OK) {
        ESP_LOGE(log_tag, "failed to start the WiFi connection process: code=%s",
                 status::code_to_str(code));

        return code;
    }

    code = wifi_network_->wait();
    if (code != status::StatusCode::OK) {
        return code;
    }

    return status::StatusCode::OK;
}

status::StatusCode HTTPServerPipeline::try_start_mdns_() {
    auto code = mdns_provider_->start();
    if (code != status::StatusCode::OK) {
        ESP_LOGE(log_tag, "failed to start the mDNS service: code=%s",
                 status::code_to_str(code));

        return code;
    }

    code =
        mdns_provider_->add_service("_http", "_tcp", CONFIG_OCS_NETWORK_HTTP_SERVER_PORT);
    if (code != status::StatusCode::OK) {
        return code;
    }

    return status::StatusCode::OK;
}

void HTTPServerPipeline::stop_wifi_() {
    const auto code = wifi_network_->stop();
    if (code != status::StatusCode::OK) {
        ESP_LOGE(log_tag, "failed to stop the WiFi connection process: code=%s",
                 status::code_to_str(code));
    }

    wifi_network_ = nullptr;
}

void HTTPServerPipeline::stop_mdns_() {
    const auto code = mdns_provider_->stop();
    if (code != status::StatusCode::OK) {
        ESP_LOGE(log_tag, "failed to stop the mDNS service: code=%s",
                 status::code_to_str(code));
    }
}

} // namespace iot
} // namespace ocs