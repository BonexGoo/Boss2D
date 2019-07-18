﻿#pragma once
#include <boss.hpp>

namespace BOSS
{
    //! \brief 재질-색상
    class Color
    {
    public:
        enum Sample:uint32
        {
            Black =           0xFF000000,
            White =           0xFFFFFFFF,
            DarkGray =        0xFF404040,
            Gray =            0xFF808080,
            LightGray =       0xFFC0C0C0,
            Red =             0xFF0000FF,
            Green =           0xFF00FF00,
            Blue =            0xFFFF0000,
            Cyan =            0xFFFFFF00,
            Magenta =         0xFFFF00FF,
            Yellow =          0xFF00FFFF,
            DarkRed =         0xFF000080,
            DarkGreen =       0xFF008000,
            DarkBlue =        0xFF800000,
            DarkCyan =        0xFF808000,
            DarkMagenta =     0xFF800080,
            DarkYellow =      0xFF008080,
            ColoringDefault = 0x80808080
        };

    public:
        Color();
        Color(const Color& rhs);
        Color(uint32 rgba);
        Color(uint08 r, uint08 g, uint08 b, uint08 a = 0xFF);
        Color(chars rgb_or_rgba);
        Color(chars rgb, float a);
        ~Color();

        Color& operator=(const Color& rhs);
        Color& operator*=(const Color& rhs);
        Color operator*(const Color& rhs) const;
        Color& operator=(chars rhs);
        operator chars() const;

    public:
        union
        {
            uint32 rgba;
            struct
            {
                uint08 r;
                uint08 g;
                uint08 b;
                uint08 a;
            };
        };
    };
    typedef Array<Color, datatype_class_canmemcpy> Colors;
}
