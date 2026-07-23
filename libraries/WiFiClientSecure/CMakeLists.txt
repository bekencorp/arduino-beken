set(srcs
    src/WiFiClientSecure.cpp
    src/ssl_client.cpp
    src/crt_bundle.c
)

armino_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS src
    REQUIRES arduino-beken-core WiFi psa_mbedtls lwip_intf_v2_1
    PRIV_REQUIRES bk_common bk_rtos
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 11)
