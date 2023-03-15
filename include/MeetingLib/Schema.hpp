#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <chrono>

namespace MeetingLib {

using Date = std::chrono::year_month_day;

bool is_nickname_valid(std::string_view nickname);

struct User {
public:
    User(std::string_view nickname);
    
    friend class Database;
private:
    User(std::string_view nickname, int id);

    std::string m_nickname{};
    std::optional<int> m_id{ std::nullopt };
};

struct Meeting {
public:
    Meeting(const Date& date);
    
    friend class Database;
private:
    Meeting(const Date& date, int id);

    Date m_date{};
    std::optional<int> m_id{ std::nullopt };
};

}