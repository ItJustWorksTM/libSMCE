#
# client/CMakeLists.txt
# Written by Rylander and Mejborn, Team 1, DAT265.
#
#


cmake_minimum_required (VERSION 3.16)
project (SMCE_client)

find_package (Threads REQUIRED)
find_package (SMCE REQUIRED)

set (SMCE_RES "${PROJECT_BINARY_DIR}/SMCE_Res")

include (FetchContent)
FetchContent_Declare (Termcolor
        GIT_REPOSITORY "https://github.com/ikalnytskyi/termcolor"
        GIT_TAG "v2.0.0"
        GIT_SHALLOW On
        GIT_PROGRESS On
        )
FetchContent_Declare (Lyra
        GIT_REPOSITORY "https://github.com/bfgroup/Lyra"
        GIT_TAG "1.5.1"
        GIT_SHALLOW On
        GIT_PROGRESS On
        )
FetchContent_MakeAvailable(Termcolor Lyra)

include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable (${PROJECT_NAME} SMCE_Client.cpp)
target_sources (${PROJECT_NAME} PRIVATE 
        src/UartToFile.hpp
        src/UartToFile.cpp
        )
target_link_libraries (${PROJECT_NAME} PRIVATE SMCE::SMCE termcolor::termcolor bfg::Lyra)
target_include_directories (${PROJECT_NAME} PUBLIC
        "${termcolor_SOURCE_DIR}/include/termcolor"
        "${lyra_SOURCE_DIR}/include/lyra"
        )
target_compile_definitions (${PROJECT_NAME} PRIVATE "SMCE_RESOURCES_DIR=\"${SMCE_RES}\"")
file (MAKE_DIRECTORY "${SMCE_RES}")
execute_process (COMMAND "${CMAKE_COMMAND}" -E tar xf "${SMCE_RESOURCES_ARK}"
        WORKING_DIRECTORY "${SMCE_RES}")

add_custom_command (TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SMCE::SMCE>" "$<TARGET_FILE_DIR:SMCE_client>"
        )