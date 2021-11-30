/*
 *  test/Polyfills.cpp
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
#include <fstream>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "SMCE/Board.hpp"
#include "SMCE/BoardConf.hpp"
#include "SMCE/BoardView.hpp"
#include "SMCE/Toolchain.hpp"
#include "defs.hpp"

using namespace std::literals;

#if SMCE_ARDRIVO_MQTT
/*
 * Polyfills - Implementing a feature that the board does not generally support - like SD and WiFi.
 * This test case checks if the wifi and mqtt preproc libraries can be successfully compiled on a sketch.
 */

TEST_CASE("WiFi intended use", "[WiFi]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    // clang-format off
    smce::Sketch sk{SKETCHES_PATH "wifi", {
        .fqbn = "arduino:avr:nano",
        .legacy_preproc_libs = { { "WiFi" }, { "MQTT" } }
    }};
    // clang-format on
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE_FALSE(ec);
}

#endif // SMCE_ARDRIVO_MQTT

/*
 * This test checks whether the SD polyfilling is working.
 * It includes not just compiling the sketch with the SD library, but also tests the correct workflow of the filesystem.
 */

TEST_CASE("SD polyfill", "[SD File]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    // clang-format off
    smce::Sketch sk{SKETCHES_PATH "sd_fs", {
        .fqbn = "arduino:avr:nano",
        .legacy_preproc_libs = { {"SD"} }
    }};
    // clang-format on
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << ec.message() << '\n' << tc.build_log().second << std::endl;
    REQUIRE_FALSE(ec);

    smce::Board br{};
    // clang-format off
    smce::BoardConfig bc{
        /* .pins = */{0},
        /* .gpio_drivers = */{ smce::BoardConfig::GpioDrivers{0, smce::BoardConfig::GpioDrivers::DigitalDriver{true, true}, std::nullopt} },
        {},
        /* .sd_cards = */{ smce::BoardConfig::SecureDigitalStorage{0, STORAGE_PATH } },
        {}
    };
    // clang-format on
    REQUIRE(br.configure(std::move(bc)));

    if (std::filesystem::exists(STORAGE_PATH))
        std::filesystem::remove_all(STORAGE_PATH);
    std::filesystem::create_directory(STORAGE_PATH);
    REQUIRE(br.attach_sketch(sk));
    REQUIRE(br.start());
    auto d0 = br.view().pins[0].digital();
    test_pin_delayable(d0, true, 16384, 1ms); // wait for the pin to be set
    REQUIRE(br.stop());
    std::cerr << br.runtime_log().second << std::endl;

    REQUIRE(std::filesystem::exists(STORAGE_PATH "foo"));
    REQUIRE(std::filesystem::is_directory(STORAGE_PATH "foo"));
    REQUIRE(std::filesystem::exists(STORAGE_PATH "bar"));
    REQUIRE(std::filesystem::is_directory(STORAGE_PATH "bar"));
    REQUIRE(std::filesystem::exists(STORAGE_PATH "bar/baz"));
    REQUIRE(std::filesystem::is_regular_file(STORAGE_PATH "bar/baz"));
    std::ifstream baz{STORAGE_PATH "bar/baz"};
    REQUIRE(baz.is_open());
    std::string s;
    baz >> s;
    REQUIRE(s == "quxx");
}
