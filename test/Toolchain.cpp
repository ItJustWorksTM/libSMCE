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
#include <catch2/catch_test_macros.hpp>
#include "SMCE/Toolchain.hpp"
#include "defs.hpp"
#include <iostream>
#include <fstream>

TEST_CASE("Toolchain invalid", "[Toolchain]") {
    SECTION("empty directory") {
        const auto path = SMCE_TEST_DIR "/empty_dir";
        std::filesystem::create_directory(path);
        smce::Toolchain tc{path};
        REQUIRE(tc.resource_dir() == path);
        REQUIRE(tc.check_suitable_environment() == smce::toolchain_error::resdir_empty);
    }
    SECTION("absent directory") {
        const auto path = SMCE_TEST_DIR "/absent_dir";
        smce::Toolchain tc{path};
        REQUIRE(tc.resource_dir() == path);
        REQUIRE(tc.check_suitable_environment() == smce::toolchain_error::resdir_absent);
    }
    SECTION("directory is file") {
        const auto dir = SMCE_TEST_DIR "/dir_is_file";
        std::filesystem::create_directory(dir);
        const auto path = SMCE_TEST_DIR "/dir_is_file/file.txt";
        std::ofstream output(path);
        smce::Toolchain tc{path};
        REQUIRE(tc.resource_dir() == path);
        REQUIRE(tc.check_suitable_environment() == smce::toolchain_error::resdir_file);
    }
    SECTION("invalid sketch path") {
        smce::Toolchain tc{SMCE_PATH};
        REQUIRE(!tc.check_suitable_environment());
        smce::Sketch sk{SKETCHES_PATH "invalid", {.fqbn = "arduino:avr:nano"}};
        REQUIRE(tc.compile(sk) == smce::toolchain_error::sketch_invalid);
    }
    SECTION("invalid plugin") {
        smce::Toolchain tc{SMCE_PATH};
        REQUIRE(!tc.check_suitable_environment());
        // clang-format off
        smce::PluginManifest invalid_pl{
            ".",
            "0.0",
            {},
            {},
            "..",
            "file://" PATCHES_PATH "invalid",
            smce::PluginManifest::Defaults::arduino,
            {},
            {},
            {},
            {}
        };

        smce::SketchConfig skc{
            "arduino:avr:nano",
            {},
            { smce::SketchConfig::ArduinoLibrary{"."} },
            { std::move(invalid_pl) }
        };
        // clang-format on
        smce::Sketch sk{SKETCHES_PATH "patch", std::move(skc)};
        REQUIRE(tc.compile(sk) == smce::toolchain_error::invalid_plugin_name);
    }
}

TEST_CASE("Toolchain valid", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    REQUIRE(tc.resource_dir() == SMCE_PATH);
    REQUIRE_FALSE(tc.cmake_path().empty());
}
