#pragma once
#include <boss.hpp>

namespace BOSS
{
    //! \brief 2D벡터
    class Vector : public vector128f
    {
    public:
        Vector();
        Vector(const vector128f& rhs);
        Vector(float x, float y, float vx, float vy);
        Vector(const point64f& pos, const point64f& vec);
        ~Vector();

        Vector& operator=(const vector128f& rhs);
        Vector& operator*=(const float rhs);
        Vector operator*(const float rhs) const;
        Vector& operator+=(const point64f& rhs);
        Vector operator+(const point64f& rhs) const;
        Vector& operator-=(const point64f& rhs);
        Vector operator-(const point64f& rhs) const;
        bool operator==(const vector128f& rhs) const;
        bool operator!=(const vector128f& rhs) const;
    };
    typedef Array<Vector, datatype_class_canmemcpy> Vectors;
}
