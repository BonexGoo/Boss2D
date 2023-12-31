#pragma once
#include <boss.hpp>
#include "zaymodel.hpp"
#include "zm_activity.hpp"

class ABAnimate
{
public:
    void SetDomHeader(const ZayModel& box, sint32 animate_idx = -1);
    void UpdateDom() const;
    void Import(const Context& json);
    void Export(Context& json) const;
    void ClearActivity(bool destroy_only);
    void AddActivity(chars type, chars key, double value);
    void AddMatrixActivity(const ZMMatrix& matrix);
    void AddSpeedActivity(uint32 msec);
    void RemoveActivity(sint32 index);
    void DoneActivity();
    ZMMatrices GenerateActivity() const;

public:
    inline const ZMMatrix& LastMatrix() const
    {return mLastMatrix;}

private:
    void CheckLastActivity(chars type);

private:
    String mDomHeader; // DOM헤더
    String mLastActivityType; // 마지막 동작내역 타입
    ZMMatrix mLastMatrix; // 최종 행렬
    ZMMatrix mPrevMatrix; // 이전 행렬
    ABActivities mActivities; // 동작내역
};
typedef Array<ABAnimate> ABAnimates;
