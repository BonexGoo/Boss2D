#pragma once
#include <boss.hpp>

namespace BOSS
{
    //! \brief 2D행렬
    class Matrix
    {
    public:
        Matrix();
        Matrix(const Matrix& rhs);
        Matrix(float m11, float m12, float m21, float m22, float dx, float dy);
        ~Matrix();

        Matrix& operator=(const Matrix& rhs);
        Matrix& operator*=(const Matrix& rhs);
        Matrix operator*(const Matrix& rhs) const;

        void AddOffset(const float x, const float y);
        void AddRotate(const float angle, const float cx, const float cy);
        void AddScale(const float w, const float h, const float cx, const float cy);
        void AddByTouch(const float a1x, const float a1y, const float a2x, const float a2y,
            const float b1x, const float b1y, const float b2x, const float b2y);

    public:
        float m11;
        float m12;
        float m21;
        float m22;
        float dx;
        float dy;
    };
    typedef Array<Matrix, datatype_class_canmemcpy> Matrixes;
}
