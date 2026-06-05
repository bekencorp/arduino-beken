set(srcs
    src/WiFi.cpp
    src/WiFiClient.cpp
    src/WiFiServer.cpp
    src/WiFiUdp.cpp
)

if(NOT DEFINED ARDUINO_VARIANT_DIR OR "${ARDUINO_VARIANT_DIR}" STREQUAL "")
    include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/arduino_variant.cmake")
    arduino_beken_resolve_variant("${CMAKE_CURRENT_LIST_DIR}/../../variants" _arduino_target _arduino_variant ARDUINO_VARIANT_DIR)
endif()

armino_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS src
    REQUIRES arduino-beken-core bk_wifi bk_event
    PRIV_REQUIRES bk_common bk_rtos
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 11)
