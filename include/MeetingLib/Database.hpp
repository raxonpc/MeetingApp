#pragma once

#include <filesystem>
#include "Schema.hpp"
		
namespace MeetingLib {
    class Database {
    public:
        enum ErrorCode {
            ok,
            databaseNotCreated,
            userAlreadyExists,
            internalError,
        };
    public:
        Database(const std::filesystem::path& path_to_db);
        ~Database();

        ErrorCode add_user(const User& user) noexcept;
        
    private:
        struct Impl;
        Impl* m_impl;

        void create_database();
    };
}

