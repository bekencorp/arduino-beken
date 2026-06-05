set(srcs
    init_stub.c
    main.c
)

armino_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "."
    PRIV_REQUIRES bk_init
)
