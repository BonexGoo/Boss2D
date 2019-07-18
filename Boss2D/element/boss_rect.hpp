#pragma once
#include <boss.hpp>

namespace BOSS
{
    //! \brief 2D영역
    class Rect : public rect128f
    {
    public:
        Rect();
        Rect(const rect128f& rhs);
        Rect(const rect128& rhs);
        Rect(const point64f point, const size64f size);
        Rect(const point64f point1, const point64f point2);
        Rect(float l, float t, float r, float b);
        ~Rect();

        Rect& operator=(const rect128f& rhs);
        Rect& operator+=(const point64f& rhs);
        Rect operator+(const point64f& rhs) const;
        Rect& operator-=(const point64f& rhs);
        Rect operator-(const point64f& rhs) const;
        Rect& operator+=(const size64f& rhs);
        Rect operator+(const size64f& rhs) const;
        Rect& operator-=(const size64f& rhs);
        Rect operator-(const size64f& rhs) const;
        Rect& operator*=(float rhs);
        Rect operator*(float rhs) const;
        Rect& operator/=(float rhs);
        Rect operator/(float rhs) const;
        bool operator==(const rect128f& rhs) const;
        bool operator!=(const rect128f& rhs) const;

        const float Width() const;
        const float Height() const;
        const point64f Center() const;
        const float CenterX() const;
        const float CenterY() const;
        const bool PtInRect(const point64f point) const;
        const bool PtInRect(const float x, const float y) const;
        const bool ContactTest(const Rect rect) const;
        Rect Inflate(const float x, const float y) const;
        Rect Deflate(const float x, const float y) const;
    };
    typedef Array<Rect, datatype_class_canmemcpy> Rects;
}
