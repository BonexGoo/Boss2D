#include <boss.hpp>
#include "zm_joint.hpp"

#include <service/boss_zaywidget.hpp>

ABJoint::ABJoint(ZMClassID id) : ABTransform(id), mAnimateFocus(-1), mGeneratedMsec(0)
{
}

ABJoint::~ABJoint()
{
}

chars ABJoint::Type() const
{
    return "joint";
}

chars ABJoint::ObjectID() const
{
    return "tool";
}

void ABJoint::Import(const Context& json)
{
    ABTransform::Import(json);
    mChildID = json("child").GetText();
    mAnimateFocus = json("animate_focus").GetInt(-1);
    mAnimates.Clear();
    for(sint32 i = 0, iend = json("animate").LengthOfIndexable(); i < iend; ++i)
        mAnimates.AtAdding().Import(json("animate")[i]);
}

void ABJoint::Export(Context& json) const
{
    ABTransform::Export(json);
    json.At("child").Set(mChildID);
    json.At("animate_focus").Set(String::FromInteger(mAnimateFocus));
    for(sint32 i = 0, iend = mAnimates.Count(); i < iend; ++i)
        mAnimates[i].Export(json.At("animate").AtAdding());
}

const sint32 ABJoint::ChildCount() const
{
    return 1;
}

const String ABJoint::Child(sint32 index) const
{
    return mChildID;
}

void ABJoint::ClearActivity()
{
    mGeneratedMsec = 0;
    if(mAnimateFocus == -1)
    {
        mActivity.ClearActivity(false);
        mSupportMatrix = mActivity.LastMatrix();
    }
    else
    {
        mAnimates.At(mAnimateFocus).ClearActivity(false);
        mLocalMatrix = mAnimates.At(mAnimateFocus).LastMatrix();
    }
}

void ABJoint::AddActivity(chars type, chars key, double value)
{
    mGeneratedMsec = 0;
    if(mAnimateFocus == -1)
    {
        mActivity.AddActivity(type, key, value);
        mSupportMatrix = mActivity.LastMatrix();
    }
    else
    {
        if(!String::Compare(type, "matrix"))
            mAnimates.At(mAnimateFocus).AddMatrixActivity(Matrix(true));
        else if(!String::Compare(type, "speed"))
            mAnimates.At(mAnimateFocus).AddSpeedActivity(500);
        else mAnimates.At(mAnimateFocus).AddActivity(type, key, value);
        mLocalMatrix = mAnimates.At(mAnimateFocus).LastMatrix();
    }
}

void ABJoint::RemoveActivity(sint32 index)
{
    mGeneratedMsec = 0;
    if(mAnimateFocus == -1)
    {
        mActivity.RemoveActivity(index);
        mSupportMatrix = mActivity.LastMatrix();
    }
    else
    {
        mAnimates.At(mAnimateFocus).RemoveActivity(index);
        mLocalMatrix = mAnimates.At(mAnimateFocus).LastMatrix();
    }
}

void ABJoint::DoneActivity()
{
    mGeneratedMsec = 0;
    if(mAnimateFocus == -1)
        mActivity.DoneActivity();
    else mAnimates.At(mAnimateFocus).DoneActivity();
}

void ABJoint::SaveDefault() const
{
    Context Collector;
    mActivity.Export(Collector);
    for(sint32 i = 0, iend = mAnimates.Count(); i < iend; ++i)
    {
        auto& NewAnimate = Collector.At("animate").AtAdding();
        mAnimates[i].Export(NewAnimate);
    }
    Collector.SaveJson().ToAsset(String::Format("zaymodel/joint/%s.json", (chars) mTitle.Lower()), true);
}

bool ABJoint::LoadDefault()
{
    const String OneJson = String::FromAsset(String::Format("zaymodel/joint/%s.json", (chars) mTitle.Lower()));
    if(0 < OneJson.Length())
    {
        Context OneJoint(ST_Json, SO_OnlyReference, OneJson);
        mActivity.Import(OneJoint);
        mAnimates.Clear();
        for(sint32 i = 0, iend = OneJoint("animate").LengthOfIndexable(); i < iend; ++i)
            mAnimates.AtAdding().Import(OneJoint("animate")[i]);
        mGeneratedMsec = 0;
        if(mAnimateFocus == -1)
            mSupportMatrix = mActivity.LastMatrix();
        return true;
    }
    return false;
}

sint32 ABJoint::AnimateFocus() const
{
    return mAnimateFocus;
}

void ABJoint::SetAnimateFocus(sint32 index)
{
    mAnimateFocus = index;
    ZayWidgetDOM::SetValue(String::Format("zaymodel.%s.animate_focus", (chars) ID()), String::FromInteger(mAnimateFocus));
    if(mAnimateFocus == -1)
        mLocalMatrix.Identity();
}

void ABJoint::CreateAnimate()
{
    DoneActivity();
    const sint32 AnimateIdx = mAnimates.Count();
    auto& NewAnimate = mAnimates.AtAdding();
    NewAnimate.SetDomHeader(*this, AnimateIdx);
    NewAnimate.AddMatrixActivity(Matrix(true));
    SetAnimateFocus(AnimateIdx);
}

void ABJoint::RemoveAnimate(sint32 index)
{
    DoneActivity();
    mAnimates.At(index).ClearActivity(true);
    mAnimates.SubtractionSection(index);
    if(mAnimates.Count() == 0)
        ZayWidgetDOM::RemoveVariables(String::Format("zaymodel.%s.animate.", (chars) ID()));
    else if(index == mAnimates.Count())
        ZayWidgetDOM::SetValue(String::Format("zaymodel.%s.animate.count", (chars) ID()), String::FromInteger(mAnimates.Count()));
    else for(sint32 i = index, iend = mAnimates.Count(); i < iend; ++i)
    {
        auto& CurAnimate = mAnimates.At(i);
        CurAnimate.SetDomHeader(*this, i);
        CurAnimate.UpdateDom();
    }
    SetAnimateFocus(Math::Min(index, mAnimates.Count() - 1));
}

void ABJoint::PlayAnimate(sint32 index)
{
    mGeneratedMsec = 0;
    mLocalMatrix = mAnimates.At(index).LastMatrix();
    GenerateMatrices();
}

const ZMMatrix& ABJoint::Matrix(bool local) const
{
    if(local)
    {
        if(auto CurMatrix = AnimatedMatrix())
            return *CurMatrix;
        return mLocalMatrix;
    }
    return mGlobalMatrix;
}

const ZMMatrix& ABJoint::CalcMatrix(const ZMMatrix& other)
{
    mGlobalSupportMatrix = other;
    mGlobalSupportMatrix.Multiply(mSupportMatrix);
    mGlobalMatrix = mGlobalSupportMatrix;
    mGlobalMatrix.Multiply(Matrix(true));
    return mGlobalMatrix;
}

const ZMMatrix* ABJoint::SupportMatrix() const
{
    return &mGlobalSupportMatrix;
}

bool ABJoint::IsAnimating() const
{
    return (0 < mGeneratedMsec);
}

static uint64 gNowMsec = 0;
void ABJoint::SetAnimateNow(uint64 msec)
{
    gNowMsec = msec;
}

const ZMMatrix* ABJoint::AnimatedMatrix() const
{
    if(0 < mGeneratedMsec)
    {
        const uint64 CurFrame = (gNowMsec - mGeneratedMsec) * 60 / 1000;
        if(CurFrame < mGeneratedMatrices.Count())
            return &mGeneratedMatrices[CurFrame];
        else mGeneratedMsec = 0;
    }
    return nullptr;
}

void ABJoint::GenerateMatrices()
{
    mGeneratedMsec = gNowMsec;
    if(mAnimateFocus == -1)
        mGeneratedMatrices = mActivity.GenerateActivity();
    else mGeneratedMatrices = mAnimates[mAnimateFocus].GenerateActivity();
}
