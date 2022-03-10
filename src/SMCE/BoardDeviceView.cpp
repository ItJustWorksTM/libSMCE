/*
 *  BoardDeviceView.hpp
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
#include "SMCE/BoardView.hpp"
#include "SMCE/internal/BoardData.hpp"
#include "SMCE/internal/BoardDeviceView.hpp"

namespace smce {

BoardDeviceView::BoardDeviceView(BoardView& bv) noexcept : m_bdat{bv.m_bdat} {}

[[nodiscard]] smce_rt::BoardDeviceAllocationPtrBases BoardDeviceView::getBases(std::string_view dev_name) {
    smce_rt::BoardDeviceAllocationPtrBases ret{};
    if (!m_bdat)
        return ret;
    const auto it = std::lower_bound(m_bdat->device_allocation_map.begin(), m_bdat->device_allocation_map.end(),
                                     dev_name, [](const auto& p, std::string_view sv) {
                                         return std::string_view{p.first.c_str(), p.first.size()} < sv;
                                     });
    const auto& rel_bases = it->second;
    ret.count = rel_bases.count;
    ret.r8 = m_bdat->raw_bank.data() + rel_bases.r8;
    ret.r16 = m_bdat->raw_bank.data() + rel_bases.r16;
    ret.r32 = m_bdat->raw_bank.data() + rel_bases.r32;
    ret.r64 = m_bdat->raw_bank.data() + rel_bases.r64;
    ret.a8 = m_bdat->a8_bank.data() + rel_bases.a8;
    ret.a16 = m_bdat->a16_bank.data() + rel_bases.a16;
    ret.a32 = m_bdat->a32_bank.data() + rel_bases.a32;
    ret.a64 = m_bdat->a64_bank.data() + rel_bases.a64;
    ret.mtx = m_bdat->mtx_bank.data() + rel_bases.mtx;
    return ret;
}

} // namespace smce
