#include "MeetingLib/Database.hpp"
#include "sqlite3.h"
#include <cassert>

namespace MeetingLib
{
    struct Database::Impl
    {
        sqlite3 *m_db;
    };

    static constexpr std::array<Meeting 13> Database::get_holidays() const noexcept; 
    



    Database::Database(const std::filesystem::path &path_to_db)
        : m_impl{new Impl}
    {
        // try to open the database
        int status = sqlite3_open(path_to_db.c_str(), &m_impl->m_db);
        if (status != SQLITE_OK)
        {
            throw std::runtime_error{std::string{"Cannot open database: "} + path_to_db.c_str()};
        }

        create();
    }

    Database::~Database()
    {
        sqlite3_close(m_impl->m_db);
        delete m_impl;
    }

    Database::Result<int> Database::add_user(const User &user) noexcept
    {
        if(!is_nickname_valid(user.get_nickname())) {
            return { .m_err = ErrorCode::invalidNickname };
        }

        sqlite3_stmt *insert_stmt;

        int status = sqlite3_prepare_v2(m_impl->m_db, "insert into Users (nickname) values (?);", -1, &insert_stmt, NULL);
        if (status != SQLITE_OK)
        {
            return { .m_err = ErrorCode::internalError };
        }
        status = sqlite3_bind_text(insert_stmt, 1, user.get_nickname().c_str(), -1, SQLITE_TRANSIENT);
        if (status != SQLITE_OK)
        {
            return { .m_err = ErrorCode::internalError };
        }
        status = sqlite3_step(insert_stmt);
        sqlite3_finalize(insert_stmt);
        switch (status)
        {
            case SQLITE_DONE: break;
            case SQLITE_CONSTRAINT: return { .m_err = ErrorCode::userAlreadyExists };
            default: return { .m_err = ErrorCode::internalError };
        }

        return { .m_some = sqlite3_last_insert_rowid(m_impl->m_db), .m_err = ErrorCode::ok };
    }

    Database::Result<User> Database::find_user(std::string_view nickname) noexcept {
        std::optional<User> output;
        if(!is_nickname_valid(nickname)) {
            return Result<User>{ .m_some = std::nullopt, .m_err = ErrorCode::invalidNickname };
        }

        std::string select_query = "select * from Users where nickname = \'";
        select_query += nickname;
        select_query += "\';";

        auto add_callback = [](void* out, int argc, char** argv, char** colNames) {
            auto user = static_cast<std::optional<User>*>(out);
            *user = User{ argv[1], std::stoi(argv[0])};
            return 0;
        };

        int status = sqlite3_exec(m_impl->m_db, select_query.c_str(), add_callback, (void*)&output, nullptr);

        if(status != SQLITE_OK) {
            return Result<User>{ .m_some = std::nullopt, .m_err = ErrorCode::internalError };
        }

        if(output == std::nullopt) {
            return Result<User>{ .m_some = std::nullopt, .m_err = ErrorCode::userNotFound };
        }
        return Result<User>{ .m_some = output, .m_err = ErrorCode::ok };
    }

    Database::Result<User> Database::find_user(int id) noexcept {
        std::optional<User> output;

        std::string select_query = "select * from Users where id = \'";
        select_query += std::to_string(id);
        select_query += "\';";

        auto add_callback = [](void* out, int argc, char** argv, char** colNames) {
            auto user = static_cast<std::optional<User>*>(out);
            *user = User{ argv[1], std::stoi(argv[0])};
            return 0;
        };

        int status = sqlite3_exec(m_impl->m_db, select_query.c_str(), add_callback, (void*)&output, nullptr);

        if(status != SQLITE_OK) {
            return Result<User>{ .m_some = std::nullopt, .m_err = ErrorCode::internalError };
        }

        if(output == std::nullopt) {
            return Result<User>{ .m_some = std::nullopt, .m_err = ErrorCode::userNotFound };
        }
        return Result<User>{ .m_some = output, .m_err = ErrorCode::ok };
    }

    Database::ErrorCode Database::update_user(int id, std::string_view new_nickname) noexcept {
        auto user = find_user(id);
        if(user.m_err != ErrorCode::ok) {
            return user.m_err;
        }
        if(!is_nickname_valid(new_nickname)) {
            return ErrorCode::invalidNickname;
        }

        std::string update_query = 
            "update Users SET nickname = \'";
        update_query += new_nickname;
        update_query += "\' where id = \'";
        update_query += std::to_string(id);
        update_query += "\';";

        int status = sqlite3_exec(m_impl->m_db, update_query.c_str(), nullptr, nullptr, nullptr);
    
        if(status != SQLITE_OK) {
            return ErrorCode::internalError;
        }
    
        return ErrorCode::ok;
    };

    Database::ErrorCode Database::delete_user(int id) noexcept {
        auto found_user = find_user(id);
        if(found_user.m_err != ErrorCode::ok) {
            return found_user.m_err;
        }

        std::string delete_query = "delete from Users where id = \'";
        delete_query += std::to_string(id);
        delete_query += "\';";

        int status = sqlite3_exec(m_impl->m_db, delete_query.c_str(), nullptr, nullptr, nullptr);
    
        if(status != SQLITE_OK) {
            return ErrorCode::internalError;
        }
    
        return ErrorCode::ok;
    }

    Database::Result<int> Database::add_meeting(const Meeting& meeting) noexcept {
        sqlite3_stmt *insert_stmt;

        int status = sqlite3_prepare_v2(m_impl->m_db, "insert into Meetings (date, start, duration) values (?, ?, ?);", -1, &insert_stmt, NULL);
        if (status != SQLITE_OK)
        {
            return { .m_err = ErrorCode::internalError };
        }
        status = sqlite3_bind_text(insert_stmt, 1, date_to_string(meeting.get_date()).c_str(), -1, SQLITE_TRANSIENT);
        if (status != SQLITE_OK)
        {
            return { .m_err = ErrorCode::internalError };
        }
        status = sqlite3_bind_int(insert_stmt, 2, meeting.get_start().count());
        if (status != SQLITE_OK)
        {
            return { .m_err = ErrorCode::internalError };
        }
        status = sqlite3_bind_int(insert_stmt, 3, meeting.get_duration().count());
        if (status != SQLITE_OK)
        {
            return { .m_err = ErrorCode::internalError };
        }
        status = sqlite3_step(insert_stmt);
        sqlite3_finalize(insert_stmt);
        switch (status)
        {
            case SQLITE_DONE: break;
            default: return { .m_err = ErrorCode::internalError };
        }

        return { .m_some = sqlite3_last_insert_rowid(m_impl->m_db), .m_err = ErrorCode::ok };
    }
    
    Database::Result<Meeting> Database::find_meeting(int id) noexcept {
        std::optional<Meeting> output;

        std::string select_query = "select * from Meetings where id = \'";
        select_query += std::to_string(id);
        select_query += "\';";

        auto add_callback = [](void* out, int argc, char** argv, char** colNames) {
            auto meeting = static_cast<std::optional<Meeting>*>(out);
            int year = 0;
            int month = 0;
            unsigned int day = 0;
            sscanf(argv[1], "%d/%d/%d", &year, &month, &day);
            *meeting = Meeting{ Date{ std::chrono::day{day}/month/year },
                                      std::chrono::hours{ std::atoi(argv[2]) },
                                      std::chrono::hours{ std::atoi(argv[3]) },
                                      std::stoi(argv[0]) };
            return 0;
        };

        int status = sqlite3_exec(m_impl->m_db, select_query.c_str(), add_callback, (void*)&output, nullptr);

        if(status != SQLITE_OK) {
            return { .m_some = std::nullopt, .m_err = ErrorCode::internalError };
        }

        if(output == std::nullopt) {
            return { .m_some = std::nullopt, .m_err = ErrorCode::meetingNotFound };
        }
        return { .m_some = output, .m_err = ErrorCode::ok };
    }

    Database::ErrorCode Database::add_meeting_to_user(const Meeting& meeting, int user_id) noexcept {
        auto added_meeting = add_meeting(meeting);
        if(added_meeting.m_err != ErrorCode::ok) {
            return added_meeting.m_err;
        }

        auto found_user = find_user(user_id);
        if(found_user.m_err != ErrorCode::ok) {
            return found_user.m_err;
        }

        std::string insert_query = "insert into Users_Meetings (user_id, meeting_id) values (";
        insert_query += std::to_string(user_id);
        insert_query += ", ";
        insert_query += std::to_string(*added_meeting.m_some);
        insert_query += ");";

        int status = sqlite3_exec(m_impl->m_db, insert_query.c_str(), nullptr, nullptr, nullptr);
    
        switch(status) {
            case SQLITE_CONSTRAINT: return ErrorCode::nonExistentField;
            case SQLITE_OK: return ErrorCode::ok;
            default: return ErrorCode::internalError;
        }
    }

    Database::ErrorCode Database::add_meeting_to_user(int user_id, int meeting_id) noexcept {
        auto found_meeting = find_meeting(meeting_id);
        if(found_meeting.m_err != ErrorCode::ok) {
            return found_meeting.m_err;
        }

        auto found_user = find_user(user_id);
        if(found_user.m_err != ErrorCode::ok) {
            return found_user.m_err;
        }

        std::string insert_query = "insert into Users_Meetings (user_id, meeting_id) values (";
        insert_query += std::to_string(user_id);
        insert_query += ", ";
        insert_query += std::to_string(meeting_id);
        insert_query += ");";

        int status = sqlite3_exec(m_impl->m_db, insert_query.c_str(), nullptr, nullptr, nullptr);
    
        switch(status) {
            case SQLITE_CONSTRAINT: return ErrorCode::nonExistentField;
            case SQLITE_OK: return ErrorCode::ok;
            default: return ErrorCode::internalError;
        }
    }

    Database::Result<std::vector<Meeting>> Database::get_user_meetings(int id) noexcept {
        auto found_user = find_user(id);
        if(found_user.m_err != ErrorCode::ok) {
            return { .m_err = found_user.m_err };
        }

        auto add_callback = [](void* out, int argc, char** argv, char** colNames) {
            auto meeting_vec = static_cast<std::vector<Meeting>*>(out);
            int year = 0;
            int month = 0;
            unsigned int day = 0;
            sscanf(argv[1], "%d/%d/%d", &year, &month, &day);
            meeting_vec->emplace_back(Date{ std::chrono::day{day}/month/year },
                                      std::chrono::hours{ std::atoi(argv[2]) },
                                      std::chrono::hours{ std::atoi(argv[3]) },
                                      std::stoi(argv[0]));
            return 0;
        };

        std::string query = "SELECT Meetings.* FROM Meetings "
                            "JOIN Users_Meetings ON Meetings.id = Users_Meetings.meeting_id "
                            "WHERE Users_Meetings.user_id = " + std::to_string(id);

        std::vector<Meeting> output;
        int status = sqlite3_exec(m_impl->m_db, query.c_str(), add_callback, (void*)&output, nullptr);
        if(status != SQLITE_OK) {
            return { .m_some = std::nullopt, .m_err = ErrorCode::internalError };
        }
        return { .m_some = output, .m_err = ErrorCode::ok };
    }
 
    Database::Result<std::vector<Meeting>> Database::get_user_meetings(int id, const Date& date) noexcept {
        auto found_user = find_user(id);
        if(found_user.m_err != ErrorCode::ok) {
            return { .m_err = found_user.m_err };
        }

        auto add_callback = [](void* out, int argc, char** argv, char** colNames) {
            auto meeting_vec = static_cast<std::vector<Meeting>*>(out);
            int year = 0;
            int month = 0;
            unsigned int day = 0;
            sscanf(argv[1], "%d/%d/%d", &year, &month, &day);
            meeting_vec->emplace_back(Date{ std::chrono::day{day}/month/year },
                                      std::chrono::hours{ std::atoi(argv[2]) },
                                      std::chrono::hours{ std::atoi(argv[3]) },
                                      std::stoi(argv[0]));
            return 0;
        };

        std::string query = "SELECT Meetings.* FROM Meetings "
                            "JOIN Users_Meetings ON Meetings.id = Users_Meetings.meeting_id "
                            "WHERE Users_Meetings.user_id = " + std::to_string(id);
        
        query += " AND date = \'" + date_to_string(date) + "\';";

        std::vector<Meeting> output;
        int status = sqlite3_exec(m_impl->m_db, query.c_str(), add_callback, (void*)&output, nullptr);
        if(status != SQLITE_OK) {
            return { .m_some = std::nullopt, .m_err = ErrorCode::internalError };
        }
        return { .m_some = output, .m_err = ErrorCode::ok };
    }

    Database::ErrorCode Database::update_meeting(int id, const Meeting& new_meeting) noexcept {
        auto meeting = find_meeting(id);
        if(meeting.m_err != ErrorCode::ok) {
            return meeting.m_err;
        }

        if(!new_meeting.get_date().ok()) {
            return ErrorCode::invalidNickname;
        }
        if(new_meeting.get_start().count() < 0) {
            return ErrorCode::invalidStart;
        }
        if(new_meeting.get_duration().count() <= 0) {
            return ErrorCode::invalidDuration;
        }

        std::string update_query = 
            "update Meetings SET date = \'";
        update_query += date_to_string(new_meeting.get_date());
        update_query += "\', start = \'";
        update_query += std::to_string(new_meeting.get_start().count());
        update_query += "\', duration = \'";
        update_query += std::to_string(new_meeting.get_duration().count());
        update_query += "\' where id = \'";
        update_query += std::to_string(id);
        update_query += "\';";

        int status = sqlite3_exec(m_impl->m_db, update_query.c_str(), nullptr, nullptr, nullptr);
    
        if(status != SQLITE_OK) {
            return ErrorCode::internalError;
        }
    
        return ErrorCode::ok;
    }

    Database::ErrorCode Database::delete_meeting(int id) noexcept {
        auto found_meeting = find_meeting(id);

        if(found_meeting.m_err != ErrorCode::ok) {
            return found_meeting.m_err;
        }

        std::string delete_query = "delete from Meetings where id = \'";
        delete_query += std::to_string(id);
        delete_query += "\';";

        int status = sqlite3_exec(m_impl->m_db, delete_query.c_str(), nullptr, nullptr, nullptr);
    
        if(status != SQLITE_OK) {
            return ErrorCode::internalError;
        }
    
        return ErrorCode::ok;
    }

    Database::Result<int> Database::arrange_meeting(Meeting meeting_data, const std::vector<int>& user_ids) noexcept {
        std::vector<Meeting> meetings;
        for(int user_id : user_ids) {
            auto user_meetings = get_user_meetings(user_id);
            if(user_meetings.m_err != ErrorCode::ok) {
                return { .m_err = user_meetings.m_err };
            }
            
            for(const auto& meeting : *user_meetings.m_some) {
                meetings.push_back(meeting);
            }
        }


        // if a good spare date is found it is set to true
        bool found_match = false;
        while(!found_match) {
            found_match = true;
            for(const auto meeting : meetings) {
                if(
                    (meeting.get_start() <= meeting_data.get_start() && 
                    meeting.get_start() + meeting.get_duration() > meeting_data.get_start()) ||
                    (meeting_data.get_start() <= meeting.get_start() && 
                    meeting_data.get_start() + meeting_data.get_duration() > meeting.get_start())
                    ) {
                        meeting_data.postpone_by(Hours{ 1 });
                        if(meeting_data.get_start().count() == 0) {
                            // so there are no night meetings
                            meeting_data.postpone_by(Hours{ 8 });
                        }

                        found_match = false;
                        break;

                }
            }
        }

        auto added_meeting = add_meeting(meeting_data);
        if(added_meeting.m_err != ErrorCode::ok) {
            return { .m_err = added_meeting.m_err };
        }

        for(int user_id : user_ids) {
            auto code = add_meeting_to_user(user_id, *added_meeting.m_some);
            if(code != ErrorCode::ok) {
                return { .m_err = code };
            }
        }

        return { .m_some = *added_meeting.m_some };
    }

    void Database::create()
    {
        static constexpr std::string_view create_base_query{
            "PRAGMA foreign_keys = ON;"
            "CREATE TABLE IF NOT EXISTS Users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "nickname TEXT NOT NULL UNIQUE);"

            "CREATE TABLE IF NOT EXISTS Meetings ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "date TEXT NOT NULL,"
            "start INTEGER NOT NULL,"
            "duration INTEGER NOT NULL);"

            "CREATE TABLE IF NOT EXISTS Users_Meetings ("
            "user_id INTEGER REFERENCES Users(id),"
            "meeting_id INTEGER REFERENCES Meetings(id));"};

        char *error_msg = nullptr;
        int status = sqlite3_exec(m_impl->m_db, create_base_query.data(), nullptr, nullptr, &error_msg);

        if (status != SQLITE_OK)
        {
            std::string exception_msg{"Error creating database"};
            exception_msg += error_msg;
            sqlite3_free(error_msg);
            throw std::runtime_error{exception_msg};
        }
        sqlite3_free(error_msg);
    }


}