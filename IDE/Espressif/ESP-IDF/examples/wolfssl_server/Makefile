#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := tls_server

# Default compiler flags
CFLAGS   ?=
CXXFLAGS ?=

CFLAGS += -DWOLFSSL_USER_SETTINGS

# Check if CONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG is set to 1 in environment
ifeq ($(CONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG),1)
    $(info Setting CONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG from environment variablev for Makefile)
    CFLAGS   += -DCONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG=1
    CXXFLAGS += -DCONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG=1
else
    $(info CONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG not set, not using private config.)
endif

# if there isn't the directory, please disable the line below.
EXTRA_COMPONENT_DIRS = $(IDF_PATH)/examples/common_components/protocol_examples_common

include $(IDF_PATH)/make/project.mk

