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
            userAlreadyExists,
            userNotFound,
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

        Result<int> add_user(const User&) noexcept;
        Result<User> find_user(std::string_view) noexcept;
        Result<User> find_user(int) noexcept;

        ErrorCode update_user(int, std::string_view) noexcept;
        ErrorCode delete_user(int) noexcept;


        Result<int> add_meeting(const Meeting&) noexcept;
        ErrorCode add_meeting_to_user(const Meeting&, int) noexcept;

    private:
        struct Impl;
        Impl* m_impl;

        void create();
    };
}

