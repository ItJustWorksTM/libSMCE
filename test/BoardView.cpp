/*
 *  test/BoardView.cpp
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

#include <array>
#include <chrono>
#include <iostream>
#include <thread>
#include <catch2/catch_test_macros.hpp>
#include "SMCE/Board.hpp"
#include "SMCE/BoardConf.hpp"
#include "SMCE/BoardView.hpp"
#include "SMCE/Toolchain.hpp"
#include "defs.hpp"

using namespace std::literals;
// Delete the space there to pass the format check
TEST_CASE("BoardView GPIO", "[BoardView]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    smce::Sketch sk{SKETCHES_PATH "pins", {.fqbn = "arduino:avr:nano"}};
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE_FALSE(ec);
    smce::Board br{};
    // clang-format off
    smce::BoardConfig bc{
        /* .pins = */{0, 2},
        /* .gpio_drivers = */{
            smce::BoardConfig::GpioDrivers{
                0,
                smce::BoardConfig::GpioDrivers::DigitalDriver{true, false},
                smce::BoardConfig::GpioDrivers::AnalogDriver{true, false}
            },
            smce::BoardConfig::GpioDrivers{
                2,
                smce::BoardConfig::GpioDrivers::DigitalDriver{false, true},
                smce::BoardConfig::GpioDrivers::AnalogDriver{false, true}
            },
        }
    };
    // clang-format on
    REQUIRE(br.configure(std::move(bc)));
    REQUIRE(br.attach_sketch(sk));
    REQUIRE(br.start());
    auto bv = br.view();
    REQUIRE(bv.valid());
    auto pin0 = bv.pins[0];
    REQUIRE(pin0.exists());
    auto pin0d = pin0.digital();
    REQUIRE(pin0d.exists());
    REQUIRE(pin0d.can_read());
    REQUIRE_FALSE(pin0d.can_write());
    auto pin0a = pin0.analog();
    REQUIRE(pin0a.exists());
    REQUIRE(pin0a.can_read());
    REQUIRE_FALSE(pin0a.can_write());
    auto pin1 = bv.pins[1];
    REQUIRE_FALSE(pin1.exists());
    auto pin2 = bv.pins[2];
    REQUIRE(pin2.exists());
    auto pin2d = pin2.digital();
    REQUIRE(pin2d.exists());
    REQUIRE_FALSE(pin2d.can_read());
    REQUIRE(pin2d.can_write());
    auto pin2a = pin2.analog();
    REQUIRE(pin2a.exists());
    REQUIRE_FALSE(pin2a.can_read());
    REQUIRE(pin2a.can_write());
    std::this_thread::sleep_for(1ms);
    pin0d.write(false);
    test_pin_delayable(pin2d, true, 16384, 1ms);
    pin0d.write(true);
    test_pin_delayable(pin2d, false, 16384, 1ms);
    REQUIRE(br.stop());
}

TEST_CASE("BoardView UART", "[BoardView]") {
    // Initialize toolchain and set the path to smce_root(SMCE recourse directory)
    // is the resources_dir in variable m_res_dir?
    smce::Toolchain tc{SMCE_PATH};
    // Checking that we have a sutable environment? returns something if
    // that is not the case?
    REQUIRE(!tc.check_suitable_environment());
    //The sketch is in /sketches/uart is the source in m_source? and conf in m_conf?
    smce::Sketch sk{SKETCHES_PATH "uart", {.fqbn = "arduino:avr:nano"}};
    //compile the provided sketch. check if tha path exists, then configure, then build
    // where do we get the plugins?
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE_FALSE(ec);
    //Initialize a board
    smce::Board br{};
    // ask about configure()
    REQUIRE(br.configure({.uart_channels = {{}}}));
    // attach the sketch to the board
    REQUIRE(br.attach_sketch(sk));
    // the sketch is now running if br.start() returns true
    REQUIRE(br.start());
    // create a new boardView for the board. Sharedboarddata?
    auto bv = br.view();
    REQUIRE(bv.valid());
    auto uart0 = bv.uart_channels[0];
    REQUIRE(uart0.exists());
    REQUIRE(uart0.rx().exists());
    REQUIRE(uart0.rx().exists());
    REQUIRE(uart0.tx().exists());
    // do we only have the one uart channel since we only set one in uart.ino?
    auto uart1 = bv.uart_channels[1];
    REQUIRE_FALSE(uart1.exists());
    REQUIRE_FALSE(uart1.rx().exists());
    REQUIRE_FALSE(uart1.tx().exists());
    std::this_thread::sleep_for(1ms);
    // The data we want to send to the board
    std::array out = {'H', 'E', 'L', 'L', 'O', ' ', 'U', 'A', 'R', 'T', '\0'};
    // create an array of 64 bites to check if max lenght is correct.
    std::array<char, out.size()> in{};
    // Send data to the board, Serial.readString() is used to read it
    REQUIRE(uart0.rx().write(out) == out.size());
    //   Add test for rx size
    // REQUIRE(uart0.rx().size() != 0);
    int ticks = 16'000;
    do {
        if (ticks-- == 0)
            FAIL("Timed out");
        std::this_thread::sleep_for(1ms);
    } while (uart0.tx().size() != in.size());
    // Now we have recieved the same data from the board, serial.print()
    //check if the letter at the front is H
    REQUIRE(uart0.tx().front() == 'H');
    // read the data from the uart channel and the data is erased after reading.
    REQUIRE(uart0.tx().read(in) == in.size());
    // front returns "\0" if it is empty
    REQUIRE(uart0.tx().front() == '\0');
    REQUIRE(uart0.tx().size() == 0);
    // now both in and out should be the same size
    REQUIRE(in == out);

#if !MSVC_DEBUG
    std::reverse(out.begin(), out.end());
    REQUIRE(uart0.rx().write(out) == out.size());
    ticks = 16'000;
    do {
        if (ticks-- == 0)
            FAIL("Timed out");
        std::this_thread::sleep_for(1ms);
    } while (uart0.tx().size() != in.size());
    REQUIRE(uart0.tx().read(in) == in.size());
    REQUIRE(uart0.tx().size() == 0);
    REQUIRE(in == out);
#endif

    REQUIRE(br.stop());
}

constexpr auto div_ceil(std::size_t lhs, std::size_t rhs) { return lhs / rhs + !!(lhs % rhs); }

constexpr std::byte operator""_b(char c) noexcept { return static_cast<std::byte>(c); }

constexpr std::size_t bpp_444 = 4 + 4 + 4;
constexpr std::size_t bpp_888 = 8 + 8 + 8;

TEST_CASE("BoardView RGB444 cvt", "[BoardView]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    smce::Sketch sk{SKETCHES_PATH "noop", {.fqbn = "arduino:avr:nano"}};
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE_FALSE(ec);
    smce::Board br{};
    REQUIRE(br.configure({.frame_buffers = {{}}}));
    REQUIRE(br.attach_sketch(sk));
    REQUIRE(br.prepare());
    auto bv = br.view();
    REQUIRE(bv.valid());
    REQUIRE(br.start());
    REQUIRE(br.suspend());
    //Add test for storage get root
    std::uint16_t m_cspin = 0;
    REQUIRE_FALSE( bv.storage_get_root(smce::BoardView::Link::SPI, m_cspin) == "{}");

    auto fb = bv.frame_buffers[0];
    REQUIRE(fb.exists());

    {
        constexpr std::size_t height = 1;
        constexpr std::size_t width = 1;

        constexpr std::array in = {'\xBC'_b, '\x0A'_b};
        constexpr std::array expected_out = {'\xA0'_b, '\xB0'_b, '\xC0'_b};
        static_assert(in.size() == expected_out.size() / 3 * 2);

        fb.set_height(height);
        fb.set_width(width);
        REQUIRE(fb.write_rgb444(in));

        std::array<std::byte, std::size(expected_out)> out;
        REQUIRE(fb.read_rgb888(out));
        REQUIRE(out == expected_out);
    }

    {
        constexpr std::size_t height = 2;
        constexpr std::size_t width = 2;

        constexpr std::array in = {'\x23'_b, '\xF1'_b, '\x56'_b, '\xF4'_b, '\x89'_b, '\xF7'_b, '\xBC'_b, '\xFA'_b};
        constexpr std::array expected_out = {'\x10'_b, '\x20'_b, '\x30'_b, '\x40'_b, '\x50'_b, '\x60'_b,
                                             '\x70'_b, '\x80'_b, '\x90'_b, '\xA0'_b, '\xB0'_b, '\xC0'_b};
        static_assert(in.size() == expected_out.size() / 3 * 2);

        fb.set_height(height);
        REQUIRE(fb.get_height() == height);
        fb.set_width(width);
        REQUIRE(fb.get_width() == width);
        fb.write_rgb444(in);

        std::array<std::byte, std::size(expected_out)> out;
        fb.read_rgb888(out);
        REQUIRE(out == expected_out);
    }

    {
        constexpr std::size_t height = 1;
        constexpr std::size_t width = 1;

        constexpr std::array in = {'\xAD'_b, '\xBE'_b, '\xCF'_b};
        constexpr std::array expected_out = {'\xBC'_b, '\x0A'_b};
        static_assert(expected_out.size() == in.size() / 3 * 2);

        fb.set_height(height);
        fb.set_width(width);
        REQUIRE(fb.write_rgb888(in));

        std::array<std::byte, std::size(expected_out)> out;
        REQUIRE(fb.read_rgb444(out));
        REQUIRE(out == expected_out);
    }

    {
        constexpr std::size_t height = 2;
        constexpr std::size_t width = 2;

        constexpr std::array in = {'\x1A'_b, '\x2B'_b, '\x3C'_b, '\x4D'_b, '\x5E'_b, '\x6F'_b,
                                   '\x7A'_b, '\x8B'_b, '\x9C'_b, '\xAD'_b, '\xBE'_b, '\xCF'_b};
        constexpr std::array expected_out = {'\x23'_b, '\x01'_b, '\x56'_b, '\x04'_b,
                                             '\x89'_b, '\x07'_b, '\xBC'_b, '\x0A'_b};
        static_assert(expected_out.size() == in.size() / 3 * 2);

        fb.set_height(height);
        fb.set_width(width);
        fb.write_rgb888(in);

        std::array<std::byte, std::size(expected_out)> out;
        fb.read_rgb444(out);
        REQUIRE(out == expected_out);
    }


    REQUIRE(br.resume());
    REQUIRE(br.stop());
}

TEST_CASE("BoardView GPIO Analog", "[BoardView]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    smce::Sketch sk{SKETCHES_PATH "analogread", {.fqbn = "arduino:avr:nano"}};
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE_FALSE(ec);
    smce::Board br{};
    // clang-format off
    smce::BoardConfig bc{
        /* .pins = */{0, 2},
        /* .gpio_drivers = */{
            smce::BoardConfig::GpioDrivers{
                0,
                smce::BoardConfig::GpioDrivers::DigitalDriver{true, false},
                smce::BoardConfig::GpioDrivers::AnalogDriver{true, false}
            },
            smce::BoardConfig::GpioDrivers{
                2,
                smce::BoardConfig::GpioDrivers::DigitalDriver{false, true},
                smce::BoardConfig::GpioDrivers::AnalogDriver{false, true}
            },
        }
    };
    // clang-format on
    REQUIRE(br.configure(std::move(bc)));
    REQUIRE(br.attach_sketch(sk));
    REQUIRE(br.start());
    auto bv = br.view();
    REQUIRE(bv.valid());
    auto pin0 = bv.pins[0];
    REQUIRE(pin0.exists());
    // Just testing analogRead and AnalogWrite
    auto pin0a = pin0.analog();
    REQUIRE(pin0a.exists());
    REQUIRE(pin0a.can_read());
    REQUIRE_FALSE(pin0a.can_write());
    auto pin1 = bv.pins[1];
    REQUIRE_FALSE(pin1.exists());
    auto pin2 = bv.pins[2];
    REQUIRE(pin2.exists());
    auto pin2a = pin2.analog();
    REQUIRE(pin2a.exists());
    REQUIRE_FALSE(pin2a.can_read());
    REQUIRE(pin2a.can_write());
    smce::VirtualPin::DataDirection dir = smce::VirtualPin::DataDirection::in;
    REQUIRE(pin2.get_direction() == dir);
    std::this_thread::sleep_for(1ms);
    pin0a.write(42);
    // Checking if the information read from analogPin 0 that is used to write
    // to analogPin 2 are correct. We expect the value in analogPin 2
    // to be 42.
    test_pin_delayable(pin2a, 42, 16384, 1ms);
    pin0a.write(64);
    test_pin_delayable(pin2a, 64, 16384, 1ms);
    REQUIRE(br.stop());
}

TEST_CASE("BoardView UART", "[BoardView]") {
    // Initialize toolchain and set the path to smce_root(SMCE recourse directory)
    // is the resources_dir in variable m_res_dir?
    smce::Toolchain tc{SMCE_PATH};
    // Checking that we have a sutable environment? returns something if
    // that is not the case?
    REQUIRE(!tc.check_suitable_environment());
    //The sketch is in /sketches/uart is the source in m_source? and conf in m_conf?
    smce::Sketch sk{SKETCHES_PATH "uart", {.fqbn = "arduino:avr:nano"}};
    //compile the provided sketch. check if tha path exists, then configure, then build
    // where do we get the plugins?
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << tc.build_log().second;
    REQUIRE_FALSE(ec);
    //Initialize a board
    smce::Board br{};
    // ask about configure()
    REQUIRE(br.configure({.uart_channels = {{}}}));
    // attach the sketch to the board
    REQUIRE(br.attach_sketch(sk));
    // the sketch is now running if br.start() returns true
    REQUIRE(br.start());
    // create a new boardView for the board. Sharedboarddata?
    auto bv = br.view();
    REQUIRE(bv.valid());
    auto uart0 = bv.uart_channels[0];
    REQUIRE(uart0.exists());
    REQUIRE(uart0.rx().exists());
    REQUIRE(uart0.rx().exists());
    REQUIRE(uart0.tx().exists());
    // do we only have the one uart channel since we only set one in uart.ino?
    auto uart1 = bv.uart_channels[1];
    REQUIRE_FALSE(uart1.exists());
    REQUIRE_FALSE(uart1.rx().exists());
    REQUIRE_FALSE(uart1.tx().exists());
    std::this_thread::sleep_for(1ms);
    // The data we want to send to the board
    std::array out = {'H', 'E', 'L', 'L', 'O', ' ', 'U', 'A', 'R', 'T', '\0'};
    // create an array of 64 bites to check if max lenght is correct.
    std::array<char, out.size()> in{};
    // Send data to the board, Serial.readString() is used to read it
    REQUIRE(uart0.rx().write(out) == out.size());
    //   Add test for rx size
    // REQUIRE(uart0.rx().size() != 0);
    int ticks = 16'000;
    do {
        if (ticks-- == 0)
            FAIL("Timed out");
        std::this_thread::sleep_for(1ms);
    } while (uart0.tx().size() != in.size());
    // Now we have recieved the same data from the board, serial.print()
    //check if the letter at the front is H
    REQUIRE(uart0.tx().front() == 'H');
    // read the data from the uart channel and the data is erased after reading.
    REQUIRE(uart0.tx().read(in) == in.size());
    // front returns "\0" if it is empty
    REQUIRE(uart0.tx().front() == '\0');
    REQUIRE(uart0.tx().size() == 0);
    // now both in and out should be the same size
    REQUIRE(in == out);

#if !MSVC_DEBUG
    std::reverse(out.begin(), out.end());
    REQUIRE(uart0.rx().write(out) == out.size());
    ticks = 16'000;
    do {
        if (ticks-- == 0)
            FAIL("Timed out");
        std::this_thread::sleep_for(1ms);
    } while (uart0.tx().size() != in.size());
    REQUIRE(uart0.tx().read(in) == in.size());
    REQUIRE(uart0.tx().size() == 0);
    REQUIRE(in == out);
#endif

    REQUIRE(br.stop());
}
