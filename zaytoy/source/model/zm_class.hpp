#pragma once
#include <boss.hpp>

class ZMClassID
{
public:
    uint16 mDevice;
    uint16 mElement;
public:
    ZMClassID() : mDevice(0), mElement(0) {}
    ZMClassID(uint16 device, uint16 element) : mDevice(device), mElement(element) {}
    ZMClassID(const ZMClassID& rhs) : mDevice(rhs.mDevice), mElement(rhs.mElement) {}
public:
    void FromText(chars classid);
    String ToText() const;
};

class ZMVector
{
public:
    double x;
    double y;
    double z;
public:
    ZMVector() : x(0.0), y(0.0), z(0.0) {}
    ZMVector(const ZMVector& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
};

class ZMQuaternion
{
public:
    double x;
    double y;
    double z;
    double w;
public:
    ZMQuaternion() : x(0.0), y(0.0), z(0.0), w(0.0) {}
    ZMQuaternion(const ZMQuaternion& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
public:
    void FromVector(const ZMVector& axis, const double radian);
};

class ZMMatrix
{
public:
    union
    {
        double m[16];
        struct
        {
            double m00, m01, m02, m03;
            double m10, m11, m12, m13;
            double m20, m21, m22, m23;
            double m30, m31, m32, m33;
        };
    };
public:
    ZMMatrix() :
        m00(1.0), m01(0.0), m02(0.0), m03(0.0),
        m10(0.0), m11(1.0), m12(0.0), m13(0.0),
        m20(0.0), m21(0.0), m22(1.0), m23(0.0),
        m30(0.0), m31(0.0), m32(0.0), m33(1.0) {}
    ZMMatrix(const ZMMatrix& rhs) :
        m00(rhs.m00), m01(rhs.m01), m02(rhs.m02), m03(rhs.m03),
        m10(rhs.m10), m11(rhs.m11), m12(rhs.m12), m13(rhs.m13),
        m20(rhs.m20), m21(rhs.m21), m22(rhs.m22), m23(rhs.m23),
        m30(rhs.m30), m31(rhs.m31), m32(rhs.m32), m33(rhs.m33) {}
public:
    void FromText(chars matrix);
    String ToText() const;
    void Identity();
    void Multiply(const ZMMatrix& other);
    static ZMMatrix MakeMove(const ZMVector& move);
    static ZMMatrix MakeScale(const ZMVector& scale);
    static ZMMatrix MakeRotate(const ZMQuaternion& rotate);
    static ZMMatrix MakeRotateX(const double radian);
    static ZMMatrix MakeRotateY(const double radian);
    static ZMMatrix MakeRotateZ(const double radian);
};
typedef Array<ZMMatrix> ZMMatrices;
