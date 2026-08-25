idf_component_register(SRCS "main2.c" "config.c" "display.c" "buzzer.c" "mqtt.c"
                       PRIV_REQUIRES spi_flash esp_lcd esp_timer heap
		       INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
