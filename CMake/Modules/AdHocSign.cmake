
set (OSX_NEEDS_SIGN False)
if (APPLE AND NOT CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
  string (FIND "${CMAKE_OSX_ARCHITECTURES}" "arm64" osx_target_arm64)
  if (osx_target_arm64 GREATER_EQUAL 0)
    message (STATUS "Targeting Apple Silicon - ad-hoc signatures required")
    set (OSX_NEEDS_SIGN True)
    find_program (CODESIGN_EXECUTABLE codesign REQUIRED)
  endif()
endif ()

macro (ad_hoc_sign target)
  if (OSX_NEEDS_SIGN)
    add_custom_command (TARGET "${target}" POST_BUILD
        COMMAND "${CODESIGN_EXECUTABLE}" --force --deep -s - --digest-algorithm=sha1,sha256 "$<TARGET_FILE:${target}>"
        COMMENT "Stamping ad-hoc signature on target ${target}"
    )
  endif ()
endmacro ()
