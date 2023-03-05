#include <catch2/catch_test_macros.hpp>
#include "../src/Database.hpp"

TEST_CASE( "Database should be opened", "[database]" ) {
    REQUIRE_NOTHROW([](){ MeetingLib::Database db{"foo.db"}; }());
}
