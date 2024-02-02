#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := wolfssl_client

#OS := $(shell uname -s)
#ifeq ($(OS),Linux)
    #CFLAGS += -DOS_LINUX
#endif
#ifeq ($(OS),Darwin)
    #CFLAGS += -DOS_MACOS
#endif
#ifneq (,$(findstring MINGW,$(OS)))
    #CFLAGS += -DOS_WINDOWS_MINGW
#endif
#ifneq (,$(findstring CYGWIN,$(OS)))
    #CFLAGS += -DOS_WINDOWS_CYGWIN
#endif
#CFLAGS += -DOS_WINDOWS

CFLAGS += -DWOLFSSL_USER_SETTINGS
# if directory not available, please disable the line below.
EXTRA_COMPONENT_DIRS = $(IDF_PATH)/examples/common_components/protocol_examples_common

# LDFLAGS += -T ./my.esp8266.project.ld

include $(IDF_PATH)/make/project.mk
