#include <catch2/catch_test_macros.hpp>
#include "MeetingLib/Database.hpp"

TEST_CASE( "opening and creating database", "[database]" ) {
    REQUIRE_NOTHROW([](){ MeetingLib::Database db{":memory:" }; }());
}

TEST_CASE( "validating user's nickname", "[user]") {
    auto ok = []() { MeetingLib::User user{ "Terry" }; };
    auto too_short = []() { MeetingLib::User user{ "" }; };
    auto invalid_characters = []() { MeetingLib::User user{ "John123@" }; };

    REQUIRE_NOTHROW(ok());
    REQUIRE_THROWS(too_short());
    REQUIRE_THROWS(invalid_characters());
}

TEST_CASE( "inserting user into database", "[user][database]") {
    MeetingLib::User user{ "Walter" };
    
    SECTION( "should insert to database" ) {
        MeetingLib::Database db{ ":memory:" };
        MeetingLib::Database::ErrorCode code = db.add_user(user);
        REQUIRE(code == MeetingLib::Database::ErrorCode::ok);
    }
    SECTION( "duplicates are not allowed" ) {
        MeetingLib::Database db{ ":memory:" };
        MeetingLib::Database::ErrorCode code1 = db.add_user(user);
        MeetingLib::Database::ErrorCode code2 = db.add_user(user);
        REQUIRE(code1 == MeetingLib::Database::ErrorCode::ok);
        REQUIRE(code2 == MeetingLib::Database::ErrorCode::userAlreadyExists);
    }
}
