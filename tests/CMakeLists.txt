include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG  v3.3.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(db_tests db_tests.cpp)
target_link_libraries(db_tests PRIVATE Catch2::Catch2WithMain meeting_lib)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(db_tests)