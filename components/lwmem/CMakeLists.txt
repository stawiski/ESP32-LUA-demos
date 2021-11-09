idf_component_register(SRCS "${CMAKE_CURRENT_LIST_DIR}/lwmem/lwmem/src/lwmem/lwmem.c"
                    INCLUDE_DIRS 
                        ${CMAKE_CURRENT_LIST_DIR}/include
                        ${CMAKE_CURRENT_LIST_DIR}/lwmem/lwmem/src/include
                    )

target_compile_options(${COMPONENT_LIB} PRIVATE -Werror -O3 -DESP32)
