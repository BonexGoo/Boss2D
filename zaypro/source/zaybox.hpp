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
    enum class ChildType {None, Inner, Insider, Setter};
    enum class ClickMode {Click, Click_DoubleClick, Click_LongPress, Click_Swipe, Click_DoubleClick_LongPress, Touch, Error};
    typedef std::function<ZEZayBoxObject(chars compname)> CreatorCB;

public:
    ZEZayBox();
    virtual ~ZEZayBox();

public:
    static ZEZayBoxMap& TOP() {static ZEZayBoxMap _; return _;}
    static CreatorCB& CREATOR() {static CreatorCB _; return _;}
    static void ResetLastID();

protected:
    static sint32 MakeLastID();
    static sint32 ValidLastID(sint32 id);

public:
    static void Load(sint32s& children, const Context& json, const ZEZayBox* self, sint32 group);
    static void Save(const sint32s& children, Context& json, bool makeid);

public:
    virtual void ReadJson(const Context& json);
    virtual void WriteJson(Context& json, bool makeid) const;
    virtual void Render(ZayPanel& panel);
    virtual void RecalcSize();
    virtual sint32 GetChildrenGroupCount() const;
    virtual sint32s* GetChildrenGroup(sint32 group);
    virtual void SubParam(sint32 i);
    virtual void SubInput(sint32 i);
    virtual void SubExtInput(sint32 i);
    virtual void SubInsiderBall(sint32 group);
    virtual chars GetComment() const;
    virtual void OnClickMode();
    virtual Point GetBallPos(sint32 group) const;

public:
    void Init(sint32 id, chars type, Color color, chars colorres, bool expand, sint32 x, sint32 y);
    void AddChild(ZEZayBox& child, sint32 group);
    void SubChild(ZEZayBox& child);
    void ChangeChild(ZEZayBox& oldchild, ZEZayBox& newchild);
    inline sint32 id() const {return mID;}
    inline sint32 compid() const {return mCompID;}
    inline sint32 parent() const {return mParent;}
    inline sint32s& children() {return mChildren;}
    inline const sint32s& children() const {return mChildren;}
    inline Color color() const {return mColor;}
    inline bool hooked() const {return mHooked;}
    inline Point hookpos() const {return mHookPos;}

public:
    void RenderTitle(ZayPanel& panel, chars title, bool hook,
        ChildType childtype, bool copy, bool expand, bool resize, bool remove);
    void RenderHook(ZayPanel& panel, chars uiname);
    void RenderBall(ZayPanel& panel, chars uiname);
    void RenderGroupMoveButton(ZayPanel& panel, chars uiname);
    void RenderGroupCopyButton(ZayPanel& panel, chars uiname);
    void RenderExpandButton(ZayPanel& panel, chars uiname);
    void RenderResizeButton(ZayPanel& panel, chars uiname);
    void RenderRemoveButton(ZayPanel& panel, chars uiname, bool group);
    void RenderHookRemoveButton(ZayPanel& panel, chars uiname);

public:
    void Resize(sint32 add);
    void FlushTitleDrag();
    void FlushTitleDragWith(bool withhook);
    sint32 Copy();
    void Sort();
    Rect GetRect() const;
    void RemoveChildren(sint32 group);
    void RemoveChildrenAll();
    void ClearCompID();
    void ClearParentHook();
    void ClearChildrenHook(sint32 group, bool nearhook);
    void ClearChildrenHookAll();
    void ClearMyHook();
    void MoveMyHook(sint32 addx, sint32 addy);

protected: // 데이터
    sint32 mID; // 나의 인스턴스ID
    String mCompType;
    mutable sint32 mCompID;
    sint32 mParent; // 부모의 인스턴스ID
    sint32 mDebugOrder; // 디버깅시 보여질 나의 순번
    sint32s mChildren;

protected: // UI정보
    Color mColor;
    String mColorRes;
    bool mExpanded;
    double mPosX;
    double mPosY;
    sint32 mAddW;
    Point mTitleDrag;
    Size mBodySize;
    bool mHooked;
    Point mHookPos;
    Point mHookDrag;
    String mRemovingUIName;
    sint32 mRemovingCount;

public:
    static const sint32 TitleBarHeight = 30;
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
    class BodyElement : public ZayExtend::Renderer
    {
    public:
        BodyElement(ZEZayBox& box);
        virtual ~BodyElement();
    public:
        BodyElement& operator=(const BodyElement&)
        {BOSS_ASSERT("잘못된 시나리오입니다", false); return *this;}
    public:
        chars DomName(chars uiname) const override {return "editor";}
        bool HasInsider(chars uiname, chars rendername) const override {return false;}
        bool RenderInsider(chars uiname, chars rendername, ZayPanel& panel, sint32 pv) const override {return false;}
    public:
        virtual void ReadJson(const Context& json) = 0;
        virtual void WriteJson(Context& json, bool makeid) const = 0;
        virtual sint32 GetCalcedSize(const BodyElement* sub = nullptr) const = 0;
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
        ~BodyComment() override;
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json, bool makeid) const override;
        sint32 GetCalcedSize(const BodyElement* sub = nullptr) const override;
        void RenderCommentEditor(ZayPanel& panel, chars uiname);
    public:
        chars GetText(chars uiname) const override;
        void SetText(chars uiname, chars text) override;
        void ShowTip(chars uiname) const override;
        bool RenderInsider(chars uiname, chars rendername, ZayPanel& panel, sint32 pv) const override;
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
        ~BodyNameComment() override;
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json, bool makeid) const override;
        sint32 GetCalcedSize(const BodyElement* sub = nullptr) const override;
        void RenderNameCommentEditor(ZayPanel& panel, chars uiname, chars nametype);
    public:
        chars GetText(chars uiname) const override;
        void SetText(chars uiname, chars text) override;
        void ShowTip(chars uiname) const override;
        bool RenderInsider(chars uiname, chars rendername, ZayPanel& panel, sint32 pv) const override;
    public:
        String mName;
        String mSavedNameType;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyParamGroup
    ////////////////////////////////////////////////////////////////////////////////
    class BodyParamGroup : public BodyElement
    {
    public:
        BodyParamGroup(ZEZayBox& box);
        ~BodyParamGroup() override;
    public:
        void AddParam(chars param);
        void SubParam(sint32 i);
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json, bool makeid) const override;
        sint32 GetCalcedSize(const BodyElement* sub = nullptr) const override;
        void RenderParamGroup(ZayPanel& panel);
        void RenderParamEditor(ZayPanel& panel, chars uiname, sint32 i);
        void RenderParamComments(ZayPanel& panel, chars uiname, chars comments) const;
    public:
        chars GetText(chars uiname) const override;
        void SetText(chars uiname, chars text) override;
        void ShowTip(chars uiname) const override;
        bool RenderInsider(chars uiname, chars rendername, ZayPanel& panel, sint32 pv) const override;
    public:
        Strings mParams;
        String mComments;
        mutable sint32s mParamCommentDefaults;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyInputGroup
    ////////////////////////////////////////////////////////////////////////////////
    class BodyInputGroup : public BodyElement
    {
    public:
        BodyInputGroup(ZEZayBox& box);
        ~BodyInputGroup() override;
    public:
        void AddValue(chars key, chars value);
        void SubValue(sint32 i);
        void TurnClickMode();
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json, bool makeid) const override;
        sint32 GetCalcedSize(const BodyElement* sub = nullptr) const override;
        void RenderValueGroup(ZayPanel& panel, chars name, BodyInputGroup* sub = nullptr, bool showmode = false);
        void RenderValueEditor(ZayPanel& panel, chars uiname, sint32 i, sint32 extmode);
    public:
        chars GetText(chars uiname) const override;
        void SetText(chars uiname, chars text) override;
        void ShowTip(chars uiname) const override;
        bool RenderInsider(chars uiname, chars rendername, ZayPanel& panel, sint32 pv) const override;
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
        ~BodyLoopOperation() override;
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json, bool makeid) const override;
        sint32 GetCalcedSize(const BodyElement* sub = nullptr) const override;
        void RenderOperationEditor(ZayPanel& panel, chars uiname, chars itname);
    public:
        chars GetText(chars uiname) const override;
        void SetText(chars uiname, chars text) override;
        void ShowTip(chars uiname) const override;
        bool RenderInsider(chars uiname, chars rendername, ZayPanel& panel, sint32 pv) const override;
    public:
        String mOperation;
        String mSavedItName;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyConditionOperation
    ////////////////////////////////////////////////////////////////////////////////
    class BodyConditionOperation : public BodyElement
    {
    public:
        BodyConditionOperation(ZEZayBox& box);
        ~BodyConditionOperation() override;
    public:
        void Init(chars operation, bool withelse, bool eventflag);
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json, bool makeid) const override;
        sint32 GetCalcedSize(const BodyElement* sub = nullptr) const override;
        void RenderOperationEditor(ZayPanel& panel, chars uiname);
    public:
        chars GetText(chars uiname) const override;
        void SetText(chars uiname, chars text) override;
        void ShowTip(chars uiname) const override;
        bool RenderInsider(chars uiname, chars rendername, ZayPanel& panel, sint32 pv) const override;
    public:
        bool mWithElse;
        String mOperation;
        bool mEventFlag;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BodyInsideGroup
    ////////////////////////////////////////////////////////////////////////////////
    class BodyInsideGroup : public BodyElement
    {
    public:
        BodyInsideGroup(ZEZayBox& box);
        ~BodyInsideGroup() override;
    public:
        static ZEZayBox*& BOX() {static ZEZayBox* _ = nullptr; return _;}
    public:
        void AddBall();
        void SubBall(sint32 i);
    public:
        void ReadJson(const Context& json) override;
        void WriteJson(Context& json, bool makeid) const override;
        sint32 GetCalcedSize(const BodyElement* sub = nullptr) const override;
        void RenderBalls(ZayPanel& panel);
    public:
        class Ball
        {
        public:
            String mName;
            Point mBallPos;
            sint32s mChildren;
        };
        typedef Array<Ball> Balls;
        Balls mBalls;
        String mSamples;
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
    void WriteJson(Context& json, bool makeid) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    void SubInput(sint32 i) override;
    chars GetComment() const override;

protected: // 데이터
    BodyComment mComment;
    BodyInputGroup mCreateGroup;
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
    static ZEZayBoxObject Create(ChildType childtype, bool param, chars comments, chars samples);

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json, bool makeid) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    void SubInput(sint32 i) override;
    sint32 GetChildrenGroupCount() const override;
    sint32s* GetChildrenGroup(sint32 group) override;
    void SubParam(sint32 i) override;
    void SubInsiderBall(sint32 group) override;
    chars GetComment() const override;
    Point GetBallPos(sint32 group) const override;

protected: // 데이터
    BodyComment mComment;
    BodyParamGroup mParamGroup;
    BodyInsideGroup mInsideGroup;
    BodyInputGroup mSetGroup;
    ChildType mChildType;
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
    static ZEZayBoxObject Create(chars comments);

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json, bool makeid) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    void SubParam(sint32 i) override;
    void SubInput(sint32 i) override;
    void SubExtInput(sint32 i) override;
    chars GetComment() const override;
    void OnClickMode() override;

protected: // 데이터
    BodyNameComment mNameComment;
    BodyParamGroup mParamGroup;
    BodyInputGroup mTouchGroup;
    BodyInputGroup mClickGroup;
    ClickMode mClickMode;
};

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxCode
////////////////////////////////////////////////////////////////////////////////
class ZEZayBoxCode : public ZEZayBox
{
public:
    ZEZayBoxCode();
    ~ZEZayBoxCode() override;

public:
    static ZEZayBoxObject Create();

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json, bool makeid) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    void SubInput(sint32 i) override;
    chars GetComment() const override;

protected: // 데이터
    BodyComment mComment;
    BodyInputGroup mCodeGroup;
};

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxJumpOrGate
////////////////////////////////////////////////////////////////////////////////
class ZEZayBoxJumpOrGate : public ZEZayBox
{
public:
    ZEZayBoxJumpOrGate();
    ~ZEZayBoxJumpOrGate() override;

public:
    static ZEZayBoxObject Create(bool gate);

public:
    void ReadJson(const Context& json) override;
    void WriteJson(Context& json, bool makeid) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    chars GetComment() const override;

protected: // 데이터
    BodyNameComment mNameComment;
    bool mIsGate;
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
    void WriteJson(Context& json, bool makeid) const override;
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
    void WriteJson(Context& json, bool makeid) const override;
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
    void WriteJson(Context& json, bool makeid) const override;
    void Render(ZayPanel& panel) override;
    void RecalcSize() override;
    chars GetComment() const override;

protected: // 데이터보전
    BodyNameComment mNameComment;
    BodyParamGroup mParamGroup;
    BodyInputGroup mTouchGroup;
    BodyInputGroup mClickGroup;
    ClickMode mClickMode;
};
