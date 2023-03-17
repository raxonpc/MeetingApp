#include "MeetingLib/Schema.hpp"
#include <algorithm>
#include <stdexcept>

namespace MeetingLib
{
    bool is_nickname_valid(std::string_view nickname) 
    {
        if(nickname.size() < 1) return false;

        // the nickname can only consist of letters
        if (std::find_if_not(nickname.cbegin(), nickname.cend(), [](const auto ch)
                             { return isalpha(ch); }) != nickname.cend())
        {
            return false;
        }

        return true;
    }

    User::User(std::string_view nickname)
    {
        if(!is_nickname_valid(nickname)) {
            throw std::runtime_error{ 
                "The nickname can only consist of ASCII / ISO 8859-1 (Latin-1) letters" };
        }
        m_nickname = nickname;
    }

    User::User(std::string_view nickname, int id)
    {
        if(!is_nickname_valid(nickname)) {
            throw std::runtime_error{ 
                "The nickname can only consist of ASCII / ISO 8859-1 (Latin-1) letters" };
        }
        m_nickname = nickname;
        set_id(id);
    }

    std::optional<int> User::get_id() const noexcept {
        return m_id;
    }

    std::string User::get_nickname() const noexcept {
        return m_nickname;
    }

    void User::set_id(int id) noexcept {
        if(id >= 1) {
            m_id = id;
        }
    }

    void User::set_nickname(std::string_view nickname) noexcept {
        if(is_nickname_valid(nickname)) {
            m_nickname = nickname;
        }
    }

    Meeting::Meeting(const Date& date, const Hours& duration)
    {
        if(!date.ok()) {
            throw std::runtime_error{
                "Invalid date entered!"
            };
        }
        if(duration.count() <= 0) {
            throw std::runtime_error{
                "Invalid duration!"
            };
        }
        m_duration = duration;
        m_date = date;
    }

    Meeting::Meeting(const Date& date, const Hours& duration, int id)
    {
        if(!date.ok()) {
            throw std::runtime_error{
                "Invalid date!"
            };
        }
        if(duration.count() <= 0) {
            throw std::runtime_error{
                "Invalid duration!"
            };
        }
        m_date = date;
        m_duration = duration;
        set_id(id);
    }

    std::optional<int> Meeting::get_id() const noexcept {
        return m_id;
    }
    Date Meeting::get_date() const noexcept {
        return m_date;
    }

    Hours Meeting::get_duration() const noexcept {
        return m_duration;
    }

    void Meeting::set_id(int id) noexcept {
        if(id >= 1) {
            m_id = id;
        }
    }

    void Meeting::set_date(const Date& date) noexcept {
        if(date.ok()) {
            m_date = date;
        }
    }

    std::string date_to_string(const Date& date) {
        std::string output;
        output.reserve(11);
        output += std::to_string(static_cast<int>(date.year()));
        output += '/';
        output += std::to_string(static_cast<unsigned int>(date.month()));
        output += '/';
        output += std::to_string(static_cast<unsigned int>(date.day()));

        return output;
    }
}