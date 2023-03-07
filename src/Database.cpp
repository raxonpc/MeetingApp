#include "MeetingLib/Database.hpp"
#include "sqlite3.h"
#include <cassert>

namespace MeetingLib
{
    struct Database::Impl
    {
        sqlite3 *m_db;
    };

    Database::Database(const std::filesystem::path &path_to_db)
        : m_impl{new Impl}
    {
        // try to open the database
        int status = sqlite3_open(path_to_db.c_str(), &m_impl->m_db);
        if (status != SQLITE_OK)
        {
            throw std::runtime_error{std::string{"Cannot open database: "} + path_to_db.c_str()};
        }

        create_database();
    }

    Database::~Database()
    {
        sqlite3_close(m_impl->m_db);
        delete m_impl;
    }

    Database::ErrorCode Database::add_user(const User &user) noexcept
    {
        if (m_impl->m_db == nullptr)
        {
            assert(false && "Handle should never be null");
        }

        sqlite3_stmt *insert_stmt;

        int status = sqlite3_prepare_v2(m_impl->m_db, "insert into Users (nickname) values (?);", -1, &insert_stmt, NULL);
        if (status != SQLITE_OK)
        {
            return ErrorCode::internalError;
        }
        status = sqlite3_bind_text(insert_stmt, 1, user.m_nickname.c_str(), -1, SQLITE_TRANSIENT);
        if (status != SQLITE_OK)
        {
            return ErrorCode::internalError;
        }
        status = sqlite3_step(insert_stmt);
        sqlite3_finalize(insert_stmt);
        switch (status)
        {
            case SQLITE_DONE: break;
            case SQLITE_CONSTRAINT: return ErrorCode::userAlreadyExists;
            default: return ErrorCode::internalError;
        }

        return ErrorCode::ok;
    }

    void Database::create_database()
    {
        static constexpr std::string_view create_base_query{
            "PRAGMA foreign_keys = ON;"
            "CREATE TABLE IF NOT EXISTS Users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "nickname TEXT NOT NULL UNIQUE);"

            "CREATE TABLE IF NOT EXISTS Meetings ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "datetime TEXT NOT NULL,"
            "duration_hours INTEGER,"
            "duration_minutes INTEGER);"

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
    }
}