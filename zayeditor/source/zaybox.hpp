#pragma once
#include <service/boss_zay.hpp>

class ZEZayBox;
typedef Object<ZEZayBox> ZEZayBoxObject;
typedef Map<ZEZayBoxObject> ZEZayBoxMap;

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox
////////////////////////////////////////////////////////////////////////////////
class ZEZayBox
{
public:
    typedef std::function<ZEZayBoxObject(chars compname)> CreatorCB;

public:
    ZEZayBox();
    virtual ~ZEZayBox();

public:
    static void ResetLastID();
protected:
    static sint32 MakeLastID();
    static sint32 ValidLastID(sint32 id);

public:
    void LoadChildren(const Context& json, ZEZayBoxMap& boxmap, CreatorCB cb);
    void SaveChildren(Context& json, const ZEZayBoxMap& boxmap) const;

public:
    virtual void ReadJson(const Context& json);
    virtual void WriteJson(Context& json) const;
    virtual void Render(ZayPanel& panel);
    virtual void RecalcSize();
    virtual void SubParam(sint32 i);
    virtual void SubInput(sint32 i);
    virtual chars GetComment() const;

public:
    void Init(sint32 id, chars type, Color color, bool expand, sint32 x, sint32 y);
    void InitCompID();
    void AddChild(ZEZayBox& child);
    void SubChild(ZEZayBox& child);
    void ChangeChild(ZEZayBox& oldchild, ZEZayBox& newchild);
    inline sint32 id() const {return mID;}
    inline sint32 parent() const {return mParent;}
    inline Color color() const {return mColor;}
    inline bool hooked() const {return mHooked;}
    inline Point hookpos() const {return mHookPos;}

public:
    void RenderTitle(ZayPanel& panel, chars title, bool hook, bool ball, bool copy, bool expand, bool remove);
    void RenderHook(ZayPanel& panel, chars uiname);
    void RenderBall(ZayPanel& panel, chars uiname);
    void RenderGroupMoveButton(ZayPanel& panel, chars uiname);
    void RenderGroupCopyButton(ZayPanel& panel, chars uiname);
    void RenderExpandButton(ZayPanel& panel, chars uiname);
    void RenderRemoveButton(ZayPanel& panel, chars uiname, bool group);
    void RenderHookRemoveButton(ZayPanel& panel, chars uiname);

public:
    void Move(Point drag);
    void FlushTitleDrag(ZEZayBoxMap& boxmap);
    void FlushTitleDragWith(ZEZayBoxMap& boxmap, bool withhook);
    sint32 Copy(ZEZayBoxMap& boxmap, CreatorCB cb);
    void Sort(ZEZayBoxMap& boxmap);
    Rect GetRect() const;
    Point GetBallPos() const;
    void RemoveChildren(ZEZayBoxMap& boxmap);
    void ClearChildrenHook(ZEZayBoxMap& boxmap);
    void ClearMyHook();

protected: // 데이터
    sint32 mID; // 인스턴스ID
    String mCompType;
    mutable sint32 mCompID;
    sint32 mParent;
    sint32 mOrder;
    sint32s mChildren;

protected: // UI정보
    Color mColor;
    bool mExpanded;
    double mPosX;
    double mPosY;
    Point mTitleDrag;
    Size mBodySize;
    bool mHooked;
    Point mHookPos;
    Point mHookDrag;
    String mRemovingUIName;
    sint32 mRemovingCount;

public:
    static const sint32 TitleBarHeight = 20;
    static const sint32 BodyWidth = 220;
    static const sint32 EditorHeight = 28;
    static const sint32 ParamHeight = 20;
    static const sint32 ParamAddWidth = 30;
    static const sint32 ValueHeight = 20;
    static const sint32 ValueAddWidth = 30;
    static const sint32 BallX = 15;

public:
    ////////////////////////////////////////////////////////////////////////////////
    // CommentTagService
    ////////////////////////////////////////////////////////////////////////////////
    class CommentTagService
    {
    public:
        static sint32 GetCount();
        static bool SetFirstFocus();
        static bool SetNextFocus();
        static chars GetFocusText();
        static Color GetFocusColor();
        static Point GetFocusPos();
    };

protected:
    ////////////////////////////////////////////////////////////////////////////////
    // BodyElement
    ////////////////////////////////////////////////////////////////////////////////
    class BodyElement
    {
    public:
        BodyElement(ZEZayBox& box);
        ~BodyElement();
    public:
        BodyElement& operator=(const BodyElement&)
        {BOSS_ASSERT("잘못된 시나리오입니다", false); return *this;}
    public:
        virtual void ReadJson(const Context& json) = 0;
        virtual void WriteJson(Context& json) const = 0;
        virtual sint32 GetCalcedSize() = 0;
    protected:
        ZEZayBox& mBox;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyComment
    ////////////////////////////////////////////////////////////////////////////////
    class BodyComment : public BodyElement
    {
    public:
        BodyComment(ZEZayBox& box);
        ~BodyComment();
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json) const override;
        sint32 GetCalcedSize() override;
        void RenderCommentEditor(ZayPanel& panel, chars uiname);
    public:
        String mComment;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyNameComment
    ////////////////////////////////////////////////////////////////////////////////
    class BodyNameComment : public BodyComment
    {
    public:
        BodyNameComment(ZEZayBox& box);
        ~BodyNameComment();
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json) const override;
        sint32 GetCalcedSize() override;
        void RenderNameCommentEditor(ZayPanel& panel, chars uiname);
    public:
        String mName;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyParamGroup
    ////////////////////////////////////////////////////////////////////////////////
    class BodyParamGroup : public BodyElement
    {
    public:
        BodyParamGroup(ZEZayBox& box);
        ~BodyParamGroup();
    public:
        void AddParam(chars param);
        void SubParam(sint32 i);
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json) const override;
        sint32 GetCalcedSize() override;
        void RenderParamGroup(ZayPanel& panel);
        void RenderParamEditor(ZayPanel& panel, chars uiname, sint32 i);
        void RenderParamComment(ZayPanel& panel, chars uiname, chars comment);
    public:
        Strings mParams;
        String mParamComment;
        sint32s mParamCommentDefaults;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyInputGroup
    ////////////////////////////////////////////////////////////////////////////////
    class BodyInputGroup : public BodyElement
    {
    public:
        BodyInputGroup(ZEZayBox& box);
        ~BodyInputGroup();
    public:
        void AddValue(chars key, chars value);
        void SubValue(sint32 i);
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json) const override;
        sint32 GetCalcedSize() override;
        void RenderValueGroup(ZayPanel& panel, chars name);
        void RenderValueEditor(ZayPanel& panel, chars uiname, sint32 i);
    public:
        class Input
        {
        public:
            String mKey;
            String mValue;
        };
        typedef Array<Input> Inputs;
        Inputs mInputs;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyLoopOperation
    ////////////////////////////////////////////////////////////////////////////////
    class BodyLoopOperation : public BodyElement
    {
    public:
        BodyLoopOperation(ZEZayBox& box);
        ~BodyLoopOperation();
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json) const override;
        sint32 GetCalcedSize() override;
        void RenderOperationEditor(ZayPanel& panel, chars uiname, chars itname);
    public:
        String mOperation;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyConditionOperation
    ////////////////////////////////////////////////////////////////////////////////
    class BodyConditionOperation : public BodyElement
    {
    public:
        BodyConditionOperation(ZEZayBox& box);
        ~BodyConditionOperation();
    public:
        void Init(chars operation, bool withelse, bool eventflag);
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json) const override;
        sint32 GetCalcedSize() override;
        void RenderOperationEditor(ZayPanel& panel, chars uiname);
    public:
        bool mWithElse;
        String mOperation;
        bool mEventFlag;
    };
};

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxStarter
////////////////////////////////////////////////////////////////////////////////
class ZEZayBoxStarter : public ZEZayBox
{
public:
    ZEZayBoxStarter();
    ~ZEZayBoxStarter() override;

public:
    static ZEZayBoxObject Create();

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    void SubInput(sint32 i) override;
    chars GetComment() const override;

protected: // 데이터
    BodyComment mComment;
    BodyInputGroup mInputGroup;
};

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxContent
////////////////////////////////////////////////////////////////////////////////
class ZEZayBoxContent : public ZEZayBox
{
public:
    ZEZayBoxContent();
    ~ZEZayBoxContent() override;

public:
    static ZEZayBoxObject Create(bool child, bool param, chars paramcomment);

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    void SubParam(sint32 i) override;
    chars GetComment() const override;

protected: // 데이터
    BodyComment mComment;
    BodyParamGroup mParamGroup;
    bool mHasChild;
    bool mHasParam;
};

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxLayout
////////////////////////////////////////////////////////////////////////////////
class ZEZayBoxLayout : public ZEZayBox
{
public:
    ZEZayBoxLayout();
    ~ZEZayBoxLayout() override;

public:
    static ZEZayBoxObject Create(chars paramcomment);

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    void SubParam(sint32 i) override;
    void SubInput(sint32 i) override;
    chars GetComment() const override;

protected: // 데이터
    BodyNameComment mNameComment;
    BodyParamGroup mParamGroup;
    BodyInputGroup mInputGroup;
};

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxLoop
////////////////////////////////////////////////////////////////////////////////
class ZEZayBoxLoop : public ZEZayBox
{
public:
    ZEZayBoxLoop();
    ~ZEZayBoxLoop() override;

public:
    static ZEZayBoxObject Create();

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    chars GetComment() const override;

protected: // 데이터
    BodyNameComment mNameComment;
    BodyLoopOperation mOperation;
};

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxCondition
////////////////////////////////////////////////////////////////////////////////
class ZEZayBoxCondition : public ZEZayBox
{
public:
    ZEZayBoxCondition();
    ~ZEZayBoxCondition() override;

public:
    static ZEZayBoxObject Create(chars operation = nullptr, bool eventflag = false);

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;

protected: // 데이터
    BodyConditionOperation mOperation;
    bool mHasElseAndOperation;
};

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxError
////////////////////////////////////////////////////////////////////////////////
class ZEZayBoxError : public ZEZayBox
{
public:
    ZEZayBoxError();
    ~ZEZayBoxError() override;

public:
    static ZEZayBoxObject Create();

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    chars GetComment() const override;

protected: // 데이터보전
    BodyNameComment mNameComment;
    BodyParamGroup mParamGroup;
    BodyInputGroup mInputGroup;
};
