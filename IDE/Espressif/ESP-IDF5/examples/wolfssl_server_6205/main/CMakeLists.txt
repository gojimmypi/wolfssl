set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")

idf_component_register(
                    SRCS "station_example_main.c"
                    INCLUDE_DIRS "."
                    REQUIRES wolfssl esp_wifi nvs_flash)
set_source_files_properties(server-tls.c PROPERTIES HEADER_FILE_ONLY TRUE)
