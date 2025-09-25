# Collect all source files
set(ENGINE_SRC
    parts/debug.c
    parts/console.c
    parts/engine.c
    parts/parts.c
    tool/lex.c
    tool/machine.c
    tool/collection.c
    tool/parse.c
    port/engine_qoraal.c
    qshell/enginecmd.c
    engine.c
    starter.c
)

# Add the library target
add_library(qoraal-engine STATIC ${ENGINE_SRC})

# Link necessary libraries
target_link_libraries(qoraal-engine qoraal)

