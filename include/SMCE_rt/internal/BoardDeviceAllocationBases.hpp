/*
 *  BoardDeviceAllocationBases.hpp
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

// Must remain C++11 compatible

#ifndef LIBSMCE_BOARDDEVICEALLOCATIONBASES_HPP
#define LIBSMCE_BOARDDEVICEALLOCATIONBASES_HPP

#include <cstddef>
#include "SMCE_api.hpp"

namespace smce_rt {

/// \internal
struct SMCE_PROXY_API BoardDeviceAllocationIntBases {
    std::size_t count;
    std::size_t r8;
    std::size_t r16;
    std::size_t r32;
    std::size_t r64;
    std::size_t a8;
    std::size_t a16;
    std::size_t a32;
    std::size_t a64;
    std::size_t mtx;
};

/// \internal
struct SMCE_PROXY_API BoardDeviceAllocationPtrBases {
    std::size_t count{};
    unsigned char* r8{};
    unsigned char* r16{};
    unsigned char* r32{};
    unsigned char* r64{};
    void* a8{};
    void* a16{};
    void* a32{};
    void* a64{};
    void* mtx{};
};

} // namespace smce_rt

#endif // LIBSMCE_BOARDDEVICEALLOCATIONBASES_HPP
