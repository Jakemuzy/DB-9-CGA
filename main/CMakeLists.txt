idf_component_register(SRCS "main2.c" "config.c" "display.c" "buzzer.c" "mqtt.c" "network.c"
		       REQUIRES mqtt esp_netif esp_event
		       PRIV_REQUIRES spi_flash esp_lcd esp_timer heap
		       INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
