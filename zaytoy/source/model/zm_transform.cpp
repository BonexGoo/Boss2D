#include <boss.hpp>
#include "zm_transform.hpp"

#include <service/boss_zaywidget.hpp>

ABTransform::ABTransform(ZMClassID id) : ZayModel(id)
{
    mActivity.SetDomHeader(*this);
}

ABTransform::~ABTransform()
{
}

chars ABTransform::Type() const
{
    return "transform";
}

void ABTransform::Import(const Context& json)
{
    ZayModel::Import(json);
    mActivity.Import(json);
    mLocalMatrix = mActivity.LastMatrix();
}

void ABTransform::Export(Context& json) const
{
    ZayModel::Export(json);
    mActivity.Export(json);
}

void ABTransform::ClearActivity()
{
    mActivity.ClearActivity(false);
    mLocalMatrix = mActivity.LastMatrix();
}

void ABTransform::AddActivity(chars type, chars key, double value)
{
    mActivity.AddActivity(type, key, value);
    mLocalMatrix = mActivity.LastMatrix();
}

void ABTransform::RemoveActivity(sint32 index)
{
    mActivity.RemoveActivity(index);
    mLocalMatrix = mActivity.LastMatrix();
}

void ABTransform::DoneActivity()
{
    mActivity.DoneActivity();
}

const ZMMatrix& ABTransform::Matrix(bool local) const
{
    if(local) return mLocalMatrix;
    return mGlobalMatrix;
}

const ZMMatrix& ABTransform::CalcMatrix(const ZMMatrix& other)
{
    mGlobalMatrix = other;
    mGlobalMatrix.Multiply(mLocalMatrix);
    return mGlobalMatrix;
}

ZMMatrix ABTransform::Move(chars key, double value)
{
    ZMVector Collector;
    if(!String::Compare(key, "x")) Collector.x = value;
    else if(!String::Compare(key, "y")) Collector.y = value;
    else if(!String::Compare(key, "z")) Collector.z = value;
    return ZMMatrix::MakeMove(Collector);
}

ZMMatrix ABTransform::Scale(chars key, double value)
{
    ZMVector Collector;
    if(!String::Compare(key, "x")) Collector.x = value;
    else if(!String::Compare(key, "y")) Collector.y = value;
    else if(!String::Compare(key, "z")) Collector.z = value;
    return ZMMatrix::MakeScale(Collector);
}

ZMMatrix ABTransform::Rotate(chars key, double value)
{
    if(!String::Compare(key, "x")) return ZMMatrix::MakeRotateX(value);
    else if(!String::Compare(key, "y")) return ZMMatrix::MakeRotateY(value);
    else if(!String::Compare(key, "z")) return ZMMatrix::MakeRotateZ(value);
    return ZMMatrix();
}
