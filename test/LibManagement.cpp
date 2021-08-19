/*
 *  test/LibManagement.cpp
 *  Copyright 2020-2021 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <chrono>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "SMCE/Board.hpp"
#include "SMCE/BoardView.hpp"
#include "SMCE/Sketch.hpp"
#include "SMCE/SketchConf.hpp"
#include "SMCE/Toolchain.hpp"
#include "defs.hpp"

using namespace std::literals;

#if SMCE_ARDRIVO_MQTT

TEST_CASE("Board remote preproc lib", "[Board]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    // clang-format off
    smce::Sketch sk{SKETCHES_PATH "remote_pp", {
        .fqbn = "arduino:avr:nano",
        .preproc_libs = { smce::SketchConfig::RemoteArduinoLibrary{"MQTT", ""} }
    }};
    // clang-format on
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE_FALSE(ec);
}

#endif

TEST_CASE("Patch lib", "[Board]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    // clang-format off
    smce::SketchConfig skc{
        "arduino:avr:nano",
        {},
        {},
        { smce::SketchConfig::LocalArduinoLibrary{PATCHES_PATH "ESP32_analogRewrite", "ESP32 AnalogWrite"} },
        {},
        {}
    };
    // clang-format on
    smce::Sketch sk{SKETCHES_PATH "patch", std::move(skc)};
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE_FALSE(ec);
    smce::Board br{};
    // clang-format off
    smce::BoardConfig bc{
        /* .pins = */{0},
        /* .gpio_drivers = */{ smce::BoardConfig::GpioDrivers{0, std::nullopt, smce::BoardConfig::GpioDrivers::AnalogDriver{true, true}} },
        {},
        {},
        {}
    };
    // clang-format on
    REQUIRE(br.configure(std::move(bc)));
    REQUIRE(br.attach_sketch(sk));
    REQUIRE(br.start());
    auto bv = br.view();
    auto pin0 = bv.pins[0].analog();
    REQUIRE(pin0.exists());
    std::this_thread::sleep_for(1ms);
    test_pin_delayable(pin0, 42, 16384, 1ms);
    REQUIRE(br.stop());
}
