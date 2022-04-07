/*
 *  test/BoardDevice.cpp
 *  Copyright 2020-2022 ItJustWorksTM
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
#include "SMCE/BoardConf.hpp"
#include "SMCE/Sketch.hpp"
#include "SMCE/Toolchain.hpp"
#include "TestUDD.hpp"
#include "defs.hpp"

using namespace std::literals;

TEST_CASE("Basic board device", "[BoardDevice]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    // clang-format off
    smce::PluginManifest uudprov_pm{
        "UDDProvider",
        "1.0",
        {},
        {"TestUDD"},
        "file://" PLUGINS_PATH "UDDProvider",
        "",
        smce::PluginManifest::Defaults::arduino,
    };
    smce::Sketch sk{SKETCHES_PATH "udd_consumer", {
            .fqbn = "arduino:avr:nano",
            .plugins = { std::move(uudprov_pm) },
            .genbind_devices = { TestUDD::specification }
    }};
    // clang-format on
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE(!ec);

    smce::Board br{};
    smce::BoardConfig bc{{}, {}, {}, {}, {}, {{TestUDD::specification, 2}}};

    REQUIRE(br.configure(std::move(bc)));
    REQUIRE(br.attach_sketch(sk));
    REQUIRE(br.start());
    auto bv = br.view();
    REQUIRE(bv.valid());
    auto devs = TestUDD::getObjects(bv);
    REQUIRE(devs.size() == 2);
    {
        std::size_t ticks = 8192;
        do {
            if (ticks-- == 0)
                FAIL("Timed out UDD atomic-wait");
            std::this_thread::sleep_for(2ms);
        } while (devs[0].f2.load() != 10);
        REQUIRE(devs[0].f2.load() == 10);
        REQUIRE(devs[0].f5.load() != 10);
        REQUIRE(devs[1].f2.load() != 10);
        REQUIRE(devs[1].f5.load() != 10);
    }
    {
        std::size_t ticks = 8192;
        do {
            if (ticks-- == 0)
                FAIL("Timed out UDD atomic-wait");
            std::this_thread::sleep_for(2ms);
        } while (devs[1].f2.load() != 65500);
        REQUIRE(devs[1].f2.load() == 65500);
        REQUIRE(devs[0].f2.load() != 65500);
    }
    // FIXME test mutexes and constant storage
    REQUIRE(br.stop());
}
