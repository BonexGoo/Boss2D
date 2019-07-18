#pragma once
#include <boss.hpp>

namespace BOSS
{
    //! \brief 재질-폰트
    class Font
    {
    public:
        Font();
        Font(const Font& rhs);
        ~Font();
        Font& operator=(const Font& rhs);

    public:
        bool is_freefont;
        String system_name;
        float system_size;
        String freefont_nickname;
        sint32 freefont_height;
    };
    typedef Array<Font> Fonts;
}
