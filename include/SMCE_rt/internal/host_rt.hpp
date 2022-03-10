/*
 *  host_rt.hpp
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

#ifndef LIBSMCE_HOST_RT_HPP
#define LIBSMCE_HOST_RT_HPP

#include <string_view>
#include <SMCE/SMCE_iface.h>
#include <SMCE/fwd.hpp>
#include <SMCE_rt/internal/BoardDeviceAllocationBases.hpp>

namespace smce_rt {

SMCE_API BoardDeviceAllocationPtrBases getBases(smce::BoardView&, std::string_view);

}

#endif // LIBSMCE_HOST_RT_HPP
