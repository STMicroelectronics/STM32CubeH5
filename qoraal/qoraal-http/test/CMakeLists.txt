# Collect all test source files
set(TEST_SRCS
    main.c
    services/www/wserver_inst.c
    services/www/css/wcss.c
    services/www/html/wabout.c
    services/www/html/wnlog.c
    services/www/html/wrtlog.c
    services/www/html/wservices.c
    services/www/html/wshell.c
    services/www/html/wsystem.c
    services/www/html/wwebapi.c
    services/www/image/wimage.c
)

# Add the executable target
add_executable(httptest ${TEST_SRCS})

# Link the qoraal library and pthread
if(WIN32)
    target_link_libraries(httptest qoraal qoraal-http pthread ws2_32)
else()
    target_link_libraries(httptest qoraal qoraal-http pthread)
endif()
target_compile_options(httptest PRIVATE -O0 -g)
target_compile_options(qoraal PRIVATE -O0 -g)
target_compile_options(qoraal-http PRIVATE -O0 -g)

# Define CFG_OS_MEM_DEBUG_ENABLE for qoraal
target_compile_definitions(qoraal PRIVATE CFG_OS_MEM_DEBUG_ENABLE CFG_PLATFORM_ENABLE_WS2)


# Generate a map file
set_target_properties(httptest PROPERTIES LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/test/posix.ld")