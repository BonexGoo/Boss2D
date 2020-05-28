#include <boss.hpp>
#include "boss_matrix.hpp"

namespace BOSS
{
    Matrix::Matrix()
    {
        m11 = 1;
        m12 = 0;
        m21 = 0;
        m22 = 1;
        dx = 0;
        dy = 0;
    }

    Matrix::Matrix(const Matrix& rhs)
    {
        operator=(rhs);
    }

    Matrix::Matrix(float m11, float m12, float m21, float m22, float dx, float dy)
    {
        this->m11 = m11;
        this->m12 = m12;
        this->m21 = m21;
        this->m22 = m22;
        this->dx = dx;
        this->dy = dy;
    }

    Matrix::~Matrix()
    {
    }

    Matrix& Matrix::operator=(const Matrix& rhs)
    {
        m11 = rhs.m11;
        m12 = rhs.m12;
        m21 = rhs.m21;
        m22 = rhs.m22;
        dx = rhs.dx;
        dy = rhs.dy;
        return *this;
    }

    Matrix& Matrix::operator*=(const Matrix& rhs)
    {
        const float New_m11 = m11 * rhs.m11 + m12 * rhs.m21;
        const float New_m12 = m11 * rhs.m12 + m12 * rhs.m22;
        const float New_m21 = m21 * rhs.m11 + m22 * rhs.m21;
        const float New_m22 = m21 * rhs.m12 + m22 * rhs.m22;
        const float New_dx = dx  * rhs.m11 + dy  * rhs.m21 + rhs.dx;
        const float New_dy = dx  * rhs.m12 + dy  * rhs.m22 + rhs.dy;
        m11 = New_m11;
        m12 = New_m12;
        m21 = New_m21;
        m22 = New_m22;
        dx = New_dx;
        dy = New_dy;
        return *this;
    }

    Matrix Matrix::operator*(const Matrix& rhs) const
    {
        return Matrix(*this).operator*=(rhs);
    }

    bool Matrix::operator==(const Matrix& rhs) const
    {
        return (m11 == rhs.m11 && m12 == rhs.m12 && m21 == rhs.m21 && m22 == rhs.m22
            && dx == rhs.dx && dy == rhs.dy);
    }

    bool Matrix::operator!=(const Matrix& rhs) const
    {
        return !operator==(rhs);
    }

    void Matrix::AddOffset(const float x, const float y)
    {
        operator*=(Matrix(1, 0, 0, 1, x, y));
    }

    void Matrix::AddRotate(const float angle, const float cx, const float cy)
    {
        const float c = Math::Cos(angle);
        const float s = Math::Sin(angle);
        operator*=(Matrix(c, s, -s, c, (1 - c) * cx + s * cy, (1 - c) * cy - s * cx));
    }

    void Matrix::AddScale(const float w, const float h, const float cx, const float cy)
    {
        operator*=(Matrix(w, 0, 0, h, (1 - w) * cx, (1 - h) * cy));
    }

    void Matrix::AddByTouch(const float a1x, const float a1y, const float a2x, const float a2y,
        const float b1x, const float b1y, const float b2x, const float b2y)
    {
        const float amx = (a1x + a2x) / 2;
        const float amy = (a1y + a2y) / 2;
        const float bmx = (b1x + b2x) / 2;
        const float bmy = (b1y + b2y) / 2;
        const float rotate = Math::Atan(b1x - bmx, b1y - bmy) - Math::Atan(a1x - amx, a1y - amy);
        const float scale = Math::Distance(b1x, b1y, b2x, b2y)
            / Math::Distance(a1x, a1y, a2x, a2y);
        AddOffset(bmx - amx, bmy - amy);
        AddRotate(rotate, bmx, bmy);
        AddScale(scale, scale, bmx, bmy);
    }
}
