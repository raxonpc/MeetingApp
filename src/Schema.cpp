#include "MeetingLib/Schema.hpp"
#include <algorithm>
#include <stdexcept>

namespace
{
    bool validate_nickname(std::string_view nickname)
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
}

namespace MeetingLib
{
    User::User(std::string_view nickname)
    {
        if(!validate_nickname(nickname)) {
            throw std::runtime_error{ 
                "The nickname can only consist of ASCII / ISO 8859-1 (Latin-1) letters" };
        }
    }

    User::User(std::string_view nickname, int id)
    : m_nickname{ nickname }, m_id{ id } {
        // can only be used by Database
    }
}