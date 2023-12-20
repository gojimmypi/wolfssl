#
# Copyright (C) 2006-2023 wolfSSL Inc.
#
# This file is part of wolfSSL.
#
# wolfSSL is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# wolfSSL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
#
#
# Component Makefile
#

CFLAGS +=-DWOLFSSL_USER_SETTINGS

WOLFSSL_ROOT := ../../../../../../../

COMPONENT_ADD_INCLUDEDIRS := .
COMPONENT_ADD_INCLUDEDIRS += include
COMPONENT_ADD_INCLUDEDIRS += $(WOLFSSL_ROOT)/.
COMPONENT_ADD_INCLUDEDIRS += $(WOLFSSL_ROOT)/wolfssl
COMPONENT_ADD_INCLUDEDIRS += $(WOLFSSL_ROOT)/wolfssl/wolfcrypt
# COMPONENT_ADD_INCLUDEDIRS += $ENV(IDF_PATH)/components/freertos/include/freertos
# COMPONENT_ADD_INCLUDEDIRS += "$ENV(IDF_PATH)/soc/esp32s3/include/soc"


# WOLFSSL_ROOT := ""
COMPONENT_SRCDIRS := $(WOLFSSL_ROOT)wolfcrypt/src
COMPONENT_SRCDIRS += $(WOLFSSL_ROOT)wolfcrypt/src/port/Espressif
COMPONENT_SRCDIRS += $(WOLFSSL_ROOT)wolfcrypt/src/port/atmel
COMPONENT_SRCDIRS += $(WOLFSSL_ROOT)wolfcrypt/benchmark
COMPONENT_SRCDIRS += $(WOLFSSL_ROOT)wolfcrypt/test
COMPONENT_SRCDIRS += include

COMPONENT_OBJEXCLUDE := $(WOLFSSL_ROOT)wolfcrypt/src/aes_asm.o
COMPONENT_OBJEXCLUDE += $(WOLFSSL_ROOT)wolfcrypt/src/evp.o
COMPONENT_OBJEXCLUDE += $(WOLFSSL_ROOT)wolfcrypt/src/misc.o
COMPONENT_OBJEXCLUDE += $(WOLFSSL_ROOT)src/bio.o

COMPONENT_SRCS += $(WOLFSSL_ROOT)src/bio.c
# COMPONENT_SRCS += src/conf.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/crl.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/dtls.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/dtls13.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/internal.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/keys.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/ocsp.c
# COMPONENT_SRCS += src/pk.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/quic.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/sniffer.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/ssl.c
# COMPONENT_SRCS += src/ssl_asn1.c
# COMPONENT_SRCS += src/ssl_bn.c
# COMPONENT_SRCS += src/ssl_certman.c
# COMPONENT_SRCS += src/ssl_crypto.c
# COMPONENT_SRCS += src/ssl_misc.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/tls.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/tls13.c
COMPONENT_SRCS += $(WOLFSSL_ROOT)src/wolfio.c
# COMPONENT_SRCS += src/x509.c
# COMPONENT_SRCS += src/x509_str.c

