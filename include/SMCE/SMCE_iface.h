/*
 *  SMCE_api.h
 *  Copyright 2020-2022 ItJustWorksTM
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

#ifndef LIBSMCE_SMCE_IFACE_H
#define LIBSMCE_SMCE_IFACE_H

#if defined(_WIN32)
#    if defined(SMCE_LIB_BUILD)
#        define SMCE_API __declspec(dllexport)
#        define SMCE_API_FRIEND __declspec(dllexport)
#    elif !defined(SMCE_WIN32_LINK_STATIC)
#        define SMCE_API __declspec(dllimport)
#        define SMCE_API_FRIEND __declspec(dllimport)
#    else
#        define SMCE_API
#        define SMCE_API_FRIEND
#    endif
#    define SMCE_INTERNAL
#else
#    if defined(SMCE_LIB_BUILD)
#        define SMCE_API [[gnu::visibility("default")]]
#        define SMCE_API_FRIEND
#        define SMCE_INTERNAL [[gnu::visibility("hidden")]]
#    else
#        define SMCE_API
#        define SMCE_API_FRIEND
#        define SMCE_INTERNAL
#    endif
#endif

#endif // LIBSMCE_SMCE_IFACE_H
