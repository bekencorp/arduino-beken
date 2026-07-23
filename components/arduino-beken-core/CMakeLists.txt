set(srcs
    src/ADC.cpp
    src/Arduino.cpp
    src/Common.cpp
    src/GeneratedSketchC.c
    src/GeneratedSketchCpp.cpp
    src/GeneratedSketch.cpp
    src/HardwareSerial.cpp
    src/HardwareTimer.cpp
    src/Print.cpp
    src/PWM.cpp
    src/SketchDefaults.cpp
    src/SPI.cpp
    src/String.cpp
    src/Stream.cpp
    src/StreamString.cpp
    src/Ticker.cpp
    src/Wire.cpp
    src/wiring_digital.cpp
    src/arduino_base64.cpp
    src/cencode.c
    src/WMath.cpp
    # Arduino API networking types used by WiFi (STA/AP IP helpers).
    include/api/IPAddress.cpp
)

if(NOT DEFINED ARDUINO_VARIANT_DIR OR "${ARDUINO_VARIANT_DIR}" STREQUAL "")
    include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/arduino_variant.cmake")
    arduino_beken_resolve_variant("${CMAKE_CURRENT_LIST_DIR}/../../variants" _arduino_target _arduino_variant ARDUINO_VARIANT_DIR)
endif()

set(incs
    include
    include/api
    ${CMAKE_CURRENT_LIST_DIR}/../../libraries/BLE/src
    ${CMAKE_CURRENT_LIST_DIR}/../../libraries/WiFi/src
    ${CMAKE_CURRENT_LIST_DIR}/../../libraries/WiFiClientSecure/src
    ${CMAKE_CURRENT_LIST_DIR}/../../libraries/HTTPClient/src
    ${CMAKE_CURRENT_LIST_DIR}/../../libraries/PubSubClient/src
    ${CMAKE_CURRENT_LIST_DIR}/../../libraries/DNSServer/src
    ${CMAKE_CURRENT_LIST_DIR}/../../libraries/Websocket/src
    ${ARDUINO_VARIANT_DIR}
)

armino_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "${incs}"
    REQUIRES bk_rtos psa_mbedtls
    PRIV_REQUIRES bk_common
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 11)
