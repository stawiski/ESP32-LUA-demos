idf_component_register(SRCS
                    "main.c"
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

add_compile_definitions(LUA_HEAP_SIZE=65536)
