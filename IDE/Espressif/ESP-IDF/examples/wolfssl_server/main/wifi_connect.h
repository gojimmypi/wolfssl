/* wifi_connect.h
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
#ifndef _TLS_WIFI_H_
#define _TLS_WIFI_H_

#include "esp_idf_version.h"
#include "esp_log.h"
//#include "esp_wifi.h"
#if ESP_IDF_VERSION_MAJOR >= 4
//#include "esp_event.h"
#else
#include "esp_event_loop.h"
#endif

#define DEFAULT_PORT                     11111

#define TLS_SMP_SERVER_TASK_NAME         "tls_sever_example"
#define TLS_SMP_SERVER_TASK_WORDS        10240
#define TLS_SMP_SERVER_TASK_PRIORITY     8

#define TLS_SMP_WIFI_SSID                CONFIG_WIFI_SSID
#define TLS_SMP_WIFI_PASS                CONFIG_WIFI_PASSWORD

/**
 ******************************************************************************
 ******************************************************************************
 ** USER APPLICATION SETTINGS BEGIN
 ******************************************************************************
 ******************************************************************************
 **/

/* when using a private config with plain text passwords, not my_private_config.h should be excluded from git updates */
#define  USE_MY_PRIVATE_CONFIG

#ifdef  USE_MY_PRIVATE_CONFIG
    #include "/workspace/my_private_config.h"
#else
    /*
    ** The examples use WiFi configuration that you can set via project
    ** configuration menu
    **
    ** If you'd rather not, just change the below entries to strings with
    ** the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
    */
    #define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
    #define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#endif

/* ESP lwip */
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

#define DEFAULT_PORT                     11111

#define TLS_SMP_CLIENT_TASK_NAME         "tls_client_example"
#define TLS_SMP_CLIENT_TASK_WORDS        10240
#define TLS_SMP_CLIENT_TASK_PRIORITY     8

#define TLS_SMP_TARGET_HOST              "192.168.25.114"

int tls_smp_server_init();
int wifi_init_sta(void);
#endif
