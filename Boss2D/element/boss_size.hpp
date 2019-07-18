#pragma once
#include <boss.hpp>

namespace BOSS
{
    //! \brief 2D구간
    class Size : public size64f
    {
    public:
        Size();
        Size(const size64f& rhs);
        Size(float w, float h);
        ~Size();

        Size& operator=(const size64f& rhs);
        bool operator==(const size64f& rhs) const;
        bool operator!=(const size64f& rhs) const;
    };
    typedef Array<Size, datatype_class_canmemcpy> Sizes;
}
