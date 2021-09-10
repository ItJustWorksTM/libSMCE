/*
 *  SMCE/UDDRuntime.cpp
 *  Copyright 2021 ItJustWorksTM
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

#include "SMCE/internal/BoardDeviceView.hpp"
#include "SMCE_rt/internal/host_rt.hpp"

namespace smce_rt {

BoardDeviceAllocationPtrBases getBases(smce::BoardView& bv, std::string_view sv) {
    return smce::BoardDeviceView{bv}.getBases(sv);
}

} // namespace smce_rt
