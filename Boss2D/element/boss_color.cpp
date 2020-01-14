#include <boss.hpp>
#include "boss_color.hpp"

namespace BOSS
{
    Color::Color()
    {
        argb = 0xFF000000;
    }

    Color::Color(uint32 argb)
    {
        this->argb = argb;
    }

    Color::Color(uint08 r, uint08 g, uint08 b, uint08 a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color::Color(chars rgb_or_rgba)
    {
        operator=(rgb_or_rgba);
    }

    Color::Color(chars rgb, float a)
    {
        operator=(rgb);
        this->a = Math::Clamp(255 * a, 0, 255);
    }

    Color::Color(const Color& rhs)
    {
        operator=(rhs);
    }

    Color::~Color()
    {
    }

    Color& Color::operator=(const Color& rhs)
    {
        argb = rhs.argb;
        return *this;
    }

    Color& Color::operator*=(const Color& rhs)
    {
        r = (uint08) ((0x80 < rhs.r)? (0xFF - ((0xFF - r) * (uint32) (0xFF - rhs.r) / 0x7F)) : (r * (uint32) rhs.r / 0x80));
        g = (uint08) ((0x80 < rhs.g)? (0xFF - ((0xFF - g) * (uint32) (0xFF - rhs.g) / 0x7F)) : (g * (uint32) rhs.g / 0x80));
        b = (uint08) ((0x80 < rhs.b)? (0xFF - ((0xFF - b) * (uint32) (0xFF - rhs.b) / 0x7F)) : (b * (uint32) rhs.b / 0x80));
        a = (uint08) ((0x80 < rhs.a)? (0xFF - ((0xFF - a) * (uint32) (0xFF - rhs.a) / 0x7F)) : (a * (uint32) rhs.a / 0x80));
        return *this;
    }

    Color Color::operator*(const Color& rhs) const
    {
        return Color(*this).operator*=(rhs);
    }

    Color& Color::operator=(chars rhs)
    {
        auto ToInt = [](char OneChar)->sint32
        {
            if('0' <= OneChar && OneChar <= '9') return OneChar - '0';
            if('a' <= OneChar && OneChar <= 'f') return 10 + OneChar - 'a';
            if('A' <= OneChar && OneChar <= 'F') return 10 + OneChar - 'A';
            return 0;
        };

        if(rhs[0] == '#')
        {
            r = (uint08) ((ToInt(rhs[1]) << 4) | ToInt(rhs[2]));
            g = (uint08) ((ToInt(rhs[3]) << 4) | ToInt(rhs[4]));
            b = (uint08) ((ToInt(rhs[5]) << 4) | ToInt(rhs[6]));
            if(rhs[7] == '\0') a = 0xFF;
            else a = (uint08) ((ToInt(rhs[7]) << 4) | ToInt(rhs[8]));
        }
        return *this;
    }

    Color::operator chars() const
    {
        String& Result = *BOSS_STORAGE(String);
        Result = String::Format("#%02X%02X%02X%02X", r, g, b, a);
        return Result;
    }
}
