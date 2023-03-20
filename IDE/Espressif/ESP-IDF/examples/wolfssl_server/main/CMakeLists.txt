idf_component_register(
                       SRCS "server-tls.c"
                       "wifi_connect.c"
                       INCLUDE_DIRS "." "./include"
                       REQUIRES "esp_wifi" "wolfssl")
