#pragma once
#include <service/boss_zay.hpp>
#include "zm_transform.hpp"

class ABJoint : public ABTransform
{
public:
    ABJoint(ZMClassID id);
    ~ABJoint() override;

public:
    chars Type() const override;
    chars ObjectID() const override;
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    const sint32 ChildCount() const override;
    const String Child(sint32 index) const override;
    void ClearActivity() override;
    void AddActivity(chars type, chars key, double value) override;
    void RemoveActivity(sint32 index) override;
    void DoneActivity() override;
    void SaveDefault() const override;
    bool LoadDefault() override;
    sint32 AnimateFocus() const override;
    void SetAnimateFocus(sint32 index) override;
    void CreateAnimate() override;
    void RemoveAnimate(sint32 index) override;
    void PlayAnimate(sint32 index) override;
    const ZMMatrix& Matrix(bool local) const override;
    const ZMMatrix& CalcMatrix(const ZMMatrix& other) override;
    const ZMMatrix* SupportMatrix() const override;
    bool IsAnimating() const override;

public:
    static void SetAnimateNow(uint64 msec);
    const ZMMatrix* AnimatedMatrix() const;
    void GenerateMatrices();

protected:
    String mChildID; // 자식ID
    sint32 mAnimateFocus; // 현재 애니메이션(-1은 기본동작)
    ABAnimates mAnimates; // 애니메이션
    ZMMatrix mSupportMatrix; // 지지대 행렬
    ZMMatrix mGlobalSupportMatrix; // 계층적용된 지지대 행렬
    mutable uint64 mGeneratedMsec; // 애니메이션용으로 생성된 실행시각
    ZMMatrices mGeneratedMatrices; // 애니메이션용으로 생성된 행렬들
};
