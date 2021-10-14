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

TEST_CASE("Toolchain invalid_no_dir", "[Toolchain]") {
    const auto path =  "/no_dir";
    smce::Toolchain tc{path};
    int result = tc.check_suitable_environment().value();
    REQUIRE(result == 1);
}

TEST_CASE("Toolchain invalid_file_dir", "[Toolchain]") {
    const auto path = SMCE_TEST_DIR "/sketches/noop/noop.ino";
    smce::Toolchain tc{path};
    int result = tc.check_suitable_environment().value();
    REQUIRE(result == 2);
}

TEST_CASE("Toolchain invalid_empty_dir", "[Toolchain]") {
    const auto path = SMCE_TEST_DIR "/empty_dir";
    std::filesystem::create_directory(path);
    smce::Toolchain tc{path};
    int result = tc.check_suitable_environment().value();
    REQUIRE(result == 3);
}

TEST_CASE("Toolchain check_cmake_path", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(tc.cmake_path() == "cmake");
}

TEST_CASE("Toolchain invalid_compile_no_Sketch", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    smce::Sketch sk{SKETCHES_PATH "noSketch", {.fqbn = "arduino:avr:nano"}};
    REQUIRE(tc.compile(sk).value() == 8);
    REQUIRE_FALSE(sk.is_compiled());

}

TEST_CASE("Toolchain invalid_compile_no_fqbn", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    smce::Sketch sk{SKETCHES_PATH "noop", {}};
    REQUIRE(tc.compile(sk).value() == 8);
    REQUIRE_FALSE(sk.is_compiled());
}







