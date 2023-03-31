include(FetchContent)
FetchContent_Declare(
        ftxui
        GIT_REPOSITORY  https://github.com/ArthurSonzogni/FTXUI.git
        GIT_TAG         v4.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(app app.cpp)
target_compile_features(app PRIVATE cxx_std_20)
target_link_libraries(app PRIVATE meeting_lib ftxui::screen ftxui::component)