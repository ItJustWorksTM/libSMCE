// HSD generated

#include <algorithm>
#include <iterator>
#include <SMCE/internal/BoardDeviceSpecification.hpp>
#include <SMCE_rt/internal/host_rt.hpp>
#include "TestUDD.hpp"

namespace smce_rt {
struct Impl {};
}

TestUDD::TestUDD(std::int8_t& f0, std::uint32_t& f1) : f0{f0}, f1{f1} {}

std::vector<TestUDD> TestUDD::getObjects(smce::BoardView& bv) {
    auto bases = smce_rt::getBases(bv, "TestUDD");
    std::vector<TestUDD> ret;
    ret.reserve(bases.count);
    std::generate_n(std::back_inserter(ret), bases.count, [&]{
        auto dev = TestUDD{[&]() -> std::int8_t& { std::int8_t& curr = *reinterpret_cast<std::int8_t*>(bases.r8); bases.r8 += 1; return curr; }(), [&]() -> std::uint32_t& { std::uint32_t& curr = *reinterpret_cast<std::uint32_t*>(bases.r32); bases.r32 += 4; return curr; }()};
        dev.f2.assign(smce_rt::Impl{}, bases.a16); bases.a16 = static_cast<char*>(bases.a16) + smce_rt::A16_size;
dev.f3.assign(smce_rt::Impl{}, bases.mtx); bases.mtx = static_cast<char*>(bases.mtx) + smce_rt::Mtx_size;
dev.f4.assign(smce_rt::Impl{}, bases.mtx); bases.mtx = static_cast<char*>(bases.mtx) + smce_rt::Mtx_size;

        return dev;
    });
    return ret;
}

const smce::BoardDeviceSpecification& TestUDD::specification{
  "\"TestUDD\" \"1.0\" \"s8 f0\" \"u32 f1\" \"au16 f2\" \"mutex f3\" \"mutex f4\" ",
  "TestUDD",
  1,
  0,
  1,
  0,
  0,
  1,
  0,
  0,
  2,
};
