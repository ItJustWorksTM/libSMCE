/*
 *  sketch_rt.hpp
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

#ifndef LIBSMCE_SKETCH_RT_HPP
#define LIBSMCE_SKETCH_RT_HPP

#include <cstdint>
#include <SMCE_dll.hpp>
#include "../SMCE_proxies.hpp"

namespace smce_rt {

SMCE__DLL_RT_API void devices_init();
SMCE__DLL_RT_API std::size_t device_count(const char* dnam);
SMCE__DLL_RT_API std::uint8_t& device_field_u8(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API std::uint16_t& device_field_u16(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API std::uint32_t& device_field_u32(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API std::uint64_t& device_field_u64(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API std::int8_t& device_field_s8(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API std::int16_t& device_field_s16(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API std::int32_t& device_field_s32(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API std::int64_t& device_field_s64(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API float& device_field_f32(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API double& device_field_f64(const char* dnam, std::size_t didx, const char* fnam);

SMCE__DLL_RT_API smce_rt::AtomicU8 device_field_au8(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicU16 device_field_au16(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicU32 device_field_au32(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicU64 device_field_au64(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicS8 device_field_as8(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicS16 device_field_as16(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicS32 device_field_as32(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicS64 device_field_as64(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicF32 device_field_af32(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::AtomicF64 device_field_af64(const char* dnam, std::size_t didx, const char* fnam);
SMCE__DLL_RT_API smce_rt::Mutex device_field_mutex(const char* dnam, std::size_t didx, const char* fnam);

} // namespace smce_rt

#endif // LIBSMCE_SKETCH_RT_HPP
