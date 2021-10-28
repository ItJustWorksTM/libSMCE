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

#if defined(WIN32)
#    define BOOST_USE_WINDOWS_H
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#endif

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <boost/predef.h>
#include <boost/process/detail/traits/wchar_t.hpp> // Needed but not included by the header below
#include <boost/process/env.hpp>
#include <boost/process/io.hpp>
#if !BOOST_OS_WINDOWS
#    include <boost/process/search_path.hpp>
#endif
#include <boost/process/start_dir.hpp>
#include <boost/process/system.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "SMCE/Board.hpp"
#include "SMCE/BoardView.hpp"
#include "SMCE/PluginManifest.hpp"
#include "SMCE/Sketch.hpp"
#include "SMCE/SketchConf.hpp"
#include "SMCE/Toolchain.hpp"
#include "defs.hpp"
#include <vector>

namespace bp = boost::process;
using namespace std::literals;

template <class F>
static auto operator<<(std::ostream& os, F&& f)
    -> std::enable_if_t<std::is_invocable_r_v<std::ostream&, F, std::ostream&>, std::ostream&> {
    return f(os);
}

TEST_CASE("Invalid manifests processing", "[Plugin]") {
#if !BOOST_OS_WINDOWS
    const char* const generator_override = std::getenv("CMAKE_GENERATOR");
    const char* const generator =
        generator_override ? generator_override : (!bp::search_path("ninja").empty() ? "Ninja" : "");
#endif

    constexpr auto module_path = SMCE_PATH "/RtResources/SMCE/share/CMake/Modules/ProcessManifests.cmake";
    const std::filesystem::path tmproot = SMCE_PATH "/tmp";

    std::string manifest =
        GENERATE("BadName", "BadUriLocation", "BadUriScheme", "NoName", "NoUri", "NoVersion", "RelativeUri");
    INFO("Using manifest " + manifest);

    const auto base_dir = tmproot / ("test-" + manifest);
    std::filesystem::create_directories(base_dir);

    const auto plugin_root = base_dir / "plugin_root";
    std::filesystem::create_directory(base_dir / "manifests");

    std::filesystem::copy(MANIFESTS_PATH + manifest + ".cmake", base_dir / "manifests");
    const auto res = bp::system(bp::shell, bp::start_dir(base_dir.generic_string()),
#if !BOOST_OS_WINDOWS
                                bp::env["CMAKE_GENERATOR"] = generator,
#endif
                                "cmake", "-P", module_path, (bp::std_out & bp::std_err) > stderr);

    REQUIRE(res != 0);
    [[maybe_unused]] std::error_code ec;
    std::filesystem::remove_all(base_dir, ec);
}

TEST_CASE("Valid manifests processing", "[Plugin]") {
#if !BOOST_OS_WINDOWS
    const char* const generator_override = std::getenv("CMAKE_GENERATOR");
    const char* const generator =
        generator_override ? generator_override : (!bp::search_path("ninja").empty() ? "Ninja" : "");
#endif

    constexpr auto module_path = SMCE_PATH "/RtResources/SMCE/share/CMake/Modules/ProcessManifests.cmake";
    const std::filesystem::path tmproot = SMCE_PATH "/tmp";

    const auto id = smce::Uuid::generate();

    const auto base_dir = tmproot / ("test-" + id.to_hex());
    std::filesystem::create_directories(base_dir);

    const auto plugin_root = base_dir / "plugin_root";
    std::filesystem::create_directory(base_dir / "manifests");

    SECTION("ArduinoRemotePatch") {
        // clang-format off
        smce::PluginManifest pm{
            "ESP32_AnalogWrite",
            "0.2",
            {},
            {},
            "https://github.com/ERROPiX/ESP32_AnalogWrite/archive/refs/tags/0.2.zip",
            "file://" PATCHES_PATH "ESP32_analogRewrite",
            smce::PluginManifest::Defaults::arduino,
            {},
            {},
            {},
            {}
        };
        // clang-format on
        write_manifest(pm, base_dir / "manifests" / "ArduinoRemote.cmake");

        [[maybe_unused]] auto cmake_require_equals = [](const char* var, const char* val) {
            return [=](std::ostream& os) -> std::ostream& {
                os << "if (NOT " << var << " STREQUAL \"" << val << "\")\n";
                os << "  message (FATAL_ERROR \"ASSERTION FAILURE\")\n";
                os << "endif ()";
                return os;
            };
        };
        auto cmake_require_target = [](const char* tgt) {
            return [=](std::ostream& os) -> std::ostream& {
                os << "if (NOT TARGET " << tgt << ")\n";
                os << "  message (FATAL_ERROR \"ASSERTION FAILURE\")\n";
                os << "endif ()\n";
                return os;
            };
        };

        {
            std::ofstream empty_source{base_dir / "empty.cxx"};
            empty_source << "# empty\n";
            std::ofstream loader{base_dir / "CMakeLists.txt"};
            loader << "cmake_minimum_required (VERSION 3.12)\n";
            loader << "project (Test)\n";
            loader << "add_library (Ardrivo INTERFACE)\n";
            loader << "add_executable (Sketch empty.cxx)\n";
            loader << "include (\"" << module_path << "\")\n";
            loader << cmake_require_target("smce_plugin_ESP32_AnalogWrite");
        }

        const auto res =
            bp::system(bp::shell, bp::start_dir(base_dir.generic_string()),
#if !BOOST_OS_WINDOWS
                       bp::env["CMAKE_GENERATOR"] = generator,
#endif
                       "cmake", "--log-level=DEBUG", "-S", ".", "-B", "build", (bp::std_out & bp::std_err) > stderr);
        REQUIRE(res == 0);
    }
}

TEST_CASE("Valid plugin dependency processing", "[Plugin]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());

    std::vector<std::string> manifests_name = {"A","B","C","D","E","F","G"};
    std::vector<std::vector<std::string>> manifests_deps = {{}, {"C", "D"}, {"E", "F"},
                                                           {"F", "G"}, {},{},{}};
    std::vector<smce::PluginManifest> plugins;

    for(int i = 0; i < manifests_name.size(); i++)
    {
        smce::PluginManifest pm{
            .name = manifests_name[i],
            .version = "0",
            .depends = manifests_deps[i],
            .uri = "file://" SMCE_PATH,
            .defaults = smce::PluginManifest::Defaults::none
        };
        plugins.push_back(pm);
    }
    smce::SketchConfig skc{
        "arduino:avr:nano",
        {},
        {},
        std::move(plugins),
        {}
    };

    smce::Sketch sk{SKETCHES_PATH "noop", std::move(skc)};

    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;

    REQUIRE_FALSE(ec);
}

TEST_CASE("Invalid plugin dependency processing (cycle check)", "[Plugin]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());

    std::vector<std::string> manifests_name = {"A","B","C"};
    std::vector<std::vector<std::string>> manifests_deps = {{"B"},{"C"},{"A"}};
    std::vector<smce::PluginManifest> plugins;

    for(int i = 0; i < manifests_name.size(); i++)
    {
        smce::PluginManifest pm{
            .name = manifests_name[i],
            .version = "0",
            .depends = manifests_deps[i],
            .uri = "file://" SMCE_PATH,
            .defaults = smce::PluginManifest::Defaults::none
        };
        plugins.push_back(pm);
    }
    smce::SketchConfig skc{
        "arduino:avr:nano",
        {},
        {},
        std::move(plugins),
        {}
    };

    smce::Sketch sk{SKETCHES_PATH "noop", std::move(skc)};

    const auto ec = tc.compile(sk);
    std::size_t found = 0;
    if(ec) {
        std::string s = std::move(tc.build_log().second);
        std::cerr << s;
        found = s.find("Plugin dependency cycle detected!");
    }
    REQUIRE(ec);
    REQUIRE(found!=std::string::npos);
}

#if SMCE_ARDRIVO_MQTT

TEST_CASE("Board remote preproc lib", "[Board]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    // clang-format off
    smce::Sketch sk{SKETCHES_PATH "remote_pp", {
        .fqbn = "arduino:avr:nano",
        .legacy_preproc_libs = { smce::SketchConfig::ArduinoLibrary{"MQTT"} }
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
    smce::PluginManifest esp32aw_pm{
        "ESP32_AnalogWrite",
        "0.2",
        {},
        {},
        "https://github.com/ERROPiX/ESP32_AnalogWrite/archive/refs/tags/0.2.zip",
        "file://" PATCHES_PATH "ESP32_analogRewrite",
        smce::PluginManifest::Defaults::arduino,
        {},
        {},
        {},
        {}
    };

    smce::SketchConfig skc{
        "arduino:avr:nano",
        {},
        { smce::SketchConfig::ArduinoLibrary{"ESP32 AnalogWrite"} },
        { std::move(esp32aw_pm) }
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
