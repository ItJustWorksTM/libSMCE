/*
 *  SMCE_proxies.hpp
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

#ifndef ADRIVOEXT_SMCE_PROXIES_HPP
#define ADRIVOEXT_SMCE_PROXIES_HPP

#include <cstdint>
#include "internal/SMCE_api.hpp"

namespace smce_rt {
struct Impl;

class SMCE_PROXY_API AtomicU8 {
    void* m_ptr{};

  public:
    AtomicU8(void* ptr, const Impl&) noexcept : m_ptr{ptr} {}
    AtomicU8() noexcept = default;
    AtomicU8(AtomicU8&) = delete;
    inline AtomicU8(AtomicU8&&) noexcept = default;

    std::uint8_t load() noexcept;
    void store(std::uint8_t v) noexcept;

    inline operator std::uint8_t() noexcept { return load(); }
    inline void operator=(std::uint8_t v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicU16 {
    void* m_ptr{};

  public:
    AtomicU16(void* ptr, const Impl&) noexcept : m_ptr{ptr} {}
    AtomicU16() noexcept = default;
    AtomicU16(AtomicU16&) = delete;
    inline AtomicU16(AtomicU16&&) noexcept = default;

    std::uint16_t load() noexcept;
    void store(std::uint16_t v) noexcept;

    inline operator std::uint16_t() noexcept { return load(); }
    inline void operator=(std::uint16_t v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicU32 {
    void* m_ptr{};

  public:
    AtomicU32(void* ptr, const Impl&) noexcept : m_ptr{ptr} {}
    AtomicU32() noexcept = default;
    AtomicU32(AtomicU32&) = delete;
    inline AtomicU32(AtomicU32&&) noexcept = default;

    std::uint32_t load() noexcept;
    void store(std::uint32_t v) noexcept;

    inline operator std::uint32_t() noexcept { return load(); }
    inline void operator=(std::uint32_t v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicU64 {
    void* m_ptr{};

  public:
    AtomicU64(void* ptr, const Impl&) noexcept : m_ptr{ptr} {}
    AtomicU64() noexcept = default;
    AtomicU64(AtomicU64&) = delete;
    inline AtomicU64(AtomicU64&&) noexcept = default;

    std::uint64_t load() noexcept;
    void store(std::uint64_t v) noexcept;

    inline operator std::uint64_t() noexcept { return load(); }
    inline void operator=(std::uint64_t v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicS8 {
    AtomicU8 m_u{};

  public:
    AtomicS8(void* ptr, const Impl& impl) noexcept : m_u{ptr, impl} {}
    AtomicS8() noexcept = default;
    AtomicS8(AtomicS8&) = delete;
    inline AtomicS8(AtomicS8&&) noexcept = default;

    inline std::int8_t load() noexcept { return static_cast<std::int8_t>(m_u.load()); }
    inline void store(std::int8_t v) noexcept { m_u.store(static_cast<std::uint8_t>(v)); }

    inline operator std::int8_t() noexcept { return load(); }
    inline void operator=(std::int8_t v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicS16 {
    AtomicU16 m_u{};

  public:
    AtomicS16(void* ptr, const Impl& impl) noexcept : m_u{ptr, impl} {}
    AtomicS16() noexcept = default;
    AtomicS16(AtomicS16&) = delete;
    inline AtomicS16(AtomicS16&&) noexcept = default;

    inline std::int16_t load() noexcept { return static_cast<std::int16_t>(m_u.load()); }
    inline void store(std::int16_t v) noexcept { m_u.store(static_cast<std::uint16_t>(v)); }

    inline operator std::int16_t() noexcept { return load(); }
    inline void operator=(std::int16_t v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicS32 {
    AtomicU32 m_u{};

  public:
    AtomicS32(void* ptr, const Impl& impl) noexcept : m_u{ptr, impl} {}
    AtomicS32() noexcept = default;
    AtomicS32(AtomicS32&) = delete;
    inline AtomicS32(AtomicS32&&) noexcept = default;

    std::int32_t load() noexcept { return static_cast<std::int32_t>(m_u.load()); }
    void store(std::int32_t v) noexcept { m_u.store(static_cast<std::uint32_t>(v)); }

    inline operator std::int32_t() noexcept { return load(); }
    inline void operator=(std::int32_t v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicS64 {
    AtomicU64 m_u{};

  public:
    AtomicS64(void* ptr, const Impl& impl) noexcept : m_u{ptr, impl} {}
    AtomicS64() noexcept = default;
    AtomicS64(AtomicS64&) = delete;
    inline AtomicS64(AtomicS64&&) noexcept = default;

    std::int64_t load() noexcept { return static_cast<std::int64_t>(m_u.load()); }
    void store(std::int64_t v) noexcept { m_u.store(static_cast<std::uint64_t>(v)); }

    inline operator std::int64_t() noexcept { return load(); }
    inline void operator=(std::int64_t v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicF32 {
    void* m_ptr{};

  public:
    AtomicF32(void* ptr, const Impl&) noexcept : m_ptr{ptr} {}
    AtomicF32() noexcept = default;
    AtomicF32(AtomicF32&) = delete;
    inline AtomicF32(AtomicF32&&) noexcept = default;

    float load() noexcept;
    void store(float v) noexcept;

    inline operator float() noexcept { return load(); }
    inline void operator=(float v) noexcept { store(v); }
};

class SMCE_PROXY_API AtomicF64 {
    void* m_ptr{};

  public:
    AtomicF64(void* ptr, const Impl&) noexcept : m_ptr{ptr} {}
    AtomicF64() noexcept = default;
    AtomicF64(AtomicF64&) = delete;
    inline AtomicF64(AtomicF64&&) noexcept = default;

    double load() noexcept;
    void store(double v) noexcept;

    inline operator double() noexcept { return load(); }
    inline void operator=(float v) noexcept { store(v); }
};

class SMCE_PROXY_API Mutex {
    void* m_ptr{};

  public:
    Mutex(void* ptr, const Impl&) noexcept : m_ptr{ptr} {}
    Mutex() noexcept = default;
    Mutex(Mutex&) = delete;
    inline Mutex(Mutex&&) noexcept = default;
    Mutex& operator=(Mutex&&) noexcept = default;

    void lock();
    bool try_lock();
    void unlock();
};

} // namespace smce_rt

#endif // ADRIVOEXT_SMCE_PROXIES_HPP
