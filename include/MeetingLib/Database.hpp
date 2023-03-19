#pragma once

#include <filesystem>
#include <vector>
#include <utility>
#include <string>
#include "Schema.hpp"
		
namespace MeetingLib {
    class Database {
    public:
        enum class ErrorCode {
            ok,
            databaseNotCreated,
            invalidNickname,
            invalidDate,
            invalidDuration,
            invalidStart,
            userAlreadyExists,
            userNotFound,
            meetingNotFound,
            nonExistentField,
            internalError,
        };
        template<typename T>
        struct Result {
            std::optional<T> m_some;
            ErrorCode m_err{ ErrorCode::ok };
        };
    public:
        Database(const std::filesystem::path& path_to_db);
        ~Database();

        Result<int> add_user(const User& user) noexcept;
        Result<User> find_user(std::string_view nickname) noexcept;
        Result<User> find_user(int id) noexcept;

        ErrorCode update_user(int id, std::string_view new_nickname) noexcept;
        ErrorCode delete_user(int id) noexcept;

        Result<int> add_meeting(const Meeting& meeting) noexcept;
        Result<Meeting> find_meeting(int id) noexcept;
        ErrorCode add_meeting_to_user(const Meeting& meeting, int user_id) noexcept;
        ErrorCode add_meeting_to_user(int user_id, int meeting_id) noexcept;

        ErrorCode update_meeting(int id, const Meeting& new_meeting) noexcept;
        ErrorCode delete_meeting(int id) noexcept;

        Result<std::vector<Meeting>> get_user_meetings(int id) noexcept;
        Result<std::vector<Meeting>> get_user_meetings(int id, const Date& date) noexcept;

        Result<int> arrange_meeting(Meeting meeting_data, const std::vector<int>& user_ids) noexcept;

    private:
        struct Impl;
        Impl* m_impl;

        void create();
    };
}

