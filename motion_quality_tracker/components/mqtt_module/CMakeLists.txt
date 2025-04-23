idf_component_register(SRCS "mqtt_module.c"
                       INCLUDE_DIRS "."
                       REQUIRES mqtt esp_wifi esp_event nvs_flash esp_netif)