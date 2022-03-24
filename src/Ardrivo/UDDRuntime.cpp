/*
 *  Ardrivo/UDDRuntime.cpp
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

#include "SMCE/BoardDeviceView.hpp"
#include "SMCE/BoardView.hpp"
#include "SMCE_rt/internal/sketch_rt.hpp"

namespace smce {
extern BoardView board_view;
extern void maybe_init();
} // namespace smce

namespace smce_rt {

static smce::VirtualDeviceField field(const char* dnam, std::size_t didx, const char* fnam) {
    return smce::BoardDeviceView{smce::board_view}[dnam][didx][fnam];
}

SMCE__DLL_API void devices_init() { smce::maybe_init(); }

SMCE__DLL_API std::size_t device_count(const char* dnam) {
    return smce::BoardDeviceView{smce::board_view}[dnam].size();
}

#define GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(cmtype, cxxtype)                                                       \
    SMCE__DLL_API cxxtype& device_field_##cmtype(const char* dnam, std::size_t didx, const char* fnam) {               \
        return field(dnam, didx, fnam).as_##cmtype();                                                                  \
    }
#define GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(cmtype, cxxtype)                                                            \
    SMCE__DLL_API cxxtype device_field_##cmtype(const char* dnam, std::size_t didx, const char* fnam) {                \
        return field(dnam, didx, fnam).as_##cmtype();                                                                  \
    }

GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(u8, std::uint8_t)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(u16, std::uint16_t)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(u32, std::uint32_t)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(u64, std::uint64_t)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(s8, std::int8_t)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(s16, std::int16_t)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(s32, std::int32_t)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(s64, std::int64_t)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(f32, float)
GEN_DEVICE_FIELD_TRAMPOLINE_FUNDAMENTAL(f64, double)

GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(au8, smce_rt::AtomicU8)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(au16, smce_rt::AtomicU16)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(au32, smce_rt::AtomicU32)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(au64, smce_rt::AtomicU64)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(as8, smce_rt::AtomicS8)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(as16, smce_rt::AtomicS16)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(as32, smce_rt::AtomicS32)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(as64, smce_rt::AtomicS64)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(af32, smce_rt::AtomicF32)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(af64, smce_rt::AtomicF64)
GEN_DEVICE_FIELD_TRAMPOLINE_OPAQUE(mutex, smce_rt::Mutex)

} // namespace smce_rt
