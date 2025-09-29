# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
# For PlatformIO, see platformio.ini for embeds
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWOLFSSL_USER_SETTINGS")
set(requires "")
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs esp_event esp-tls esp_http_client protocol_examples_common nvs_flash)
endif()
idf_component_register(SRCS "esp_http_client_example.c"
                      INCLUDE_DIRS "."
                      REQUIRES ${requires}
                      EMBED_TXTFILES howsmyssl_com_root_cert.pem
                                     postman_root_cert.pem)
