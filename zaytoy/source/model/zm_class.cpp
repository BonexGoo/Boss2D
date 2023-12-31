#include <boss.hpp>
#include "zm_class.hpp"

void ZMClassID::FromText(chars classid)
{
    if(classid)
    {
        mDevice = (uint16) Parser::GetHex32(&classid[3], 4);
        mElement = (uint16) Parser::GetHex32(&classid[7], 4);
    }
    else {mDevice = 0; mElement = 0;}
}

String ZMClassID::ToText() const
{
    if(mDevice == 0 && mElement == 0) return "x";
    return String::Format("AB_%04x%04x", mDevice, mElement);
}

void ZMQuaternion::FromVector(const ZMVector& axis, const double radian)
{
    x = axis.x * Math::Sin(radian * 0.5);
    y = axis.y * Math::Sin(radian * 0.5);
    z = axis.z * Math::Sin(radian * 0.5);
    w = Math::Cos(radian * 0.5);
}

void ZMMatrix::FromText(chars matrix)
{
    Identity();
    if(matrix[0] == 'M' && matrix[1] == '_')
    if(!!String::Compare(matrix, "M_identity"))
    {
        auto MatrixValues = String::Split(matrix + 2, '_');
        for(sint32 i = 0, iend = MatrixValues.Count(); i < iend; ++i)
            m[i] = Parser::GetFloat<double>(MatrixValues[i]);
    }
}

String ZMMatrix::ToText() const
{
    static const ZMMatrix IdentityMatrix;
    if(!Memory::Compare(&m[0], &IdentityMatrix.m[0], sizeof(double) * 16))
        return "M_identity";
    String Collector = "M";
    for(sint32 i = 0; i < 16; ++i)
        Collector += '_' + String::FromFloat(m[i]);
    return Collector;
}

void ZMMatrix::Identity()
{
    m00 = 1; m01 = 0; m02 = 0; m03 = 0;
    m10 = 0; m11 = 1; m12 = 0; m13 = 0;
    m20 = 0; m21 = 0; m22 = 1; m23 = 0;
    m30 = 0; m31 = 0; m32 = 0; m33 = 1;
}

void ZMMatrix::Multiply(const ZMMatrix& other)
{
    const ZMMatrix Temp(*this);
    m00 = other.m00 * Temp.m00 + other.m01 * Temp.m10 + other.m02 * Temp.m20 + other.m03 * Temp.m30;
    m01 = other.m00 * Temp.m01 + other.m01 * Temp.m11 + other.m02 * Temp.m21 + other.m03 * Temp.m31;
    m02 = other.m00 * Temp.m02 + other.m01 * Temp.m12 + other.m02 * Temp.m22 + other.m03 * Temp.m32;
    m03 = other.m00 * Temp.m03 + other.m01 * Temp.m13 + other.m02 * Temp.m23 + other.m03 * Temp.m33;
    m10 = other.m10 * Temp.m00 + other.m11 * Temp.m10 + other.m12 * Temp.m20 + other.m13 * Temp.m30;
    m11 = other.m10 * Temp.m01 + other.m11 * Temp.m11 + other.m12 * Temp.m21 + other.m13 * Temp.m31;
    m12 = other.m10 * Temp.m02 + other.m11 * Temp.m12 + other.m12 * Temp.m22 + other.m13 * Temp.m32;
    m13 = other.m10 * Temp.m03 + other.m11 * Temp.m13 + other.m12 * Temp.m23 + other.m13 * Temp.m33;
    m20 = other.m20 * Temp.m00 + other.m21 * Temp.m10 + other.m22 * Temp.m20 + other.m23 * Temp.m30;
    m21 = other.m20 * Temp.m01 + other.m21 * Temp.m11 + other.m22 * Temp.m21 + other.m23 * Temp.m31;
    m22 = other.m20 * Temp.m02 + other.m21 * Temp.m12 + other.m22 * Temp.m22 + other.m23 * Temp.m32;
    m23 = other.m20 * Temp.m03 + other.m21 * Temp.m13 + other.m22 * Temp.m23 + other.m23 * Temp.m33;
    m30 = other.m30 * Temp.m00 + other.m31 * Temp.m10 + other.m32 * Temp.m20 + other.m33 * Temp.m30;
    m31 = other.m30 * Temp.m01 + other.m31 * Temp.m11 + other.m32 * Temp.m21 + other.m33 * Temp.m31;
    m32 = other.m30 * Temp.m02 + other.m31 * Temp.m12 + other.m32 * Temp.m22 + other.m33 * Temp.m32;
    m33 = other.m30 * Temp.m03 + other.m31 * Temp.m13 + other.m32 * Temp.m23 + other.m33 * Temp.m33;
}

ZMMatrix ZMMatrix::MakeMove(const ZMVector& move)
{
    ZMMatrix NewMatrix;
    NewMatrix.m30 = move.x;
    NewMatrix.m31 = move.y;
    NewMatrix.m32 = move.z;
    return NewMatrix;
}

ZMMatrix ZMMatrix::MakeScale(const ZMVector& scale)
{
    ZMMatrix NewMatrix;
    NewMatrix.m00 += scale.x;
    NewMatrix.m11 += scale.y;
    NewMatrix.m22 += scale.z;
    if(NewMatrix.m00 < 0.00001) NewMatrix.m00 = 0.00001;
    if(NewMatrix.m11 < 0.00001) NewMatrix.m11 = 0.00001;
    if(NewMatrix.m22 < 0.00001) NewMatrix.m22 = 0.00001;
    return NewMatrix;
}

ZMMatrix ZMMatrix::MakeRotate(const ZMQuaternion& rotate)
{
    ZMMatrix NewMatrix;
    NewMatrix.m00 = 1 - 2 * (rotate.y * rotate.y + rotate.z * rotate.z);
    NewMatrix.m01 = 2 * (rotate.x * rotate.y - rotate.w * rotate.z);
    NewMatrix.m02 = 2 * (rotate.x * rotate.y + rotate.w * rotate.y);
    NewMatrix.m10 = 2 * (rotate.x * rotate.y + rotate.w * rotate.z);
    NewMatrix.m11 = 1 - 2 * (rotate.x * rotate.x + rotate.z * rotate.z);
    NewMatrix.m12 = 2 * (rotate.y * rotate.z + rotate.w * rotate.x);
    NewMatrix.m20 = 2 * (rotate.x * rotate.z - rotate.w * rotate.y);
    NewMatrix.m21 = 2 * (rotate.y * rotate.z - rotate.w * rotate.x);
    NewMatrix.m22 = 1 - 2 * (rotate.x * rotate.x + rotate.y * rotate.y);
    return NewMatrix;
}

ZMMatrix ZMMatrix::MakeRotateX(const double radian)
{
    ZMVector NewAxis;
    NewAxis.x = 1;
    NewAxis.y = 0;
    NewAxis.z = 0;
    ZMQuaternion NewQuaternion;
    NewQuaternion.FromVector(NewAxis, radian);
    return MakeRotate(NewQuaternion);
}

ZMMatrix ZMMatrix::MakeRotateY(const double radian)
{
    ZMVector NewAxis;
    NewAxis.x = 0;
    NewAxis.y = 1;
    NewAxis.z = 0;
    ZMQuaternion NewQuaternion;
    NewQuaternion.FromVector(NewAxis, radian);
    return MakeRotate(NewQuaternion);
}

ZMMatrix ZMMatrix::MakeRotateZ(const double radian)
{
    ZMVector NewAxis;
    NewAxis.x = 0;
    NewAxis.y = 0;
    NewAxis.z = 1;
    ZMQuaternion NewQuaternion;
    NewQuaternion.FromVector(NewAxis, radian);
    return MakeRotate(NewQuaternion);
}
