/*
 *  test/Toolchain.cpp
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
#include <filesystem>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "SMCE/Toolchain.hpp"
#include "defs.hpp"

TEST_CASE("Toolchain invalid", "[Toolchain]") {
    const auto path = SMCE_TEST_DIR "/empty_dir";
    std::filesystem::create_directory(path);
    smce::Toolchain tc{path};
    REQUIRE(tc.check_suitable_environment());
    REQUIRE(tc.resource_dir() == path);
}

TEST_CASE("Toolchain valid", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    REQUIRE(tc.resource_dir() == SMCE_PATH);
    REQUIRE_FALSE(tc.cmake_path().empty());
}

TEST_CASE("Sketch compilation valid", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    REQUIRE(tc.resource_dir() == SMCE_PATH);
    REQUIRE_FALSE(tc.cmake_path().empty());

    smce::Sketch sk{SKETCHES_PATH "uart", {.fqbn = "test_board", .legacy_preproc_libs = {{"WiFi"}}}};
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE(!ec);
}

TEST_CASE("Sketch compilation with different toolchain errors", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    REQUIRE(tc.resource_dir() == SMCE_PATH);
    REQUIRE_FALSE(tc.cmake_path().empty());

    SECTION("Invalid sketch path") {
        smce::Sketch sk{SKETCHES_PATH "non_existing", {.fqbn = "test_board", .legacy_preproc_libs = {{"WiFi"}}}};
        const auto ec = tc.compile(sk);
        if (ec)
            std::cerr << tc.build_log().second;
        REQUIRE(static_cast<smce::toolchain_error>(ec.value()) == smce::toolchain_error::sketch_invalid);
    }

    SECTION("Sketch containing invalid syntax") {
        smce::Sketch sk{SKETCHES_PATH "uart_invalid", {.fqbn = "test_board", .legacy_preproc_libs = {{"WiFi"}}}};
        const auto ec = tc.compile(sk);
        if (ec)
            std::cerr << tc.build_log().second;
        REQUIRE(static_cast<smce::toolchain_error>(ec.value()) == smce::toolchain_error::build_failed);
    }

    SECTION("Invalid plugin name") {
        smce::Sketch sk{SKETCHES_PATH "uart",
                        {.fqbn = "test_board", .legacy_preproc_libs = {{"WiFi"}}, .plugins = {{"/invalid"}}}};
        const auto ec = tc.compile(sk);
        if (ec)
            std::cerr << tc.build_log().second;
        REQUIRE(static_cast<smce::toolchain_error>(ec.value()) == smce::toolchain_error::invalid_plugin_name);
    }
}