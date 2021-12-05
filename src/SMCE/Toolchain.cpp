/*
 *  Toolchain.cpp
 *  Copyright 2021 ItJustWorksTM
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

#include <SMCE/Toolchain.hpp>

#include <fstream>
#include <string>
#include <system_error>
#include <boost/predef.h>
#include <boost/process.hpp>
#if BOOST_OS_WINDOWS
#    include <boost/process/windows.hpp>
#endif
#include <SMCE/PluginManifest.hpp>
#include <SMCE/SMCE_iface.h>
#include <SMCE/Sketch.hpp>
#include <SMCE/SketchConf.hpp>
#include <SMCE/internal/BoardDeviceSpecification.hpp>
#include <SMCE/internal/utils.hpp>

using namespace std::literals;

namespace bp = boost::process;

namespace smce {
namespace detail {

struct toolchain_error_category : public std::error_category {
  public:
    const char* name() const noexcept override { return "smce.toolchain"; }

    std::string message(int ev) const override {
        switch (static_cast<toolchain_error>(ev)) {
        case toolchain_error::resdir_absent:
            return "Resource directory does not exist";
        case toolchain_error::resdir_empty:
            return "Resource directory empty";
        case toolchain_error::resdir_file:
            return "Resource directory is a file";
        case toolchain_error::cmake_not_found:
            return "CMake not found in PATH";
        case toolchain_error::cmake_unknown_output:
            return "CMake output unrecognized";
        case toolchain_error::invalid_plugin_name:
            return "Plugin name is \".\", \"..\", or contains a forward slash";
        case toolchain_error::sketch_invalid:
            return "Sketch path is invalid";
        case toolchain_error::configure_failed:
            return "CMake configure failed";
        case toolchain_error::build_failed:
            return "CMake build failed";
        default:
            return "smce.toolchain error";
        }
    }

    std::error_condition default_error_condition(int ev) const noexcept override {
        return std::error_condition{ev, *this};
    }

    bool equivalent(int ev, const std::error_condition& condition) const noexcept override {
        return condition.value() == ev && &condition.category() == this;
    }

    bool equivalent(const std::error_code& error, int ev) const noexcept override {
        return error.value() == ev && &error.category() == this;
    }
};

const std::error_category& get_exec_ctx_error_category() noexcept {
    static const toolchain_error_category cat{};
    return cat;
}

} // namespace detail

std::error_code make_error_code(toolchain_error ev) noexcept {
    return std::error_code{static_cast<std::underlying_type<toolchain_error>::type>(ev),
                           detail::get_exec_ctx_error_category()};
}

struct ProcessedLibs {
    std::string pp_remote_arg = "-DPREPROC_REMOTE_LIBS=";
};
ProcessedLibs process_libraries(const SketchConfig& skonf) noexcept {
    ProcessedLibs ret;
    for (const auto& lib : skonf.legacy_preproc_libs) {
        ret.pp_remote_arg += lib.name;
        if (!lib.version.empty())
            ret.pp_remote_arg += '@' + lib.version;
        ret.pp_remote_arg += ';';
    }

    if (ret.pp_remote_arg.back() == ';')
        ret.pp_remote_arg.pop_back();

    return ret;
}
SMCE_INTERNAL std::error_code write_manifests(const SketchConfig& skonf, const stdfs::path& tmpdir) {
    const auto manifests_dir = tmpdir / "manifests";
    {
        std::error_code ec;
        stdfs::create_directory(manifests_dir, ec);
        if (ec)
            return ec;
    }

    for (const auto& pm : skonf.plugins) {
        if (pm.name == "." || pm.name == ".." || pm.name.find('/') != std::string::npos)
            return toolchain_error::invalid_plugin_name;
        if (const auto ec = write_manifest(pm, manifests_dir / (pm.name + ".cmake")))
            return ec;
    }
    return {};
}

SMCE_INTERNAL void write_devices_specs(const SketchConfig& skonf, const stdfs::path& tmpdir) {
    std::ofstream f{tmpdir / "Devices.cmake"};
    f << "# HSD generated\ninclude (BindGen)\n";
    for (const auto& e : skonf.genbind_devices)
        f << "smce_bindgen_sketch (" << e.get().full_string << ")\n";
}

Toolchain::Toolchain(stdfs::path resources_dir) noexcept : m_res_dir{std::move(resources_dir)} {
    m_build_log.reserve(4096);
}

std::error_code Toolchain::do_configure(Sketch& sketch) noexcept {
    const auto sketch_hexid = sketch.m_uuid.to_hex();
    sketch.m_tmpdir = this->m_res_dir / "tmp" / sketch_hexid;

    {
        std::error_code ec;
        stdfs::create_directories(sketch.m_tmpdir, ec);
        if (ec)
            return ec;
    }

#if !BOOST_OS_WINDOWS
    const char* const generator_override = std::getenv("CMAKE_GENERATOR");
    const char* const generator =
        generator_override ? generator_override : (!bp::search_path("ninja").empty() ? "Ninja" : "");
#endif

    write_devices_specs(sketch.m_conf, sketch.m_tmpdir);

    if (const auto ec = write_manifests(sketch.m_conf, sketch.m_tmpdir))
        return ec;
    ProcessedLibs libs = process_libraries(sketch.m_conf);

    namespace bp = boost::process;
    bp::ipstream cmake_conf_out;
    // clang-format off
    auto cmake_config = bp::child{
        m_cmake_path,
#if !BOOST_OS_WINDOWS
        bp::env["CMAKE_GENERATOR"] = generator,
#endif
        "-DSMCE_DIR=" + m_res_dir.string(),
        "-DSKETCH_HEXID=" + sketch_hexid,
        "-DSKETCH_FQBN=" + sketch.m_conf.fqbn,
        "-DSKETCH_PATH=" + stdfs::absolute(sketch.m_source).generic_string(),
        std::move(libs.pp_remote_arg),
        "-P",
        m_res_dir.string() + "/RtResources/SMCE/share/CMake/Scripts/ConfigureSketch.cmake",
        (bp::std_out & bp::std_err) > cmake_conf_out
#if BOOST_OS_WINDOWS
       , bp::windows::create_no_window
#endif
    };
    // clang-format on

    {
        std::string line;
        while (std::getline(cmake_conf_out, line)) {
            if (line.starts_with("-- SMCE: ")) {
                line.erase(0, line.find_first_of('"') + 1);
                line.pop_back();
                sketch.m_executable = std::move(line);
                break;
            }
            [[maybe_unused]] std::lock_guard lk{m_build_log_mtx};
            (m_build_log += line) += '\n';
        }
    }

    cmake_config.join();
    if (cmake_config.native_exit_code() != 0)
        return toolchain_error::configure_failed;
    return {};
}

std::error_code Toolchain::do_build(Sketch& sketch) noexcept {
    bp::ipstream cmake_build_out;
    // clang-format off
    auto cmake_build = bp::child{
#if BOOST_OS_WINDOWS
        bp::env["MSBUILDDISABLENODEREUSE"] = "1", // MSBuild "feature" which uses your child processes as potential deamons, forever
#endif
        m_cmake_path,
        "--build", (sketch.m_tmpdir / "build").string(),
        "--config", "Release",
        (bp::std_out & bp::std_err) > cmake_build_out
#if BOOST_OS_WINDOWS
       , bp::windows::create_no_window
#endif
    };
    // clang-format on

    for (std::string line; std::getline(cmake_build_out, line);) {
        [[maybe_unused]] std::lock_guard lk{m_build_log_mtx};
        (m_build_log += line) += '\n';
    }

    cmake_build.join();
    if (cmake_build.native_exit_code() != 0)
        return toolchain_error::build_failed;

    std::error_code ec;
    const bool binary_exists = stdfs::exists(sketch.m_executable, ec);
    if (ec)
        return ec;
    if (!binary_exists)
        return toolchain_error::build_failed;
    return {};
}

[[nodiscard]] std::error_code Toolchain::check_suitable_environment() noexcept {
    if (std::error_code ec; !stdfs::exists(m_res_dir, ec))
        return toolchain_error::resdir_absent;
    else if (ec)
        return ec;

    if (std::error_code ec; !stdfs::is_directory(m_res_dir, ec))
        return toolchain_error::resdir_file;
    else if (ec)
        return ec;

    if (std::error_code ec; stdfs::is_empty(m_res_dir, ec))
        return toolchain_error::resdir_empty;
    else if (ec)
        return ec;

    std::error_code ec = check_cmake_availability();
    if(ec)
        return ec;

    bp::ipstream cmake_out;
    // clang-format off
    const int cmres = bp::system(
        m_cmake_path,
        "--version",
        bp::std_out > cmake_out
#if BOOST_OS_WINDOWS
        , bp::windows::create_no_window
#endif
    );
    // clang-format on

    if (cmres != 0)
        return toolchain_error::cmake_failing;

    std::string line;
    std::getline(cmake_out, line);
    if (!line.starts_with("cmake"))
        return toolchain_error::cmake_unknown_output;

    return {};
}

[[nodiscard]] std::error_code Toolchain::check_cmake_availability() noexcept {
    if (m_cmake_path != (m_res_dir / "RtResources" / "CMake").string()) {
        if (std::error_code ec; stdfs::is_empty(m_cmake_path, ec)) {
            return toolchain_error::cmake_not_found;
        }
        else if (ec) {
            return ec;
        }
    } else {
        m_cmake_path = bp::search_path(m_cmake_path).string();
        if (m_cmake_path.empty()) {
            return toolchain_error::cmake_not_found;
        }
    }
    return {};
}

std::error_code Toolchain::compile(Sketch& sketch) noexcept {
    sketch.m_built = false;
    std::error_code ec;

    const bool source_exists = stdfs::exists(sketch.m_source, ec);
    if (ec)
        return ec;
    if (!source_exists)
        return toolchain_error::sketch_invalid;

    if (sketch.m_conf.fqbn.empty())
        return toolchain_error::sketch_invalid;

    ec = do_configure(sketch);
    if (ec)
        return ec;
    ec = do_build(sketch);
    if (ec)
        return ec;

    sketch.m_built = true;
    return {};
}

} // namespace smce
