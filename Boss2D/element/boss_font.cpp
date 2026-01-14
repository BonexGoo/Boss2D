#include <boss.hpp>
#include "boss_font.hpp"

namespace BOSS
{
    Font::Font()
    {
        is_freefont = false;
        system_name = "Arial";
        system_size = 12;
        freefont_height = 0;
    }

    Font::Font(const Font& rhs)
    {
        operator=(rhs);
    }

    Font::~Font()
    {
    }

    Font& Font::operator=(const Font& rhs)
    {
        is_freefont = rhs.is_freefont;
        system_name = rhs.system_name;
        system_size = rhs.system_size;
        freefont_nickname = rhs.freefont_nickname;
        freefont_height = rhs.freefont_height;
        return *this;
    }
}
