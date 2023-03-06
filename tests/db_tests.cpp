#include <catch2/catch_test_macros.hpp>
#include "MeetingLib/Database.hpp"

TEST_CASE( "Database should be opened and created", "[database]" ) {
    REQUIRE_NOTHROW([](){ MeetingLib::Database db{":memory:" }; }());
}

