/*
 *  PluginManifest.cpp
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

#include <array>
#include <fstream>
#include <ostream>
#include <system_error>
#include "SMCE/PluginManifest.hpp"
#include "SMCE/internal/portable/ostream_joiner.hpp"

template <class C>
auto cmake_list(const C& c) {
    return [&c](std::ostream& os) -> std::ostream& {
        std::copy(c.begin(), c.end(), smce::portable::ostream_joiner{os, ';'});
        return os;
    };
}

template <class F>
static auto operator<<(std::ostream& os, F&& f)
    -> std::enable_if_t<std::is_invocable_r_v<std::ostream&, F, std::ostream&>, std::ostream&> {
    return f(os);
}

namespace smce {

std::error_code write_manifest(const PluginManifest& manifest, stdfs::path location) noexcept {
    {
        std::error_code ec;
        stdfs::create_directories(location.parent_path(), ec);
        if (ec)
            return ec;
    }

    constexpr std::array<const char*, 5> defaults_strs{"ARDUINO", "SINGLE", "C", "", "CMAKE"};

    std::ofstream file{location};
    file << "# HSD generated\n";
    file << "include_guard ()\n\n";
    file << "set (PLUGIN_NAME \"" << manifest.name << "\")\n";
    file << "set (PLUGIN_VERSION \"" << manifest.version << "\")\n";
    file << "set (PLUGIN_DEPENDS " << cmake_list(manifest.depends) << ")\n";
    file << "set (PLUGIN_NEEDS_DEVICES " << cmake_list(manifest.needs_devices) << ")\n";
    file << "set (PLUGIN_DEV \"" << manifest.development << "\")\n";
    file << "set (PLUGIN_URI \"" << manifest.uri << "\")\n";
    file << "set (PLUGIN_PATCH_URI \"" << manifest.patch_uri << "\")\n";
    file << "set (PLUGIN_DEFAULTS \"" << defaults_strs[static_cast<unsigned>(manifest.defaults)] << "\")\n";
    file << "set (PLUGIN_INCDIRS " << cmake_list(manifest.incdirs) << ")\n";
    file << "set (PLUGIN_SOURCES " << cmake_list(manifest.sources) << ")\n";
    file << "set (PLUGIN_LINKDIRS " << cmake_list(manifest.linkdirs) << ")\n";
    file << "set (PLUGIN_LINKLIBS " << cmake_list(manifest.linklibs) << ")\n";

    return {};
}

} // namespace smce
