/*
 *  Sketch.hpp
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

#ifndef SMCE_SKETCH_HPP
#define SMCE_SKETCH_HPP

#include "SMCE/SMCE_fs.hpp"
#include "SMCE/SMCE_iface.h"
#include "SMCE/SketchConf.hpp"
#include "SMCE/Uuid.hpp"
#include "SMCE/fwd.hpp"

namespace smce {

// clang-format off
enum class sketch_error {
    invalid_source_path = 1,
};
// clang-format on

/**
 * A sketch
 **/
class SMCE_API Sketch {
    friend Board;
    friend Toolchain;

    Uuid m_uuid = Uuid::generate();
    SketchConfig m_conf;
    stdfs::path m_source;
    stdfs::path m_tmpdir;
    stdfs::path m_executable;
    bool m_built = false;
    // bool m_dirty = true;

  public:
    explicit Sketch(stdfs::path source, SketchConfig conf) noexcept
        : m_conf(std::move(conf)), m_source{std::move(source)} {}
    ~Sketch();

    [[nodiscard]] const stdfs::path& get_source() const noexcept { return m_source; }

    [[nodiscard]] bool is_compiled() const noexcept { return m_built; }

    const Uuid& get_uuid() const noexcept { return m_uuid; }
};

} // namespace smce

#endif // LIBSMCE_SKETCH_HPP
