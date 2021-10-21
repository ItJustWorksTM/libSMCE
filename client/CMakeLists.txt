#
# client/CMakeLists.txt
# Written by Rylander and Mejborn, Team 1, DAT265.
#
#


cmake_minimum_required (VERSION 3.16)
project (client)

find_package (Threads REQUIRED)
find_package (SMCE REQUIRED)

set (SMCE_RES "${PROJECT_BINARY_DIR}/SMCE_Res")

add_executable (SMCE_client SMCE_Client.cpp)
target_link_libraries (SMCE_client PRIVATE SMCE::SMCE)
target_compile_definitions (SMCE_client PRIVATE "SMCE_RESOURCES_DIR=\"${SMCE_RES}\"")
file (MAKE_DIRECTORY "${SMCE_RES}")
execute_process (COMMAND "${CMAKE_COMMAND}" -E tar xf "${SMCE_RESOURCES_ARK}"
        WORKING_DIRECTORY "${SMCE_RES}")

add_custom_command (TARGET SMCE_client POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "$ENV{SMCE_ROOT}/bin/SMCE.dll" "${PROJECT_BINARY_DIR}/Debug"
) 