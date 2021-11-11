idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                    # ESP-IDF built-in components
                    spi_flash
                    spiffs
                    # External components
                    esp-idf-lua
                    lwmem
                    lvgl
                    lvgl_esp32_drivers
)

add_compile_definitions(LUA_HEAP_SIZE=65536)
add_compile_definitions(LV_LVGL_H_INCLUDE_SIMPLE)
