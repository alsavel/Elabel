idf_component_register(SRCS "elabel.c"
                       PRIV_REQUIRES 
                                    esp_timer  esp_psram esp_lcd esp_driver_gpio lvglcr
                       INCLUDE_DIRS "")

