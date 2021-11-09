idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES
                    # ESP-IDF built-in components
                    spi_flash spiffs
                    # External components
                    esp-idf-lua lwmem)

add_compile_definitions(LUA_HEAP_SIZE=65536)
