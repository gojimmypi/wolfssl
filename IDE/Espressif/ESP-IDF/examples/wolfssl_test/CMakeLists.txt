# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(COMPONENTS
  main
  wolfssl
) # set components

project(wolfssl_test)

message(STATUS "")
message(STATUS "IDF_PATH             =  ${IDF_PATH}")
message(STATUS "PROJECT_SOURCE_DIR   =  ${PROJECT_SOURCE_DIR}")
message(STATUS "")

if (0)
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


string(TIMESTAMP "WOLFSSL_USER_SETTINGS_TIMESTAMP" "%Y_%m_%d_%H_%M")

# The source file is in the local project /libs/ directory and is named user_settings_[idf target].h
set(WOLFSSL_USER_SETTINGS_SOURCE "${PROJECT_SOURCE_DIR}/libs/user_settings_${CONFIG_IDF_TARGET}.h")

# the destination is in the ESP-IDF components directory
set(WOLFSSL_USER_SETTINGS_TARGET "${IDF_PATH}/components/wolfssl/include/user_settings.h")

# prepare a backup filename
set(WOLFSSL_USER_SETTINGS_BACKUP_IDF_PATH  "${IDF_PATH}/components/wolfssl/include/user_settings.h.${WOLFSSL_USER_SETTINGS_TIMESTAMP}.bak")

set(WOLFSSL_USER_SETTINGS_BACKUP_PROJECT   "${PROJECT_SOURCE_DIR}/libs/user_settings.h.${WOLFSSL_USER_SETTINGS_TIMESTAMP}.bak")

message(STATUS "")
message(STATUS "WOLFSSL_USER_SETTINGS_TIMESTAMP = ${WOLFSSL_USER_SETTINGS_TIMESTAMP}" )
message(STATUS "WOLFSSL_USER_SETTINGS_SOURCE    = ${WOLFSSL_USER_SETTINGS_SOURCE}"    )
message(STATUS "WOLFSSL_USER_SETTINGS_TARGET    = ${WOLFSSL_USER_SETTINGS_TARGET}"    )
message(STATUS "WOLFSSL_USER_SETTINGS_BACKUP_IDF_PATH = ${WOLFSSL_USER_SETTINGS_BACKUP_IDF_PATH}" )
message(STATUS "WOLFSSL_USER_SETTINGS_BACKUP_PROJECT  = ${WOLFSSL_USER_SETTINGS_BACKUP_PROJECT}"  )
message(STATUS "")

# first, we'll copy wolfssl user_settings
if (EXISTS "${IDF_PATH}/components/wolfssl/")
    execute_process( COMMAND ${CMAKE_COMMAND} -E compare_files ${WOLFSSL_USER_SETTINGS_SOURCE} ${WOLFSSL_USER_SETTINGS_TARGET}
                     RESULT_VARIABLE wolfssl_user_settings_compare
                    )

    if( wolfssl_user_settings_compare EQUAL 0)
        message("The components/wolfssl/include/user_settings.h and libs/user_settings_${CONFIG_IDF_TARGET}.h file are identical. Copy skipped")
    elseif( wolfssl_user_settings_compare EQUAL 1)
        message("The files are different. Saving backups...")
        configure_file("${WOLFSSL_USER_SETTINGS_TARGET}" "${WOLFSSL_USER_SETTINGS_BACKUP_IDF_PATH}" COPYONLY)
        configure_file("${WOLFSSL_USER_SETTINGS_TARGET}" "${WOLFSSL_USER_SETTINGS_BACKUP_PROJECT}"  COPYONLY)

        if(EXISTS "${WOLFSSL_USER_SETTINGS_SOURCE}" )
            # found the desired file
        else()
            message(STATUS "WARNING: Could not find ${WOLFSSL_USER_SETTINGS_SOURCE}, using /libs/user_settings.h" )
            set(WOLFSSL_USER_SETTINGS_SOURCE "${PROJECT_SOURCE_DIR}/libs/user_settings.h")
        endif()

        message("Copying ${WOLFSSL_USER_SETTINGS_SOURCE} to ${WOLFSSL_USER_SETTINGS_TARGET}")
        configure_file("${WOLFSSL_USER_SETTINGS_SOURCE}"   "${WOLFSSL_USER_SETTINGS_TARGET}"  COPYONLY)

    else()
        message("Error while comparing wolfssl user_settings files.")
    endif()

endif() # EXISTS "${IDF_PATH}/components/wolfssl/"


