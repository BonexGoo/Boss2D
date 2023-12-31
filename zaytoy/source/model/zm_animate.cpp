#include <boss.hpp>
#include "zm_animate.hpp"

#include <service/boss_zaywidget.hpp>

void ABAnimate::SetDomHeader(const ZayModel& box, sint32 animate_idx)
{
    // DOM초기화
    if(0 < mDomHeader.Length())
        ZayWidgetDOM::RemoveVariables(mDomHeader + '.');

    if(animate_idx == -1)
        mDomHeader = String::Format("zaymodel.%s.activity", (chars) box.ID());
    else mDomHeader = String::Format("zaymodel.%s.animate.%d.activity", (chars) box.ID(), animate_idx);

    // DOM업데이트
    ZayWidgetDOM::SetValue(mDomHeader + ".done", "1");
    ZayWidgetDOM::SetValue(mDomHeader + ".count", "0");
    if(animate_idx != -1)
        ZayWidgetDOM::SetValue(String::Format("zaymodel.%s.animate.count", (chars) box.ID()), String::FromInteger(animate_idx + 1));
}

void ABAnimate::UpdateDom() const
{
    // 행렬 재구성
    for(sint32 i = 0, iend = mActivities.Count(); i < iend; ++i)
    {
        // DOM업데이트
        Context ActivityJson;
        mActivities[i]->Export(ActivityJson);
        ZayWidgetDOM::SetJson(ActivityJson, String::Format("%s.%d.", (chars) mDomHeader, i));
    }
    ZayWidgetDOM::SetValue(mDomHeader + ".count", String::FromInteger(mActivities.Count()));
}

void ABAnimate::Import(const Context& json)
{
    DoneActivity();
    mLastMatrix.Identity();
    mPrevMatrix.Identity();
    mActivities.Clear();
    for(sint32 i = 0, iend = json("activity").LengthOfIndexable(); i < iend; ++i)
    {
        hook(json("activity")[i])
        {
            auto& NewActivity = mActivities.AtAdding();
            NewActivity = ABActivity::Create(fish("type").GetText());
            NewActivity->Import(fish);

            if(!String::Compare(NewActivity->Type(), "matrix"))
                mLastMatrix = NewActivity->Value();
            else if(!!String::Compare(mActivities[i]->Type(), "speed"))
                mLastMatrix.Multiply(NewActivity->Value());
        }
    }
    mPrevMatrix = mLastMatrix;
}

void ABAnimate::Export(Context& json) const
{
    for(sint32 i = 0, iend = mActivities.Count(); i < iend; ++i)
    {
        auto& NewActivity = json.At("activity").AtAdding();
        mActivities[i]->Export(NewActivity);
    }
}

void ABAnimate::ClearActivity(bool destroy_only)
{
    if(mDomHeader.Length() == 0)
        return;

    // DOM초기화
    ZayWidgetDOM::RemoveVariables(mDomHeader + '.');
    if(destroy_only) return;

    DoneActivity();
    mLastMatrix.Identity();
    mPrevMatrix.Identity();
    mActivities.Clear();
    ZayWidgetDOM::SetValue(mDomHeader + ".count", "0");
}

void ABAnimate::AddActivity(chars type, chars key, double value)
{
    CheckLastActivity(type);

    // 행렬 최신화
    if(auto ActivityCount = mActivities.Count())
    if(auto OneActivity = mActivities.At(ActivityCount - 1).Ptr())
    {
        const ZMMatrix NewMatrix = OneActivity->AddValue(key, value);
        mLastMatrix = mPrevMatrix;
        mLastMatrix.Multiply(NewMatrix);

        // DOM업데이트
        if(key) ZayWidgetDOM::SetValue(String::Format("%s.%d.%s", (chars) mDomHeader, ActivityCount - 1, key), String::FromFloat(value));
    }
}

void ABAnimate::AddMatrixActivity(const ZMMatrix& matrix)
{
    CheckLastActivity("matrix");

    // 행렬 최신화
    if(auto ActivityCount = mActivities.Count())
    if(auto OneActivity = mActivities.At(ActivityCount - 1).Ptr())
    if(!String::Compare(OneActivity->Type(), "matrix"))
    {
        ((ABActivityMatrix*) OneActivity)->SetMatrix(matrix);
        mLastMatrix = matrix;

        // DOM업데이트
        ZayWidgetDOM::SetValue(String::Format("%s.%d.matrix", (chars) mDomHeader, ActivityCount - 1), "'" + matrix.ToText() + "'");
    }
}

void ABAnimate::AddSpeedActivity(uint32 msec)
{
    CheckLastActivity("speed");

    // 행렬 최신화
    if(auto ActivityCount = mActivities.Count())
    if(auto OneActivity = mActivities.At(ActivityCount - 1).Ptr())
    if(!String::Compare(OneActivity->Type(), "speed"))
    {
        ((ABActivitySpeed*) OneActivity)->SetSpeed(msec);

        // DOM업데이트
        ZayWidgetDOM::SetValue(String::Format("%s.%d.msec", (chars) mDomHeader, ActivityCount - 1), String::FromInteger((sint32) msec));
    }
}

void ABAnimate::RemoveActivity(sint32 index)
{
    if(mDomHeader.Length() == 0)
        return;

    // DOM초기화
    ZayWidgetDOM::RemoveVariables(mDomHeader + '.');

    DoneActivity();
    mLastMatrix.Identity();
    mPrevMatrix.Identity();
    mActivities.SubtractionSection(index);
    ZayWidgetDOM::SetValue(mDomHeader + ".count", String::FromInteger(mActivities.Count()));

    // 행렬 재구성
    for(sint32 i = 0, iend = mActivities.Count(); i < iend; ++i)
    {
        const ZMMatrix CurMatrix = mActivities[i]->Value();
        if(!String::Compare(mActivities[i]->Type(), "matrix"))
            mLastMatrix = CurMatrix;
        else if(!!String::Compare(mActivities[i]->Type(), "speed"))
            mLastMatrix.Multiply(CurMatrix);

        // DOM업데이트
        Context ActivityJson;
        mActivities[i]->Export(ActivityJson);
        ZayWidgetDOM::SetJson(ActivityJson, String::Format("%s.%d.", (chars) mDomHeader, i));
    }
}

void ABAnimate::DoneActivity()
{
    mLastActivityType.Empty();

    // DOM업데이트
    ZayWidgetDOM::SetValue(mDomHeader + ".done", "1");
}

ZMMatrices ABAnimate::GenerateActivity() const
{
    ZMMatrices Collector;
    ZMMatrix OldMatrix;
    uint32 SpeedMsec = ABActivitySpeed::Default;
    for(sint32 i = 0, iend = mActivities.Count(); i < iend; ++i)
    {
        hook(mActivities[i])
        {
            if(!String::Compare(fish->Type(), "matrix"))
            {
                if(0 < Collector.Count())
                {
                    const sint32 FrameCount = Math::Max(1, 60 * SpeedMsec / 1000);
                    for(sint32 j = 0; j < FrameCount; ++j)
                    {
                        const double Rate = (j + 1) / double(FrameCount);
                        const ZMMatrix CurMatrix = fish->Value();
                        ZMMatrix NewMatrix = OldMatrix;
                        for(sint32 k = 0; k < 16; ++k)
                            NewMatrix.m[k] = NewMatrix.m[k] * (1 - Rate) + CurMatrix.m[k] * Rate;
                        Collector.AtAdding() = NewMatrix;
                    }
                }
                // 행렬이 첫리스트일때만 즉시 적용
                else Collector.AtAdding() = fish->Value();
            }
            else if(!String::Compare(fish->Type(), "speed"))
                SpeedMsec = ((const ABActivitySpeed*) fish.ConstPtr())->Speed();
            else
            {
                const sint32 FrameCount = Math::Max(1, 60 * SpeedMsec / 1000);
                for(sint32 j = 0; j < FrameCount; ++j)
                {
                    const ZMMatrix CurMatrix = fish->RatedValue((j + 1) / double(FrameCount));
                    ZMMatrix NewMatrix = OldMatrix;
                    NewMatrix.Multiply(CurMatrix);
                    Collector.AtAdding() = NewMatrix;
                }
            }
        }
        OldMatrix = Collector[-1];
    }
    return Collector;
}

void ABAnimate::CheckLastActivity(chars type)
{
    // 마지막 동작이랑 타입이 다르면 새 동작을 추가
    if(!!mLastActivityType.Compare(type))
    {
        mLastActivityType = type;
        mPrevMatrix = mLastMatrix;
        ZayWidgetDOM::SetValue(mDomHeader + ".done", "0");

        mActivities.AtAdding() = ABActivity::Create(type);
        ZayWidgetDOM::SetValue(mDomHeader + ".count", String::FromInteger(mActivities.Count()));
        const sint32 NewActivityIndex = mActivities.Count() - 1;

        // DOM업데이트
        Context ActivityJson;
        mActivities[NewActivityIndex]->Export(ActivityJson);
        ZayWidgetDOM::SetJson(ActivityJson, String::Format("%s.%d.", (chars) mDomHeader, NewActivityIndex));
    }
}
