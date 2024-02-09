#
# This is a project Makefile.
# It is assumed the directory this Makefile resides in is a
# project subdirectory containing an entire project.
#

PROJECT_NAME := wolfssl_client

# Calling shell causes unintuiive error in Windows:
#     OS := $(shell uname -s)
# But OS should already be defined:
ifeq ($(OS),Linux)
    CFLAGS += -DOS_LINUX
endif
ifeq ($(OS),Windows_NT)
    CFLAGS += -DWOLFSSL_MAKE_SYSTEM_NAME_WINDOWS
endif
ifeq ($(OS),Darwin)
    CFLAGS += -DWOLFSSL_MAKE_SYSTEM_NAME_APPLE
endif
ifneq (,$(findstring MINGW,$(OS)))
    CFLAGS += -DWOLFSSL_MAKE_SYSTEM_NAME_MINGW
endif
ifneq (,$(findstring CYGWIN,$(OS)))
    CFLAGS += -DWOLFSSL_MAKE_SYSTEM_NAME_CYGWIN
endif

# It is essential that the build process sees the WOLFSSL_USER_SETTINGS
CFLAGS += -DWOLFSSL_USER_SETTINGS

# if directory not available, please disable the line below.
EXTRA_COMPONENT_DIRS = $(IDF_PATH)/examples/common_components/protocol_examples_common

include $(IDF_PATH)/make/project.mk
