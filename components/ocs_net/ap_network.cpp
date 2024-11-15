/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "esp_netif.h"

#include "ocs_core/log.h"
#include "ocs_net/ap_network.h"

namespace ocs {
namespace net {

namespace {

const char* log_tag = "ap_network";

} // namespace

ApNetwork::ApNetwork(const Params& params)
    : params_(params) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    netif_ = make_netif_shared(esp_netif_create_default_wifi_ap());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &handle_event_, this, &instance_any_id_));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));

    strncpy(reinterpret_cast<char*>(wifi_config.ap.ssid), params_.ssid.c_str(),
            sizeof(wifi_config.sta.ssid));

    strncpy(reinterpret_cast<char*>(wifi_config.ap.password), params_.password.c_str(),
            sizeof(wifi_config.sta.password));

    wifi_config.ap.channel = params_.channel;
    wifi_config.ap.max_connection = params_.max_connection;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    /* wifi_config.ap.authmode = WIFI_AUTH_WPA2_WPA3_PSK; */
    /* wifi_config.ap.sae_pwe_h2e = WPA3_SAE_PWE_BOTH; */
    wifi_config.ap.pmf_cfg.required = true;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
}

ApNetwork::~ApNetwork() {
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                          &instance_any_id_));

    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_event_loop_delete_default());

    // Note: Deinitialization is not supported yet
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_netif.html#_CPPv416esp_netif_deinitv
    configASSERT(esp_netif_deinit() == ESP_ERR_NOT_SUPPORTED);
}

status::StatusCode ApNetwork::start() {
    const auto err = esp_wifi_start();
    if (err != ESP_OK) {
        ocs_loge(log_tag, "esp_wifi_start(): %s", esp_err_to_name(err));
        return status::StatusCode::Error;
    }

    return status::StatusCode::OK;
}

status::StatusCode ApNetwork::stop() {
    const auto err = esp_wifi_stop();
    if (err != ESP_OK) {
        ocs_loge(log_tag, "esp_wifi_stop(): %s", esp_err_to_name(err));
        return status::StatusCode::Error;
    }

    return status::StatusCode::OK;
}

status::StatusCode ApNetwork::wait() {
    return status::StatusCode::OK;
}

std::optional<ip_addr_t> ApNetwork::get_ip_addr() const {
    return std::nullopt;
}

void ApNetwork::handle_event_(void* event_arg,
                              esp_event_base_t event_base,
                              int32_t event_id,
                              void* event_data) {
    configASSERT(event_base == WIFI_EVENT);
    configASSERT(event_arg);

    ApNetwork& self = *static_cast<ApNetwork*>(event_arg);

    if (event_base == WIFI_EVENT) {
        self.handle_wifi_event_(event_id, event_data);
    } else {
        ocs_loge(log_tag, "unsupported event: event_id=%ld", event_id);
    }
}

void ApNetwork::handle_wifi_event_(int32_t event_id, void* event_data) {
    switch (event_id) {
    case WIFI_EVENT_AP_STACONNECTED:
        handle_wifi_event_ap_sta_connected_(event_data);
        break;

    case WIFI_EVENT_AP_STADISCONNECTED:
        handle_wifi_event_ap_sta_disconnected_(event_data);
        break;

    default:
        break;
    }
}

void ApNetwork::handle_wifi_event_ap_sta_connected_(void* event_data) {
    wifi_event_ap_staconnected_t event =
        *static_cast<wifi_event_ap_staconnected_t*>(event_data);

    ocs_logi(log_tag, "station mac=" MACSTR " join, AID=%d", MAC2STR(event.mac),
             event.aid);
}

void ApNetwork::handle_wifi_event_ap_sta_connected_(void* event_data) {
    wifi_event_ap_staconnected_t event =
        *static_cast<wifi_event_ap_staconnected_t*>(event_data);

    ocs_logi(log_tag, "station mac=" MACSTR " leave, AID=%d", MAC2STR(event.mac),
             event.aid);
}

} // namespace net
} // namespace ocs
