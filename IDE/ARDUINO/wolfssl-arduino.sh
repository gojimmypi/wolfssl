#!/bin/sh

# this script will reformat the wolfSSL source code to be compatible with
# an Arduino project
# run as bash ./wolfssl-arduino.sh [INSTALL] [path]

# ROOT_DIR="/mnt/c/Users/gojimmypi/Documents/Arduino/libraries"
ROOT_DIR="/wolfSSL"

if [ $# -gt 0 ]; then
    THIS_OPERATION="$1"
    if [ "$THIS_OPERATION" = "INSTALL" ]; then
        echo "Install is active"

        # Check environment
        if [ -n "$WSL_DISTRO_NAME" ]; then
            # we found a non-blank WSL environment distro name
            current_path="$(pwd)"
            pattern="/mnt/?"
            if [ "$(echo "$current_path" | grep -E "^$pattern")" ]; then
                # if we are in WSL and shared Windows file system, 'ln' does not work.
                ARDUINO_ROOT="/mnt/c/Users/$USER/Documents/Arduino/libraries"
            else
                ARDUINO_ROOT="~/Arduino/libraries"
            fi
        fi

        if [ -d "$ARDUINO_ROOT/$ROOT_DIR" ]; then
            echo "Error: the installation directory already exists: $ARDUINO_ROOT/$ROOT_DIR"
            exit 1
        fi
    else
        echo "Error: not a valid operation: $THIS_OPERATION"
        exit 1
    fi
fi

ROOT_SRC_DIR="${ROOT_DIR}/src"
WOLFSSL_SRC="${ROOT_SRC_DIR}/src"
WOLFSSL_HEADERS="${ROOT_SRC_DIR}/wolfssl"
WOLFCRYPT_ROOT="${ROOT_SRC_DIR}/wolfcrypt"
WOLFCRYPT_SRC="${WOLFCRYPT_ROOT}/src"
WOLFCRYPT_HEADERS="${WOLFSSL_HEADERS}/wolfcrypt"
OPENSSL_DIR="${WOLFSSL_HEADERS}/openssl"
WOLFSSL_VERSION="5.6.4"

# TOP indicates the file directory comes from the top level of the wolfssl repo
TOP_DIR="../.."
WOLFSSL_SRC_TOP="${TOP_DIR}/src"
WOLFSSL_HEADERS_TOP="${TOP_DIR}/wolfssl"
WOLFCRYPT_ROOT_TOP="${TOP_DIR}/wolfcrypt"
WOLFCRYPT_SRC_TOP="${WOLFCRYPT_ROOT_TOP}/src"
WOLFCRYPT_HEADERS_TOP="${WOLFSSL_HEADERS_TOP}/wolfcrypt"
OPENSSL_DIR_TOP="${WOLFSSL_HEADERS_TOP}/openssl"


# TODO: Parse version number
WOLFSSL_VERSION=$(grep -i "LIBWOLFSSL_VERSION_STRING" ${TOP_DIR}/wolfssl/version.h | cut -d '"' -f 2)


DIR=${PWD##*/}

if [ "$DIR" = "ARDUINO" ]; then
    if [ ! -d ".${ROOT_DIR}" ]; then
        echo "Line 38: mkdir .${ROOT_DIR}"
        mkdir .${ROOT_DIR}
    fi
    if [ ! -d ".${ROOT_SRC_DIR}" ]; then
        echo "Line 42: mkdir .${ROOT_SRC_DIR}"
        mkdir .${ROOT_SRC_DIR}
    fi

    if [ ! -d ".${WOLFSSL_HEADERS}" ]; then
        echo "Line 47: mkdir .${WOLFSSL_HEADERS}"
        mkdir .${WOLFSSL_HEADERS}
    fi

    echo "Line 51: cp ${WOLFSSL_HEADERS_TOP}/*.h .${WOLFSSL_HEADERS}"
    cp ${WOLFSSL_HEADERS_TOP}/*.h .${WOLFSSL_HEADERS}
    if [ ! -d ".${WOLFCRYPT_HEADERS}" ]; then
        echo "Line 54: mkdir .${WOLFCRYPT_HEADERS}"
        mkdir .${WOLFCRYPT_HEADERS}
        mkdir .${WOLFCRYPT_HEADERS}/port
        mkdir .${WOLFCRYPT_HEADERS}/port/atmel
        mkdir .${WOLFCRYPT_HEADERS}/port/Espressif
    fi

    # is this a dupe?
    if [ ! -d ".${WOLFCRYPT_HEADERS}" ]; then
        echo "Line 54: mkdir .${WOLFCRYPT_HEADERS}"
        mkdir .${WOLFCRYPT_HEADERS}
        mkdir .${WOLFCRYPT_HEADERS}/port
        mkdir .${WOLFCRYPT_HEADERS}/port/atmel
        mkdir .${WOLFCRYPT_HEADERS}/port/Espressif
    fi

    echo "Line 58: cp ${WOLFCRYPT_HEADERS_TOP}/*.h .${WOLFCRYPT_HEADERS}"
    cp ${WOLFCRYPT_HEADERS_TOP}/*.h .${WOLFCRYPT_HEADERS}
    cp ${WOLFCRYPT_HEADERS_TOP}/port/atmel/*.h     .${WOLFCRYPT_HEADERS}/port/atmel
    cp ${WOLFCRYPT_HEADERS_TOP}/port/Espressif/*.h .${WOLFCRYPT_HEADERS}/port/Espressif

    # Add in source files to wolfcrypt/src
    if [ ! -d ".${WOLFCRYPT_ROOT}" ]; then
        echo "Line 63: mkdir .${WOLFCRYPT_ROOT}"
        mkdir .${WOLFCRYPT_ROOT}
    fi
    if [ ! -d ".${WOLFCRYPT_SRC}" ]; then
        echo "Line 67: mkdir .${WOLFCRYPT_SRC}"
        mkdir .${WOLFCRYPT_SRC}
        mkdir .${WOLFCRYPT_SRC}/port
        mkdir .${WOLFCRYPT_SRC}/port/atmel
        mkdir .${WOLFCRYPT_SRC}/port/Espressif
    fi

    echo "Line $LINENO: cp ${WOLFCRYPT_SRC_TOP}/*.c .${WOLFCRYPT_SRC}"
    cp -r ${WOLFCRYPT_SRC_TOP}/*.c                  .${WOLFCRYPT_SRC}
    cp -r ${WOLFCRYPT_SRC_TOP}/port/atmel/*.c       .${WOLFCRYPT_SRC}/port/atmel
    cp -r ${WOLFCRYPT_SRC_TOP}/port/Espressif/*.c   .${WOLFCRYPT_SRC}/port/Espressif

    # Add in source files to top level src folders
    if [ ! -d ".${WOLFSSL_SRC}" ]; then
        echo "Line $LINENO: mkdir .${WOLFSSL_SRC}"
        mkdir .${WOLFSSL_SRC}
    fi
    cp ${WOLFSSL_SRC_TOP}/*.c .${WOLFSSL_SRC}
    # put bio and evp as includes
    cp .${WOLFSSL_SRC}/bio.c .${WOLFSSL_HEADERS}
    cp .${WOLFCRYPT_SRC}/evp.c .${WOLFSSL_HEADERS}

    # make a copy of evp.c and bio.c for ssl.c to include inline
    cp .${WOLFSSL_HEADERS}/evp.c .${WOLFCRYPT_SRC}/evp.c
    cp .${WOLFSSL_HEADERS}/bio.c .${WOLFCRYPT_SRC}/bio.c

    # copy openssl compatibility headers to their appropriate location
    if [ ! -d ".${OPENSSL_DIR}" ]; then
        mkdir .${OPENSSL_DIR}
    fi
    cp ${OPENSSL_DIR_TOP}/* .${OPENSSL_DIR}


    cat > .${ROOT_SRC_DIR}/wolfssl.h <<EOF
/* Generated wolfSSL header file for Arduino */
#include <user_settings.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
EOF


# Creates user_settings file if one does not exist
#     if [ ! -f ".${ROOT_SRC_DIR}/user_settings.h" ]; then
#         cat > .${ROOT_SRC_DIR}/user_settings.h <<EOF
# /* Generated wolfSSL user_settings.h file for Arduino */
# #ifndef ARDUINO_USER_SETTINGS_H
# #define ARDUINO_USER_SETTINGS_H
#
# /* Platform */
# #define WOLFSSL_ARDUINO
#
# /* Math library (remove this to use normal math)*/
#define USE_FAST_MATH
#define TFM_NO_ASM
#define NO_ASN_TIME
#
# /* When using Intel Galileo Uncomment the line below */
# /* #define INTEL_GALILEO */
#
# /* RNG DEFAULT !!FOR TESTING ONLY!! */
# /* comment out the error below to get started w/ bad entropy source
#  * This will need fixed before distribution but is OK to test with */
#error "needs solved, see: https://www.wolfssl.com/docs/porting-guide/"
#define WOLFSSL_GENSEED_FORTEST

#endif /* ARDUINO_USER_SETTINGS_H */
# EOF
#     fi

#    cp .${WOLFCRYPT_HEADERS}/settings.h .${WOLFCRYPT_HEADERS}/settings.h.bak
#    cat > .${WOLFCRYPT_HEADERS}/settings.h <<EOF
# /*wolfSSL Generated ARDUINO settings */
# #ifndef WOLFSSL_USER_SETTINGS
#    #define WOLFSSL_USER_SETTINGS
#endif /* WOLFSSL_USER_SETTINGS */
#/*wolfSSL Generated ARDUINO settings: END */
#
#EOF
#    cat .${WOLFCRYPT_HEADERS}/settings.h.bak >> .${WOLFCRYPT_HEADERS}/settings.h

    #Creating library.properties file based off of:
    #https://arduino.github.io/arduino-cli/0.35/library-specification/#libraryproperties-file-format

    cat > .${ROOT_DIR}/library.properties <<EOF
name=wolfSSL
version=${WOLFSSL_VERSION}
author=wolfSSL inc
maintainer=wolfSSL inc <support@wolfssl.com>
sentence=A lightweight SSL/TLS library written in ANSI C and targeted for embedded, RTOS, and resource-constrained environments.
paragraph=Manual: https://www.wolfssl.com/documentation/manuals/wolfssl/index.html.
category=Communication
url=https://www.wolfssl.com/
architectures=*

EOF

else
    echo "ERROR: You must be in the IDE/ARDUINO directory to run this script"
    exit 1
fi

# Optionally install to a separate directory.
# Note we should have exited above if a problem was encountered,
# as we'll never want to install a bad library.
if [ "$THIS_OPERATION" = "INSTALL" ]; then
    cp ../../examples/configs/user_settings_arduino.h  ".${ROOT_SRC_DIR}/user_settings.h"
    echo "mv $ROOT_DIR $ARDUINO_ROOT"
    mv ".$ROOT_DIR" "$ARDUINO_ROOT"
fi
