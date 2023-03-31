

# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/meeting/*.hpp")

add_library(meeting_lib Schema.cpp Database.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(meeting_lib
        PUBLIC
        $<BUILD_INTERFACE:${MeetingApp_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>

        PRIVATE
        ${SQLite3_INCLUDE_DIRS}
        )
target_link_libraries(meeting_lib ${SQLite3_LIBRARIES})

# All users of this library will need at least C++20
target_compile_features(meeting_lib PUBLIC cxx_std_20)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

