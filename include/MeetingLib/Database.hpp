#pragma once

#include <filesystem>
#include "Schema.hpp"
		
namespace MeetingLib {
    class Database {
    public:
        Database(const std::filesystem::path& path_to_db);
        ~Database();
        
    private:
        struct Impl;
        Impl* m_impl;

        void create_database();
    };
}

