cmake_minimum_required(VERSION 3.14...3.25)

# ----------------- PROJECT SETUP -----------------

project(
    MeetingApp
    VERSION 0.1
    LANGUAGES CXX
)

option(MEETING_BUILD_TESTS "Build tests for Meeting" ON)
option(MEETING_BUILD_APP "Build tui app" ON)

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

include(FindSQLite3)

if(NOT SQLite3_FOUND)
    message(FATAL_ERROR "Error: SQLite3 is required to build MeetingApp")
endif()

add_subdirectory(src)
if(MEETING_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(MEETING_BUILD_APP)
    add_subdirectory(app)
endif()
