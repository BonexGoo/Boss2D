#pragma once
#include <service/boss_zay.hpp>
#include "zm_transform.hpp"

class ABChild : public ABTransform
{
public:
    ABChild(ZMClassID id);
    ~ABChild() override;

public:
    chars Type() const override;
    chars ObjectID() const override;
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    const sint32 ChildCount() const override;
    const String Child(sint32 index) const override;
    void SetChildren(const Strings& children) override;

protected:
    String mObjectID; // 오브젝트ID
    Strings mChildIDs; // 자식ID들
    String mComment;
};
