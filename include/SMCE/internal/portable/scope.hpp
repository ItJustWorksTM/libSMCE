/*
 *  scope_exit.hpp
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

#ifndef LIBSMCE_SCOPEEXIT_HPP
#define LIBSMCE_SCOPEEXIT_HPP

#if __has_include(<experimental/scope>)
#    include <experimental/scope>
#    if __cpp_lib_experimental_scope >= 201902
#        define SMCE_PORTABLE_HAVE_EXPERIMENTAL_SCOPE
#    endif
#endif

#include <type_traits>
#include <utility>

namespace smce::portable {

#ifdef SMCE_PORTABLE_HAVE_EXPERIMENTAL_SCOPE

using std::experimental::scope_exit;
using std::experimental::scope_fail;

#else

#    include <exception>

template <class EF>
class scope_exit {
    EF m_exit_function;
    bool m_active = true;

  public:
    template <class Fn>
    explicit scope_exit(Fn&& fn) noexcept(std::is_nothrow_constructible_v<EF, Fn> ||
                                          std::is_nothrow_constructible_v<EF, Fn&>)
        : m_exit_function{std::forward<Fn>(fn)} {}

    scope_exit(scope_exit&& other) noexcept(std::is_nothrow_move_constructible_v<EF> ||
                                            std::is_nothrow_copy_constructible_v<EF>)
        : m_exit_function{std::move(other.m_exit_function)}, m_active{other.m_active} {
        other.release();
    }

    scope_exit(const scope_exit&) = delete;
    ~scope_exit() noexcept {
        if (m_active)
            m_exit_function();
    }

    void release() noexcept { m_active = false; }
};

template <class EF>
class scope_fail {
    EF m_exit_function;
    int m_exceptions_count;
    bool m_active = true;

  public:
    template <class Fn>
    explicit scope_fail(Fn&& fn) noexcept(std::is_nothrow_constructible_v<EF, Fn> ||
                                          std::is_nothrow_constructible_v<EF, Fn&>)
        : m_exit_function{std::forward<Fn>(fn)}, m_exceptions_count{std::uncaught_exceptions()} {}

    scope_fail(scope_fail&& other) noexcept(std::is_nothrow_move_constructible_v<EF> ||
                                            std::is_nothrow_copy_constructible_v<EF>)
        : m_exit_function{std::move(other.m_exit_function)}, m_active{other.m_active} {
        other.release();
    }

    scope_fail(const scope_fail&) = delete;
    ~scope_fail() noexcept {
        if (m_active && m_exceptions_count != std::uncaught_exceptions())
            m_exit_function();
    }

    void release() noexcept { m_active = false; }
};

template <class EF>
class scope_success {
    EF m_exit_function;
    int m_exceptions_count;
    bool m_active = true;

  public:
    template <class Fn>
    explicit scope_success(Fn&& fn) noexcept(std::is_nothrow_constructible_v<EF, Fn> ||
                                             std::is_nothrow_constructible_v<EF, Fn&>)
        : m_exit_function{std::forward<Fn>(fn)}, m_exceptions_count{std::uncaught_exceptions()} {}

    scope_success(scope_success&& other) noexcept(std::is_nothrow_move_constructible_v<EF> ||
                                                  std::is_nothrow_copy_constructible_v<EF>)
        : m_exit_function{std::move(other.m_exit_function)}, m_active{other.m_active} {
        other.release();
    }

    scope_success(const scope_success&) = delete;
    ~scope_success() noexcept(noexcept(std::declval<EF&>()())) {
        if (m_active && m_exceptions_count <= std::uncaught_exceptions())
            m_exit_function();
    }

    void release() noexcept { m_active = false; }
};

#endif

} // namespace smce::portable

#endif // LIBSMCE_SCOPE_EXIT_HPP
