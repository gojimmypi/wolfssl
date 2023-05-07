cmake_minimum_required(VERSION 3.10)

project(wolfssl C)

# Set the path to the wolfssl source code
set(WOLFSSL_ROOT /workspace/wolfssl-gojimmypi/ )

# Set the include directories for the wolfssl library
# target_include_directories(wolfssl PUBLIC "${WOLFSSL_ROOT}/wolfssl")

# Add the wolfssl source code to the project
add_library(wolfssl STATIC
   "${WOLFSSL_ROOT}/src/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/port/Espressif/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/port/atmel/"
                        "${WOLFSSL_ROOT}/wolfcrypt/benchmark/"
                        "${WOLFSSL_ROOT}/wolfcrypt/test/"
 )

# Set the include directories for the wolfssl library
target_include_directories(wolfssl PUBLIC ${WOLFSSL_ROOT}/wolfssl)

message(STATUS "WOLFSSL_ROOT = ${WOLFSSL_ROOT}" )

# Set the language for the wolfssl target
set_target_properties(wolfssl PROPERTIES LINKER_LANGUAGE C)

# Set the compiler flags for the wolfssl library
target_compile_options(wolfssl PUBLIC -Wall)

# Set the linker flags for the wolfssl library
target_link_options(wolfssl PUBLIC -pthread)
