#pragma once
#include <service/boss_zay.hpp>
#include "zm_class.hpp"

class ZayModel
{
public:
    ZayModel(ZMClassID id);
    virtual ~ZayModel();

public:
    static ZMClassID CreateID();
    static ZayModel* CreateBox(chars type, const ZMClassID id);

public:
    inline String ID() const {return mID.ToText();}
    inline String ParentID() const {return mParentID.ToText();}
    inline point64 BoxPos() const {return {mX, mY};}

public:
    virtual chars Type() const;
    virtual chars ObjectID() const;
    virtual void Import(const Context& json);
    virtual void Export(Context& json) const;
    // ABChild
    virtual const sint32 ChildCount() const {return 0;}
    virtual const String Child(sint32 index) const {return String();}
    virtual void SetChildren(const Strings& children) {}
    // ABTransform
    virtual void ClearActivity() {}
    virtual void AddActivity(chars type, chars key, double value) {}
    virtual void RemoveActivity(sint32 index) {}
    virtual void DoneActivity() {}
    virtual const ZMMatrix& Matrix(bool local) const {static ZMMatrix _; return _;}
    virtual const ZMMatrix& CalcMatrix(const ZMMatrix& other) {static ZMMatrix _; return _;}
    // ABJoint
    virtual void SaveDefault() const {}
    virtual bool LoadDefault() {return false;}
    virtual sint32 AnimateFocus() const {return -1;}
    virtual void SetAnimateFocus(sint32 index) {}
    virtual void CreateAnimate() {}
    virtual void RemoveAnimate(sint32 index) {}
    virtual void PlayAnimate(sint32 index) {}
    virtual const ZMMatrix* SupportMatrix() const {return nullptr;}
    virtual bool IsAnimating() const {return false;}

public:
    void UpdateDOM();
    void RemoveBox();
    void ChangeBoxPos(sint32 x, sint32 y);
    void SetParent(chars classid);
    void SetText(chars text);
    void SetStatus(chars status);

protected:
    const ZMClassID mID; // 자기ID
    ZMClassID mParentID; // 부모ID
    String mTitle;
    String mText;
    String mStatus;
    sint32 mX;
    sint32 mY;
};
