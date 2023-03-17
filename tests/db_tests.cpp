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

        auto found_user = db.find_user(user.get_nickname());
        REQUIRE(found_user.m_err == Database::ErrorCode::ok);
        REQUIRE(found_user.m_some->get_id() == 1);
        REQUIRE(found_user.m_some->get_nickname() == user.get_nickname());
    }

    SECTION( "should insert to database and find it by id" ) {
        Database db{ ":memory:" };
        Database::ErrorCode code = db.add_user(user);
        REQUIRE(code == Database::ErrorCode::ok);

        // the previous test should test if 1 is the valid ID
        auto found_user = db.find_user(1);
        REQUIRE(found_user.m_err == Database::ErrorCode::ok);
        REQUIRE(found_user.m_some->get_id() == 1);
        REQUIRE(found_user.m_some->get_nickname() == user.get_nickname());


    }

    SECTION( "should not find user if it was not added" ) {
        Database db{ ":memory:" };
    
        auto found_user = db.find_user(user.get_nickname());
        REQUIRE(found_user.m_err == Database::ErrorCode::userNotFound);
    }
}

TEST_CASE( "update user in database", "[user][database]") {
    User user{ "Walter" };
    
    SECTION( "should update user's nickname" ) {
        Database db{ ":memory:" };
        Database::ErrorCode code = db.add_user(user);
        REQUIRE(code == Database::ErrorCode::ok);

        code = db.update_user(
            *db.find_user(user.get_nickname())
            .m_some->get_id(), "Jessie");
        REQUIRE(code == Database::ErrorCode::ok);

        auto found_user = db.find_user("Jessie");
        REQUIRE(found_user.m_err == Database::ErrorCode::ok);
        REQUIRE(found_user.m_some->get_id() == 1);
        REQUIRE(found_user.m_some->get_nickname() == "Jessie");
    }
}

TEST_CASE( "delete user from database", "[user][database]") {
    User user{ "Walter" };
    
    SECTION( "should delete user" ) {
        Database db{ ":memory:" };
        Database::ErrorCode code = db.add_user(user);
        REQUIRE(code == Database::ErrorCode::ok);

        code = db.delete_user(
            *db.find_user(user.get_nickname()).m_some->get_id()
        );

        REQUIRE(code == Database::ErrorCode::ok);

        auto found_user = db.find_user(user.get_nickname());
        REQUIRE(found_user.m_err == Database::ErrorCode::userNotFound);
    }
}
