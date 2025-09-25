# Collect all source files
set(FLASH_SRC
    qoraal.c
    httpclient.c
    httpdwnld.c
    httpserver.c
    httpwebapi.c
    httpparse.c
    wserver_m.c
    network.c
    json/frozen.c
    json/jsmn.c
    json/jsmn_tools.c
    qshell/httpcmds.c
)


# Add the library target
add_library(qoraal-http STATIC ${FLASH_SRC})

# Link necessary libraries
target_link_libraries(qoraal-http qoraal)

