#pragma once
#include <service/boss_zay.hpp>
#include <functional>

namespace BOSS
{
    /// @brief 뷰스크립트 문서객체모델
    class ZaySonDocument
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(ZaySonDocument)
    public:
        ZaySonDocument(chars chain);
        virtual ~ZaySonDocument();
        ZaySonDocument(ZaySonDocument&& rhs);
        ZaySonDocument& operator=(ZaySonDocument&& rhs);

    public:
        typedef std::function<void(const Solver*)> UpdateCB;

    public:
        bool ExistVariable(chars variable) const;
        void SetVariableFilter(chars variable, SolverValueCB cb);
        SolverValue GetValue(chars variable) const;
        void SetValue(chars variable, chars formula);
        String GetComment(chars variable) const;
        void SetComment(chars variable, chars text);
        void SetJson(const Context& json, const String nameheader = "");
        void GetJson(Context& json, const String nameheader = "");
        void Remove(chars variable);
        void RemoveMatchedVariables(chars keyword, SolverVariableCB cb = nullptr);
        void CheckUpdatedSolvers(uint64 msec, UpdateCB cb);

    protected:
        Solver& LinkedSolver(chars variable);

    private:
        const String mChain;
        Solvers mSolvers;
    };

    /// @brief 뷰스크립트 엘리먼트통신
    class ZaySonElementCall
    {
    public:
        static void SetCursor(sint32 elementid, CursorRole role);
        static bool SetVariable(sint32 elementid, chars uiname, chars key, chars value);
        static bool SendPress(sint32 elementid, chars uiname);
        static bool SendClick(sint32 elementid, chars uiname, bool doubleclicked, bool longpressed,
            bool upswiped, bool downswiped, bool leftswiped, bool rightswiped, bool outreleased, bool cancelreleased);
        static bool IsValidDoubleClick(sint32 elementid);
        static bool IsValidLongPress(sint32 elementid);
        static bool IsValidSwipe(sint32 elementid);
        static sint32 GetCompID(sint32 elementid);
    };

    /// @brief 뷰스크립트 원형
    class ZaySonInterface
    {
    public:
        enum class ConditionType {Unknown, If, IfFocused, IfHovered, IfPressed,
            IfDoubleClicked, IfNDoubleClicked, IfLongPressed, IfNLongPressed,
            IfUpSwiped, IfNUpSwiped, IfDownSwiped, IfNDownSwiped, IfLeftSwiped, IfNLeftSwiped, IfRightSwiped, IfNRightSwiped,
            IfOutReleased, IfCancelReleased, Else, Endif};
        enum class RequestType {Unknown, SetVariable, VoidFunction, ReturnFunction};

    public:
        virtual void SetViewAndDom(chars viewname, chars domheader) = 0;
        virtual const String& ViewName() const = 0;
        virtual const String& DomHeader() const = 0;
        virtual ZaySonInterface& AddComponent(ZayExtend::ComponentType type, chars name, ZayExtend::ComponentCB cb,
            chars param_comments = nullptr, chars inside_samples = nullptr) = 0;
        virtual ZaySonInterface& AddGlue(chars name, ZayExtend::GlueCB cb,
            chars param_comments = nullptr, chars document = nullptr) = 0;
        virtual void JumpCall(chars gatename, sint32 runcount) = 0;
        virtual void JumpCallWithArea(chars gatename, sint32 runcount, float x, float y, float w, float h) = 0;
        virtual void JumpClear() = 0;
    };

    /// @brief 뷰스크립트 도구
    class ZaySonUtility
    {
    public:
        static String GetSafetyString(chars text);
        static Strings GetCommaStrings(chars text);
        static bool IsFunctionCall(chars text, sint32* prmbegin = nullptr, sint32* prmend = nullptr);
        static ZaySonInterface::ConditionType ToCondition(chars text, bool* withelse = nullptr);
    };

    /// @brief 뷰스크립트
    class ZaySon : public ZaySonInterface
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(ZaySon)
    public:
        ZaySon();
        ~ZaySon();
        ZaySon(ZaySon&& rhs);
        ZaySon& operator=(ZaySon&& rhs);

    public:
        enum class LogType {Info, Warning, Error, Option};
        typedef std::function<void(LogType type, chars title, chars detail)> LoggerCB;

    public:
        void Load(chars viewname, chars domheader, const Context& context);
        void Reload(const Context& context);
        void SetLogger(LoggerCB cb);
        void SetViewAndDom(chars viewname, chars domheader) override;
        const String& ViewName() const override;
        const String& DomHeader() const override;
        ZaySonInterface& AddComponent(ZayExtend::ComponentType type, chars name, ZayExtend::ComponentCB cb,
            chars param_comments = nullptr, chars inside_samples = nullptr) override;
        ZaySonInterface& AddGlue(chars name, ZayExtend::GlueCB cb,
            chars param_comments = nullptr, chars document = nullptr) override;
        void JumpCall(chars gatename, sint32 runcount) override;
        void JumpCallWithArea(chars gatename, sint32 runcount, float x, float y, float w, float h) override;
        void JumpClear() override;
        const ZayExtend* FindComponent(chars name) const;
        const ZayExtend* FindGlue(chars name) const;
        const void* FindGate(chars name) const;
        const Strings AllComponentNames() const;
        const Strings AllGlueNames() const;
        const Strings AllGateNames() const;
        bool Render(ZayPanel& panel);

    private:
        void SetGlobalSolvers(Solvers& solvers) const;

    private:
        String mViewName;
        String mDomHeader;
        void* mUIElement;
        Map<ZayExtend> mExtendMap;
        Strings mJumpCalls;

    public:
        void SendCursor(CursorRole role) const;
        void SendAtlas(chars json) const;
        void SendInfoLog(chars title, chars detail) const;
        void SendWarningLog(chars title, chars detail) const;
        void SendErrorLog(chars title, chars detail) const;
        void SetFocusCompID(sint32 id);
        void ClearFocusCompID();
        inline sint32 debugFocusedCompID() const
        {return mDebugFocusedCompID;}

    private:
        LoggerCB mDebugLogger;
        sint32 mDebugFocusedCompID;
    };
}
