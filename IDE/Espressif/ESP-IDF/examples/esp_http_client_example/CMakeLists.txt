# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWOLFSSL_USER_SETTINGS")

# Different ESP-IDF versions have different lock files.
# Not starting fresh has unintuitive results.
# So force a fresh dependencies.lock by deleting the current one:
file(REMOVE "./dependencies.lock")

# Force custom partition table regardless of IDE presets
set(PARTITION_TABLE_CUSTOM ON CACHE BOOL "" FORCE)
set(PARTITION_TABLE_FILENAME "${CMAKE_SOURCE_DIR}/partitions_singleapp_large.csv" CACHE STRING "" FORCE)

if("${IDF_TARGET}" STREQUAL "linux")
    set(COMPONENTS main)
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(esp_http_client_example)
