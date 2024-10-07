idf_component_register(SRCS "web_server.c" "wifi.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash wpa_supplicant esp_wifi)
