set(srcs "LCD.c"
    "Driver.c"
    "Example_Main.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")

