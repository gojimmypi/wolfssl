cmake_minimum_required(VERSION 3.10)

project(wolfssl_cmd C)
# Set the path to the wolfssl source code
set(WOLFSSL_ROOT /workspace/wolfssl-gojimmypi/ )

# Add the wolfssl project as a subdirectory
# add_subdirectory(wolfssl)
target_include_directories(wolfssl PUBLIC ${WOLFSSL_ROOT}/wolfssl)

# Create the wolfssl_cmd executable
add_executable(wolfssl_cmd main.c)

# Link against the wolfssl library
target_link_libraries(wolfssl_cmd wolfssl)

# Set the include directory for wolfssl headers
target_include_directories(wolfssl_cmd PUBLIC "wolfssl" "wolfssl/include"  )

# Set the include directory for the main.c file
target_include_directories(wolfssl_cmd PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    /workspace/wolfssl-gojimmypi/wolfssl # Add this line
    ${WOLFSSL_ROOT}/wolfssl
)

# Set the compiler flags for the wolfssl_cmd executable
target_compile_options(wolfssl_cmd PUBLIC -Wall)

# Set the linker flags for the wolfssl_cmd executable
target_link_options(wolfssl_cmd PUBLIC -pthread)
