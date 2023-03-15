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
    : m_nickname{ nickname }, m_id{ id } {
        // can only be used by Database
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
    : m_date{ date }, m_id{ id }
    // can only be used by Database
    {
    }
}