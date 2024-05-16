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
        bool GlueCall(chars name, const Strings params);
        void JumpCall(chars name, sint32 count = 1);
        void UpdateAtlas(chars json);
        void SendLog(chars text);

    public:
        inline const String& viewname() const {return mZaySonViewName;}

    public:
        static void SetAssetPath(id_assetpath assetpath);
        static void BuildComponents(chars viewname, ZaySonInterface& interface, ResourceCB pcb);
        static void BuildGlues(chars viewname, ZaySonInterface& interface, ResourceCB pcb);

    private:
        ZaySon mZaySon;
        String mZaySonViewName;
        String mZaySonAssetName;
        uint64 mZaySonFileSize;
        uint64 mZaySonModifyTime;
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
        static bool ExistVariable(chars variable);
        static void SetVariableFilter(chars variable, SolverValueCB cb);
        static SolverValue GetValue(chars variable);
        static void SetValue(chars variable, chars formula);
        static String GetComment(chars variable);
        static void SetComment(chars variable, chars text);
        static void SetJson(const Context& json, const String nameheader = "");
        static void GetJson(Context& json, const String nameheader = "");
        static void RemoveVariables(chars keyword);
        static void BindPipe(id_pipe pipe);
        static void UnbindPipe(id_pipe pipe);
        static void SetFocus(sint32 compid);

    private:
        void ConfirmUpdate();
        void ConfirmRemove(chars variable);
        static void UpdateToPipe(id_pipe pipe, const Solver* solver);
        static void RemoveToPipe(id_pipe pipe, chars variable);
        static void FocusToPipe(id_pipe pipe, sint32 compid);

    private:
        class Pipe
        {
        public:
            Pipe() {mRefPipe = nullptr; mUpdatedMsec = 0;}
            ~Pipe() {}
        public:
            id_pipe mRefPipe;
            uint64 mUpdatedMsec;
        };

    private:
        ZayWidgetDocumentP* const mDocument;
        Map<Pipe> mPipeMap;
        bool mHasUpdate;
        bool mHasRemove;
        sint32 mHasFocusedCompID;
        Map<bool> mRemoveVariables;
        sint32 mUpdateProcedure;
    };

    /// @brief 제이컨트롤
    class ZayControl
    {
    public:
        enum LanguageMode {LM_English, LM_Korean, LM_Max};

    private:
        static ZayControl& ST()
        {return *BOSS_STORAGE(ZayControl);}

    public:
        static bool RenderEditBox(ZayPanel& panel, const String& uiname, const String& domname,
            sint32 border, bool enabled, bool password, bool dualsave, const ZayExtend::Renderer* renderer = nullptr);
        static void KeyPressing(const String& domname, wchar_t code, bool dualsave);
        static void KeyPressingBack(const String& domname, bool dualsave);
        static void KeyPressingEnter(const String& domname, bool dualsave, ZayObject* view);
        static void LangTurn(const String& domname, bool dualsave);

    private:
        class ReleaseCaptureInfo
        {
        public:
            String mUIName;
            ZayExtend::Renderer* mRenderer;
        };
        const String SecretFilter(bool ispassword, chars text) const;
        sint32 RenderText(ZayPanel& panel, const String& uiname, chars text,
            sint32& cursor, sint32 pos, sint32 height, const ZayExtend::Renderer* renderer);
        String AddCodeToIME(wchar_t code);
        String AddKeyToIME(char key);
        void FlushIME(const String& domname, const String added, bool dualsave);
        bool FlushSavedIME(const String& domname, bool dualsave);
        void OnKeyPressed(ZayObject* view, const String& uiname, const String& domname, sint32 code, char key, bool dualsave);
        template<bool DUALSAVE>
        static void OnReleaseCapture(payload olddata, payload newdata);
        template<bool DUALSAVE>
        static void OnReleaseCaptureByRenderer(payload olddata, payload newdata);

    private:
        class RenderInfo
        {
        public:
            sint32 GetPos(chars text, sint32 index) const
            {
                sint32 Cursor = 0;
                while(0 < index)
                {
                    const sint32 Length = String::GetLengthOfFirstLetter(text);
                    text += Length;
                    index -= Length;
                    Cursor++;
                }
                return (Cursor < mPosArray.Count())? mPosArray[Cursor] : 0;
            }

            void UpdatePos(sint32 cursor, sint32 pos)
            {
                mPosArray.AtWherever(cursor) = pos;
                mPosMax = cursor;
            }

            void ClearFocus()
            {
                mFocus = 0;
            }

            sint32 GetIndex(chars text) const
            {
                sint32 Index = 0;
                for(sint32 cursor = 0; cursor < mFocus; ++cursor)
                    Index += String::GetLengthOfFirstLetter(text + Index);
                return Index;
            }

            inline sint32 GetFocus() const
            {return mFocus;}

            void SetFocusByIndex(chars text, sint32 index)
            {
                sint32 Cursor = 0;
                while(0 < index)
                {
                    const sint32 Length = String::GetLengthOfFirstLetter(text);
                    text += Length;
                    index -= Length;
                    Cursor++;
                }
                mFocus = Cursor;
            }

            bool SetFocusByPos(sint32 pos)
            {
                sint32 NewFocus = mPosMax;
                for(sint32 cursor = 0; cursor < mPosMax; ++cursor)
                {
                    if(pos < (mPosArray[cursor] + mPosArray[cursor + 1]) / 2)
                    {
                        NewFocus = cursor;
                        break;
                    }
                }
                if(mFocus != NewFocus)
                {
                    mFocus = NewFocus;
                    return true;
                }
                return false;
            }

            bool GetScroll(sint32& pos) const
            {
                const sint32 CurScrollTarget = mScrollTarget + mScrollWheel;
                const sint32 CurScrollPos = mScrollPos;
                pos = CurScrollPos;
                if(CurScrollTarget < CurScrollPos)
                    return true;
                else if(CurScrollPos < CurScrollTarget)
                    return true;
                return false;
            }

            bool UpdateScroll(sint32& pos)
            {
                const sint32 CurScrollTarget = mScrollTarget + mScrollWheel;
                const sint32 CurScrollPos = mScrollPos;
                if(CurScrollTarget < CurScrollPos)
                {
                    mScrollPos = Math::MaxF(mScrollPos * 0.9 + CurScrollTarget * 0.1 - 0.5, CurScrollTarget);
                    pos = sint32(mScrollPos);
                    return true;
                }
                else if(CurScrollPos < CurScrollTarget)
                {
                    mScrollPos = Math::MinF(mScrollPos * 0.9 + CurScrollTarget * 0.1 + 0.5, CurScrollTarget);
                    pos = sint32(mScrollPos);
                    return true;
                }
                pos = CurScrollPos;
                return false;
            }

            void MoveScrollTarget(sint32 add, sint32 scrollmax)
            {
                if(mPosMax < mPosArray.Count())
                {
                    mScrollMax = scrollmax;
                    mScrollTarget = Math::Clamp(sint32(mScrollPos) + add, -scrollmax, 0);
                }
            }

            void ZeroScrollTarget()
            {
                mScrollTarget = 0;
            }

            void MoveScrollWheel(sint32 add)
            {
                mScrollWheel += add;
            }

            void ZeroScrollWheel()
            {
                mScrollWheel = 0;
            }

            void FlushScrollWheel()
            {
                mScrollTarget = Math::Clamp(mScrollTarget + mScrollWheel, -mScrollMax, 0);
                mScrollWheel = 0;
            }

        private:
            sint32s mPosArray;
            sint32 mPosMax {0};
            sint32 mFocus {0};
            sint32 mScrollMax {0};
            sint32 mScrollTarget {0};
            sint32 mScrollWheel {0};
            float mScrollPos {0.0};
        };

    private:
        // 필드정보
        sint32 mCapturedCursorIndex {0};
        sint32 mCapturedCursorAni {0};
        wchar_t mCapturedIMEChar;
        String mCapturedSavedText; // Esc를 위한 원본텍스트
        Map<RenderInfo> mRenderInfoMap;
        // 복사정보
        sint32 mCopyAni {0};
        static const sint32 mCopyAniMax {20};
        sint32 mCopyAreaA {0};
        sint32 mCopyAreaB {0};
        // 키정보
        sint32 mLastPressCode {0};
        char mLastPressKey {0};
        sint64 mLastPressMsec {0};
        LanguageMode mLastLanguage {LM_English};
    };
}
