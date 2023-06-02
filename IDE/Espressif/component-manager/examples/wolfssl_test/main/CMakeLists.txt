
#
# wolfssl ESP Registry Component test
#

# it is important for all wolfssl files to "see" the WOLFSSL_USER_SETTINGS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")

set(COMPONENT_SRCS "main.c")


# Note the build-system name of a dependency installed by the component manager.
# It's always `namespace__component`. The wolfSSL name space is `wolfssl`.
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS ""
                       REQUIRES wolfssl__wolfssl freertos)

# Optionally disable warning messages related to format string checks during compilation
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
