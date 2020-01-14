#include <boss.hpp>
#include "boss_size.hpp"

namespace BOSS
{
    Size::Size()
    {
        w = 0;
        h = 0;
    }

    Size::Size(const size64f& rhs)
    {
        operator=(rhs);
    }

    Size::Size(float w, float h)
    {
        this->w = w;
        this->h = h;
    }

    Size::~Size()
    {
    }

    Size& Size::operator=(const size64f& rhs)
    {
        w = rhs.w;
        h = rhs.h;
        return *this;
    }

    bool Size::operator==(const size64f& rhs) const
    {
        return (w == rhs.w && h == rhs.h);
    }

    bool Size::operator!=(const size64f& rhs) const
    {
        return !operator==(rhs);
    }
}
