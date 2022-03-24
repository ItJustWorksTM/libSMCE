/*
 *  BindGenProxies.cpp
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

#include "SMCE/internal/BoardData.hpp"
#include "SMCE_rt/SMCE_proxies.hpp"

using A8 = smce::IpcAtomicValue<std::uint8_t>;
using A16 = smce::IpcAtomicValue<std::uint16_t>;
using A32 = smce::IpcAtomicValue<std::uint32_t>;
using A64 = smce::IpcAtomicValue<std::uint64_t>;
using F32 = smce::IpcAtomicValue<float>;
using F64 = smce::IpcAtomicValue<double>;
using Mtx = smce::IpcMovableMutex;

namespace smce_rt {

std::uint8_t AtomicU8::load() noexcept { return static_cast<A8*>(m_ptr)->load(); }
void AtomicU8::store(std::uint8_t v) noexcept { static_cast<A8*>(m_ptr)->store(v); }

std::uint16_t AtomicU16::load() noexcept { return static_cast<A16*>(m_ptr)->load(); }
void AtomicU16::store(std::uint16_t v) noexcept { static_cast<A16*>(m_ptr)->store(v); }

std::uint32_t AtomicU32::load() noexcept { return static_cast<A32*>(m_ptr)->load(); }
void AtomicU32::store(std::uint32_t v) noexcept { static_cast<A32*>(m_ptr)->store(v); }

std::uint64_t AtomicU64::load() noexcept { return static_cast<A64*>(m_ptr)->load(); }
void AtomicU64::store(std::uint64_t v) noexcept { static_cast<A64*>(m_ptr)->store(v); }

float AtomicF32::load() noexcept { return static_cast<F32*>(m_ptr)->load(); }
void AtomicF32::store(float v) noexcept { static_cast<F32*>(m_ptr)->store(v); }

double AtomicF64::load() noexcept { return static_cast<F64*>(m_ptr)->load(); }
void AtomicF64::store(double v) noexcept { static_cast<F64*>(m_ptr)->store(v); }

void Mutex::lock() { static_cast<Mtx*>(m_ptr)->lock(); }
bool Mutex::try_lock() { return static_cast<Mtx*>(m_ptr)->try_lock(); }
void Mutex::unlock() { static_cast<Mtx*>(m_ptr)->unlock(); }

} // namespace smce_rt
