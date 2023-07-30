# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# This tag is used to include this file in the ESP Component Registry:
# __ESP_COMPONENT_SOURCE__

# (Not part of the boilerplate)
# This example uses an extra component for common functions such as Wi-Fi and Ethernet connection.
# disable the following line if there isn't the directory
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

# set(WOLFSSL_ROOT "/workspace/wolfssl-gojimmypi-PR" )

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wolfssl_client_6637)
