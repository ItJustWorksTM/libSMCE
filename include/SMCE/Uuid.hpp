/*
 *  Uuid.hpp
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

#ifndef SMCE_UUID_HPP
#define SMCE_UUID_HPP

#include <array>
#include <cstddef>
#include <string>
#include <SMCE/SMCE_iface.h>
#include <SMCE/fwd.hpp>

namespace smce {

class SMCE_API Uuid {
  public:
    std::array<std::byte, 16> bytes;

    std::string to_hex() const noexcept;

    static Uuid generate() noexcept;
};

} // namespace smce

#endif // SMCE_UUID_HPP
