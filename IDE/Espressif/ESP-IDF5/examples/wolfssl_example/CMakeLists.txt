cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")
project(wolfssl_example C)

# Add the wolfssl library as a subdirectory
add_subdirectory(/workspace/wolfssl-gojimmypi wolfssl)

# Create the main executable
add_executable(main main.c)

# Link against the wolfssl library
target_link_libraries(main wolfssl)

# Set the include directory for wolfssl headers
target_include_directories(main PUBLIC /workspace/wolfssl-gojimmypi/wolfssl /workspace/wolfssl-gojimmypi/wolfssl/include)

# Set compiler flags for the main executable
target_compile_options(main PUBLIC -Wall)

# Set linker flags for the main executable
# target_link_options(main PUBLIC -pthread)

# Add the system's standard include directories
include_directories(${CMAKE_SYSROOT}/usr/include)