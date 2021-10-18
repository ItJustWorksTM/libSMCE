  # HSD generated
  math (EXPR last_arg_idx "${CMAKE_ARGC} - 1")
  set (start_idx -1)
  foreach (i RANGE ${last_arg_idx})
    if ("${CMAKE_ARGV${i}}" STREQUAL "-P")
      set (start_idx ${i})
      break ()
    endif ()
  endforeach ()
  math (EXPR archive_path_idx "${start_idx} + 2")
  set (archive_path "${CMAKE_ARGV${archive_path_idx}}")
  math (EXPR output_dir_idx "${start_idx} + 3")
  set (output_dir "${CMAKE_ARGV${output_dir_idx}}")

  file (TIMESTAMP "${CMAKE_ARGV3}" curr_filetime)
  if (EXISTS "${CMAKE_BINARY_DIR}/ark-filetime.txt")
    file (READ "${CMAKE_BINARY_DIR}/ark-filetime.txt" prev_filetime)
    if (curr_filetime STREQUAL prev_filetime)
      return ()
    endif ()
  endif ()

  message (STATUS "Inflating resources for testsuite execution")
  execute_process (COMMAND "${CMAKE_COMMAND}" -E tar xf "${archive_path}"
    WORKING_DIRECTORY "${output_dir}"
    RESULT_VARIABLE tar_exit_code
  )
  if (tar_exit_code)
    message ("Where archive_path: ${archive_path}")
    message ("Where output_dir: ${output_dir}")
    message (FATAL_ERROR "Resource inflation failed: ${tar_exit_code}")
  endif ()
  file (WRITE "${CMAKE_BINARY_DIR}/ark-filetime.txt" "${curr_filetime}")
