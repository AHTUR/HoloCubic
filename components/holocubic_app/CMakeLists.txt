idf_component_register(SRCS "holocubic_app_main.c" 
                    INCLUDE_DIRS "."
                    REQUIRES spi_flash driver ik_bsp lcd
                    )
