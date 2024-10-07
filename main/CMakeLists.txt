idf_component_register(SRCS "mqtt.c" "dht11.c" "udp_server.c" "esp_http_client_example.c" "main.c" "menu.c" "task.c" "web_server.c" "wifi.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash wpa_supplicant esp_wifi
                    EMBED_FILES "upload_script.html")