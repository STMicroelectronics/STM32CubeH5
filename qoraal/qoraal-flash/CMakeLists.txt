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
FetchContent_MakeAvailable(qoraal)

# Include directories
include_directories(include)
include_directories("${qoraal_SOURCE_DIR}/include")

# Add subdirectories
add_subdirectory(src)

# Option to build tests
option(BUILD_REGTEST "Build the test directory" OFF)

if(BUILD_REGTEST)
    add_subdirectory(test)
endif()

