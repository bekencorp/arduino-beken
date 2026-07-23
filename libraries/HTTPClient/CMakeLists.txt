set(srcs
    src/HTTPClient.cpp
)

armino_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS src
    REQUIRES arduino-beken-core WiFi WiFiClientSecure
    PRIV_REQUIRES bk_common bk_rtos
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 11)
