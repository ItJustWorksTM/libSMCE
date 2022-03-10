/*
 *  SketchConf.hpp
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

#ifndef SMCE_SKETCHCONF_HPP
#define SMCE_SKETCHCONF_HPP

#include <functional>
#include <string>
#include <variant>
#include <vector>
#include <SMCE/PluginManifest.hpp>
#include <SMCE/SMCE_fs.hpp>
#include <SMCE/SMCE_iface.h>
#include <SMCE/fwd.hpp>

namespace smce {

/**
 * Configuration for sketch building
 **/
struct SMCE_API SketchConfig {
    /**
     * Library to pull from the Arduino library manager
     **/
    struct ArduinoLibrary {
        std::string name;    // Library name as found in the install command
        std::string version; // Version string; empty if latest
    };

    std::string fqbn;                                /// Fully-qualified board name that the sketch is targeting
    std::vector<std::string> extra_board_uris;       /// Extra board.txt URIs for ArduinoCLI
    std::vector<ArduinoLibrary> legacy_preproc_libs; /// Libraries to use during legacy preprocessing
    std::vector<PluginManifest> plugins;             /// Plugins to compile with
    std::vector<std::reference_wrapper<const BoardDeviceSpecification>>
        genbind_devices; /// Board devices to generate bindings for
};

} // namespace smce

#endif // SMCE_SKETCHCONF_HPP
