/*
 *  Board.cpp
 *  Copyright 2021-2022 ItJustWorksTM
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

#include <SMCE/Board.hpp>
#include <boost/predef.h>

#if BOOST_OS_UNIX || BOOST_OS_MACOS
#    include <csignal>
#elif BOOST_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#    include <boost/process/windows.hpp>
#    include <winternl.h>
#    pragma comment(lib, "ntdll.lib")
extern "C" {
__declspec(dllimport) LONG NTAPI NtResumeProcess(HANDLE ProcessHandle);
__declspec(dllimport) LONG NTAPI NtSuspendProcess(HANDLE ProcessHandle);
}
#else
#    error "Unsupported platform"
#endif

#include <iostream>
#include <string>
#include <type_traits>
#include <SMCE/BoardConf.hpp>
#include <SMCE/BoardView.hpp>
#include <SMCE/Toolchain.hpp>
#include <SMCE/Uuid.hpp>
#include <SMCE/internal/BoardData.hpp>
#include <SMCE/internal/SharedBoardData.hpp>
#include <SMCE/internal/portable/scope.hpp>
#include <SMCE/internal/utils.hpp>
#include <boost/process.hpp>

namespace bp = boost::process;

namespace smce {

struct SMCE_INTERNAL Board::Internal {
    Uuid uuid = Uuid::generate();
    SharedBoardData sbdata;
    bp::child sketch;
    bp::ipstream sketch_log;
    std::thread sketch_log_grabber;
};

Board::Board(std::function<void(int)> exit_notify) noexcept
    : m_exit_notify{std::move(exit_notify)}, m_internal{std::make_unique<Internal>()} {
    m_runtime_log.reserve(4096);
}

Board::~Board() { do_reap(); }

[[nodiscard]] BoardView Board::view() noexcept {
    if (m_status != Status::prepared && m_status != Status::running && m_status != Status::suspended)
        return {};
    return BoardView{*m_internal->sbdata.get_board_data()};
}

bool Board::attach_sketch(const Sketch& sketch) noexcept {
    if (m_status == Status::running || m_status == Status::suspended)
        return false;
    m_sketch_ptr = &sketch;
    return true;
}

void Board::tick() noexcept {
    switch (m_status) {
    case Status::running:
    case Status::suspended: {
        auto& in = *m_internal;
        if (!in.sketch.running()) {
            const auto exit_code = m_internal->sketch.exit_code();
            do_sweep();
            m_status = Status::stopped;
            if (m_exit_notify)
                m_exit_notify(exit_code);
        }
    }
    default:;
    }
}

bool Board::reset() noexcept {
    switch (m_status) {
    case Status::running:
    case Status::suspended:
        return false;
    default:
        do_reap();
        m_sketch_ptr = nullptr;
        m_conf_opt = std::nullopt;
        m_internal = std::make_unique<Internal>();
        m_runtime_log.clear();
        m_status = Status::clean;
        return true;
    }
}

bool Board::configure(BoardConfig bconf) noexcept {
    if (m_status != Status::clean && m_status != Status::configured)
        return false;

    m_conf_opt = std::move(bconf);
    m_status = Status::configured;
    return true;
}

bool Board::prepare() noexcept {
    if (m_status != Status::configured && m_status != Status::stopped)
        return false;

    m_internal->sbdata.configure("SMCE-Runner-" + m_internal->uuid.to_hex(), *m_conf_opt);

    m_status = Status::prepared;
    return true;
}

bool Board::start() noexcept {
    if (m_status == Status::configured)
        prepare();
    if (m_status != Status::prepared && m_status != Status::stopped)
        return false;

    if (!m_sketch_ptr || !m_sketch_ptr->is_compiled())
        return false;

    for (const auto& skdev : m_sketch_ptr->m_conf.genbind_devices) {
        if (std::find_if(m_conf_opt->board_devices.cbegin(), m_conf_opt->board_devices.cend(), [&](const auto& e) {
                return e.spec.to_cmake() == skdev.to_cmake();
            }) == m_conf_opt->board_devices.cend())
            return false;
    }

    do_spawn();

    m_status = Status::running;
    return true;
}

bool Board::suspend() noexcept {
    if (m_status != Status::running)
        return false;

#if defined(__unix__)
    ::kill(m_internal->sketch.native_handle(), SIGSTOP);
#elif defined(_WIN32) || defined(WIN32)
    ::NtSuspendProcess(m_internal->sketch.native_handle());
#endif

    m_status = Status::suspended;
    return true;
}

bool Board::resume() noexcept {
    if (m_status != Status::suspended)
        return false;

#if defined(__unix__)
    ::kill(m_internal->sketch.native_handle(), SIGCONT);
#elif defined(_WIN32) || defined(WIN32)
    ::NtResumeProcess(m_internal->sketch.native_handle());
#endif

    m_status = Status::running;
    return true;
}

bool Board::terminate() noexcept {
    if (m_status != Status::running && m_status != Status::suspended)
        return false;

    do_reap();

    m_status = Status::stopped;
    return true;
}

bool Board::stop(std::chrono::milliseconds timeout) noexcept {
    if (m_status != Status::running)
        return false;

    m_internal->sbdata.get_board_data()->stop_requested = true;

    const bool exited = m_internal->sketch.wait_for(timeout);
    if (exited) {
        do_reap();
        m_status = Status::stopped;
    }

    return exited;
}

/**
 * Spawns the child process and its log grabber
 **/
void Board::do_spawn() noexcept {

    // clang-format off
    m_internal->sketch = bp::child{
        bp::env["SEGNAME"] = "SMCE-Runner-" + m_internal->uuid.to_hex(),
        "\"" + m_sketch_ptr->m_executable.string() + "\"",
        bp::std_out > bp::null,
        bp::std_err > m_internal->sketch_log
#if BOOST_OS_WINDOWS
        , bp::windows::create_no_window
#endif
    };
    // clang-format on

    m_internal->sketch_log_grabber = std::thread{[&] {
        const smce::portable::scope_fail<void (*)()> exception_detector{
            [] { std::cerr << "Detected in-flight exception in sketch log grabber\n"; }};
        auto& stream = m_internal->sketch_log;
        constexpr size_t buf_len = 1024;
        std::string buf;
        buf.reserve(buf_len);
        while (stream.good()) {
            const int head = stream.get();
            if (head == std::remove_cvref_t<decltype(stream)>::traits_type::eof())
                break;
            buf.resize(stream.rdbuf()->in_avail());
            const auto count = stream.readsome(buf.data(), buf.size());
            [[maybe_unused]] std::lock_guard lk{m_runtime_log_mtx};
            const auto existing = m_runtime_log.size();
            m_runtime_log.resize(existing + count + 1);
            m_runtime_log[existing] = static_cast<char>(head);
            std::memcpy(m_runtime_log.data() + existing + 1, buf.data(), count);
        }
        stream.pipe().close();
    }};
}

/**
 * Cleans up after a suicidal sketch
 **/
void Board::do_sweep() noexcept {
    auto& in = *m_internal;
    [[maybe_unused]] std::error_code ignored;
    in.sketch.wait(ignored);
    in.sketch = bp::child{}; // clear pid
    if (in.sketch_log_grabber.joinable())
        in.sketch_log_grabber.join();
}

/**
 * Reap a sketch and clean it up
 **/
void Board::do_reap() noexcept {
    if (!m_internal)
        return;
    auto& in = *m_internal;

    [[maybe_unused]] std::error_code ignored;
    in.sketch.terminate(ignored);
    in.sketch.wait(ignored);
    in.sketch = bp::child{}; // clear pid
    if (in.sketch_log_grabber.joinable()) {
        in.sketch_log_grabber.join();
    }
}

} // namespace smce
