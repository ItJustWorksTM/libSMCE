#
#  BuildProfiles/OpenSuseLeap.cmake
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
set (SMCE_PROFILE_VERSION 1)

include ("${CMAKE_CURRENT_LIST_DIR}/Common/Fedora.cmake")

set (SMCE_BUILD_SHARED On)
set (SMCE_BUILD_STATIC On)
set (SMCE_CXXRT_LINKING SHARED)
set (SMCE_BOOST_LINKING SOURCE)
set (SMCE_ARDRIVO_MQTT On)
set (SMCE_MOSQUITTO_LINKING SOURCE)
set (SMCE_OPENSSL_LINKING SHARED)
set (SMCE_ARDRIVO_OV767X On)

set (SMCE_OS_RELEASE "%{?dist}")
set (SMCE_CPACK_PROFILE "Templates/Fedora")
