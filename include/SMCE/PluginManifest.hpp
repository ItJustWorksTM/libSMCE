/*
 *  PluginManifest.hpp
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

#ifndef LIBSMCE_PLUGINMANIFEST_HPP
#define LIBSMCE_PLUGINMANIFEST_HPP

#include <string>
#include <vector>
#include "SMCE/SMCE_fs.hpp"
#include "SMCE/SMCE_iface.h"

namespace smce {

struct PluginManifest {
    // clang-format off
    enum class Defaults {
        arduino,    /// src/** is sources, src/ is incdir, no linkdir
        single_dir, /// ./* is sources, ./ is incdir, ./ is linkdir
        c,          /// src/* is sources, include/ is incdir, lib is linkdir
        none,       /// empty
        cmake,      /// do not generate a target and do add_subdirectory
    };
    // clang-format on

    std::string name;
    std::string version;
    std::vector<std::string> depends; /// required plugins
    std::string uri;                  /// file:// of the source-root or http[s]:// of the tar/zip archive
    std::string patch_uri;            /// same as above but for patching
    Defaults defaults;
    std::vector<std::string> incdirs;
    std::vector<std::string> sources;
    std::vector<std::string> linkdirs;
    std::vector<std::string> linklibs;
    bool development = false; /// set to true to CONFIGURE_DEPENDS all the globs
};

/// \internal
SMCE_API std::error_code write_manifest(const PluginManifest& manifest, stdfs::path location) noexcept;

} // namespace smce

#endif // LIBSMCE_PLUGINMANIFEST_HPP
