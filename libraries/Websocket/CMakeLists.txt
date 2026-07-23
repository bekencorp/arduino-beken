set(srcs
    src/HttpClient.cpp
    src/WebSocketClient.cpp
    src/URLEncoder.cpp
    src/b64.cpp
    src/utility/URLParser/http_parser.c
)

armino_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS src
    REQUIRES arduino-beken-core WiFi WiFiClientSecure
    PRIV_REQUIRES bk_common bk_rtos
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 11)
