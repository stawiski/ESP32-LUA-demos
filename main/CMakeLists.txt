idf_component_register(SRCS
                    "main.cpp"
                    "lua_app.c"
                    "gui_app.c"
                    "utils.c"
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

# LUA
add_compile_definitions(LUA_HEAP_SIZE=262144) # 256KB
# LCD
add_compile_definitions(LCD_WIDTH_PX=320)
add_compile_definitions(LCD_HEIGHT_PX=240)
