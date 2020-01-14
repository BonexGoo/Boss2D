#pragma once
#include <platform/boss_platform.hpp>

namespace BOSS
{
    /// @brief 브리통신
    class BriiConnector
    {
    public:
        BriiConnector();
        virtual ~BriiConnector();
        BriiConnector& operator=(const BriiConnector&);

    public:
        void SettingForServer(uint16 port);
        void SettingForClient(chars domain, uint16 port);
        bool AddAnswer(buffer answer);

    protected:
        virtual void InitForThread();
        void QuitForThread();
        void BindTaskQueue(Queue<buffer>* taskAnswer);
        void UnbindTaskQueue();

    protected:
        void LinkStep_AddIcon_BotToDebugger(chars name, bytes icon, sint32 iconLength) const;
        void LinkStep_RunScript_DebuggerToBot(chars mainClassName, sint32 runID) const;
        void LinkStep_ExitScript_DebuggerToBot(sint32 runID) const;
        void LinkStep_KickScript_BotToDebugger(sint32 runID) const;
        void DebugStep_AddTrace_BotToDebugger(sint32 level, chars comment) const;
        sint32 DebugStep_AssertBreak_BotToDebugger(chars comment, chars filename, sint32 linenumber, chars funcname) const;
        String DebugStep_GetApiClasses_DebuggerToBot() const;
        String DebugStep_GetApiClassDetail_DebuggerToBot(chars className) const;
        String DebugStep_GetApiMethods_DebuggerToBot(chars className) const;
        String DebugStep_GetApiMethodDetail_DebuggerToBot(chars className, chars methodName) const;
        const Remote::Method RunStep_BeginEvent_BotToDebugger(chars methodName, sint32 eventID) const;
        void RunStep_EndEvent_DebuggerToBot(sint32 eventID, sint32 success) const;
        void RunStep_CreateApiClass_DebuggerToBot(chars className, sint32 classID) const;
        void RunStep_RemoveApiClass_DebuggerToBot(sint32 classID) const;
        const Remote::Method RunStep_CallApiMethod_DebuggerToBot(sint32 classID, chars methodName) const;

    private:
        Queue<buffer>* m_taskAnswer;

    protected:
        id_remote m_remote;
        String m_domain;
        uint16 m_port;
        const Remote::Method* m_addIcon_B2D;
        const Remote::Method* m_runScript_D2B;
        const Remote::Method* m_exitScript_D2B;
        const Remote::Method* m_kickScript_B2D;
        const Remote::Method* m_addTrace_B2D;
        const Remote::Method* m_assertBreak_B2D;
        const Remote::Method* m_getApiClasses_D2B;
        const Remote::Method* m_getApiClassDetail_D2B;
        const Remote::Method* m_getApiMethods_D2B;
        const Remote::Method* m_getApiMethodDetail_D2B;
        const Remote::Method* m_beginEvent_B2D;
        const Remote::Method* m_endEvent_D2B;
        const Remote::Method* m_createApiClass_D2B;
        const Remote::Method* m_removeApiClass_D2B;
        const Remote::Method* m_callApiMethod_D2B;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BriiEvent
    ////////////////////////////////////////////////////////////////////////////////
    #define BRII_EVENT_CLASS(CLASS, INITLIST) \
        public: CLASS() : INITLIST {} \
        public: ~CLASS() {} \
        public: CLASS& operator=(const CLASS&) \
        {BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false); return *this;}
    enum BriiEventType {BET_Null,
        BET_EventResult_Done, BET_EventResult_ExitScript, BET_EventResult_NotFound,
        BET_IconPack, BET_RunScript, BET_InitClass, BET_QuitClass, BET_CallEvent, BET_UserEnum};

    class BriiEventClass
    {
    protected:
        BriiEventClass(BriiEventType type = BET_Null) : m_type(type) {}
        ~BriiEventClass() {}
    public:
        const BriiEventType m_type;
    };

    class BriiEvent
    {
    public:
        class EventResult_Done : public BriiEventClass
        {
            BRII_EVENT_CLASS(EventResult_Done, BriiEventClass(BET_EventResult_Done))
        };
        class EventResult_ExitScript : public BriiEventClass
        {
            BRII_EVENT_CLASS(EventResult_ExitScript, BriiEventClass(BET_EventResult_ExitScript))
        };
        class EventResult_NotFound : public BriiEventClass
        {
            BRII_EVENT_CLASS(EventResult_NotFound, BriiEventClass(BET_EventResult_NotFound))
        };
        class IconPack : public BriiEventClass
        {
            BRII_EVENT_CLASS(IconPack, BriiEventClass(BET_IconPack))
        public:
            Remote::StrParam m_botName;
            Remote::HexParam m_iconBinary;
        };
        class RunScript : public BriiEventClass
        {
            BRII_EVENT_CLASS(RunScript, BriiEventClass(BET_RunScript))
        };
        class InitClass : public BriiEventClass
        {
            BRII_EVENT_CLASS(InitClass, BriiEventClass(BET_InitClass))
        public:
            sint32 m_runID;
        };
        class QuitClass : public BriiEventClass
        {
            BRII_EVENT_CLASS(QuitClass, BriiEventClass(BET_QuitClass))
        };
        class CallEvent : public BriiEventClass
        {
            BRII_EVENT_CLASS(CallEvent, BriiEventClass(BET_CallEvent))
        public:
            Remote::StrParam m_methodName;
            sint32 m_eventID;
            Remote::Params m_params;
        };
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BriiDebuggerConnector
    ////////////////////////////////////////////////////////////////////////////////
    class BriiDebuggerConnector : public BriiConnector
    {
    public:
        BriiDebuggerConnector();
        ~BriiDebuggerConnector() override;
        BriiDebuggerConnector& operator=(const BriiDebuggerConnector&);

    private:
        void InitForThread() override;

    public:
        bool IsRunning();
        buffer RunScript(chars mainClassName);
        buffer ExitScript();
        const Remote::Method CallApiMethod(sint32 classID, chars methodName);
        void EndEvent(sint32 eventID, bool success);

    private:
        static void OnAddIcon(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnKickScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnAddTrace(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnAssertBreak(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnBeginEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);

    private:
        static void OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);

    public:
        static sint32 OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

    private:
        bool m_isRunning;
        sint32 m_lastRunID;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BriiBotConnector
    ////////////////////////////////////////////////////////////////////////////////
    class BriiBotConnector : public BriiConnector
    {
    public:
        BriiBotConnector();
        ~BriiBotConnector() override;
        BriiBotConnector& operator=(const BriiBotConnector&);

    private:
        void InitForThread() override;

    public:
        void SetIcon(chars name, chars filename);
        bool IsRunningAndNotEventing();
        void AddApiMethod(chars apiMethodName, Remote::Method::GlueSubCB cb);
        const Remote::Method BeginEvent(chars methodName);

    private:
        enum EndEventResult {EER_Done, EER_ExitScript, EER_NotFound};
        void EndEventProc(EndEventResult result);

    private:
        static void OnRunScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnExitScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnGetApiClasses(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnGetApiClassDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnGetApiMethods(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnGetApiMethodDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnEndEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnCreateApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnRemoveApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnCallApiMethod(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);

    private:
        static void OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);

    public:
        static sint32 OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

    private:
        bool m_needSighUp;
        bool m_isRunning;
        bool m_isEventing;
        sint32 m_lastEventID;
        String m_name;
        buffer m_icon;
        Map<Remote::Method::GlueSubCB> m_apiMethod;
    };
}
