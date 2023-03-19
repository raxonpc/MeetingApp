#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <chrono>

namespace MeetingLib {

using Date = std::chrono::year_month_day;
using Hours = std::chrono::hours;

bool is_nickname_valid(std::string_view nickname);

struct User {
public:
    User() = default;
    User(std::string_view nickname);
    User(std::string_view nickname, int id);

    std::optional<int> get_id() const noexcept;
    std::string get_nickname() const noexcept;
    
    void set_id(int) noexcept;
    void set_nickname(std::string_view) noexcept;
private:
    std::string m_nickname{};
    std::optional<int> m_id{ std::nullopt };
};

struct Meeting {
public:
    Meeting(const Date& date, const Hours& start, const Hours& duration);
    Meeting(const Date& date, const Hours& start, const Hours& duration, int id);
    
    std::optional<int> get_id() const noexcept;
    Date get_date() const noexcept;
    Hours get_start() const noexcept;
    Hours get_duration() const noexcept;

    void set_id(int) noexcept;
    void set_date(const Date&) noexcept;
    void set_start(const Hours&) noexcept;
    void set_duration(const Hours&) noexcept;
private:
    Date m_date{};
    Hours m_start{};
    Hours m_duration{};
    std::optional<int> m_id{ std::nullopt };
};

// TODO:
std::string date_to_string(const Date& date);

}