#include <boss.hpp>
#include "zm_trigger.hpp"

ABTrigger::ABTrigger(ZMClassID id) : ABTransform(id)
{
}

ABTrigger::~ABTrigger()
{
}

chars ABTrigger::Type() const
{
    return "trigger";
}

chars ABTrigger::ObjectID() const
{
    return "tool";
}

void ABTrigger::Import(const Context& json)
{
    ABTransform::Import(json);
    mChildID = json("child").GetText();
    mScripts.Clear();
    for(sint32 i = 0, iend = json("scripts").LengthOfIndexable(); i < iend; ++i)
        mScripts.AtAdding() = json("scripts")[i].GetText();
}

void ABTrigger::Export(Context& json) const
{
    ABTransform::Export(json);
    json.At("child").Set(mChildID);
    for(sint32 i = 0, iend = mScripts.Count(); i < iend; ++i)
        json.At("scripts").AtAdding().Set(mScripts[i]);
}
