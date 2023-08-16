idf_component_register(SRCS "uitask.c"
                    INCLUDE_DIRS "include"
                    REQUIRES lvgl
                    lvgl_esp32_drivers
                    porting
                    uicode
                    )
