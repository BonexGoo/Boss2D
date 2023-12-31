#pragma once
#include <service/boss_zay.hpp>
#include "zm_transform.hpp"

class ABTrigger : public ABTransform
{
public:
    ABTrigger(ZMClassID id);
    ~ABTrigger() override;

public:
    chars Type() const override;
    chars ObjectID() const override;
    void Import(const Context& json) override;
    void Export(Context& json) const override;

protected:
    String mChildID; // 자식ID
    Strings mScripts;
};
