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
        bool ExistValue(chars variable) const;
        SolverValue GetValue(chars variable) const;
        void SetValue(chars variable, chars formula);
        String GetComment(chars variable) const;
        void SetComment(chars variable, chars text);
        void SetJson(const Context& json, const String nameheader = "");
        void Remove(chars variable);
        void RemoveMatchedVariables(chars keyword, SolverRemoveCB cb = nullptr, payload param = nullptr);
        void CheckUpdatedSolvers(uint64 msec, UpdateCB cb);

    protected:
        Solver& LinkedSolver(chars variable);

    private:
        const String mChain;
        Solvers mSolvers;
    };

    /// @brief 뷰스크립트 원형
    class ZaySonInterface
    {
    public:
        enum class ConditionType {Unknown, If, IfFocused, IfHovered, IfPressed,
            IfDoubleClicked, IfLongPressed, IfOutReleased, IfCancelReleased, Else, Endif};
        enum class RequestType {Unknown, Function, Variable};

    public:
        virtual const String& ViewName() const = 0;
        virtual ZaySonInterface& AddComponent(ZayExtend::ComponentType type, chars name,
            ZayExtend::ComponentCB cb, chars paramcomments = nullptr, chars insidenames = nullptr) = 0;
        virtual ZaySonInterface& AddGlue(chars name, ZayExtend::GlueCB cb) = 0;
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
        void Load(chars viewname, const Context& context);
        void Reload(const Context& context);
        void SetLogger(LoggerCB cb);
        const String& ViewName() const override;
        ZaySonInterface& AddComponent(ZayExtend::ComponentType type, chars name,
            ZayExtend::ComponentCB cb, chars comments = nullptr, chars samples = nullptr) override;
        ZaySonInterface& AddGlue(chars name, ZayExtend::GlueCB cb) override;
        const ZayExtend* FindComponent(chars name) const;
        const ZayExtend* FindGlue(chars name) const;
        const void* FindGate(chars name) const;
        void Render(ZayPanel& panel);

    private:
        String mViewName;
        void* mUIElement;
        Map<ZayExtend> mExtendMap;

    public:
        void SendCursor(CursorRole role) const;
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
