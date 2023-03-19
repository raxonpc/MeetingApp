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
        auto code = db.add_user(user);
        REQUIRE(code.m_err == Database::ErrorCode::ok);
    }
    SECTION( "duplicates are not allowed" ) {
        Database db{ ":memory:" };
        auto code1 = db.add_user(user);
        auto code2 = db.add_user(user);
        REQUIRE(code1.m_err == Database::ErrorCode::ok);
        REQUIRE(code2.m_err == Database::ErrorCode::userAlreadyExists);
    }
}

TEST_CASE( "finding user in database", "[user][database]") {
    User user{ "Walter" };
    
    SECTION( "should insert to database and find it by nickname" ) {
        Database db{ ":memory:" };
        auto code = db.add_user(user);
        REQUIRE(code.m_err == Database::ErrorCode::ok);

        auto found_user = db.find_user(user.get_nickname());
        REQUIRE(found_user.m_err == Database::ErrorCode::ok);
        REQUIRE(*found_user.m_some->get_id() == *code.m_some);
        REQUIRE(found_user.m_some->get_nickname() == user.get_nickname());
    }

    SECTION( "should insert to database and find it by id" ) {
        Database db{ ":memory:" };
        auto code = db.add_user(user);
        REQUIRE(code.m_err == Database::ErrorCode::ok);

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
        auto added_user = db.add_user(user);
        REQUIRE(added_user.m_err == Database::ErrorCode::ok);

        auto code = db.update_user(
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
        auto added_user = db.add_user(user);
        REQUIRE(added_user.m_err == Database::ErrorCode::ok);

        auto code = db.delete_user(
            *db.find_user(user.get_nickname()).m_some->get_id()
        );

        REQUIRE(code == Database::ErrorCode::ok);

        auto found_user = db.find_user(user.get_nickname());
        REQUIRE(found_user.m_err == Database::ErrorCode::userNotFound);
    }
}

TEST_CASE( "should convert a date to string", "[meeting]") {
    Date date{ std::chrono::day{22}/3/2023 };
    std::string expected = "2023/3/22";

    std::string converted = date_to_string(date);

    REQUIRE(converted == expected);
}

TEST_CASE( "inserting meeting into database", "[meeting][database]") {
    Meeting meeting{ std::chrono::day{22}/3/2023, Hours{ 2 }};
    
    SECTION( "should insert to database" ) {
        Database db{ ":memory:" };
        auto added = db.add_meeting(meeting);
        REQUIRE(added.m_err == Database::ErrorCode::ok);
    }
    
}

TEST_CASE( "assigning meeting to user in database", "[user][meeting][database]") {
    Meeting meeting{ std::chrono::day{22}/3/2023, Hours{ 2 }};
    User user{ "Walter" };

    SECTION( "should create a new meeting and assign it" ) {
        Database db{ ":memory:" };
        auto added_user = db.add_user(user);
        REQUIRE(added_user.m_err == Database::ErrorCode::ok);

        auto code = db.add_meeting_to_user(meeting, *added_user.m_some);
        REQUIRE(code == Database::ErrorCode::ok);
    }

    SECTION( "should assign an existing meeting" ) {
        Database db{ ":memory:" };
        auto added_user = db.add_user(user);
        REQUIRE(added_user.m_err == Database::ErrorCode::ok);

        auto added_meeting = db.add_meeting(meeting);
        REQUIRE(added_meeting.m_err == Database::ErrorCode::ok);

        auto code = db.add_meeting_to_user(*added_user.m_some, *added_meeting.m_some);
        REQUIRE(code == Database::ErrorCode::ok);
    }
    
}

TEST_CASE( "finding meeting in database", "[meeting][database]") {
    Meeting meeting{ std::chrono::day{22}/3/2023, Hours{ 2 }};
    SECTION( "should insert to database and find it by id" ) {
        Database db{ ":memory:" };
        auto code = db.add_meeting(meeting);
        REQUIRE(code.m_err == Database::ErrorCode::ok);

        // the previous test should test if 1 is the valid ID
        auto found_meeting = db.find_meeting(1);
        REQUIRE(found_meeting.m_err == Database::ErrorCode::ok);
        REQUIRE(found_meeting.m_some->get_id() == 1);
        REQUIRE(found_meeting.m_some->get_date() == meeting.get_date());
        REQUIRE(found_meeting.m_some->get_duration() == meeting.get_duration());
    }
}

TEST_CASE( "fetching users' meetings from database", "[user][meeting][database]") {
    Meeting meeting{ std::chrono::day{22}/3/2023, Hours{ 2 }};
    User user{ "Walter" };

    SECTION( "should return meetings" ) {
        Database db{ ":memory:" };
        auto added_user = db.add_user(user);
        REQUIRE(added_user.m_err == Database::ErrorCode::ok);

        auto code = db.add_meeting_to_user(meeting, *added_user.m_some);
        REQUIRE(code == Database::ErrorCode::ok);
        code = db.add_meeting_to_user(meeting, *added_user.m_some);
        REQUIRE(code == Database::ErrorCode::ok);

        auto meetings = db.get_user_meeting(*added_user.m_some);
        REQUIRE(meetings.m_err == Database::ErrorCode::ok);

        REQUIRE(meetings.m_some->size() == 2);
        REQUIRE(meetings.m_some->at(0).get_date() == meeting.get_date());
        REQUIRE(meetings.m_some->at(1).get_date() == meeting.get_date());

        REQUIRE(meetings.m_some->at(0).get_duration() == meeting.get_duration());
        REQUIRE(meetings.m_some->at(1).get_duration() == meeting.get_duration());
    }

}

TEST_CASE( "updating meeting in database", "[meeting][database]") {
    Meeting meeting{ std::chrono::day{22}/3/2023, Hours{ 2 }};

    Meeting new_meeting{ std::chrono::day{25}/4/2023, Hours{ 1 } };

    SECTION("should insert to database and update it" ) {
        Database db{ ":memory:" };

        auto added = db.add_meeting(meeting);
        REQUIRE(added.m_err == Database::ErrorCode::ok);

        auto code = db.update_meeting(*added.m_some, new_meeting);
        REQUIRE(code == Database::ErrorCode::ok);

        auto found_meeting = db.find_meeting(*added.m_some);
        
        REQUIRE(found_meeting.m_err == Database::ErrorCode::ok);
        REQUIRE(found_meeting.m_some->get_date() == new_meeting.get_date());
        REQUIRE(found_meeting.m_some->get_duration() == new_meeting.get_duration());
    }
}