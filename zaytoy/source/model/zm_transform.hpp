#pragma once
#include <service/boss_zay.hpp>
#include "zaymodel.hpp"
#include "zm_animate.hpp"

class ABTransform : public ZayModel
{
public:
    ABTransform(ZMClassID id);
    ~ABTransform() override;

public:
    chars Type() const override;
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    void ClearActivity() override;
    void AddActivity(chars type, chars key, double value) override;
    void RemoveActivity(sint32 index) override;
    void DoneActivity() override;
    const ZMMatrix& Matrix(bool local) const override;
    const ZMMatrix& CalcMatrix(const ZMMatrix& other) override;

private:
    ZMMatrix Move(chars key, double value);
    ZMMatrix Scale(chars key, double value);
    ZMMatrix Rotate(chars key, double value);

protected:
    ABAnimate mActivity; // 동작내역
    ZMMatrix mLocalMatrix; // 지역화된 행렬
    ZMMatrix mGlobalMatrix; // 계층적용된 행렬
};
