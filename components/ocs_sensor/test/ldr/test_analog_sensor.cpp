/*
 * Copyright (c) 2025, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "unity.h"

#include "ocs_sensor/analog_config.h"
#include "ocs_sensor/ldr/analog_sensor.h"
#include "ocs_test/memory_storage.h"

namespace ocs {
namespace sensor {
namespace ldr {

namespace {

struct TestAdc : public io::adc::IAdc, public core::NonCopyable<> {
    status::StatusCode read(int& raw) override {
        raw = value;

        return status::StatusCode::OK;
    }

    status::StatusCode convert(int& voltage, int raw) override {
        voltage = raw;

        return status::StatusCode::OK;
    }

    int value { 0 };
};

} // namespace

TEST_CASE("LDR analog sensor: receive in range", "[ocs_sensor], [ldr_analog_sensor]") {
    const uint16_t def_min = 10;
    const uint16_t def_max = 20;
    const char* id = "test";

    test::MemoryStorage storage;

    AnalogConfig config(storage, def_min, def_max, id);
    TEST_ASSERT_TRUE(config.valid());

    TestAdc adc;
    AnalogSensor sensor(adc, config);

    const int raw = def_min + 1;
    TEST_ASSERT_TRUE(raw < def_max);

    adc.value = raw;

    TEST_ASSERT_EQUAL(status::StatusCode::OK, sensor.run());

    const auto data = sensor.get_data();
    TEST_ASSERT_EQUAL(raw, data.raw);
    TEST_ASSERT_EQUAL(raw, data.voltage);
    TEST_ASSERT_EQUAL(10, data.lightness);
}

TEST_CASE("LDR analog sensor: receive out of range",
          "[ocs_sensor], [ldr_analog_sensor]") {
    const uint16_t def_min = 10;
    const uint16_t def_max = 20;
    const char* id = "test";

    test::MemoryStorage storage;

    AnalogConfig config(storage, def_min, def_max, id);
    TEST_ASSERT_TRUE(config.valid());

    TestAdc adc;
    AnalogSensor sensor(adc, config);

    int raw = def_min - 1;
    adc.value = raw;
    TEST_ASSERT_EQUAL(status::StatusCode::OK, sensor.run());
    TEST_ASSERT_EQUAL(0, sensor.get_data().lightness);

    raw = def_max + 1;
    adc.value = raw;
    TEST_ASSERT_EQUAL(status::StatusCode::OK, sensor.run());
    TEST_ASSERT_EQUAL(100, sensor.get_data().lightness);
}

TEST_CASE("LDR analog sensor: read config invalid", "[ocs_sensor], [ldr_analog_sensor]") {
    const uint16_t def_min = 20;
    const uint16_t def_max = 10;
    TEST_ASSERT_TRUE(def_min > def_max);

    const char* id = "test";

    test::MemoryStorage storage;

    AnalogConfig config(storage, def_min, def_max, id);
    TEST_ASSERT_FALSE(config.valid());

    TestAdc adc;
    AnalogSensor sensor(adc, config);

    const int raw = (def_min + def_max) / 2;
    adc.value = raw;

    TEST_ASSERT_EQUAL(status::StatusCode::InvalidState, sensor.run());

    const auto data = sensor.get_data();
    TEST_ASSERT_EQUAL(0, data.raw);
    TEST_ASSERT_EQUAL(0, data.voltage);
    TEST_ASSERT_EQUAL(0, data.lightness);
}

} // namespace ldr
} // namespace sensor
} // namespace ocs
