/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ocs_pipeline/httpserver/sht41_handler.h"

namespace ocs {
namespace pipeline {
namespace httpserver {

SHT41Handler::SHT41Handler(scheduler::AsyncFuncScheduler& func_scheduler,
                           http::Server& http_server,
                           net::MdnsProvider& mdns_provider,
                           sensor::sht41::Sensor& sensor)
    : func_scheduler_(func_scheduler)
    , sensor_(sensor) {
    http_server.add_GET("/sensor/sht41/reset", [this](httpd_req_t* req) {
        return handle_operation_(req, [](sensor::sht41::Sensor& sensor) {
            return sensor.reset();
        });
    });
    http_server.add_GET("/sensor/sht41/heat", [this](httpd_req_t* req) {
        return handle_operation_(req, [](sensor::sht41::Sensor& sensor) {
            return sensor.heat();
        });
    });

    mdns_provider.add_txt_records(net::MdnsProvider::Service::Http,
                                  net::MdnsProvider::Proto::Tcp,
                                  net::MdnsProvider::TxtRecordList {
                                      {
                                          "sensor_sht41_reset",
                                          "/sensor/sht41/reset",
                                      },
                                      {
                                          "sensor_sht41_heat",
                                          "/sensor/sht41/heat",
                                      },
                                  });
}

status::StatusCode
SHT41Handler::handle_operation_(httpd_req_t* req,
                                SHT41Handler::HandleOperationFunc func) {
    auto future = func_scheduler_.add([this, func]() {
        return func(sensor_);
    });
    if (!future) {
        return status::StatusCode::InvalidState;
    }
    if (future->wait(wait_op_interval_) != status::StatusCode::OK) {
        return status::StatusCode::Timeout;
    }
    if (future->code() != status::StatusCode::OK) {
        return future->code();
    }

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

} // namespace httpserver
} // namespace pipeline
} // namespace ocs
