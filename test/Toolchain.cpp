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
    // Empty directory
    const auto path = SMCE_TEST_DIR "/empty_dir";
    std::filesystem::create_directory(path);
    smce::Toolchain tc{path};
    REQUIRE(tc.resource_dir() == path);
    REQUIRE(tc.check_suitable_environment() == smce::toolchain_error::resdir_empty);
    // Absent directory
    const auto path2 = SMCE_TEST_DIR "/absent_dir";
    smce::Toolchain tc2{path2};
    REQUIRE(tc2.resource_dir() == path2);
    REQUIRE(tc2.check_suitable_environment() == smce::toolchain_error::resdir_absent);
    // Directory is file
    const auto dir3 = SMCE_TEST_DIR "/dir_is_file";
    std::filesystem::create_directory(dir3);
    const auto dirfile = SMCE_TEST_DIR "/dir_is_file/file.txt";
    std::ofstream output(dirfile);
    smce::Toolchain tc3{dirfile};
    REQUIRE(tc3.resource_dir() == dirfile);
    REQUIRE(tc3.check_suitable_environment() == smce::toolchain_error::resdir_file);
    // Invalid sketch path
    smce::Toolchain tc4{SMCE_PATH};
    REQUIRE(!tc4.check_suitable_environment());
    smce::Sketch sk{SKETCHES_PATH "invalid", {.fqbn = "arduino:avr:nano"}};
    const auto ec4 = tc4.compile(sk);
    REQUIRE(ec4 == smce::toolchain_error::sketch_invalid);
    // Invalid plugin
    smce::Toolchain tc5{SMCE_PATH};
    REQUIRE(!tc5.check_suitable_environment());
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
    smce::Sketch sk5{SKETCHES_PATH "patch", std::move(skc)};
    const auto ec5 = tc5.compile(sk5);
    REQUIRE(ec5 == smce::toolchain_error::invalid_plugin_name);
}

// TEST_CASE("Toolchain error","[Toolchain]"){
//    const auto path = SMCE_TEST_DIR "";
//    std::string m_cmake_path = "cmake";
//    std::string m_build_log;
//    std::mutex m_build_log_mtx;
//    std::filesystem::create_directory(path);
//    smce::Toolchain tc{path};
//    REQUIRE(tc.resource_dir() == path);
//    REQUIRE(tc.check_suitable_environment());
//    std::error_code code;
//    REQUIRE(code.message() == "resdir_empty");
//}

TEST_CASE("Toolchain valid", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    REQUIRE(tc.resource_dir() == SMCE_PATH);
    REQUIRE_FALSE(tc.cmake_path().empty());
}

// TEST_CASE("Toolchain sketch_invalid","[Toolchain]"){
//    REQUIRE(smce::toolchain_error::resdir_absent );
//}
//    const auto path = SMCE_TEST_DIR "/empty_dir";
//    std::filesystem::create_directory(path);
//    smce::Toolchain tc{path};
//    REQUIRE(tc.resource_dir() == path);
//    REQUIRE(tc.check_suitable_environment());
//    std::error_code code;
//    REQUIRE(code.message() == "The operation completed successfully.");
//}
