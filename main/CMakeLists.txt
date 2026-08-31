idf_component_register(SRCS "main2.c" "config.c" "display.c" "buzzer.c" "mqtt.c" "network.c" "secrets.c"
		       REQUIRES mqtt esp_netif esp_event nvs_flash esp_wifi
		       PRIV_REQUIRES spi_flash esp_lcd esp_timer esp_driver_ledc heap
		       INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
