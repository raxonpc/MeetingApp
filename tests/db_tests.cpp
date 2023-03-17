#include <catch2/catch_test_macros.hpp>
#include "MeetingLib/Database.hpp"

using namespace MeetingLib;

TEST_CASE( "opening and creating database", "[database]" ) {
    REQUIRE_NOTHROW([](){ Database db{":memory:" }; }());
}

TEST_CASE( "validating user's nickname", "[user]") {
    auto ok = []() { User user{ "Terry" }; };
    auto too_short = []() { User user{ "" }; };
    auto invalid_characters = []() { User user{ "John123@" }; };

    REQUIRE_NOTHROW(ok());
    REQUIRE_THROWS(too_short());
    REQUIRE_THROWS(invalid_characters());
}

TEST_CASE( "inserting user into database", "[user][database]") {
    User user{ "Walter" };
    
    SECTION( "should insert to database" ) {
        Database db{ ":memory:" };
        Database::ErrorCode code = db.add_user(user);
        REQUIRE(code == Database::ErrorCode::ok);
    }
    SECTION( "duplicates are not allowed" ) {
        Database db{ ":memory:" };
        Database::ErrorCode code1 = db.add_user(user);
        Database::ErrorCode code2 = db.add_user(user);
        REQUIRE(code1 == Database::ErrorCode::ok);
        REQUIRE(code2 == Database::ErrorCode::userAlreadyExists);
    }
}

TEST_CASE( "finding user in database", "[user][database]") {
    User user{ "Walter" };
    
    SECTION( "should insert to database and find it by nickname" ) {
        Database db{ ":memory:" };
        Database::ErrorCode code = db.add_user(user);
        REQUIRE(code == Database::ErrorCode::ok);

        auto found_user = db.find_user(user.getNickname());
        REQUIRE(found_user.m_err == Database::ErrorCode::ok);
        REQUIRE(found_user.m_some->getId() == 1);
        REQUIRE(found_user.m_some->getNickname() == user.getNickname());
    }

    SECTION( "should insert to database and find it by id" ) {
        Database db{ ":memory:" };
        Database::ErrorCode code = db.add_user(user);
        REQUIRE(code == Database::ErrorCode::ok);

        // the previous test should test if 1 is the valid ID
        auto found_user = db.find_user(1);
        REQUIRE(found_user.m_err == Database::ErrorCode::ok);
        REQUIRE(found_user.m_some->getId() == 1);
        REQUIRE(found_user.m_some->getNickname() == user.getNickname());


    }

    SECTION( "should not find user if it was not added" ) {
        Database db{ ":memory:" };
    
        auto found_user = db.find_user(user.getNickname());
        REQUIRE(found_user.m_err == Database::ErrorCode::userNotFound);
    }
}
