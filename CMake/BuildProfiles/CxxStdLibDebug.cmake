#
#  BuildProfiles/CxxStdLibDebug.cmake
#  Copyright 2021-2022 ItJustWorksTM
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

include_guard ()

message (WARNING "This profile is NOT intended for packaging")

set (SMCE_BUILD_SHARED On)
set (SMCE_BUILD_STATIC Off)
set (SMCE_CXXRT_LINKING SHARED)
set (SMCE_BOOST_LINKING SOURCE)
set (SMCE_ARDRIVO_MQTT On)
set (SMCE_MOSQUITTO_LINKING SOURCE)
set (SMCE_OPENSSL_LINKING SHARED)
set (SMCE_ARDRIVO_OV767X On)

# leaky leaky, but needs to be
add_compile_definitions (_GLIBCXX_DEBUG=1 _ITERATOR_DEBUG_LEVEL=2) # libstdc++
add_compile_definitions (_LIBCPP_DEBUG=1 _LIBCPP_ENABLE_NODISCARD=1) # libc++
add_compile_definitions (_ITERATOR_DEBUG_LEVEL=2) # MSSTL
