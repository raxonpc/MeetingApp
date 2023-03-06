#include <catch2/catch_test_macros.hpp>
#include "MeetingLib/Database.hpp"

TEST_CASE( "Database should be opened and created", "[database]" ) {
    REQUIRE_NOTHROW([](){ MeetingLib::Database db{":memory:" }; }());
}

TEST_CASE( "Should validate user's nickname", "[user]") {
    auto ok = []() { MeetingLib::User user{ "Terry" }; };
    auto too_short = []() { MeetingLib::User user{ "" }; };
    auto invalid_characters = []() { MeetingLib::User user{ "John123@" }; };

    REQUIRE_NOTHROW(ok());
    REQUIRE_THROWS(too_short());
    REQUIRE_THROWS(invalid_characters());
}
