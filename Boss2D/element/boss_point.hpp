#pragma once
#include <element/boss_matrix.hpp>

namespace BOSS
{
    //! \brief 2D좌표
    class Point : public point64f
    {
    public:
        Point();
        Point(const point64f& rhs);
        Point(float x, float y);
        ~Point();

        Point& operator=(const point64f& rhs);
        Point& operator*=(const Matrix& rhs);
        Point operator*(const Matrix& rhs) const;
        Point& operator*=(const float rhs);
        Point operator*(const float rhs) const;
        Point& operator*=(const size64f& rhs);
        Point operator*(const size64f& rhs) const;
        Point& operator/=(const float rhs);
        Point operator/(const float rhs) const;
        Point& operator+=(const point64f& rhs);
        Point operator+(const point64f& rhs) const;
        Point& operator+=(const size64f& rhs);
        Point operator+(const size64f& rhs) const;
        Point& operator-=(const point64f& rhs);
        Point operator-(const point64f& rhs) const;
        Point& operator-=(const size64f& rhs);
        Point operator-(const size64f& rhs) const;
        bool operator==(const point64f& rhs) const;
        bool operator!=(const point64f& rhs) const;

        const bool ClockwiseTest(const point64f begin, const point64f end) const;
    };
    typedef Array<Point, datatype_class_canmemcpy> Points;
}
