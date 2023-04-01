include(FetchContent)
FetchContent_Declare(
        ftxui
        GIT_REPOSITORY  https://github.com/ArthurSonzogni/FTXUI.git
        GIT_TAG         v4.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(api_example api_example.cpp)
target_compile_features(api_example PRIVATE cxx_std_20)
target_link_libraries(api_example PRIVATE meeting_lib)