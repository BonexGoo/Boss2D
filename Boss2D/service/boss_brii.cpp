#include <boss.hpp>
#include "boss_brii.hpp"

#include <platform/boss_platform.hpp>

namespace BOSS
{
    BriiConnector::BriiConnector()
    {
        m_taskAnswer = nullptr;
        m_remote = nullptr;
        m_port = 0;
        m_playerEntry_P2E = nullptr;
        m_runScript_E2P = nullptr;
        m_runScriptDone_E2P = nullptr;
        m_exitScript_E2P = nullptr;
        m_kickScript_P2E = nullptr;
        m_addTrace_P2E = nullptr;
        m_assertBreak_P2E = nullptr;
        m_getApiClasses_E2P = nullptr;
        m_getApiClassDetail_E2P = nullptr;
        m_getApiMethods_E2P = nullptr;
        m_getApiMethodDetail_E2P = nullptr;
        m_beginEvent_P2E = nullptr;
        m_endEvent_E2P = nullptr;
        m_setCurrentMission_P2E = nullptr;
        m_setMissionScript_P2E = nullptr;
        m_getMissionScript_P2E = nullptr;
        m_forceRun_P2E = nullptr;
        m_forceStop_P2E = nullptr;
        m_createApiClass_E2P = nullptr;
        m_removeApiClass_E2P = nullptr;
        m_callApiMethod_E2P = nullptr;
    }

    BriiConnector::~BriiConnector()
    {
        BOSS_ASSERT("잘못된 시나리오입니다", !m_playerEntry_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_runScript_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_runScriptDone_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_exitScript_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_kickScript_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_addTrace_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_assertBreak_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getApiClasses_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getApiClassDetail_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getApiMethods_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getApiMethodDetail_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_beginEvent_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_endEvent_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_setCurrentMission_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_setMissionScript_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getMissionScript_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_forceRun_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_forceStop_P2E);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_createApiClass_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_removeApiClass_E2P);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_callApiMethod_E2P);
    }

    BriiConnector& BriiConnector::operator=(const BriiConnector&)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void BriiConnector::SettingForServer(uint16 port)
    {
        m_domain = "";
        m_port = port;
    }

    void BriiConnector::SettingForClient(chars domain, uint16 port)
    {
        m_domain = domain;
        m_port = port;
    }

    bool BriiConnector::AddAnswer(buffer answer)
    {
        if(m_taskAnswer)
        {
            m_taskAnswer->Enqueue(answer);
            return true;
        }
        return false;
    }

    void BriiConnector::InitForThread()
    {
        BOSS_ASSERT("잘못된 시나리오입니다", false);
    }

    void BriiConnector::QuitForThread()
    {
        Remote::Disconnect(m_remote);
        m_remote = nullptr;

        delete m_playerEntry_P2E;
        delete m_runScript_E2P;
        delete m_runScriptDone_E2P;
        delete m_exitScript_E2P;
        delete m_kickScript_P2E;
        delete m_addTrace_P2E;
        delete m_assertBreak_P2E;
        delete m_getApiClasses_E2P;
        delete m_getApiClassDetail_E2P;
        delete m_getApiMethods_E2P;
        delete m_getApiMethodDetail_E2P;
        delete m_beginEvent_P2E;
        delete m_endEvent_E2P;
        delete m_setCurrentMission_P2E;
        delete m_setMissionScript_P2E;
        delete m_getMissionScript_P2E;
        delete m_forceRun_P2E;
        delete m_forceStop_P2E;
        delete m_createApiClass_E2P;
        delete m_removeApiClass_E2P;
        delete m_callApiMethod_E2P;
        m_playerEntry_P2E = nullptr;
        m_runScript_E2P = nullptr;
        m_runScriptDone_E2P = nullptr;
        m_exitScript_E2P = nullptr;
        m_kickScript_P2E = nullptr;
        m_addTrace_P2E = nullptr;
        m_assertBreak_P2E = nullptr;
        m_getApiClasses_E2P = nullptr;
        m_getApiClassDetail_E2P = nullptr;
        m_getApiMethods_E2P = nullptr;
        m_getApiMethodDetail_E2P = nullptr;
        m_beginEvent_P2E = nullptr;
        m_endEvent_E2P = nullptr;
        m_setCurrentMission_P2E = nullptr;
        m_setMissionScript_P2E = nullptr;
        m_getMissionScript_P2E = nullptr;
        m_forceRun_P2E = nullptr;
        m_forceStop_P2E = nullptr;
        m_createApiClass_E2P = nullptr;
        m_removeApiClass_E2P = nullptr;
        m_callApiMethod_E2P = nullptr;
    }

    void BriiConnector::BindTaskQueue(Queue<buffer>* taskAnswer)
    {
        m_taskAnswer = taskAnswer;
    }

    void BriiConnector::UnbindTaskQueue()
    {
        m_taskAnswer = nullptr;
    }

    void BriiConnector::LinkStep_PlayerEntry_PlayerToEditor(chars name, bytes icon, sint32 iconLength) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_playerEntry_P2E);
        (*m_playerEntry_P2E)(name)(icon, iconLength);
    }

    void BriiConnector::LinkStep_RunScript_EditorToPlayer(chars mainClassName, chars userName, sint32 runID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_runScript_E2P);
        (*m_runScript_E2P)(mainClassName)(userName)(runID);
    }

    void BriiConnector::LinkStep_RunScriptDone_EditorToPlayer(sint32 runID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_runScriptDone_E2P);
        (*m_runScriptDone_E2P)(runID);
    }

    void BriiConnector::LinkStep_ExitScript_EditorToPlayer(sint32 runID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_exitScript_E2P);
        (*m_exitScript_E2P)(runID);
    }

    void BriiConnector::LinkStep_KickScript_PlayerToEditor(sint32 runID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_kickScript_P2E);
        (*m_kickScript_P2E)(runID);
    }

    void BriiConnector::DebugStep_AddTrace_PlayerToEditor(sint32 level, chars comment) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_addTrace_P2E);
        (*m_addTrace_P2E)(level)(comment);
    }

    sint32 BriiConnector::DebugStep_AssertBreak_PlayerToEditor(chars comment, chars filename, sint32 linenumber, chars funcname) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_assertBreak_P2E);
        Remote::IntParam Result((id_cloned_share) (*m_assertBreak_P2E)(comment)(filename)(linenumber)(funcname));
        return (sint32) Result.ConstValue();
    }

    String BriiConnector::DebugStep_GetApiClasses_EditorToPlayer() const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getApiClasses_E2P);
        return (id_cloned_share) (*m_getApiClasses_E2P)();
    }

    String BriiConnector::DebugStep_GetApiClassDetail_EditorToPlayer(chars className) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getApiClassDetail_E2P);
        return (id_cloned_share) (*m_getApiClassDetail_E2P)(className);
    }

    String BriiConnector::DebugStep_GetApiMethods_EditorToPlayer(chars className) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getApiMethods_E2P);
        return (id_cloned_share) (*m_getApiMethods_E2P)(className);
    }

    String BriiConnector::DebugStep_GetApiMethodDetail_EditorToPlayer(chars className, chars methodName) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getApiMethodDetail_E2P);
        return (id_cloned_share) (*m_getApiMethodDetail_E2P)(className)(methodName);
    }

    const Remote::Method BriiConnector::RunStep_BeginEvent_PlayerToEditor(sint32 eventID, chars methodName) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_beginEvent_P2E);
        return (*m_beginEvent_P2E)(eventID)(methodName);
    }

    const Remote::Method BriiConnector::RunStep_EndEvent_EditorToPlayer(sint32 eventID, sint32 success) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_endEvent_E2P);
        return (*m_endEvent_E2P)(eventID)(success);
    }

    void BriiConnector::RunStep_SetCurrentMission_PlayerToEditor(chars missionName) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_setCurrentMission_P2E);
        (*m_setCurrentMission_P2E)(missionName);
    }

    void BriiConnector::RunStep_SetMissionScript_PlayerToEditor(chars script) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_setMissionScript_P2E);
        (*m_setMissionScript_P2E)(script);
    }

    String BriiConnector::RunStep_GetMissionScript_PlayerToEditor(chars missionName) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getMissionScript_P2E);
        return (id_cloned_share) (*m_getMissionScript_P2E)(missionName);
    }

    void BriiConnector::RunStep_ForceRun_PlayerToEditor() const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_forceRun_P2E);
        (*m_forceRun_P2E)();
    }

    void BriiConnector::RunStep_ForceStop_PlayerToEditor() const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_forceStop_P2E);
        (*m_forceStop_P2E)();
    }

    void BriiConnector::RunStep_CreateApiClass_EditorToPlayer(chars className, sint32 classID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_createApiClass_E2P);
        (*m_createApiClass_E2P)(className)(classID);
    }

    void BriiConnector::RunStep_RemoveApiClass_EditorToPlayer(sint32 classID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_removeApiClass_E2P);
        (*m_removeApiClass_E2P)(classID);
    }

    const Remote::Method BriiConnector::RunStep_CallApiMethod_EditorToPlayer(sint32 classID, chars methodName) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_callApiMethod_E2P);
        return (*m_callApiMethod_E2P)(classID)(methodName);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // BriiEditor
    ////////////////////////////////////////////////////////////////////////////////
    BriiEditor::BriiEditor()
    {
        m_isRunning = false;
        m_lastRunID = 0;
    }

    BriiEditor::~BriiEditor()
    {
    }

    BriiEditor& BriiEditor::operator=(const BriiEditor&)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void BriiEditor::InitForThread()
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_remote == nullptr);
        m_playerEntry_P2E = new Remote::Method("OnPlayerEntry", OnPlayerEntry, this);
        m_runScript_E2P = new Remote::Method("OnRunScript", OnNull, this);
        m_runScriptDone_E2P = new Remote::Method("OnRunScriptDone", OnNull, this);
        m_exitScript_E2P = new Remote::Method("OnExitScript", OnNull, this);
        m_kickScript_P2E = new Remote::Method("OnKickScript", OnKickScript, this);
        m_addTrace_P2E = new Remote::Method("OnAddTrace", OnAddTrace, this);
        m_assertBreak_P2E = new Remote::Method("OnAssertBreak", OnAssertBreak, this);
        m_getApiClasses_E2P = new Remote::Method("OnGetApiClasses", OnNull, this);
        m_getApiClassDetail_E2P = new Remote::Method("OnGetApiClassDetail", OnNull, this);
        m_getApiMethods_E2P = new Remote::Method("OnGetApiMethods", OnNull, this);
        m_getApiMethodDetail_E2P = new Remote::Method("OnGetApiMethodDetail", OnNull, this);
        m_beginEvent_P2E = new Remote::Method("OnBeginEvent", OnBeginEvent, this);
        m_endEvent_E2P = new Remote::Method("OnEndEvent", OnNull, this);
        m_setCurrentMission_P2E = new Remote::Method("OnSetCurrentMission", OnSetCurrentMission, this);
        m_setMissionScript_P2E = new Remote::Method("OnSetMissionScript", OnSetMissionScript, this);
        m_getMissionScript_P2E = new Remote::Method("OnGetMissionScript", OnGetMissionScript, this);
        m_forceRun_P2E = new Remote::Method("OnForceRun", OnForceRun, this);
        m_forceStop_P2E = new Remote::Method("OnForceStop", OnForceStop, this);
        m_createApiClass_E2P = new Remote::Method("OnCreateApiClass", OnNull, this);
        m_removeApiClass_E2P = new Remote::Method("OnRemoveApiClass", OnNull, this);
        m_callApiMethod_E2P = new Remote::Method("OnCallApiMethod", OnNull, this);

        BOSS_ASSERT("접속정보가 없습니다", m_port != 0);
        Remote::MethodPtrs Methods;
        Methods.AtAdding() = m_playerEntry_P2E;
        Methods.AtAdding() = m_runScript_E2P;
        Methods.AtAdding() = m_runScriptDone_E2P;
        Methods.AtAdding() = m_exitScript_E2P;
        Methods.AtAdding() = m_kickScript_P2E;
        Methods.AtAdding() = m_addTrace_P2E;
        Methods.AtAdding() = m_assertBreak_P2E;
        Methods.AtAdding() = m_getApiClasses_E2P;
        Methods.AtAdding() = m_getApiClassDetail_E2P;
        Methods.AtAdding() = m_getApiMethods_E2P;
        Methods.AtAdding() = m_getApiMethodDetail_E2P;
        Methods.AtAdding() = m_beginEvent_P2E;
        Methods.AtAdding() = m_endEvent_E2P;
        Methods.AtAdding() = m_setCurrentMission_P2E;
        Methods.AtAdding() = m_setMissionScript_P2E;
        Methods.AtAdding() = m_getMissionScript_P2E;
        Methods.AtAdding() = m_forceRun_P2E;
        Methods.AtAdding() = m_forceStop_P2E;
        Methods.AtAdding() = m_createApiClass_E2P;
        Methods.AtAdding() = m_removeApiClass_E2P;
        Methods.AtAdding() = m_callApiMethod_E2P;
        if(0 < m_domain.Length())
            m_remote = Remote::ConnectForClient(m_domain, m_port, Methods, true);
        else m_remote = Remote::ConnectForServer(m_port, Methods, false);
    }

    bool BriiEditor::IsRunning()
    {
        return m_isRunning;
    }

    bool BriiEditor::SelectPlayer(sint32 peerID)
    {
        return Remote::SelectPartnerByPeerID(m_remote, peerID);
    }

    void BriiEditor::KickPlayer(sint32 peerID)
    {
        Remote::KickPartner(m_remote, peerID);
    }

    sint32 BriiEditor::RunScript(chars mainClassName, chars userName)
    {
        if(!m_isRunning)
        {
            const sint32 NewRunID = ++m_lastRunID;
            m_isRunning = true;
            LinkStep_RunScript_EditorToPlayer(mainClassName, userName, NewRunID);
            return NewRunID;
        }
        return 0;
    }

    void BriiEditor::RunScriptDone()
    {
        if(m_isRunning)
            LinkStep_RunScriptDone_EditorToPlayer(m_lastRunID);
    }

    bool BriiEditor::ExitScript()
    {
        if(m_isRunning)
        {
            m_isRunning = false;
            LinkStep_ExitScript_EditorToPlayer(m_lastRunID);
            return true;
        }
        return false;
    }

    sint32 BriiEditor::CreateClassID()
    {
        static sint32 LastClassID = -1;
        return ++LastClassID;
    }

    const Remote::Method BriiEditor::CallApiMethod(sint32 classID, chars methodName)
    {
        return RunStep_CallApiMethod_EditorToPlayer(classID, methodName);
    }

    const Remote::Method BriiEditor::EndEvent(sint32 eventID, bool success)
    {
        if(m_isRunning)
            return RunStep_EndEvent_EditorToPlayer(eventID, (sint32) success);
        return Remote::Method();
    }

    void BriiEditor::OnPlayerEntry(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiEditor* Self = (BriiEditor*) data;

        auto NewPlayerEntry = (BriiEvent::PlayerEntry*) Buffer::Alloc<BriiEvent::PlayerEntry>(BOSS_DBG 1);
        NewPlayerEntry->m_playerID = Remote::GlobalValue::LastPeerID();
        NewPlayerEntry->m_playerName = params[0].Drain();
        NewPlayerEntry->m_iconBinary = params[1].Drain();

        const bool Result = Self->AddAnswer((buffer) NewPlayerEntry);
        BOSS_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiEditor::OnKickScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiEditor* Self = (BriiEditor*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiEditor::OnAddTrace(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiEditor* Self = (BriiEditor*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiEditor::OnAssertBreak(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiEditor* Self = (BriiEditor*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiEditor::OnBeginEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiEditor* Self = (BriiEditor*) data;

        auto NewCallEvent = (BriiEvent::CallEvent*) Buffer::Alloc<BriiEvent::CallEvent>(BOSS_DBG 1);
        NewCallEvent->m_eventID = (sint32) Remote::IntParam(params[0].Drain()).ConstValue();
        NewCallEvent->m_methodName = params[1].Drain();
        for(sint32 i = 2, iend = params.Count(); i < iend; ++i)
            NewCallEvent->m_params.AtAdding().Store(params[i].Drain());

        const bool Result = Self->AddAnswer((buffer) NewCallEvent);
        BOSS_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiEditor::OnSetCurrentMission(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        auto NewSetCurrentMission = (BriiEvent::SetCurrentMission*) Buffer::Alloc<BriiEvent::SetCurrentMission>(BOSS_DBG 1);
        NewSetCurrentMission->m_missionName = params[0].Drain();

        const bool Result = Self->AddAnswer((buffer) NewSetCurrentMission);
        BOSS_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiEditor::OnSetMissionScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        auto NewSetMissionScript = (BriiEvent::SetMissionScript*) Buffer::Alloc<BriiEvent::SetMissionScript>(BOSS_DBG 1);
        NewSetMissionScript->m_script = params[0].Drain();

        const bool Result = Self->AddAnswer((buffer) NewSetMissionScript);
        BOSS_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiEditor::OnGetMissionScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiEditor::OnForceRun(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        auto NewForceRun = (BriiEvent::ForceRun*) Buffer::Alloc<BriiEvent::ForceRun>(BOSS_DBG 1);

        const bool Result = Self->AddAnswer((buffer) NewForceRun);
        BOSS_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiEditor::OnForceStop(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        auto NewForceStop = (BriiEvent::ForceStop*) Buffer::Alloc<BriiEvent::ForceStop>(BOSS_DBG 1);

        const bool Result = Self->AddAnswer((buffer) NewForceStop);
        BOSS_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiEditor::OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("플레이어가 시나리오에 맞지 않는 잘못된 호출을 하였습니다", localcall);
    }

    sint32 BriiEditor::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        BriiEditor* Self = (BriiEditor*) self;
        Self->BindTaskQueue(&answer);
        if(!Self->m_remote) Self->InitForThread();
        const bool IsConnected = Remote::CommunicateOnce(Self->m_remote);
        Self->UnbindTaskQueue();

        // 연결종료된 피어정보의 전달
        auto& ExitPeerIDs = Remote::GlobalValue::ExitPeerIDs();
        for(sint32 i = 0, iend = ExitPeerIDs.Count(); i < iend; ++i)
        {
            auto NewPlayerExit = (BriiEvent::PlayerExit*) Buffer::Alloc<BriiEvent::PlayerExit>(BOSS_DBG 1);
            NewPlayerExit->m_playerID = ExitPeerIDs[i];
            answer.Enqueue((buffer) NewPlayerExit);
        }

        // 어플리케이션과의 통신
        while(buffer OneBuffer = query.Dequeue())
        {
            chars Text = (chars) OneBuffer;
            if(!String::Compare(Text, "{exit}"))
            {
                Self->QuitForThread();
                Buffer::Free(OneBuffer);
                return -1;
            }
            Buffer::Free(OneBuffer);
        }
        return 100;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // BriiPlayer
    ////////////////////////////////////////////////////////////////////////////////
    BriiPlayer::BriiPlayer()
    {
        m_needSighUp = true;
        m_isRunning = false;
        m_lastEventID = 0;
        m_icon = nullptr;
    }

    BriiPlayer::~BriiPlayer()
    {
        Buffer::Free(m_icon);
    }

    BriiPlayer& BriiPlayer::operator=(const BriiPlayer&)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void BriiPlayer::SetIcon(chars name, chars filename)
    {
        m_name = name;

        id_asset_read File = Asset::OpenForRead(filename);
        sint32 FileSize = Asset::Size(File);
        Buffer::Free(m_icon);

        m_icon = Buffer::Alloc(BOSS_DBG FileSize);
        Asset::Read(File, (uint08*) m_icon, FileSize);
        Asset::Close(File);
    }

    void BriiPlayer::AddApiMethod(chars apiMethodName, Remote::Method::GlueSubCB cb)
    {
        m_apiMethod(apiMethodName) = cb;
    }

    const Remote::Method BriiPlayer::BeginEvent(chars methodName, sint32* getEventID)
    {
        const sint32 NewEventID = ++m_lastEventID;
        m_eventWaiting[NewEventID] = true;
        if(getEventID) *getEventID = NewEventID;
        return RunStep_BeginEvent_PlayerToEditor(NewEventID, methodName);
    }

    void BriiPlayer::SetCurrentMission(chars missionName)
    {
        RunStep_SetCurrentMission_PlayerToEditor(missionName);
    }

    void BriiPlayer::SetMissionScript(chars script)
    {
        RunStep_SetMissionScript_PlayerToEditor(script);
    }

    String BriiPlayer::GetMissionScript(chars missionName)
    {
        return RunStep_GetMissionScript_PlayerToEditor(missionName);
    }

    void BriiPlayer::ForceRun()
    {
        RunStep_ForceRun_PlayerToEditor();
    }

    void BriiPlayer::ForceStop()
    {
        RunStep_ForceStop_PlayerToEditor();
    }

    void BriiPlayer::EndEventProc(EndEventType type, sint32 eventID, id_cloned_share result)
    {
        if(m_eventWaiting.Access(eventID))
        {
            m_eventWaiting.Remove(eventID);
            bool Result = false;
            switch(type)
            {
            case EET_Done:
                {
                    auto NewResult = (BriiEvent::EventResult_Done*) Buffer::Alloc<BriiEvent::EventResult_Done>(BOSS_DBG 1);
                    NewResult->m_eventID = eventID;
                    if(result) NewResult->m_result.Store(result);
                    Result = AddAnswer((buffer) NewResult);
                }
                break;
            case EET_NotFound:
                {
                    auto NewResult = (BriiEvent::EventResult_NotFound*) Buffer::Alloc<BriiEvent::EventResult_NotFound>(BOSS_DBG 1);
                    NewResult->m_eventID = eventID;
                    Result = AddAnswer((buffer) NewResult);
                }
                break;
            }
            BOSS_ASSERT("잘못된 시나리오입니다", Result);
        }
    }

    void BriiPlayer::InitForThread()
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_remote == nullptr);
        m_playerEntry_P2E = new Remote::Method("OnPlayerEntry", OnNull, this);
        m_runScript_E2P = new Remote::Method("OnRunScript", OnRunScript, this);
        m_runScriptDone_E2P = new Remote::Method("OnRunScriptDone", OnRunScriptDone, this);
        m_exitScript_E2P = new Remote::Method("OnExitScript", OnExitScript, this);
        m_kickScript_P2E = new Remote::Method("OnKickScript", OnNull, this);
        m_addTrace_P2E = new Remote::Method("OnAddTrace", OnNull, this);
        m_assertBreak_P2E = new Remote::Method("OnAssertBreak", OnNull, this);
        m_getApiClasses_E2P = new Remote::Method("OnGetApiClasses", OnGetApiClasses, this);
        m_getApiClassDetail_E2P = new Remote::Method("OnGetApiClassDetail", OnGetApiClassDetail, this);
        m_getApiMethods_E2P = new Remote::Method("OnGetApiMethods", OnGetApiMethods, this);
        m_getApiMethodDetail_E2P = new Remote::Method("OnGetApiMethodDetail", OnGetApiMethodDetail, this);
        m_beginEvent_P2E = new Remote::Method("OnBeginEvent", OnNull, this);
        m_endEvent_E2P = new Remote::Method("OnEndEvent", OnEndEvent, this);
        m_setCurrentMission_P2E = new Remote::Method("OnSetCurrentMission", OnNull, this);
        m_setMissionScript_P2E = new Remote::Method("OnSetMissionScript", OnNull, this);
        m_getMissionScript_P2E = new Remote::Method("OnGetMissionScript", OnNull, this);
        m_forceRun_P2E = new Remote::Method("OnForceRun", OnNull, this);
        m_forceStop_P2E = new Remote::Method("OnForceStop", OnNull, this);
        m_createApiClass_E2P = new Remote::Method("OnCreateApiClass", OnCreateApiClass, this);
        m_removeApiClass_E2P = new Remote::Method("OnRemoveApiClass", OnRemoveApiClass, this);
        m_callApiMethod_E2P = new Remote::Method("OnCallApiMethod", OnCallApiMethod, this);

        BOSS_ASSERT("접속정보가 없습니다", m_port != 0);
        Remote::MethodPtrs Methods;
        Methods.AtAdding() = m_playerEntry_P2E;
        Methods.AtAdding() = m_runScript_E2P;
        Methods.AtAdding() = m_runScriptDone_E2P;
        Methods.AtAdding() = m_exitScript_E2P;
        Methods.AtAdding() = m_kickScript_P2E;
        Methods.AtAdding() = m_addTrace_P2E;
        Methods.AtAdding() = m_assertBreak_P2E;
        Methods.AtAdding() = m_getApiClasses_E2P;
        Methods.AtAdding() = m_getApiClassDetail_E2P;
        Methods.AtAdding() = m_getApiMethods_E2P;
        Methods.AtAdding() = m_getApiMethodDetail_E2P;
        Methods.AtAdding() = m_beginEvent_P2E;
        Methods.AtAdding() = m_endEvent_E2P;
        Methods.AtAdding() = m_setCurrentMission_P2E;
        Methods.AtAdding() = m_setMissionScript_P2E;
        Methods.AtAdding() = m_getMissionScript_P2E;
        Methods.AtAdding() = m_forceRun_P2E;
        Methods.AtAdding() = m_forceStop_P2E;
        Methods.AtAdding() = m_createApiClass_E2P;
        Methods.AtAdding() = m_removeApiClass_E2P;
        Methods.AtAdding() = m_callApiMethod_E2P;
        if(0 < m_domain.Length())
            m_remote = Remote::ConnectForClient(m_domain, m_port, Methods, true);
        else m_remote = Remote::ConnectForServer(m_port, Methods, false);
    }

    void BriiPlayer::OnRunScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        Self->m_isRunning = true;
        auto NewResult = (BriiEvent::RunScript*) Buffer::Alloc<BriiEvent::RunScript>(BOSS_DBG 1);
        NewResult->m_runID = (sint32) Remote::IntParam(params[2].Drain()).ConstValue();
        NewResult->m_mainClassName = params[0].Drain();
        NewResult->m_userName = params[1].Drain();

        Self->AddAnswer((buffer) NewResult);
    }

    void BriiPlayer::OnRunScriptDone(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        auto NewResult = (BriiEvent::RunScriptDone*) Buffer::Alloc<BriiEvent::RunScriptDone>(BOSS_DBG 1);
        NewResult->m_runID = (sint32) Remote::IntParam(params[0].Drain()).ConstValue();

        Self->AddAnswer((buffer) NewResult);
    }

    void BriiPlayer::OnExitScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        Self->m_isRunning = false;
        auto NewResult = (BriiEvent::ExitScript*) Buffer::Alloc<BriiEvent::ExitScript>(BOSS_DBG 1);
        NewResult->m_runID = (sint32) Remote::IntParam(params[0].Drain()).ConstValue();

        Self->AddAnswer((buffer) NewResult);
    }

    void BriiPlayer::OnGetApiClasses(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiPlayer::OnGetApiClassDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiPlayer::OnGetApiMethods(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiPlayer::OnGetApiMethodDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiPlayer::OnEndEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        const sint32 EventID = (sint32) Remote::IntParam(params[0].Drain()).ConstValue();
        Remote::IntParam Success = params[1].Drain();
        Self->EndEventProc((Success.ConstValue())? EET_Done : EET_NotFound,
            EventID, (2 < params.Count())? params[2].Drain() : nullptr);
    }

    void BriiPlayer::OnCreateApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiPlayer::OnRemoveApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiPlayer::OnCallApiMethod(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiPlayer* Self = (BriiPlayer*) data;

        Remote::IntParam ClassID = params[0].Drain();
        Remote::StrParam MethodName = params[1].Drain();
        if(auto CurMethod = Self->m_apiMethod.Access(MethodName))
        {
            if(2 < params.Count())
                (*CurMethod)(data, &params[2], params.Count() - 2, ret);
            else (*CurMethod)(data, nullptr, 0, ret);
        }
    }

    void BriiPlayer::OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("에디터가 시나리오에 맞지 않는 잘못된 호출을 하였습니다", localcall);
    }

    sint32 BriiPlayer::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        BriiPlayer* Self = (BriiPlayer*) self;
        Self->BindTaskQueue(&answer);
        if(!Self->m_remote) Self->InitForThread();
        const bool IsConnected = Remote::CommunicateOnce(Self->m_remote);
        Self->UnbindTaskQueue();

        // 연결종료된 피어정보의 전달
        auto& ExitPeerIDs = Remote::GlobalValue::ExitPeerIDs();
        if(0 < ExitPeerIDs.Count())
        {
            auto NewEditorExit = (BriiEvent::EditorExit*) Buffer::Alloc<BriiEvent::EditorExit>(BOSS_DBG 1);
            answer.Enqueue((buffer) NewEditorExit);
            Self->m_needSighUp = true;
        }

        if(IsConnected && Self->m_needSighUp)
        {
            Self->m_needSighUp = false;
            Self->LinkStep_PlayerEntry_PlayerToEditor(Self->m_name, (bytes) Self->m_icon, Buffer::CountOf(Self->m_icon));
        }

        // 어플리케이션과의 통신
        while(buffer OneBuffer = query.Dequeue())
        {
            chars Text = (chars) OneBuffer;
            if(!String::Compare(Text, "{exit}"))
            {
                Self->QuitForThread();
                Buffer::Free(OneBuffer);
                return -1;
            }
            Buffer::Free(OneBuffer);
        }
        return 100;
    }
}
