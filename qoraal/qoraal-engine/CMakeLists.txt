cmake_minimum_required(VERSION 3.16)
project(QoraalEngine LANGUAGES C)

# By default, the registry is disabled.
option(CFG_ENGINE_REGISTRY_ENABLE "Enable the engine registry (and include Qoraal-Flash)" OFF)

include(FetchContent)

# Fetch the main Qoraal repo
FetchContent_Declare(
    qoraal
    GIT_REPOSITORY https://github.com/navaro/qoraal.git
    GIT_TAG        main
    SOURCE_SUBDIR  .
)
FetchContent_MakeAvailable(qoraal)
include_directories("${qoraal_SOURCE_DIR}/include")

# Only fetch and include Qoraal-Flash if the registry is enabled.
if(CFG_ENGINE_REGISTRY_ENABLE)
    FetchContent_Declare(
        qoraal_flash
        GIT_REPOSITORY https://github.com/navaro/qoraal-flash.git
        GIT_TAG        main
        SOURCE_SUBDIR  .
    )
    FetchContent_MakeAvailable(qoraal_flash)
    include_directories("${qoraal_flash_SOURCE_DIR}/include")
    
    # Define the compile flag so that dependent code can conditionally compile.
    add_compile_definitions(CFG_ENGINE_REGISTRY_ENABLE=1)
endif()

include_directories(include)

# Add subdirectories
add_subdirectory(src)

# Option to build tests
option(BUILD_TOASTER "Build the test directory" OFF)
if(BUILD_TOASTER)
    add_subdirectory(test)
endif()
