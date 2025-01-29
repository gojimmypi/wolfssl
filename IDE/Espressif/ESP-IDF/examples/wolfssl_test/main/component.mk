#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behavior is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#
# (Uses default behavior of compiling all source files in directory, adding 'include' to include path.)

# We'll add the explicit lines only for old SDK requirements (e.h. ESP8266)
COMPONENT_SRCDIRS := .
COMPONENT_ADD_INCLUDEDIRS := .
COMPONENT_ADD_INCLUDEDIRS += include

# Ensure main.c gets compiled
COMPONENT_OBJS := main.o
