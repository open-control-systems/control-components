/*
 * Copyright (c) 2024, Tendry Lab authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cstdint>

namespace ocs {
namespace sensor {
namespace soil {

//! Known soil statuses.
//!
//! @reference
//!  - https://connectedcrops.ca/the-ultimate-guide-to-soil-moisture/
enum class SoilStatus : uint8_t {
    //! Default status.
    None,

    //! Completely wet soil, the soil is happy.
    Saturated,

    //! Soil moisture between dry and saturated, may need watering soon.
    Wet,

    //! Soil has started to starve, needs watering.
    Depletion,

    //! Totally dry soil, needs watering.
    Dry,

    //! Sensor is broken, disconnected or reporting invalid values.
    Error,

    //! Invalid status.
    Last,
};

} // namespace soil
} // namespace sensor
} // namespace ocs
