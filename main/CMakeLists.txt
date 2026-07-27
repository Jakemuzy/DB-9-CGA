idf_component_register(SRCS "main.c" "display.c" "letters.c"
                       PRIV_REQUIRES spi_flash esp_lcd esp_timer
		       INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
