cmake_minimum_required (VERSION 3.8)
add_library(wolfssl SHARED "/workspace/wolfssl-gojimmypi/wolfssl/include/")

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
# We are currently in [root]/IDE/Espressif/ESP-IDF/examples/wolfssl_test/components/wolfssl
# The root of wolfSSL is 7 directories up from here:
get_filename_component(WOLFSSL_ROOT  "../../../../../../" ABSOLUTE)
message(STATUS "WOLFSSL_ROOT = ${WOLFSSL_ROOT}")

include_directories("/workspace/wolfssl-gojimmypi/wolfssl")

# get a list of all wolfcrypt assembly files; we'll exclude them as they don't target Xtensa
file(GLOB EXCLUDE_ASM *.S)
file(GLOB_RECURSE EXCLUDE_ASM ${CMAKE_SOURCE_DIR} "${WOLFSSL_ROOT}/wolfcrypt/src/*.S")

set(MY_INCLUDE_PATH ${WOLFSSL_ROOT})

set(COMPONENT_SRCDIRS "${WOLFSSL_ROOT}/src/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/port/Espressif/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/port/atmel/"
                        "${WOLFSSL_ROOT}/wolfcrypt/benchmark/"
                        "${WOLFSSL_ROOT}/wolfcrypt/test/"
                        )


target_include_directories(wolfssl PUBLIC "/workspace/wolfssl-gojimmypi/wolfssl/include")
include_directories( MY_INCLUDE_PATH)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET wolfssl PROPERTY CXX_STANDARD 20)
endif()
