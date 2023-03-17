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

    Meeting::Meeting(const Date& date)
    {
        if(date.ok()) {
            throw std::runtime_error{
                "Invalid date entered!"
            };
        }
        m_date = date;
    }

    Meeting::Meeting(const Date& date, int id)
    {
        if(date.ok()) {
            throw std::runtime_error{
                "Invalid date entered!"
            };
        }
        m_date = date;
        set_id(id);
    }

    void Meeting::set_id(int id) noexcept {
        if(id >= 1) {
            m_id = id;
        }
    }
}