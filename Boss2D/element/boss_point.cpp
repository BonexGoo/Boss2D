#include <boss.hpp>
#include "boss_point.hpp"

namespace BOSS
{
    Point::Point()
    {
        x = 0;
        y = 0;
    }

    Point::Point(const point64f& rhs)
    {
        operator=(rhs);
    }

    Point::Point(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    Point::~Point()
    {
    }

    Point& Point::operator=(const point64f& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    Point& Point::operator*=(const Matrix& rhs)
    {
        const float New_x = x * rhs.m11 + y * rhs.m21 + rhs.dx;
        const float New_y = x * rhs.m12 + y * rhs.m22 + rhs.dy;
        x = New_x;
        y = New_y;
        return *this;
    }

    Point Point::operator*(const Matrix& rhs) const
    {
        return Point(*this).operator*=(rhs);
    }

    Point& Point::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    Point Point::operator*(const float rhs) const
    {
        return Point(*this).operator*=(rhs);
    }

    Point& Point::operator*=(const size64f& rhs)
    {
        x *= rhs.w;
        y *= rhs.h;
        return *this;
    }

    Point Point::operator*(const size64f& rhs) const
    {
        return Point(*this).operator*=(rhs);
    }

    Point& Point::operator/=(const float rhs)
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    Point Point::operator/(const float rhs) const
    {
        return Point(*this).operator/=(rhs);
    }

    Point& Point::operator+=(const point64f& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Point Point::operator+(const point64f& rhs) const
    {
        return Point(*this).operator+=(rhs);
    }

    Point& Point::operator+=(const size64f& rhs)
    {
        x += rhs.w;
        y += rhs.h;
        return *this;
    }

    Point Point::operator+(const size64f& rhs) const
    {
        return Point(*this).operator+=(rhs);
    }

    Point& Point::operator-=(const point64f& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Point Point::operator-(const point64f& rhs) const
    {
        return Point(*this).operator-=(rhs);
    }

    Point& Point::operator-=(const size64f& rhs)
    {
        x -= rhs.w;
        y -= rhs.h;
        return *this;
    }

    Point Point::operator-(const size64f& rhs) const
    {
        return Point(*this).operator-=(rhs);
    }

    bool Point::operator==(const point64f& rhs) const
    {
        return (x == rhs.x && y == rhs.y);
    }

    bool Point::operator!=(const point64f& rhs) const
    {
        return !operator==(rhs);
    }

    const bool Point::ClockwiseTest(const point64f begin, const point64f end) const
    {
        return (0 < (end.x - begin.x) * (y - begin.y) - (end.y - begin.y) * (x - begin.x));
    }
}
