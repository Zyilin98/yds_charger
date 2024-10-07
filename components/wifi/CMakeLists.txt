idf_component_register(SRCS "web_server.c" "wifi.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash wpa_supplicant esp_wifi
                    EMBED_FILES "upload_script.html")                
