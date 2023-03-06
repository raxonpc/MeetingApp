#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace MeetingLib {

struct User {
public:
    User(std::string_view nickname);
    
    friend class Database;
private:
    User(std::string_view nickname, int id);

    std::string m_nickname{};
    std::optional<int> m_id{ std::nullopt };
};

}