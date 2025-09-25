# Collect all test source files
set(TEST_SRCS
    main.c
    system.c
)

# set options for compiling the qoraal library
target_compile_options(qoraal PRIVATE -O0 -g)
target_compile_definitions(qoraal PRIVATE CFG_OS_POSIX)
target_compile_options(qoraal-flash PRIVATE -O0 -g)
target_compile_definitions(qoraal-flash PRIVATE CFG_OS_POSIX)

# Add the executable target
add_executable(flashtest ${TEST_SRCS})
target_compile_options(flashtest PRIVATE -O0 -g)
# Link the qoraal library and pthread
if(WIN32)
    target_link_libraries(flashtest qoraal qoraal-flash pthread ws2_32)
else()
    target_link_libraries(flashtest qoraal qoraal-flash pthread)
endif()



# Generate a map file
set_target_properties(flashtest PROPERTIES LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/test/posix.ld")