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
        void LinkStep_PlayerEntry_PlayerToEditor(chars name, bytes icon, sint32 iconLength) const;
        void LinkStep_RunScript_EditorToPlayer(chars mainClassName, chars userName, sint32 runID) const;
        void LinkStep_RunScriptDone_EditorToPlayer(sint32 runID) const;
        void LinkStep_ExitScript_EditorToPlayer(sint32 runID) const;
        void LinkStep_KickScript_PlayerToEditor(sint32 runID) const;

    protected:
        void DebugStep_AddTrace_PlayerToEditor(sint32 level, chars comment) const;
        sint32 DebugStep_AssertBreak_PlayerToEditor(chars comment, chars filename, sint32 linenumber, chars funcname) const;
        String DebugStep_GetApiClasses_EditorToPlayer() const;
        String DebugStep_GetApiClassDetail_EditorToPlayer(chars className) const;
        String DebugStep_GetApiMethods_EditorToPlayer(chars className) const;
        String DebugStep_GetApiMethodDetail_EditorToPlayer(chars className, chars methodName) const;

    protected:
        const Remote::Method RunStep_BeginEvent_PlayerToEditor(sint32 eventID, chars methodName) const;
        const Remote::Method RunStep_EndEvent_EditorToPlayer(sint32 eventID, sint32 success) const;
        void RunStep_SetCurrentMission_PlayerToEditor(chars missionName) const;
        void RunStep_SetMissionScript_PlayerToEditor(chars script) const;
        String RunStep_GetMissionScript_PlayerToEditor(chars missionName) const;
        void RunStep_ForceRun_PlayerToEditor() const;
        void RunStep_ForceStop_PlayerToEditor() const;
        void RunStep_CreateApiClass_EditorToPlayer(chars className, sint32 classID) const;
        void RunStep_RemoveApiClass_EditorToPlayer(sint32 classID) const;
        const Remote::Method RunStep_CallApiMethod_EditorToPlayer(sint32 classID, chars methodName) const;

    private:
        Queue<buffer>* m_taskAnswer;

    protected:
        id_remote m_remote;
        String m_domain;
        uint16 m_port;
        const Remote::Method* m_playerEntry_P2E;
        const Remote::Method* m_runScript_E2P;
        const Remote::Method* m_runScriptDone_E2P;
        const Remote::Method* m_exitScript_E2P;
        const Remote::Method* m_kickScript_P2E;
        const Remote::Method* m_addTrace_P2E;
        const Remote::Method* m_assertBreak_P2E;
        const Remote::Method* m_getApiClasses_E2P;
        const Remote::Method* m_getApiClassDetail_E2P;
        const Remote::Method* m_getApiMethods_E2P;
        const Remote::Method* m_getApiMethodDetail_E2P;
        const Remote::Method* m_beginEvent_P2E;
        const Remote::Method* m_endEvent_E2P;
        const Remote::Method* m_setCurrentMission_P2E;
        const Remote::Method* m_setMissionScript_P2E;
        const Remote::Method* m_getMissionScript_P2E;
        const Remote::Method* m_forceRun_P2E;
        const Remote::Method* m_forceStop_P2E;
        const Remote::Method* m_createApiClass_E2P;
        const Remote::Method* m_removeApiClass_E2P;
        const Remote::Method* m_callApiMethod_E2P;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BriiEvent
    ////////////////////////////////////////////////////////////////////////////////
    #define BRII_EVENT_CLASS(CLASS, ...) \
        public: CLASS() : __VA_ARGS__ {} \
        public: ~CLASS() {} \
        public: CLASS& operator=(const CLASS&) \
        {BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false); return *this;}
    enum BriiEventType {BET_Null,
        BET_EventResult_Done, BET_EventResult_NotFound,
        BET_PlayerEntry, BET_PlayerExit, BET_EditorExit,
        BET_RunScript, BET_RunScriptDone, BET_ExitScript, BET_CallEvent,
        BET_SetCurrentMission, BET_SetMissionScript, BET_GetMissionScript,
        BET_ForceRun, BET_ForceStop, BET_UserEnum};

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
            BRII_EVENT_CLASS(EventResult_Done, BriiEventClass(BET_EventResult_Done), m_eventID(-1))
            public:
                sint32 m_eventID;
                Remote::Param m_result;
        };

        class EventResult_NotFound : public BriiEventClass
        {
            BRII_EVENT_CLASS(EventResult_NotFound, BriiEventClass(BET_EventResult_NotFound), m_eventID(-1))
        public:
            sint32 m_eventID;
        };

        class PlayerEntry : public BriiEventClass
        {
            BRII_EVENT_CLASS(PlayerEntry, BriiEventClass(BET_PlayerEntry), m_playerID(-1))
        public:
            sint32 m_playerID;
            Remote::StrParam m_playerName;
            Remote::HexParam m_iconBinary;
        };

        class PlayerExit : public BriiEventClass
        {
            BRII_EVENT_CLASS(PlayerExit, BriiEventClass(BET_PlayerExit), m_playerID(-1))
        public:
            sint32 m_playerID;
        };

        class EditorExit : public BriiEventClass
        {
            BRII_EVENT_CLASS(EditorExit, BriiEventClass(BET_EditorExit))
        };

        class RunScript : public BriiEventClass
        {
            BRII_EVENT_CLASS(RunScript, BriiEventClass(BET_RunScript), m_runID(-1))
        public:
            sint32 m_runID;
            Remote::StrParam m_mainClassName;
            Remote::StrParam m_userName;
        };

        class RunScriptDone : public BriiEventClass
        {
            BRII_EVENT_CLASS(RunScriptDone, BriiEventClass(BET_RunScriptDone))
        public:
            sint32 m_runID;
        };

        class ExitScript : public BriiEventClass
        {
            BRII_EVENT_CLASS(ExitScript, BriiEventClass(BET_ExitScript))
        public:
            sint32 m_runID;
        };

        class CallEvent : public BriiEventClass
        {
            BRII_EVENT_CLASS(CallEvent, BriiEventClass(BET_CallEvent), m_eventID(-1))
        public:
            sint32 m_eventID;
            Remote::StrParam m_methodName;
            Remote::Params m_params;
        };

        class SetCurrentMission : public BriiEventClass
        {
            BRII_EVENT_CLASS(SetCurrentMission, BriiEventClass(BET_SetCurrentMission))
        public:
            Remote::StrParam m_missionName;
        };

        class SetMissionScript : public BriiEventClass
        {
            BRII_EVENT_CLASS(SetMissionScript, BriiEventClass(BET_SetMissionScript))
        public:
            Remote::StrParam m_script;
        };

        class GetMissionScript : public BriiEventClass
        {
            BRII_EVENT_CLASS(GetMissionScript, BriiEventClass(BET_GetMissionScript))
        public:
            Remote::StrParam m_missionName;
        };

        class ForceRun : public BriiEventClass
        {
            BRII_EVENT_CLASS(ForceRun, BriiEventClass(BET_ForceRun))
        };

        class ForceStop : public BriiEventClass
        {
            BRII_EVENT_CLASS(ForceStop, BriiEventClass(BET_ForceStop))
        };
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BriiEditor
    ////////////////////////////////////////////////////////////////////////////////
    class BriiEditor : public BriiConnector
    {
    public:
        BriiEditor();
        ~BriiEditor() override;
        BriiEditor& operator=(const BriiEditor&);

    private:
        void InitForThread() override;

    public:
        bool IsRunning();
        bool SelectPlayer(sint32 peerID);
        void KickPlayer(sint32 peerID);
        sint32 RunScript(chars mainClassName, chars userName);
        void RunScriptDone();
        bool ExitScript();
        sint32 CreateClassID();
        const Remote::Method CallApiMethod(sint32 classID, chars methodName);
        const Remote::Method EndEvent(sint32 eventID, bool success);

    private:
        static void OnPlayerEntry(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnKickScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnAddTrace(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnAssertBreak(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnBeginEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnSetCurrentMission(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnSetMissionScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnGetMissionScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnForceRun(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnForceStop(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);

    private:
        static void OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);

    public:
        static sint32 OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

    private:
        bool m_isRunning;
        sint32 m_lastRunID;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // BriiPlayer
    ////////////////////////////////////////////////////////////////////////////////
    class BriiPlayer : public BriiConnector
    {
    public:
        BriiPlayer();
        ~BriiPlayer() override;
        BriiPlayer& operator=(const BriiPlayer&);

    private:
        void InitForThread() override;

    public:
        void SetIcon(chars name, chars filename);
        void AddApiMethod(chars apiMethodName, Remote::Method::GlueSubCB cb);
        const Remote::Method BeginEvent(chars methodName, sint32* getEventID = nullptr);
        void SetCurrentMission(chars missionName);
        void SetMissionScript(chars script);
        String GetMissionScript(chars missionName);
        void ForceRun();
        void ForceStop();

    public:
        bool IsRunning() const {return m_isRunning;}
        sint32 CountOfEventWaiting() const {return m_eventWaiting.Count();}

    private:
        enum EndEventType {EET_Done, EET_NotFound};
        void EndEventProc(EndEventType type, sint32 eventID, id_cloned_share result);

    private:
        static void OnRunScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
        static void OnRunScriptDone(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall);
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
        Map<bool> m_eventWaiting;
        sint32 m_lastEventID;
        String m_name;
        buffer m_icon;
        Map<Remote::Method::GlueSubCB> m_apiMethod;
    };
}
