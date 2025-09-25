# Collect all test source files
set(TEST_SRCS
    main.c
    tictactoe.c
    services/engine.c
    services/www/wserver_inst.c
    services/www/html/wabout.c
    services/www/html/wengine.c
    services/www/html/wnlog.c
    services/www/html/wrtlog.c
    services/www/html/wservices.c
    services/www/html/wshell.c
    services/www/html/wsystem.c
    services/www/image/wimage.c
    services/www/css/wcss.c
    services/www/parts/html.c
)

# Add the executable target
add_executable(tictactoe ${TEST_SRCS})

# Link the qoraal library and pthread
if(WIN32)
    target_link_libraries(tictactoe qoraal qoraal-http qoraal-engine pthread ws2_32)
else()
    target_link_libraries(tictactoe qoraal qoraal-http qoraal-engine pthread m)
endif()
target_compile_options(tictactoe PRIVATE -O0 -g)
target_compile_options(qoraal PRIVATE -O0 -g)
target_compile_options(qoraal-http PRIVATE -O0 -g)
target_compile_options(qoraal-engine PRIVATE -O0 -g)

# Define CFG_OS_MEM_DEBUG_ENABLE for qoraal
target_compile_definitions(qoraal PUBLIC CFG_OS_POSIX)
target_compile_definitions(qoraal PRIVATE CFG_PLATFORM_ENABLE_WS2)
target_compile_definitions(qoraal-engine PRIVATE CFG_ENGINE_REGISTRY_DISABLE)



target_link_options(tictactoe PRIVATE
    "-Wl,--whole-archive"
    "$<TARGET_FILE:qoraal-engine>"
    "-Wl,--no-whole-archive"
)

# Generate a map file
set_target_properties(tictactoe PROPERTIES LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/src/posix.ld")