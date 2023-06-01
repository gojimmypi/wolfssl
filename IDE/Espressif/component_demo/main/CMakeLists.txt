
#
# wolfssl crypt test
#
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")

set(COMPONENT_SRCS "main.c")

idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS ""
                       REQUIRES wolfssl__wolfssl freertos)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
