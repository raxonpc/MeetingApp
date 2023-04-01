#include <MeetingLib/Database.hpp>
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
    std::string db_path{ "meeting.db" };
    if(argc > 1) {
        db_path = argv[1];
    }
    if(std::filesystem::exists(db_path)) {
        std::filesystem::remove(db_path);
    }

    using namespace MeetingLib;

    // CREATING THE DATABASE
    Database db{ db_path };

    // ADDING USERS
    User mark{ "Mark" };
    User steve{ "Steve" };

    auto added_mark = db.add_user(mark);
    auto added_steve = db.add_user(steve);

    int mark_id{};
    int steve_id{};

    if(added_mark.m_err == Database::ErrorCode::ok) {
        mark_id = *added_mark.m_some;
        std::cout << "Added User of id: " << mark_id << '\n';
    } else {
        std::cerr << "Error adding User\n";
        return 1;
    }

    if(added_steve.m_err == Database::ErrorCode::ok) {
        steve_id = *added_steve.m_some;
        std::cout << "Added User of id: " << steve_id << '\n';
    } else {
        std::cerr << "Error adding User\n";
        return 1;
    }

    // ADDING MEETING

    Meeting meeting_mark{
        std::chrono::day{ 2 }/4/2023,
        Hours{ 12 },
        Hours{ 2 }
    }; // 12 - 14

    Meeting meeting_steve{
            std::chrono::day{ 2 }/4/2023,
            Hours{ 14 },
            Hours{ 1 }
    }; // 14 - 15

    db.add_meeting_to_user(meeting_mark, mark_id);
    db.add_meeting_to_user(meeting_steve, steve_id);

    // ARRANGING MEETINGS
    Meeting planned_meeting{
            std::chrono::day{ 2 }/4/2023,
            Hours{ 10 },
            Hours{ 3 }
    };

    auto arranged = db.arrange_meeting(planned_meeting, { mark_id, steve_id });
    int arranged_id{};

    if(arranged.m_err == Database::ErrorCode::ok) {
        arranged_id = *arranged.m_some;
        std::cout << "Arranged a meeting of id: " << arranged_id << '\n';
    } else {
        std::cerr << "Error arranging a meeting\n";
        return 1;
    }

    // GET MEETING DATA
    auto found_meeting = db.find_meeting(arranged_id);
    if(found_meeting.m_err != Database::ErrorCode::ok) {
        std::cerr << "Error finding meeting\n";
        return 1;
    }
    Meeting arranged_meeting = *found_meeting.m_some;
    std::cout << "Meeting date: "
        << date_to_string(arranged_meeting.get_date())
        << "; time: " << arranged_meeting.get_start().count()
        << ":00; duration: " << arranged_meeting.get_duration().count()
        << "h\n";

    // GETTING USER'S MEETINGS
    auto mark_meetings = db.get_user_meetings(mark_id);
    if(mark_meetings.m_err != Database::ErrorCode::ok) {
        std::cerr << "Error getting user's meetings\n";
        return 1;
    }

    auto meeting_list = *mark_meetings.m_some;
    std::cout << "Mark's meetings:\n";
    for(const auto& meeting : meeting_list) {
        std::cout << "Meeting date: "
                  << date_to_string(meeting.get_date())
                  << "; time: " << meeting.get_start().count()
                  << ":00; duration: " << meeting.get_duration().count()
                  << "h\n";
    }

    return 0;
}