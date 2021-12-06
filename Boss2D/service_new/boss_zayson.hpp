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
        void Add(chars variable, chars formula);
        void AddJson(const Context& json, const String nameheader = "");
        void AddFlush();
        void Update(chars variable, chars formula);
        void UpdateJson(const Context& json, const String nameheader = "");
        Strings MatchedVariables(chars keyword = nullptr);
        void Remove(chars variable);
        void CheckUpdatedSolvers(uint64 msec, UpdateCB cb);
        SolverValue GetValue(chars variable) const;

    protected:
        virtual void PostProcess(const String key, const String value) {}

    private:
        const String mChain;
        Solvers mSolvers;
        sint32 mExecutedCount;
    };

    /// @brief 뷰스크립트 원형
    class ZaySonInterface
    {
    public:
        enum class ConditionType {Unknown, If, IfFocused, IfHovered, IfPressed, Else, Endif};
        enum class DataType {Unknown, ViewScript, ImageMap};
        enum class RequestType {Unknown, Function, Variable};

    public:
        virtual const String& ViewName() const = 0;
        virtual ZaySonInterface& AddComponent(ZayExtend::ComponentType type, chars name, ZayExtend::ComponentCB cb, chars paramcomment = nullptr) = 0;
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
        ZaySonInterface& AddComponent(ZayExtend::ComponentType type, chars name, ZayExtend::ComponentCB cb, chars paramcomment = nullptr) override;
        ZaySonInterface& AddGlue(chars name, ZayExtend::GlueCB cb) override;
        const ZayExtend* FindComponent(chars name) const;
        const ZayExtend* FindGlue(chars name) const;
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
