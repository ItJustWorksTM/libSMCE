/*
 *  SMCE_support.hpp
 *  Copyright 2022 ItJustWorksTM
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

#ifndef SMCE__SMCE_SUPPORT_HPP
#define SMCE__SMCE_SUPPORT_HPP

#if __cplusplus >= 201703L
#    define SMCE__NODISCARD [[nodiscard]]
#else
#    if defined(__GNUC__) && __has_attribute(warn_unused_result)
#        define SMCE__NODISCARD [[gnu::warn_unused_result]]
#    elif defined(_MSC_VER) && _MSC_VER >= 1700
#        define SMCE__NODISCARD _Check_return_
#    else
#        define SMCE__NODISCARD
#    endif
#endif

#endif // SMCE__SMCE_SUPPORT_HPP
