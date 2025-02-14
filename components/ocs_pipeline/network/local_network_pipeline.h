/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>

#include "ocs_core/noncopyable.h"
#include "ocs_net/ap_network.h"
#include "ocs_net/inetwork_handler.h"
#include "ocs_storage/storage_builder.h"
#include "ocs_system/device_info.h"

namespace ocs {
namespace pipeline {
namespace network {

//! Local WiFi AP.
class LocalNetworkPipeline : public core::NonCopyable<> {
public:
    //! Initialize.
    //!
    //! @params
    //!  - @p storage_builder to create storages for network configuration.
    //!  - @p handler to notify about network connection status.
    //!  - @p device_info to create a unique access point SSID.
    LocalNetworkPipeline(storage::StorageBuilder& storage_builder,
                         net::INetworkHandler& handler,
                         const system::DeviceInfo& device_info);

    net::IApNetwork& get_network();

    //! Start the WiFi network.
    status::StatusCode start();

private:
    void initialize_network_(net::INetworkHandler& handler,
                             const system::DeviceInfo& device_info);

    status::StatusCode start_();

    static const unsigned max_ssid_size_ = 31;
    static const unsigned max_password_size_ = 63;

    static const TickType_t wait_start_interval_ = pdMS_TO_TICKS(1000 * 60 * 10);

    storage::StorageBuilder::IStoragePtr storage_;

    std::unique_ptr<net::ApNetwork> network_;
};

} // namespace network
} // namespace pipeline
} // namespace ocs
