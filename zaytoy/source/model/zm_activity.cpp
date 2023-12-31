#include <boss.hpp>
#include "zm_activity.hpp"

////////////////////////////////////////////////////////////////////////////////
// ABActivity
////////////////////////////////////////////////////////////////////////////////

buffer ABActivity::Create(chars type)
{
    if(!String::Compare(type, "move")) return Buffer::Alloc<ABActivityMove>(BOSS_DBG 1);
    else if(!String::Compare(type, "scale")) return Buffer::Alloc<ABActivityScale>(BOSS_DBG 1);
    else if(!String::Compare(type, "rotate_x")) return Buffer::Alloc<ABActivityRotateX>(BOSS_DBG 1);
    else if(!String::Compare(type, "rotate_y")) return Buffer::Alloc<ABActivityRotateY>(BOSS_DBG 1);
    else if(!String::Compare(type, "rotate_z")) return Buffer::Alloc<ABActivityRotateZ>(BOSS_DBG 1);
    else if(!String::Compare(type, "matrix")) return Buffer::Alloc<ABActivityMatrix>(BOSS_DBG 1);
    else if(!String::Compare(type, "speed")) return Buffer::Alloc<ABActivitySpeed>(BOSS_DBG 1);
    return Buffer::Alloc<ABActivity>(BOSS_DBG 1);
}

void ABActivity::Import(const Context& json)
{
    BOSS_ASSERT("잘못된 시나리오입니다", !json("type").GetText().Compare(Type()));
}

void ABActivity::Export(Context& json) const
{
    json.At("type").Set(Type());
}

////////////////////////////////////////////////////////////////////////////////
// ABActivityMove
////////////////////////////////////////////////////////////////////////////////

void ABActivityMove::Import(const Context& json)
{
    ABActivity::Import(json);
    mVector.x = json("x").GetFloat();
    mVector.y = json("y").GetFloat();
    mVector.z = json("z").GetFloat();
}

void ABActivityMove::Export(Context& json) const
{
    ABActivity::Export(json);
    json.At("x").Set(String::FromFloat(mVector.x));
    json.At("y").Set(String::FromFloat(mVector.y));
    json.At("z").Set(String::FromFloat(mVector.z));
}

ZMMatrix ABActivityMove::Value() const
{
    return ZMMatrix::MakeMove(mVector);
}

ZMMatrix ABActivityMove::AddValue(chars key, double& value)
{
    if(!String::Compare(key, "x")) mVector.x = (value += mVector.x);
    else if(!String::Compare(key, "y")) mVector.y = (value += mVector.y);
    else if(!String::Compare(key, "z")) mVector.z = (value += mVector.z);
    return ZMMatrix::MakeMove(mVector);
}

ZMMatrix ABActivityMove::RatedValue(double rate) const
{
    ZMVector RatedVector;
    RatedVector.x = mVector.x * rate;
    RatedVector.y = mVector.y * rate;
    RatedVector.z = mVector.z * rate;
    return ZMMatrix::MakeMove(RatedVector);
}

////////////////////////////////////////////////////////////////////////////////
// ABActivityScale
////////////////////////////////////////////////////////////////////////////////

void ABActivityScale::Import(const Context& json)
{
    ABActivity::Import(json);
    mVector.x = json("x").GetFloat(1);
    mVector.y = json("y").GetFloat(1);
    mVector.z = json("z").GetFloat(1);
}

void ABActivityScale::Export(Context& json) const
{
    ABActivity::Export(json);
    json.At("x").Set(String::FromFloat(mVector.x));
    json.At("y").Set(String::FromFloat(mVector.y));
    json.At("z").Set(String::FromFloat(mVector.z));
}

ZMMatrix ABActivityScale::Value() const
{
    return ZMMatrix::MakeScale(mVector);
}

ZMMatrix ABActivityScale::AddValue(chars key, double& value)
{
    if(!String::Compare(key, "x")) mVector.x = (value += mVector.x);
    else if(!String::Compare(key, "y")) mVector.y = (value += mVector.y);
    else if(!String::Compare(key, "z")) mVector.z = (value += mVector.z);
    return ZMMatrix::MakeScale(mVector);
}

ZMMatrix ABActivityScale::RatedValue(double rate) const
{
    ZMVector RatedVector;
    RatedVector.x = mVector.x * rate;
    RatedVector.y = mVector.y * rate;
    RatedVector.z = mVector.z * rate;
    return ZMMatrix::MakeScale(RatedVector);
}

////////////////////////////////////////////////////////////////////////////////
// ABActivityRotateX
////////////////////////////////////////////////////////////////////////////////

void ABActivityRotateX::Import(const Context& json)
{
    ABActivity::Import(json);
    mRotateX = json("x").GetFloat();
}

void ABActivityRotateX::Export(Context& json) const
{
    ABActivity::Export(json);
    json.At("x").Set(String::FromFloat(mRotateX));
}

ZMMatrix ABActivityRotateX::Value() const
{
    return ZMMatrix::MakeRotateX(mRotateX);
}

ZMMatrix ABActivityRotateX::AddValue(chars key, double& value)
{
    if(!String::Compare(key, "x")) mRotateX = (value += mRotateX);
    return ZMMatrix::MakeRotateX(mRotateX);
}

ZMMatrix ABActivityRotateX::RatedValue(double rate) const
{
    return ZMMatrix::MakeRotateX(mRotateX * rate);
}

////////////////////////////////////////////////////////////////////////////////
// ABActivityRotateY
////////////////////////////////////////////////////////////////////////////////

void ABActivityRotateY::Import(const Context& json)
{
    ABActivity::Import(json);
    mRotateY = json("y").GetFloat();
}

void ABActivityRotateY::Export(Context& json) const
{
    ABActivity::Export(json);
    json.At("y").Set(String::FromFloat(mRotateY));
}

ZMMatrix ABActivityRotateY::Value() const
{
    return ZMMatrix::MakeRotateY(mRotateY);
}

ZMMatrix ABActivityRotateY::AddValue(chars key, double& value)
{
    if(!String::Compare(key, "y")) mRotateY = (value += mRotateY);
    return ZMMatrix::MakeRotateY(mRotateY);
}

ZMMatrix ABActivityRotateY::RatedValue(double rate) const
{
    return ZMMatrix::MakeRotateY(mRotateY * rate);
}

////////////////////////////////////////////////////////////////////////////////
// ABActivityRotateZ
////////////////////////////////////////////////////////////////////////////////

void ABActivityRotateZ::Import(const Context& json)
{
    ABActivity::Import(json);
    mRotateZ = json("z").GetFloat();
}

void ABActivityRotateZ::Export(Context& json) const
{
    ABActivity::Export(json);
    json.At("z").Set(String::FromFloat(mRotateZ));
}

ZMMatrix ABActivityRotateZ::Value() const
{
    return ZMMatrix::MakeRotateZ(mRotateZ);
}

ZMMatrix ABActivityRotateZ::AddValue(chars key, double& value)
{
    if(!String::Compare(key, "z")) mRotateZ = (value += mRotateZ);
    return ZMMatrix::MakeRotateZ(mRotateZ);
}

ZMMatrix ABActivityRotateZ::RatedValue(double rate) const
{
    return ZMMatrix::MakeRotateZ(mRotateZ * rate);
}

////////////////////////////////////////////////////////////////////////////////
// ABActivityMatrix
////////////////////////////////////////////////////////////////////////////////

void ABActivityMatrix::Import(const Context& json)
{
    ABActivity::Import(json);
    mMatrix.FromText(json("matrix").GetText());
}

void ABActivityMatrix::Export(Context& json) const
{
    ABActivity::Export(json);
    json.At("matrix").Set(mMatrix.ToText());
}

ZMMatrix ABActivityMatrix::Value() const
{
    return mMatrix;
}

void ABActivityMatrix::SetMatrix(const ZMMatrix& matrix)
{
    mMatrix = matrix;
}

////////////////////////////////////////////////////////////////////////////////
// ABActivitySpeed
////////////////////////////////////////////////////////////////////////////////

void ABActivitySpeed::Import(const Context& json)
{
    ABActivity::Import(json);
    mMsec = json("msec").GetInt(Default);
}

void ABActivitySpeed::Export(Context& json) const
{
    ABActivity::Export(json);
    json.At("msec").Set(String::FromInteger((sint32) mMsec));
}

void ABActivitySpeed::SetSpeed(uint32 msec)
{
    mMsec = msec;
}

uint32 ABActivitySpeed::Speed() const
{
    return mMsec;
}
