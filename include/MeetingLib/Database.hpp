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
            userAlreadyExists,
            userNotFound,
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

        ErrorCode add_user(const User&) noexcept;
        Result<User> find_user(std::string_view) noexcept;
        Result<User> find_user(int) noexcept;

        ErrorCode update_user(int, std::string_view) noexcept;
        ErrorCode delete_user(int) noexcept;


        
    private:
        struct Impl;
        Impl* m_impl;

        void create_database();
    };
}

