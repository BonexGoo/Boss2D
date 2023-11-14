#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_zayson.hpp>
#include <service/boss_zaywidget.hpp>
#include <element/boss_tween.hpp>
#include <zaybox.hpp>

class ZEFakeZaySon : public ZaySonInterface
{
public:
    class Component
    {
    public:
        Component() {mType = ZayExtend::ComponentType::Unknown;}
        ~Component() {}

    public:
        ZayExtend::ComponentType mType;
        String mName;
        Color mColor;
        String mColorRes;
        String mParamComments;
        String mInsideSamples;
    };

public:
    ZEFakeZaySon();
    ~ZEFakeZaySon();

public:
    static Color GetComponentColor(ZayExtend::ComponentType type, String& colorres);

public:
    void ResetBoxInfo();
    const String& ViewName() const override;
    ZaySonInterface& AddComponent(ZayExtend::ComponentType type, chars name,
        ZayExtend::ComponentCB cb, chars comments = nullptr, chars samples = nullptr) override;
    ZaySonInterface& AddGlue(chars name, ZayExtend::GlueCB cb) override;
    void JumpCall(chars gatename, sint32 runcount) override;
    void JumpCallWithArea(chars gatename, sint32 runcount, float x, float y, float w, float h) override;
    void JumpClear() override;

public:
    sint32 GetComponentCount() const;
    const Component& GetComponent(sint32 i) const;
    const Component* GetComponent(chars name) const;

private:
    Array<Component> mComponents;

public:
    sint32 mLastBoxID;
    sint32 mDraggingComponentID;
    Rect mDraggingComponentRect;
};

class ZEWidgetPipe
{
public:
    ZEWidgetPipe();
    ~ZEWidgetPipe();

private:
    void RunPipe();
    void StopPipe();

public:
    bool TickOnce();
    void ResetJsonPath(chars jsonpath);
    bool IsConnected() const;
    void SendToClient(chars json);
    void SetExpandDOM(bool expand);
    void SetExpandLog(bool expand);

public:
    class Document
    {
    public:
        Document() {mUpdateMsec = 0;}
        ~Document() {}
    public:
        String mResult;
        String mFormula;
        uint64 mUpdateMsec;
    };
    class LogElement
    {
    public:
        LogElement(chars text = "", sint32 groupid = -1)
        {
            mText = text;
            static sint32 LastID = -1;
            if(groupid == -1)
                mGroupID = (++LastID) << 4;
            else mGroupID = groupid;
            mRenderPos = 0;
            mRenderTarget = 0;
            mValidValue = true;
            mNext = nullptr;
        }
        virtual ~LogElement()
        {
            LogElement* CurElement = nullptr;
            LogElement* NextElement = mNext;
            while(CurElement = NextElement)
            {
                NextElement = CurElement->mNext;
                CurElement->mNext = nullptr;
                delete CurElement;
            }
        }
    public:
        void Render(ZayPanel& panel)
        {
            mRenderTarget = (panel.screen_h() - panel.toview(0, 0).y);
            if(mRenderPos == 0) mRenderPos = mRenderTarget;
            const sint32 MovePos = mRenderPos - mRenderTarget + 0.5;
            ZAY_MOVE(panel, 0, -MovePos)
                RenderCore(panel);
        }
        LogElement* Detach(chars text)
        {
            LogElement* CurElement = this;
            while(CurElement->mNext)
            {
                if(!CurElement->mNext->mText.Compare(text))
                {
                    LogElement* OldElement = CurElement->mNext;
                    CurElement->mNext = OldElement->mNext;
                    OldElement->mNext = nullptr;
                    return OldElement;
                }
                CurElement = CurElement->mNext;
            }
            return nullptr;
        }
        sint32 Attach(LogElement* element)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", !element->mNext);
            element->mNext = mNext;
            mNext = element;
            return element->mGroupID;
        }
        void AttachLast(LogElement* element)
        {
            LogElement* CurElement = this;
            while(CurElement->mNext)
                CurElement = CurElement->mNext;
            CurElement->mNext = element;
        }
        void Remove(sint32 groupid)
        {
            LogElement* CurElement = this;
            while(CurElement->mNext)
            {
                if(CurElement->mNext->mGroupID == groupid)
                {
                    LogElement* RemoveElement = CurElement->mNext;
                    CurElement->mNext = RemoveElement->mNext;
                    RemoveElement->mNext = nullptr;
                    delete RemoveElement;
                }
                else CurElement = CurElement->mNext;
            }
            if(focusid() == groupid)
                focusid() = -1;
        }
        bool TickOnce()
        {
            bool NeedUpdate = false;
            LogElement* CurElement = this;
            while(CurElement = CurElement->mNext)
            {
                const sint32 OldPos = CurElement->mRenderPos - CurElement->mRenderTarget + 0.5;
                CurElement->mRenderPos = CurElement->mRenderPos * 0.9 + CurElement->mRenderTarget * 0.1;
                const sint32 NewPos = CurElement->mRenderPos - CurElement->mRenderTarget + 0.5;
                NeedUpdate |= (OldPos != NewPos);
                const bool NewValid = CurElement->valid();
                NeedUpdate |= (CurElement->mValidValue != NewValid);
                CurElement->mValidValue = NewValid;
            }
            return NeedUpdate;
        }
    public:
        LogElement* next() const {return mNext;}
        sint32& focusid() const {static sint32 _ = -1; return _;}
        virtual bool valid() const {return true;}
    protected:
        virtual void RenderCore(ZayPanel& panel) {}
    protected:
        String mText;
        sint32 mGroupID;
        float mRenderPos;
        float mRenderTarget;
        bool mValidValue;
        LogElement* mNext;
    };

public:
    void AddLog(chars type, chars title, chars detail);
    void RemoveLog(sint32 groupid);
    LogElement* GetLogElement();
    LogElement* NextLogElement(LogElement* element);
    void SetDOMSearch(chars text);
    void SetDOMCount(sint32 count);

public:
    inline const String& jsonpath() const {return mLastJsonPath;}
    inline const Map<Document>& dom() const {return mDOM;}
    inline const String& domsearch() const {return mDOMSearch;}
    inline sint32 domcount() const {return mDOMCount;}
    inline bool expanddom() const {return mExpandedDOM;}
    inline bool expandlog() const {return mExpandedLog;}

private:
    id_pipe mPipe;
    String mLastJsonPath;
    bool mJsonPathUpdated;
    bool mConnected;
    Map<Document> mDOM;
    String mDOMSearch;
    sint32 mDOMCount;
    bool mExpandedDOM;
    bool mExpandedLog;
    LogElement mLogTitleTop;
    LogElement mLogDetailTop;
    bool mLogTitleTurn;
};

class zayeditorData : public ZayObject
{
public:
    zayeditorData();
    ~zayeditorData();

public:
    void ResetBoxes();
    void NewProject();
    void FastSave();
    void LoadCore(const Context& json);
    void SaveCore(chars filename) const;

public:
    void RenderButton(ZayPanel& panel, chars name, Color color, ZayPanel::SubGestureCB cb);
    void RenderComponent(ZayPanel& panel, sint32 i, bool enable, bool blank);
    void RenderDOM(ZayPanel& panel);
    void RenderMiniMap(ZayPanel& panel);
    void RenderLogList(ZayPanel& panel);
    void RenderTitleBar(ZayPanel& panel);
    void RenderOutline(ZayPanel& panel);

public:
    void SetCursor(CursorRole role);
    sint32 MoveNcLeft(const rect128& rect, sint32 addx);
    sint32 MoveNcTop(const rect128& rect, sint32 addy);
    sint32 MoveNcRight(const rect128& rect, sint32 addx);
    sint32 MoveNcBottom(const rect128& rect, sint32 addy);

public:
    String mBuildTag;
    ZEFakeZaySon mZaySonAPI;
    sint32 mDraggingHook;
    Tween1D mEasySaveEffect;
    ZEWidgetPipe mPipe;
    uint64 mShowCommentTagMsec;
    Size mWorkViewSize;
    Point mWorkViewDrag;
    Point mWorkViewScroll;

public: // 윈도우상태
    static const sint32 mTitleHeight = 37;
    static const sint32 mMinWindowWidth = 1200;
    static const sint32 mMinWindowHeight = 800;
    String mWindowName;
    CursorRole mNowCursor;
    bool mNcLeftBorder;
    bool mNcTopBorder;
    bool mNcRightBorder;
    bool mNcBottomBorder;
};
