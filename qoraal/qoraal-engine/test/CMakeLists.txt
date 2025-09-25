set(TEST_SRCS
    toaster.c
    main.c
)

add_executable(toaster ${TEST_SRCS})
target_compile_definitions(qoraal PRIVATE CFG_OS_POSIX)

# If you want debugging turned off, change -O0 -g or remove them
target_link_libraries(toaster PRIVATE qoraal qoraal-flash qoraal-engine pthread)
target_compile_options(qoraal PRIVATE -O0 -g)
target_compile_options(qoraal-engine PRIVATE -O0 -g)
target_compile_options(qoraal-flash PRIVATE -O0 -g)
target_compile_options(toaster PRIVATE -O0 -g)

target_link_options(toaster PRIVATE
    "-Wl,--whole-archive"
    "$<TARGET_FILE:qoraal-engine>"
    "-Wl,--no-whole-archive"
)

# Generate a map file
set_target_properties(toaster PROPERTIES LINK_FLAGS "-Wl,-Map=output.map -T ${CMAKE_SOURCE_DIR}/test/posix.ld")