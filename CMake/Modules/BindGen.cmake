#
#  BindGen.cmake
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

include_guard()

set(SMCE_BINDGEN_TYPES
    u8 u16 u32 u64
    s8 s16 s32 s64
    f32 f64
    au8 au16 au32 au64
    as8 as16 as32 as64
    af32 af64
    mutex
    )
set(SMCE_BINDGEN_TYPES_MAPPING
    std::uint8_t& std::uint16_t& std::uint32_t& std::uint64_t&
    std::int8_t& std::int16_t& std::int32_t& std::int64_t&
    float& double&
    smce_rt::AtomicU8 smce_rt::AtomicU16 smce_rt::AtomicU32 smce_rt::AtomicU64
    smce_rt::AtomicS8 smce_rt::AtomicS16 smce_rt::AtomicS32 smce_rt::AtomicS64
    smce_rt::AtomicF32 smce_rt::AtomicF64
    smce_rt::Mutex
    )


macro(smce_bindgen_find_type OUTPUT_VARIABLE TYPE)
  list(FIND SMCE_BINDGEN_TYPES "${TYPE}" ${OUTPUT_VARIABLE})
endmacro()

macro(smce_bindgen_configure_prelude)
  set(GENERATED_DEVICE "${NAME}")
  string(LENGTH "${NAME}" GENERATED_DEVICE_LENGTH)
  set(GENERATED_DEVICE_FIELDS "")
  set(GENERATED_DEVICE_CTOR_INIT "")
  set(GENERATED_DEVICE_CTOR_ARGS "")
  set(GENERATED_DEVICE_CTOR_CALL_HOST "")
  set(GENERATED_DEVICE_CTOR_CALL_SKETCH "")
  foreach (field ${FIELDS})
    if (NOT field MATCHES "^([A-Za-z_][A-Za-z0-9_]*) ([A-Za-z_][A-Za-z0-9_]*)$")
      message(FATAL_ERROR "Invalid field format \"${field}\"")
    endif ()
    smce_bindgen_find_type(find_type_res "${CMAKE_MATCH_1}")
    if (find_type_res EQUAL -1)
      message(FATAL_ERROR "Invalid field type \"${CMAKE_MATCH_1}\" for field \"${CMAKE_MATCH_2}\"")
    endif ()
    string(LENGTH field_name_length "${CMAKE_MATCH_2}")
    if (field_name_length GREATER 32)
      message(FATAL_ERROR "Field name \"${CMAKE_MATCH_2}\" exceeding 32 characters length limit (is ${field_name_length} bytes long)")
    endif ()
    list(GET SMCE_BINDGEN_TYPES_MAPPING ${find_type_res} field_type)

    if (find_type_res LESS 8)
      string(APPEND GENERATED_DEVICE_FIELDS "    ${field_type} ${CMAKE_MATCH_2};\n")
      string(APPEND GENERATED_DEVICE_CTOR_INIT "${CMAKE_MATCH_2}{${CMAKE_MATCH_2}}, ")
    else ()
      string(APPEND GENERATED_DEVICE_CTOR_INIT "${CMAKE_MATCH_2}{std::move(${CMAKE_MATCH_2})}, ")
      string(APPEND GENERATED_DEVICE_FIELDS "    mutable ${field_type} ${CMAKE_MATCH_2};\n")
    endif ()
    string(APPEND GENERATED_DEVICE_CTOR_ARGS "${field_type} ${CMAKE_MATCH_2}, ")
    string(APPEND GENERATED_DEVICE_CTOR_CALL_HOST "dev[\"${CMAKE_MATCH_2}\"].as_${CMAKE_MATCH_1}(), ")
    string(APPEND GENERATED_DEVICE_CTOR_CALL_SKETCH "smce_rt::device_field_${CMAKE_MATCH_1}(\"${GENERATED_DEVICE}\", n, \"${CMAKE_MATCH_2}\"), ")
  endforeach ()
  string(REGEX REPLACE ", $" "" GENERATED_DEVICE_CTOR_ARGS "${GENERATED_DEVICE_CTOR_ARGS}")
  string(REGEX REPLACE ", $" "" GENERATED_DEVICE_CTOR_INIT "${GENERATED_DEVICE_CTOR_INIT}")
  string(REGEX REPLACE ", $" "" GENERATED_DEVICE_CTOR_CALL_HOST "${GENERATED_DEVICE_CTOR_CALL_HOST}")
  string(REGEX REPLACE ", $" "" GENERATED_DEVICE_CTOR_CALL_SKETCH "${GENERATED_DEVICE_CTOR_CALL_SKETCH}")
  if (NOT GENERATED_DEVICE_CTOR_INIT STREQUAL "")
    string(PREPEND GENERATED_DEVICE_CTOR_INIT ": ")
  endif ()
  set(GENERATED_DEVICE_SPECSTRING " ")
  foreach (token "${NAME}" "${VERSION}" ${FIELDS})
    string(APPEND GENERATED_DEVICE_SPECSTRING "\\\"${token}\\\":")
  endforeach ()
  string(REGEX REPLACE ":$" "" GENERATED_DEVICE_SPECSTRING "${GENERATED_DEVICE_SPECSTRING}")
endmacro ()

function (smce_bindgen_host_header OUTPUT_DIRECTORY NAME VERSION)
  set (FIELDS ${ARGN})
  smce_bindgen_configure_prelude ()
  if (NOT EXISTS "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_HOST_HEADER.hpp.in")
    file (WRITE "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_HOST_HEADER.hpp.in"
[=======[// HSD generated
#ifndef LIBSMCE_GENERATED_@GENERATED_DEVICE@_HPP
#define LIBSMCE_GENERATED_@GENERATED_DEVICE@_HPP

#include <vector>
#include <SMCE/fwd.hpp>
#include <SMCE/BoardDeviceSpecification.hpp>
#include <SMCE/BoardView.hpp>
#include <SMCE_rt/SMCE_proxies.hpp>

struct @GENERATED_DEVICE@ /* v@VERSION@ */ {

@GENERATED_DEVICE_FIELDS@

    [[nodiscard]] static std::vector<@GENERATED_DEVICE@> getObjects(smce::BoardView&);

    static const smce::BoardDeviceNativeSpecification& specification;
  private:
    @GENERATED_DEVICE@(@GENERATED_DEVICE_CTOR_ARGS@);
};

#endif
]=======]
    )
  endif ()
  configure_file ("${PROJECT_BINARY_DIR}/SMCE_BINDGEN_HOST_HEADER.hpp.in" "${OUTPUT_DIRECTORY}/${NAME}.hpp" @ONLY)
endfunction ()

function (smce_bindgen_sketch_header OUTPUT_DIRECTORY NAME VERSION)
  set (FIELDS ${ARGN})
  smce_bindgen_configure_prelude ()
  if (NOT EXISTS "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_SKETCH_HEADER.hpp.in")
    file (WRITE "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_SKETCH_HEADER.hpp.in"
[=======[// HSD generated
#ifndef LIBSMCE_GENERATED_@GENERATEDDEVICE@_HPP
#define LIBSMCE_GENERATED_@GENERATEDDEVICE@_HPP

#include <vector>
#include <SMCE_rt/SMCE_proxies.hpp>

struct @GENERATED_DEVICE@ /* v@VERSION@ */ {

@GENERATED_DEVICE_FIELDS@

    static const std::vector<@GENERATED_DEVICE@> objects;
  private:
    @GENERATED_DEVICE@(@GENERATED_DEVICE_CTOR_ARGS@);
    static std::vector<@GENERATED_DEVICE@> init();
};

#endif
]=======]
    )
  endif ()
  configure_file ("${PROJECT_BINARY_DIR}/SMCE_BINDGEN_SKETCH_HEADER.hpp.in" "${OUTPUT_DIRECTORY}/${NAME}.hpp" @ONLY)
endfunction ()

function (smce_bindgen_host_impl OUTPUT_DIRECTORY NAME VERSION)
  set (FIELDS ${ARGN})
  smce_bindgen_configure_prelude ()
  file (WRITE "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_HOST_IMPL.cpp.in"
[=======[// HSD generated

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <vector>
#include <SMCE/BoardDeviceSpecification.hpp>
#include <SMCE/BoardDeviceView.hpp>
#include "@GENERATED_DEVICE@.hpp"

@GENERATED_DEVICE@::@GENERATED_DEVICE@(@GENERATED_DEVICE_CTOR_ARGS@) @GENERATED_DEVICE_CTOR_INIT@ {}

std::vector<@GENERATED_DEVICE@> @GENERATED_DEVICE@::getObjects(smce::BoardView& bv) {
    auto devs = smce::BoardDeviceView{bv}["@GENERATED_DEVICE@"];
    std::vector<@GENERATED_DEVICE@> ret;
    ret.reserve(devs.size());
    std::size_t n = 0;
    std::generate_n(std::back_inserter(ret), devs.size(), [&]{
        auto dev = devs[n++];
        return @GENERATED_DEVICE@{@GENERATED_DEVICE_CTOR_CALL_HOST@};
    });
    return ret;
}

const smce::BoardDeviceNativeSpecification& @GENERATED_DEVICE@::specification = smce::BoardDeviceNativeSpecification{
  "@GENERATED_DEVICE_SPECSTRING@"
};
]=======]
  )
  configure_file ("${PROJECT_BINARY_DIR}/SMCE_BINDGEN_HOST_IMPL.cpp.in" "${OUTPUT_DIRECTORY}/${NAME}.cpp" @ONLY)
endfunction ()

function (smce_bindgen_sketch_impl OUTPUT_DIRECTORY NAME VERSION)
  set (FIELDS ${ARGN})
  smce_bindgen_configure_prelude ()
  file (WRITE "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_SKETCH_IMPL.cpp.in"
[=======[// HSD generated

#include <algorithm>
#include <iterator>
#include <utility>
#include <SMCE_rt/internal/sketch_rt.hpp>
#include "@GENERATED_DEVICE@.hpp"

namespace smce_rt {
struct Impl {};
}

const std::vector<@GENERATED_DEVICE@> @GENERATED_DEVICE@::objects = @GENERATED_DEVICE@::init();

@GENERATED_DEVICE@::@GENERATED_DEVICE@(@GENERATED_DEVICE_CTOR_ARGS@) @GENERATED_DEVICE_CTOR_INIT@ {}

std::vector<@GENERATED_DEVICE@> @GENERATED_DEVICE@::init() {
    smce_rt::devices_init();
    const std::size_t dev_count = smce_rt::device_count("@GENERATED_DEVICE@");
    std::vector<@GENERATED_DEVICE@> ret;
    ret.reserve(dev_count);

    for (std::size_t n = 0; n < dev_count; ++n)
        ret.push_back(@GENERATED_DEVICE@{@GENERATED_DEVICE_CTOR_CALL_SKETCH@});

    return ret;
}
]=======]
  )
  configure_file ("${PROJECT_BINARY_DIR}/SMCE_BINDGEN_SKETCH_IMPL.cpp.in" "${OUTPUT_DIRECTORY}/${NAME}.cpp" @ONLY)
endfunction ()

#[================================================================================================================[.rst:
BindGen
--------------------

Function to define a host-side SMCE board device

Usage:

.. code-block:: cmake

  smce_bindgen_host(<target> <device_name> <device_version> <field>...)

Where ``<target>`` is the object target to be created, ``<device_name>`` is the name of the device
(which must be a valid C++ class name), ``<device_version>`` is the version of the device specification.

A ``<field>`` is a string of the form ``<type> <name>``, where ``name`` is a valid C++ class member name,
and ``<type>`` is one of ``u8``, ``u16``, ``u32``, ``u64``, ``s8``, ``s16``, ``s32``, ``s64,
``au8``, ``au16``, ``au32``, ``au64``, ``as8``, ``as16``, ``as32``, ``as64``, and ``mutex``.
``u`` and ``s`` respectively mean "unsigned" and "signed", ``a`` means "atomic", and the integral suffix
is the exact number of bits used to store an object of that type.

``<target>`` needs to be linked to the SMCE target you use (eg. ``SMCE::SMCE`` or ``SMCE::SMCE_static``)

The generated header has the name ``<device_name>.hpp`` and can be included by any target linked to ``<target>``.

Note:

Field order matters thus changing it should entail bumping the specification version.

#]================================================================================================================]

function (smce_bindgen_host TARGET NAME VERSION)
  list (FIND SMCE_ACTIVE_DEVICES "${NAME}" device_index)
  if (NOT device_index EQUAL -1)
    message (FATAL_ERROR "Redefinition of device \"${NAME}\"")
  endif ()
  set (SMCE_ACTIVE_DEVICES ${SMCE_ACTIVE_DEVICES} ${NAME} PARENT_SCOPE)

  set (output_dir "${PROJECT_BINARY_DIR}/SMCE_Devices/${NAME}")
  file (MAKE_DIRECTORY "${output_dir}")
  file (MAKE_DIRECTORY "${output_dir}/src")
  file (MAKE_DIRECTORY "${output_dir}/include")

  smce_bindgen_host_header ("${output_dir}/include" "${NAME}" "${VERSION}" ${ARGN})
  smce_bindgen_host_impl ("${output_dir}/src" "${NAME}" "${VERSION}" ${ARGN})

  add_library ("${TARGET}" OBJECT "${output_dir}/include/${NAME}.hpp" "${output_dir}/src/${NAME}.cpp")
  target_compile_features ("${TARGET}" PUBLIC cxx_std_17)
  target_include_directories ("${TARGET}" PUBLIC "${output_dir}/include")
endfunction ()

function (smce_bindgen_sketch NAME VERSION)
  list (FIND SMCE_ACTIVE_DEVICES "${NAME}" device_index)
  if (NOT device_index EQUAL -1)
    message (FATAL_ERROR "Redefinition of device \"${NAME}\"")
  endif ()
  set (SMCE_ACTIVE_DEVICES ${SMCE_ACTIVE_DEVICES} ${NAME} PARENT_SCOPE)

  set (output_dir "${PROJECT_BINARY_DIR}/SMCE_Devices/")
  file (MAKE_DIRECTORY "${output_dir}")
  file (MAKE_DIRECTORY "${output_dir}/src")
  file (MAKE_DIRECTORY "${output_dir}/include")

  smce_bindgen_sketch_header ("${output_dir}/include" ${NAME} ${VERSION} ${ARGN})
  smce_bindgen_sketch_impl ("${output_dir}/src" ${NAME} ${VERSION} ${ARGN})

  add_library (smce_device_${NAME} OBJECT "${output_dir}/include/${NAME}.hpp" "${output_dir}/src/${NAME}.cpp")
  target_include_directories (smce_device_${NAME} SYSTEM PRIVATE "${SMCE_DIR}/RtResources/Ardrivo/include")
  target_include_directories (smce_device_${NAME} PUBLIC "${output_dir}/include")
  target_link_libraries (smce_device_${NAME} PUBLIC Ardrivo)
  target_link_libraries (Sketch PUBLIC smce_device_${NAME})
endfunction ()
