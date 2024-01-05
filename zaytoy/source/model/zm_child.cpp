#include <boss.hpp>
#include "zm_child.hpp"

ABChild::ABChild(ZMClassID id) : ABTransform(id)
{
}

ABChild::~ABChild()
{
}

chars ABChild::Type() const
{
    return "child";
}

chars ABChild::ObjectID() const
{
    return mObjectID;
}

void ABChild::Import(const Context& json)
{
    ABTransform::Import(json);
    mObjectID = json("object").GetText();
    mComment = json("comment").GetText();
    Strings Children;
    for(sint32 i = 0, iend = json("children").LengthOfIndexable(); i < iend; ++i)
        Children.AtAdding() = json("children")[i].GetText();
    SetChildren(Children);
}

void ABChild::Export(Context& json) const
{
    ABTransform::Export(json);
    json.At("object").Set(mObjectID);
    json.At("comment").Set(mComment);
    for(sint32 i = 0, iend = mChildIDs.Count(); i < iend; ++i)
        json.At("children").AtAdding().Set(mChildIDs[i]);
}

const sint32 ABChild::ChildCount() const
{
    return mChildIDs.Count();
}

const String ABChild::Child(sint32 index) const
{
    return mChildIDs[index];
}

void ABChild::SetChildren(const Strings& children)
{
    mChildIDs = children;
    if(mChildIDs.Count() == 0) SetText("(Empty)");
    else if(mChildIDs.Count() == 1) SetText(String::Format("%s", (chars) mChildIDs[0]));
    else SetText(String::Format("%s (+%d)", (chars) mChildIDs[0], mChildIDs.Count() - 1));
}
