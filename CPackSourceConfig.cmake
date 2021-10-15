# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BUILD_SOURCE_DIRS "/home/ludvig/programs/Group-9-backend-libSMCE;/home/ludvig/programs/Group-9-backend-libSMCE")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_COMPONENTS_ALL "")
set(CPACK_COMPONENTS_ALL_SET_BY_USER "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "/snap/clion/164/bin/cmake/linux/share/cmake-3.20/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "libSMCE built using CMake")
set(CPACK_GENERATOR "TBZ2;TGZ;TXZ;TZ")
set(CPACK_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_INSTALLED_DIRECTORIES "/home/ludvig/programs/Group-9-backend-libSMCE;/")
set(CPACK_INSTALL_CMAKE_PROJECTS "")
set(CPACK_INSTALL_PREFIX "/usr/local")
set(CPACK_MODULE_PATH "/home/ludvig/programs/Group-9-backend-libSMCE/CMake/Modules;/home/ludvig/programs/Group-9-backend-libSMCE/_deps/mosquitto-src/cmake/")
set(CPACK_NSIS_DISPLAY_NAME "libSMCE 1.4.0.5")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_PACKAGE_NAME "libSMCE 1.4.0.5")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OUTPUT_CONFIG_FILE "/home/ludvig/programs/Group-9-backend-libSMCE/CPackConfig.cmake")
set(CPACK_PACKAGE_CONTACT "ItJustWorksTM <itjustworkstm@aerostun.dev>")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION "This cross-platform C++ library provides its consumers the ability
to compile and execute Arduino sketches on a hosted environment, with bindings
to its virtual I/O ports to allow the host application to interact with its
child sketches.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "/snap/clion/164/bin/cmake/linux/share/cmake-3.20/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library to run your Arduino code on a desktop OS")
set(CPACK_PACKAGE_FILE_NAME "libSMCE-1.4.0.5-Source")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/ItJustWorksTM/libSMCE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "libSMCE 1.4.0.5")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "libSMCE 1.4.0.5")
set(CPACK_PACKAGE_NAME "libSMCE")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "ItJustWorksTM")
set(CPACK_PACKAGE_VERSION "1.4.0.5")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "4")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RESOURCE_FILE_LICENSE "/snap/clion/164/bin/cmake/linux/share/cmake-3.20/Templates/CPack.GenericLicense.txt")
set(CPACK_RESOURCE_FILE_README "/snap/clion/164/bin/cmake/linux/share/cmake-3.20/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "/snap/clion/164/bin/cmake/linux/share/cmake-3.20/Templates/CPack.GenericWelcome.txt")
set(CPACK_RPM_PACKAGE_SOURCES "ON")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "TBZ2;TGZ;TXZ;TZ")
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "/home/ludvig/programs/Group-9-backend-libSMCE;/")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/home/ludvig/programs/Group-9-backend-libSMCE/CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "libSMCE-1.4.0.5-Source")
set(CPACK_SOURCE_RPM "OFF")
set(CPACK_SOURCE_TBZ2 "ON")
set(CPACK_SOURCE_TGZ "ON")
set(CPACK_SOURCE_TOPLEVEL_TAG "Linux-x86_64-GNU-Source")
set(CPACK_SOURCE_TXZ "ON")
set(CPACK_SOURCE_TZ "ON")
set(CPACK_SOURCE_ZIP "OFF")
set(CPACK_STRIP_FILES "")
set(CPACK_SYSTEM_NAME "Linux-x86_64-GNU")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "Linux-x86_64-GNU-Source")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "/home/ludvig/programs/Group-9-backend-libSMCE/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
