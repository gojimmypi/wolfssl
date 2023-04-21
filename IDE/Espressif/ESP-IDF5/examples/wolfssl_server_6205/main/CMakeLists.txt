set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")

set(COMPONENT_REQUIRES lwip)

idf_component_register(
                    SRCS main.c
                    INCLUDE_DIRS "."
                    REQUIRES wolfssl esp_wifi nvs_flash)

