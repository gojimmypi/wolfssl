# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")

include_directories(wolfssl MY_INCLUDE_PATH)

add_subdirectory(wolfssl)
set_target_properties(wolfssl PROPERTIES LINKER_LANGUAGE CXX)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
project ("wolfssl_cmd")

# Add source to this project's executable.
add_executable (wolfssl_cmd wolfssl "wolfssl_cmd.cpp" "wolfssl_cmd.h" )
target_include_directories(wolfssl PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/wolfssl"
                          )
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET wolfssl_cmd PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
if (1)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    message(STATUS "")
    message(STATUS "ALL VARIABLES BEGIN")
    message(STATUS "")
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "")
    message(STATUS "ALL VARIABLES END")
    message(STATUS "")
endif()