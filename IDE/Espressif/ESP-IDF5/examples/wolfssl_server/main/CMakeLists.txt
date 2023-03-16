set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")

idf_component_register(
                    SRCS "station_example_main.c" server-tls.c
                    INCLUDE_DIRS "."
                    REQUIRES wolfssl esp_wifi nvs_flash)
