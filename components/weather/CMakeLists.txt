idf_component_register(SRCS "weather.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp-tls
                    esp_http_client
                    cJSON
                    uicode )
