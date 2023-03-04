cmake_minimum_required(VERSION 3.14...3.25)

# ----------------- PROJECT SETUP -----------------

project(
    MeetingApp
    VERSION 0.1
    LANGUAGES CXX
)

# Prevent in-source builds
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed")
endif()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)

    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

add_subdirectory(src)
