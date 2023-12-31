#include <boss.hpp>
#include "zaymodel.hpp"

#include <service/boss_zaywidget.hpp>
#include "zm_child.hpp"
#include "zm_joint.hpp"
#include "zm_root.hpp"
#include "zm_trigger.hpp"

ZayModel::ZayModel(ZMClassID id) : mID(id)
{
    mX = 0;
    mY = 0;
}

ZayModel::~ZayModel()
{
}

ZMClassID ZayModel::CreateID()
{
    static uint16 LastDevice = Platform::Utility::Random() % 0xFFFF;
    static uint16 LastElement = Platform::Utility::Random() % 0xFFFF;
    return ZMClassID(LastDevice, ++LastElement);
}

ZayModel* ZayModel::CreateBox(chars type, const ZMClassID id)
{
    const String Type = type;
    branch;
    jump(!Type.Compare("child")) return new ABChild(id);
    jump(!Type.Compare("joint")) return new ABJoint(id);
    jump(!Type.Compare("root")) return new ABRoot(id);
    jump(!Type.Compare("trigger")) return new ABTrigger(id);
    return nullptr;
}

chars ZayModel::Type() const
{
    return "zaymodel";
}

chars ZayModel::ObjectID() const
{
    return "";
}

void ZayModel::Import(const Context& json)
{
    // 이미 type과 id는 객체가 만들어지기 위해 사용됨
    mParentID.mDevice = json("parentid")("device").GetInt();
    mParentID.mElement = json("parentid")("element").GetInt();
    mTitle = json("title").GetText();
    mText = json("text").GetText();
    mStatus = json("status").GetText("show");
    mX = json("x").GetInt();
    mY = json("y").GetInt();
}

void ZayModel::Export(Context& json) const
{
    json.At("type").Set(Type());
    json.At("id").At("device").Set(String::FromInteger(mID.mDevice));
    json.At("id").At("element").Set(String::FromInteger(mID.mElement));
    json.At("parentid").At("device").Set(String::FromInteger(mParentID.mDevice));
    json.At("parentid").At("element").Set(String::FromInteger(mParentID.mElement));
    json.At("title").Set(mTitle);
    json.At("text").Set(mText);
    json.At("status").Set(mStatus);
    json.At("x").Set(String::FromInteger(mX));
    json.At("y").Set(String::FromInteger(mY));
}

void ZayModel::UpdateDOM()
{
    Context NewJson;
    Export(NewJson);
    const String OneHeader = String::Format("zaymodel.%s.", (chars) ID());
    ZayWidgetDOM::RemoveVariables(OneHeader);
    ZayWidgetDOM::SetJson(NewJson, OneHeader);
    ZayWidgetDOM::SetValue(OneHeader + "parent", "'" + mParentID.ToText() + "'");
}

void ZayModel::RemoveBox()
{
    const String OneHeader = String::Format("zaymodel.%s.", (chars) ID());
    ZayWidgetDOM::RemoveVariables(OneHeader);
}

void ZayModel::ChangeBoxPos(sint32 x, sint32 y)
{
    mX = x;
    mY = y;
    const String OneHeader = String::Format("zaymodel.%s.", (chars) ID());
    ZayWidgetDOM::SetValue(OneHeader + "x", String::FromInteger(mX));
    ZayWidgetDOM::SetValue(OneHeader + "y", String::FromInteger(mY));
}

void ZayModel::SetParent(chars classid)
{
    mParentID.FromText(classid);
    const String OneHeader = String::Format("zaymodel.%s.", (chars) ID());
    ZayWidgetDOM::SetValue(OneHeader + "parentid.device", String::FromInteger(mParentID.mDevice));
    ZayWidgetDOM::SetValue(OneHeader + "parentid.element", String::FromInteger(mParentID.mElement));
    ZayWidgetDOM::SetValue(OneHeader + "parent", "'" + mParentID.ToText() + "'");
}

void ZayModel::SetText(chars text)
{
    mText = text;
    const String OneHeader = String::Format("zaymodel.%s.", (chars) ID());
    ZayWidgetDOM::SetValue(OneHeader + "text", "'" + mText + "'");
}

void ZayModel::SetStatus(chars status)
{
    mStatus = status;
    const String OneHeader = String::Format("zaymodel.%s.", (chars) ID());
    ZayWidgetDOM::SetValue(OneHeader + "status", "'" + mStatus + "'");
}
