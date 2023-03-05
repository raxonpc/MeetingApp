include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG  v3.3.2
)
FetchContent_MakeAvailable(catch2)

