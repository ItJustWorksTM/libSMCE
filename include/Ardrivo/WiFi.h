/*
 *  WiFi.h
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

#ifndef WiFi_h
#define WiFi_h

#include <cstdint>
#include "IPAddress.h"
#include "SMCE_dll.hpp"
#include "WiFiClient.h"

// clang-format off
enum {
    WL_SSID_MAX_LENGTH = 32,
    WL_WPA_KEY_MAX_LENGTH = 63,
    WL_WEP_KEY_MAX_LENGTH = 13,
    WL_MAC_ADDR_LENGTH = 6,
    WL_IPV4_LENGTH = 4,
    WL_NETWORKS_LIST_MAXNUM = 10,
    MAX_SOCK_NUM = 4,
    SOCK_NOT_AVAIL = 255,
    NA_STATE = -1,
    WL_MAX_ATTEMPT_CONNECTION = 10,
};

enum wl_status_t {
    WL_NO_SHIELD = 255, // for compatibility with WiFi Shield library
    WL_IDLE_STATUS = 0,
    WL_NO_SSID_AVAIL = 1,
    WL_SCAN_COMPLETED = 2,
    WL_CONNECTED = 3,
    WL_CONNECT_FAILED = 4,
    WL_CONNECTION_LOST = 5,
    WL_DISCONNECTED = 6,
};

enum wl_enc_type {
    ENC_TYPE_WEP = 5,
    ENC_TYPE_TKIP = 2,
    ENC_TYPE_CCMP = 4,
    ENC_TYPE_NONE = 7,
    ENC_TYPE_AUTO = 8,
};
// clang-format on

struct WiFiClass {
    int begin() { return 0; }
    int begin([[maybe_unused]] const char* ssid) { return WL_NO_SHIELD; }
    int begin([[maybe_unused]] const char* ssid, [[maybe_unused]] const char* pass) { return WL_NO_SHIELD; }
    int begin([[maybe_unused]] const char* ssid, [[maybe_unused]] std::uint8_t key_idx,
              [[maybe_unused]] const char* key) {
        return WL_NO_SHIELD;
    }
    int disconnect() { return WL_NO_SHIELD; }
    void config([[maybe_unused]] IPAddress local_ip) {}
    void config([[maybe_unused]] IPAddress local_ip, [[maybe_unused]] IPAddress dns_server) {}
    void config([[maybe_unused]] IPAddress local_ip, [[maybe_unused]] IPAddress dns_server,
                [[maybe_unused]] IPAddress gateway) {}
    void config([[maybe_unused]] IPAddress local_ip, [[maybe_unused]] IPAddress dns_server,
                [[maybe_unused]] IPAddress gateway, [[maybe_unused]] IPAddress subnet) {}
    void setDNS([[maybe_unused]] IPAddress dns_server1) {}
    void setDNS([[maybe_unused]] IPAddress dns_server1, [[maybe_unused]] IPAddress dns_server2) {}
    /*[[nodiscard]]*/ char* SSID() const { return nullptr; }
    /*[[nodiscard]]*/ char* SSID([[maybe_unused]] std::uint8_t networkItem) const { return nullptr; }
    std::uint8_t* BSSID(std::uint8_t* bssid);
    /*[[nodiscard]]*/ std::int32_t RSSI() { return 0; }
    /*[[nodiscard]]*/ std::int32_t RSSI([[maybe_unused]] std::uint8_t networkItem) { return 0; }
    /*[[nodiscard]]*/ std::uint8_t encryptionType() { return ENC_TYPE_AUTO; }
    /*[[nodiscard]]*/ std::uint8_t encryptionType([[maybe_unused]] std::uint8_t networkItem) { return ENC_TYPE_AUTO; }
    std::int8_t scanNetworks() { return 0; }
    /*[[nodiscard]]*/ static std::uint8_t getSocket() { return 0; }
    std::uint8_t* macAddress(std::uint8_t* mac) const;
    /*[[nodiscard]]*/ int status() const { return WL_NO_SHIELD; }

    /*[[nodiscard]]*/ IPAddress localIP() { return {}; }
    /*[[nodiscard]]*/ IPAddress subnetMask() { return {}; }
    /*[[nodiscard]]*/ IPAddress gatewayIP() { return {}; }
};

SMCE__DLL_RT_API
extern WiFiClass WiFi;

#endif // WiFi_h
