#
#  BindGen.cmake
#  Copyright 2021 ItJustWorksTM
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

set (SMCE_BINDGEN_TYPES
    u8 u16 u32 u64
    s8 s16 s32 s64
    au8 au16 au32 au64
    as8 as16 as32 as64
    mutex
)
set (SMCE_BINDGEN_TYPES_MAPPING
    std::uint8_t std::uint16_t std::uint32_t std::uint64_t
    std::int8_t std::int16_t std::int32_t std::int64_t
    smce_rt::AtomicU8 smce_rt::AtomicU16 smce_rt::AtomicU32 smce_rt::AtomicU64
    smce_rt::AtomicS8 smce_rt::AtomicS16 smce_rt::AtomicS32 smce_rt::AtomicS64
    smce_rt::Mutex
)
set (SMCE_BINDGEN_STORAGE_MAPPING
    r8 r16 r32 r64
    r8 r16 r32 r64
    a8 a16 a32 a64
    a8 a16 a32 a64
    mtx
)
set (SMCE_BINDGEN_SIZE_MAPPING
    1 2 4 8
    1 2 4 8
    smce_rt::A8_size smce_rt::A16_size smce_rt::A32_size smce_rt::A64_size
    smce_rt::A8_size smce_rt::A16_size smce_rt::A32_size smce_rt::A64_size
    smce_rt::Mtx_size
)


macro (smce_bindgen_find_type OUTPUT_VARIABLE TYPE)
  list (FIND SMCE_BINDGEN_TYPES "${TYPE}" ${OUTPUT_VARIABLE})
endmacro ()

macro (smce_bindgen_configure_prelude)
  set (GENERATED_DEVICE "${NAME}")
  string (LENGTH "${NAME}" GENERATED_DEVICE_LENGTH)
  set (GENERATED_DEVICE_FIELDS "")
  set (GENERATED_DEVICE_FIELDS_ASSIGN "")
  set (GENERATED_DEVICE_CTOR_CALL "")
  set (GENERATED_DEVICE_CTOR_INIT ": ")
  set (GENERATED_DEVICE_CTOR_ARGS "")
  foreach (field ${FIELDS})
    if (NOT field MATCHES "^([A-Za-z_][A-Za-z0-9_]*) ([A-Za-z_][A-Za-z0-9_]*)$")
      message (FATAL_ERROR "Invalid field format \"${field}\"")
    endif ()
    smce_bindgen_find_type (find_type_res "${CMAKE_MATCH_1}")
    if (find_type_res EQUAL -1)
      message (FATAL_ERROR "Invalid field type \"${CMAKE_MATCH_1}\" for field \"${CMAKE_MATCH_2}\"")
    endif ()
    list (GET SMCE_BINDGEN_TYPES_MAPPING ${find_type_res} field_type)
    list (GET SMCE_BINDGEN_STORAGE_MAPPING ${find_type_res} field_storage)
    list (GET SMCE_BINDGEN_SIZE_MAPPING ${find_type_res} field_size)
    if (find_type_res LESS 8)
      string (APPEND GENERATED_DEVICE_FIELDS "    ${field_type}& ${CMAKE_MATCH_2};\n")
      string (APPEND GENERATED_DEVICE_CTOR_ARGS "${field_type}& ${CMAKE_MATCH_2}, ")
      string (APPEND GENERATED_DEVICE_CTOR_INIT "${CMAKE_MATCH_2}{${CMAKE_MATCH_2}}, ")
      string (APPEND GENERATED_DEVICE_CTOR_CALL "[&]() -> ${field_type}& { ${field_type}& curr = *reinterpret_cast<${field_type}*>(bases.${field_storage}); bases.${field_storage} += ${field_size}; return curr; }(), ")
    else ()
      string (APPEND GENERATED_DEVICE_FIELDS "    mutable ${field_type} ${CMAKE_MATCH_2};\n")
      string (APPEND GENERATED_DEVICE_FIELDS_ASSIGN "dev.${CMAKE_MATCH_2}.assign(smce_rt::Impl{}, bases.${field_storage}); ")
      string (APPEND GENERATED_DEVICE_FIELDS_ASSIGN "bases.${field_storage} = static_cast<char*>(bases.${field_storage}) + ${field_size};\n")
    endif ()
  endforeach ()
  string (REGEX REPLACE ", $" "" GENERATED_DEVICE_CTOR_ARGS "${GENERATED_DEVICE_CTOR_ARGS}")
  string (REGEX REPLACE ", $" "" GENERATED_DEVICE_CTOR_INIT "${GENERATED_DEVICE_CTOR_INIT}")
  string (REGEX REPLACE ", $" "" GENERATED_DEVICE_CTOR_CALL "${GENERATED_DEVICE_CTOR_CALL}")
  set (GENERATED_DEVICE_SPECSTRING)
  foreach (token "${NAME}" "${VERSION}" ${FIELDS})
    string (APPEND GENERATED_DEVICE_SPECSTRING "\\\"${token}\\\" ")
  endforeach ()
endmacro ()

function (smce_bindgen_host_header OUTPUT_DIRECTORY NAME VERSION)
  set (FIELDS ${ARGN})
  smce_bindgen_configure_prelude ()
  if (NOT EXISTS "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_HOST_HEADER.hpp.in")
    file (WRITE "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_HOST_HEADER.hpp.in"
[=======[// HSD generated
#ifndef LIBSMCE_GENERATED_@GENERATED_DEVICE@_HPP
#define LIBSMCE_GENERATED_@GENERATED_DEVICE@_HPP

#include <functional>
#include <vector>
#include <SMCE/fwd.hpp>
#include <SMCE/BoardView.hpp>
#include <SMCE_rt/SMCE_proxies.hpp>

struct @GENERATED_DEVICE@ /* v@VERSION@ */ {

@GENERATED_DEVICE_FIELDS@

    static std::vector<@GENERATED_DEVICE@> getObjects(smce::BoardView&);

    static const smce::BoardDeviceSpecification& specification;
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

#include <functional>
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
  foreach (type IN LISTS SMCE_BINDGEN_STORAGE_MAPPING)
    set (GENERATED_DEVICE_${type}_COUNT 0)
  endforeach ()
  foreach (field IN LISTS FIELDS)
    string (REGEX MATCH "^([A-Za-z_][A-Za-z0-9_]*) ([A-Za-z_][A-Za-z0-9_]*)$" ignore "${field}")
    smce_bindgen_find_type (find_type_res "${CMAKE_MATCH_1}")
    list (GET SMCE_BINDGEN_STORAGE_MAPPING ${find_type_res} field_storage)
    math (EXPR GENERATED_DEVICE_${field_storage}_COUNT "${GENERATED_DEVICE_${field_storage}_COUNT}+1")
  endforeach ()
  file (WRITE "${PROJECT_BINARY_DIR}/SMCE_BINDGEN_HOST_IMPL.cpp.in"
[=======[// HSD generated

#include <algorithm>
#include <iterator>
#include <SMCE/internal/BoardDeviceSpecification.hpp>
#include <SMCE_rt/internal/host_rt.hpp>
#include "@GENERATED_DEVICE@.hpp"

namespace smce_rt {
struct Impl {};
}

@GENERATED_DEVICE@::@GENERATED_DEVICE@(@GENERATED_DEVICE_CTOR_ARGS@) @GENERATED_DEVICE_CTOR_INIT@ {}

std::vector<@GENERATED_DEVICE@> @GENERATED_DEVICE@::getObjects(smce::BoardView& bv) {
    auto bases = smce_rt::getBases(bv, "@GENERATED_DEVICE@");
    std::vector<@GENERATED_DEVICE@> ret;
    ret.reserve(bases.count);
    std::generate_n(std::back_inserter(ret), bases.count, [&]{
        auto dev = @GENERATED_DEVICE@{@GENERATED_DEVICE_CTOR_CALL@};
        @GENERATED_DEVICE_FIELDS_ASSIGN@
        return dev;
    });
    return ret;
}

const smce::BoardDeviceSpecification& @GENERATED_DEVICE@::specification{
  "@GENERATED_DEVICE_SPECSTRING@",
  "@GENERATED_DEVICE@",
  @GENERATED_DEVICE_r8_COUNT@,
  @GENERATED_DEVICE_r16_COUNT@,
  @GENERATED_DEVICE_r32_COUNT@,
  @GENERATED_DEVICE_r64_COUNT@,
  @GENERATED_DEVICE_a8_COUNT@,
  @GENERATED_DEVICE_a16_COUNT@,
  @GENERATED_DEVICE_a32_COUNT@,
  @GENERATED_DEVICE_a64_COUNT@,
  @GENERATED_DEVICE_mtx_COUNT@,
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
    auto bases = smce_rt::getBases("@GENERATED_DEVICE@", @GENERATED_DEVICE_LENGTH@);
    std::vector<@GENERATED_DEVICE@> ret;
    ret.reserve(bases.count);
    std::generate_n(std::back_inserter(ret), bases.count, [&]{
        auto dev = @GENERATED_DEVICE@{@GENERATED_DEVICE_CTOR_CALL@};
        @GENERATED_DEVICE_FIELDS_ASSIGN@
        return std::move(dev);
    });

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
