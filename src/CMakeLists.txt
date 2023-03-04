

# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/meeting/*.hpp")

add_library(meeting_lib lib.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(meeting_lib PUBLIC ../include)

# All users of this library will need at least C++20
target_compile_features(meeting_lib PUBLIC cxx_std_20)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

