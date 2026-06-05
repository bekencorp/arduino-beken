cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{ARMINO_PATH} OR "$ENV{ARMINO_PATH}" STREQUAL "")
    message(FATAL_ERROR "ARMINO_PATH is not set. Point it to your bk_idk root before building arduino-beken.")
endif()

set(ARDUINO_VARIANTS_ROOT "${CMAKE_CURRENT_LIST_DIR}/variants")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/arduino_variant.cmake")

arduino_beken_resolve_variant("${ARDUINO_VARIANTS_ROOT}" arduino_target arduino_variant arduino_variant_dir)
set(ARDUINO_BEKEN_TARGET "${arduino_target}" CACHE STRING "Arduino Beken target SoC" FORCE)
set(ARDUINO_VARIANT "${arduino_variant}" CACHE STRING "Arduino Beken variant name" FORCE)
set(ARDUINO_VARIANT_DIR "${arduino_variant_dir}" CACHE PATH "Arduino Beken variant directory" FORCE)
message(STATUS "Arduino Beken target ${arduino_target} uses variant ${ARDUINO_VARIANT}")

set(EXTRA_COMPONENTS_DIRS
    "components/arduino-beken-core"
    "components/arduino-beken-main"
    "components/arduino-beken-linker"
    "libraries/BLE"
    "libraries/WiFi"
)

include($ENV{ARMINO_PATH}/tools/build_tools/cmake/project.cmake)
project(arduino_beken)
