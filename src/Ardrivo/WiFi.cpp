/*
 *  WiFi.cpp
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

#include <algorithm>
#include "SMCE_dll.hpp"
#include "WiFi.h"

SMCE__DLL_API WiFiClass WiFi;

std::uint8_t* WiFiClass::BSSID(std::uint8_t* bssid) {
    std::fill_n(bssid, WL_MAC_ADDR_LENGTH, 0);
    return bssid;
}

std::uint8_t* WiFiClass::macAddress(std::uint8_t* mac) const {
    std::fill_n(mac, WL_MAC_ADDR_LENGTH, 0);
    return mac;
}
