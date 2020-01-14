#include <boss.hpp>
#include "boss_vector.hpp"

namespace BOSS
{
    Vector::Vector()
    {
        x = 0;
        y = 0;
        vx = 0;
        vy = 0;
    }

    Vector::Vector(const vector128f& rhs)
    {
        operator=(rhs);
    }

    Vector::Vector(float x, float y, float vx, float vy)
    {
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
    }

    Vector::Vector(const point64f& pos, const point64f& vec)
    {
        x = pos.x;
        y = pos.y;
        vx = vec.x;
        vy = vec.y;
    }

    Vector::~Vector()
    {
    }

    Vector& Vector::operator=(const vector128f& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        vx = rhs.vx;
        vy = rhs.vy;
        return *this;
    }

    Vector& Vector::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        vx *= rhs;
        vy *= rhs;
        return *this;
    }

    Vector Vector::operator*(const float rhs) const
    {
        return Vector(*this).operator*=(rhs);
    }

    Vector& Vector::operator+=(const point64f& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector Vector::operator+(const point64f& rhs) const
    {
        return Vector(*this).operator+=(rhs);
    }

    Vector& Vector::operator-=(const point64f& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vector Vector::operator-(const point64f& rhs) const
    {
        return Vector(*this).operator-=(rhs);
    }

    bool Vector::operator==(const vector128f& rhs) const
    {
        return (x == rhs.x && y == rhs.y && vx == rhs.vx && vy == rhs.vy);
    }

    bool Vector::operator!=(const vector128f& rhs) const
    {
        return !operator==(rhs);
    }
}
