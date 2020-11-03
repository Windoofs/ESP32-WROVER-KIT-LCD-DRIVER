set(srcs "pretty_effect.c"
    "example.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")

