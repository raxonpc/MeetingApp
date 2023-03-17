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
    User() = default;
    User(std::string_view nickname);
    User(std::string_view nickname, int id);

    std::optional<int> getId() const noexcept;
    std::string getNickname() const noexcept;
    
    void setId(int) noexcept;
    void setNickname(std::string_view) noexcept;
private:
    std::string m_nickname{};
    std::optional<int> m_id{ std::nullopt };
};

struct Meeting {
public:
    Meeting(const Date& date);
    Meeting(const Date& date, int id);
    
    void setId(int) noexcept;
private:
    Date m_date{};
    std::optional<int> m_id{ std::nullopt };
};

}