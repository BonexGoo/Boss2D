#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_zayson.hpp>

namespace BOSS
{
    class ZayWidgetDocumentP;

    /// @brief 제이프로연동식 위젯
    class ZayWidget
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(ZayWidget)

    public:
        ZayWidget();
        ~ZayWidget();
        ZayWidget(ZayWidget&& rhs);
        ZayWidget& operator=(ZayWidget&& rhs);

    public:
        typedef const Image* (*ResourceCB)(chars name);
        ZaySon& Init(chars viewname, chars assetname = nullptr, ResourceCB cb = nullptr);
        void Reload(chars assetname = nullptr);
        void Render(ZayPanel& panel);
        bool TickOnce();

    public:
        static void BuildComponents(chars viewname, ZaySonInterface& interface, ResourceCB* pcb);
        static void BuildGlues(chars viewname, ZaySonInterface& interface, ResourceCB* pcb);

    private:
        ZaySon mZaySon;
        String mZaySonViewName;
        String mZaySonAssetName;
        uint64 mZaySonFileSize;
        uint64 mZaySonModifyTime;
        ResourceCB mResourceCB;
        sint32 mProcedureID;
        id_pipe mPipe;
        uint64 mPipeModifyTime;
        Strings mPipeReservers;
    };
    typedef Array<ZayWidget> ZayWidgets;

    /// @brief 제이프로연동식 문서객체모델
    class ZayWidgetDOM
    {
    private:
        static ZayWidgetDOM& ST()
        {return *BOSS_STORAGE(ZayWidgetDOM);}

    private:
        ZayWidgetDOM();
        ~ZayWidgetDOM();

    public:
        static bool ExistValue(chars variable);
        static SolverValue GetValue(chars variable);
        static void SetValue(chars variable, chars formula);
        static void SetJson(const Context& json, const String nameheader = "");
        static void RemoveVariables(chars keyword = nullptr);
        static void BindPipe(id_pipe pipe);
        static void UnbindPipe(id_pipe pipe);

    private:
        void UpdateFlush();
        static void UpdateToPipe(id_pipe pipe, const Solver* solver);
        void RemoveFlush(chars variable);
        static void RemoveToPipe(id_pipe pipe, chars variable);

    private:
        class Pipe
        {
        public:
            Pipe() {mRefPipe = nullptr; mMsec = 0;}
            ~Pipe() {}
        public:
            id_pipe mRefPipe;
            uint64 mMsec;
        };

    private:
        ZayWidgetDocumentP* const mDocument;
        Map<Pipe> mPipeMap;
    };

    /// @brief 제이컨트롤
    class ZayControl
    {
    private:
        static ZayControl& ST()
        {return *BOSS_STORAGE(ZayControl);}

    public:
        static bool RenderEditBox(ZayPanel& panel, const String& uiname, const String& domname, bool enabled, bool ispassword);

    private:
        const String SecretFilter(bool ispassword, chars text) const;
        sint32 RenderText(ZayPanel& panel, const String& uiname, chars text, sint32& cursor, sint32 pos, sint32 height);
        void SendIme(ZayObject* view, const String& uiname, const String& domname, sint32 code, char key);

    private:
        struct Cursor
        {
            sint32s mPosArray;
            sint32 mPosLength {0};
            sint32 mCursor {0};

            sint32 GetPos(sint32 cursor)
            {
                return (cursor < mPosArray.Count())? mPosArray[cursor] : 0;
            }

            void ClearFocus()
            {
                mCursor = 0;
            }

            sint32 GetFocusBy(chars text)
            {
                sint32 Index = 0;
                for(sint32 i = 0; i < mCursor; ++i)
                    Index += String::GetLengthOfFirstLetter(text + Index);
                return Index;
            }

            bool SetFocusText(sint32 pos)
            {
                sint32 NewCursor = mPosLength;
                for(sint32 i = 0; i < mPosLength; ++i)
                {
                    if(pos < mPosArray[i])
                    {
                        NewCursor = i;
                        break;
                    }
                }
                if(mCursor != NewCursor)
                {
                    mCursor = NewCursor;
                    return true;
                }
                return false;
            };
        };

    private:
        // 필드정보
        sint32 mFieldAni {0};
        sint32 mFieldTextFocus {0};
        sint32 mFieldTextLength {0};
        String mFieldSavedText; // Esc를 위한 원본텍스트
        Map<Cursor> mFieldSavedCursor;
        // 복사정보
        sint32 mCopyAni {0};
        static const sint32 mCopyAniMax {20};
        sint32 mCopyAreaX {0};
        sint32 mCopyAreaWidth {0};
        // 키정보
        sint32 mLastPressCode {0};
        char mLastPressKey {0};
        sint64 mLastPressMsec {0};
    };
}
