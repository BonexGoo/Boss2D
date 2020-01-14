#include <boss.hpp>
#include "boss_rect.hpp"

#include <element/boss_point.hpp>
#include <element/boss_size.hpp>

namespace BOSS
{
    Rect::Rect()
    {
        l = 0;
        t = 0;
        r = 0;
        b = 0;
    }

    Rect::Rect(const rect128f& rhs)
    {
        operator=(rhs);
    }

    Rect::Rect(const rect128& rhs)
    {
        l = rhs.l;
        t = rhs.t;
        r = rhs.r;
        b = rhs.b;
    }

    Rect::Rect(const point64f point, const size64f size)
    {
        l = point.x;
        t = point.y;
        r = point.x + size.w;
        b = point.y + size.h;
    }

    Rect::Rect(const point64f point1, const point64f point2)
    {
        l = (point1.x < point2.x)? point1.x : point2.x;
        t = (point1.y < point2.y)? point1.y : point2.y;
        r = (point1.x < point2.x)? point2.x : point1.x;
        b = (point1.y < point2.y)? point2.y : point1.y;
    }

    Rect::Rect(float l, float t, float r, float b)
    {
        this->l = (l < r)? l : r;
        this->t = (t < b)? t : b;
        this->r = (l < r)? r : l;
        this->b = (t < b)? b : t;
    }

    Rect::~Rect()
    {
    }

    Rect& Rect::operator=(const rect128f& rhs)
    {
        l = rhs.l;
        t = rhs.t;
        r = rhs.r;
        b = rhs.b;
        return *this;
    }

    Rect& Rect::operator+=(const point64f& rhs)
    {
        l += rhs.x;
        t += rhs.y;
        r += rhs.x;
        b += rhs.y;
        return *this;
    }

    Rect Rect::operator+(const point64f& rhs) const
    {
        return Rect(*this).operator+=(rhs);
    }

    Rect& Rect::operator-=(const point64f& rhs)
    {
        l -= rhs.x;
        t -= rhs.y;
        r -= rhs.x;
        b -= rhs.y;
        return *this;
    }

    Rect Rect::operator-(const point64f& rhs) const
    {
        return Rect(*this).operator-=(rhs);
    }

    Rect& Rect::operator+=(const size64f& rhs)
    {
        l -= rhs.w;
        t -= rhs.h;
        r += rhs.w;
        b += rhs.h;
        return *this;
    }

    Rect Rect::operator+(const size64f& rhs) const
    {
        return Rect(*this).operator+=(rhs);
    }

    Rect& Rect::operator-=(const size64f& rhs)
    {
        l += rhs.w;
        t += rhs.h;
        r -= rhs.w;
        b -= rhs.h;
        return *this;
    }

    Rect Rect::operator-(const size64f& rhs) const
    {
        return Rect(*this).operator-=(rhs);
    }

    Rect& Rect::operator*=(float rhs)
    {
        l *= rhs;
        t *= rhs;
        r *= rhs;
        b *= rhs;
        return *this;
    }

    Rect Rect::operator*(float rhs) const
    {
        return Rect(*this).operator*=(rhs);
    }

    Rect& Rect::operator/=(float rhs)
    {
        l /= rhs;
        t /= rhs;
        r /= rhs;
        b /= rhs;
        return *this;
    }

    Rect Rect::operator/(float rhs) const
    {
        return Rect(*this).operator/=(rhs);
    }

    bool Rect::operator==(const rect128f& rhs) const
    {
        return (l == rhs.l && t == rhs.t && r == rhs.r && b == rhs.b);
    }

    bool Rect::operator!=(const rect128f& rhs) const
    {
        return !operator==(rhs);
    }

    const float Rect::Width() const
    {
        return r - l;
    }

    const float Rect::Height() const
    {
        return b - t;
    }

    const point64f Rect::Center() const
    {
        return Point((r + l) / 2, (b + t) / 2);
    }

    const float Rect::CenterX() const
    {
        return (r + l) / 2;
    }

    const float Rect::CenterY() const
    {
        return (b + t) / 2;
    }

    const bool Rect::PtInRect(const point64f point) const
    {
        return (l <= point.x && t <= point.y && point.x < r && point.y < b);
    }

    const bool Rect::PtInRect(const float x, const float y) const
    {
        return (l <= x && t <= y && x < r && y < b);
    }

    const bool Rect::ContactTest(const Rect rect) const
    {
        return (l < rect.r && t < rect.b && rect.l < r && rect.t < b);
    }

    Rect Rect::Inflate(const float x, const float y) const
    {
        return Rect(*this).operator+=(Size(x, y));
    }

    Rect Rect::Deflate(const float x, const float y) const
    {
        return Rect(*this).operator-=(Size(x, y));
    }
}
