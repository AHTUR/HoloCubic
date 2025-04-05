idf_component_register(SRCS "ik_fs_port.c" "ik_mahony_filter.c" "ik_lv_port.c" "ik_heap.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver lvgl lcd ik_bsp fatfs
                    )
