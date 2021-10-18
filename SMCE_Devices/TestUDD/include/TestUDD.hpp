// HSD generated
#ifndef LIBSMCE_GENERATED_TestUDD_HPP
#define LIBSMCE_GENERATED_TestUDD_HPP

#include <functional>
#include <vector>
#include <SMCE/fwd.hpp>
#include <SMCE/BoardView.hpp>
#include <SMCE_rt/SMCE_proxies.hpp>

struct TestUDD /* v1.0 */ {

    std::int8_t& f0;
    std::uint32_t& f1;
    mutable smce_rt::AtomicU16 f2;
    mutable smce_rt::Mutex f3;
    mutable smce_rt::Mutex f4;


    static std::vector<TestUDD> getObjects(smce::BoardView&);

    static const smce::BoardDeviceSpecification& specification;
  private:
    TestUDD(std::int8_t& f0, std::uint32_t& f1);
};

#endif
