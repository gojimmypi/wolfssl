/* server-tls.h
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */
#ifndef _SERVER_TLS_
#define _SERVER_TLS_

#include "user_settings.h"

#if defined(SINGLE_THREADED)
    #define WOLFSSL_ESP_TASK int
#else
    #include "freertos/FreeRTOS.h"
    #define WOLFSSL_ESP_TASK void
#endif

/* Function to show the ciphers available. */
int ShowCiphers(void);

/* This is the TLS Server function, possibly in an RTOS thread. */
WOLFSSL_ESP_TASK tls_smp_server_task(void *args);

/* init will create an RTOS task, otherwise server is simply function call. */
#if defined(SINGLE_THREADED)
    /* no init neded */
#else
    int tls_smp_server_init(void);
#endif
#endif /* _SERVER_TLS_ */
