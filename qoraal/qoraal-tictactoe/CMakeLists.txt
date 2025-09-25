cmake_minimum_required(VERSION 3.16)
project(QoraalEngine LANGUAGES C)

# --------------------
# 1) Fetch the Qoraal repo
# --------------------
include(FetchContent)

FetchContent_Declare(
    qoraal
    GIT_REPOSITORY https://github.com/navaro/qoraal.git
    GIT_TAG        main  # or whatever branch/commit you need
    SOURCE_SUBDIR  .
)

set(CFG_ENGINE_REGISTRY_DISABLE ON CACHE BOOL "Disable engine registry")
FetchContent_Declare(
    qoraal_engine
    GIT_REPOSITORY https://github.com/navaro/qoraal-engine.git
    GIT_TAG        main  # or whatever branch/commit you need
    SOURCE_SUBDIR  .
)

FetchContent_Declare(
    qoraal_http
    GIT_REPOSITORY https://github.com/navaro/qoraal-http.git
    GIT_TAG        main  # or whatever branch/commit you need
    SOURCE_SUBDIR  .
)
FetchContent_MakeAvailable(qoraal qoraal_engine qoraal_http)

# Include directories
include_directories(include)
include_directories("${qoraal_SOURCE_DIR}/include")
include_directories("${qoraal_engine_SOURCE_DIR}/include")
include_directories("${qoraal_http_SOURCE_DIR}/include")

# Add subdirectories
add_subdirectory(src)

# Set compile-time definitions for the qoraal OS




