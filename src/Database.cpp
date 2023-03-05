#include "Database.hpp"
#include "sqlite3.h"

namespace MeetingLib {
    struct Database::Impl {
        sqlite3* m_db;
    };

    Database::Database(const std::filesystem::path& path_to_db)
    : m_impl { new Impl } {
        // try to open the database
        int status = sqlite3_open(path_to_db.c_str(), &m_impl->m_db);
        if(status != SQLITE_OK) {
            throw std::runtime_error{ std::string{ "Cannot open database: " } 
                + path_to_db.c_str() };
        }
    }

    Database::~Database() {
        sqlite3_close(m_impl->m_db);
        delete m_impl;
    }
}