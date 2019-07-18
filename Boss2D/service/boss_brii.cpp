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
        m_addIcon_B2D = nullptr;
        m_runScript_D2B = nullptr;
        m_exitScript_D2B = nullptr;
        m_kickScript_B2D = nullptr;
        m_addTrace_B2D = nullptr;
        m_assertBreak_B2D = nullptr;
        m_getApiClasses_D2B = nullptr;
        m_getApiClassDetail_D2B = nullptr;
        m_getApiMethods_D2B = nullptr;
        m_getApiMethodDetail_D2B = nullptr;
        m_beginEvent_B2D = nullptr;
        m_endEvent_D2B = nullptr;
        m_createApiClass_D2B = nullptr;
        m_removeApiClass_D2B = nullptr;
        m_callApiMethod_D2B = nullptr;
    }

    BriiConnector::~BriiConnector()
    {
        BOSS_ASSERT("잘못된 시나리오입니다", !m_addIcon_B2D);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_runScript_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_exitScript_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_kickScript_B2D);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_addTrace_B2D);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_assertBreak_B2D);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getApiClasses_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getApiClassDetail_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getApiMethods_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_getApiMethodDetail_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_beginEvent_B2D);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_endEvent_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_createApiClass_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_removeApiClass_D2B);
        BOSS_ASSERT("잘못된 시나리오입니다", !m_callApiMethod_D2B);
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

        delete m_addIcon_B2D;
        delete m_runScript_D2B;
        delete m_exitScript_D2B;
        delete m_kickScript_B2D;
        delete m_addTrace_B2D;
        delete m_assertBreak_B2D;
        delete m_getApiClasses_D2B;
        delete m_getApiClassDetail_D2B;
        delete m_getApiMethods_D2B;
        delete m_getApiMethodDetail_D2B;
        delete m_beginEvent_B2D;
        delete m_endEvent_D2B;
        delete m_createApiClass_D2B;
        delete m_removeApiClass_D2B;
        delete m_callApiMethod_D2B;
        m_addIcon_B2D = nullptr;
        m_runScript_D2B = nullptr;
        m_exitScript_D2B = nullptr;
        m_kickScript_B2D = nullptr;
        m_addTrace_B2D = nullptr;
        m_assertBreak_B2D = nullptr;
        m_getApiClasses_D2B = nullptr;
        m_getApiClassDetail_D2B = nullptr;
        m_getApiMethods_D2B = nullptr;
        m_getApiMethodDetail_D2B = nullptr;
        m_beginEvent_B2D = nullptr;
        m_endEvent_D2B = nullptr;
        m_createApiClass_D2B = nullptr;
        m_removeApiClass_D2B = nullptr;
        m_callApiMethod_D2B = nullptr;
    }

    void BriiConnector::BindTaskQueue(Queue<buffer>* taskAnswer)
    {
        m_taskAnswer = taskAnswer;
    }

    void BriiConnector::UnbindTaskQueue()
    {
        m_taskAnswer = nullptr;
    }

    void BriiConnector::LinkStep_AddIcon_BotToDebugger(chars name, bytes icon, sint32 iconLength) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_addIcon_B2D);
        (*m_addIcon_B2D)(name)(icon, iconLength);
    }

    void BriiConnector::LinkStep_RunScript_DebuggerToBot(chars mainClassName, sint32 runID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_runScript_D2B);
        (*m_runScript_D2B)(mainClassName)(runID);
    }

    void BriiConnector::LinkStep_ExitScript_DebuggerToBot(sint32 runID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_exitScript_D2B);
        (*m_exitScript_D2B)(runID);
    }

    void BriiConnector::LinkStep_KickScript_BotToDebugger(sint32 runID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_kickScript_B2D);
        (*m_kickScript_B2D)(runID);
    }

    void BriiConnector::DebugStep_AddTrace_BotToDebugger(sint32 level, chars comment) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_addTrace_B2D);
        (*m_addTrace_B2D)(level)(comment);
    }

    sint32 BriiConnector::DebugStep_AssertBreak_BotToDebugger(chars comment, chars filename, sint32 linenumber, chars funcname) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_assertBreak_B2D);
        Remote::IntParam Result((id_cloned_share) (*m_assertBreak_B2D)(comment)(filename)(linenumber)(funcname));
        return (sint32) Result.ConstValue();
    }

    String BriiConnector::DebugStep_GetApiClasses_DebuggerToBot() const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getApiClasses_D2B);
        return (id_cloned_share) (*m_getApiClasses_D2B)();
    }

    String BriiConnector::DebugStep_GetApiClassDetail_DebuggerToBot(chars className) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getApiClassDetail_D2B);
        return (id_cloned_share) (*m_getApiClassDetail_D2B)(className);
    }

    String BriiConnector::DebugStep_GetApiMethods_DebuggerToBot(chars className) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getApiMethods_D2B);
        return (id_cloned_share) (*m_getApiMethods_D2B)(className);
    }

    String BriiConnector::DebugStep_GetApiMethodDetail_DebuggerToBot(chars className, chars methodName) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_getApiMethodDetail_D2B);
        return (id_cloned_share) (*m_getApiMethodDetail_D2B)(className)(methodName);
    }

    const Remote::Method BriiConnector::RunStep_BeginEvent_BotToDebugger(chars methodName, sint32 eventID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_beginEvent_B2D);
        return (*m_beginEvent_B2D)(methodName)(eventID);
    }

    void BriiConnector::RunStep_EndEvent_DebuggerToBot(sint32 eventID, sint32 success) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_endEvent_D2B);
        (*m_endEvent_D2B)(eventID)(success);
    }

    void BriiConnector::RunStep_CreateApiClass_DebuggerToBot(chars className, sint32 classID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_createApiClass_D2B);
        (*m_createApiClass_D2B)(className)(classID);
    }

    void BriiConnector::RunStep_RemoveApiClass_DebuggerToBot(sint32 classID) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_removeApiClass_D2B);
        (*m_removeApiClass_D2B)(classID);
    }

    const Remote::Method BriiConnector::RunStep_CallApiMethod_DebuggerToBot(sint32 classID, chars methodName) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_callApiMethod_D2B);
        return (*m_callApiMethod_D2B)(classID)(methodName);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // BriiDebuggerConnector
    ////////////////////////////////////////////////////////////////////////////////
    BriiDebuggerConnector::BriiDebuggerConnector()
    {
        m_isRunning = false;
        m_lastRunID = -1;
    }

    BriiDebuggerConnector::~BriiDebuggerConnector()
    {
    }

    BriiDebuggerConnector& BriiDebuggerConnector::operator=(const BriiDebuggerConnector&)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void BriiDebuggerConnector::InitForThread()
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_remote == nullptr);
        m_addIcon_B2D = new Remote::Method("OnAddIcon", OnAddIcon, this);
        m_runScript_D2B = new Remote::Method("OnRunScript", OnNull, this);
        m_exitScript_D2B = new Remote::Method("OnExitScript", OnNull, this);
        m_kickScript_B2D = new Remote::Method("OnKickScript", OnKickScript, this);
        m_addTrace_B2D = new Remote::Method("OnAddTrace", OnAddTrace, this);
        m_assertBreak_B2D = new Remote::Method("OnAssertBreak", OnAssertBreak, this);
        m_getApiClasses_D2B = new Remote::Method("OnGetApiClasses", OnNull, this);
        m_getApiClassDetail_D2B = new Remote::Method("OnGetApiClassDetail", OnNull, this);
        m_getApiMethods_D2B = new Remote::Method("OnGetApiMethods", OnNull, this);
        m_getApiMethodDetail_D2B = new Remote::Method("OnGetApiMethodDetail", OnNull, this);
        m_beginEvent_B2D = new Remote::Method("OnBeginEvent", OnBeginEvent, this);
        m_endEvent_D2B = new Remote::Method("OnEndEvent", OnNull, this);
        m_createApiClass_D2B = new Remote::Method("OnCreateApiClass", OnNull, this);
        m_removeApiClass_D2B = new Remote::Method("OnRemoveApiClass", OnNull, this);
        m_callApiMethod_D2B = new Remote::Method("OnCallApiMethod", OnNull, this);

        BOSS_ASSERT("접속정보가 없습니다", m_port != 0);
        Remote::MethodPtrs Methods;
        Methods.AtAdding() = m_addIcon_B2D;
        Methods.AtAdding() = m_runScript_D2B;
        Methods.AtAdding() = m_exitScript_D2B;
        Methods.AtAdding() = m_kickScript_B2D;
        Methods.AtAdding() = m_addTrace_B2D;
        Methods.AtAdding() = m_assertBreak_B2D;
        Methods.AtAdding() = m_getApiClasses_D2B;
        Methods.AtAdding() = m_getApiClassDetail_D2B;
        Methods.AtAdding() = m_getApiMethods_D2B;
        Methods.AtAdding() = m_getApiMethodDetail_D2B;
        Methods.AtAdding() = m_beginEvent_B2D;
        Methods.AtAdding() = m_endEvent_D2B;
        Methods.AtAdding() = m_createApiClass_D2B;
        Methods.AtAdding() = m_removeApiClass_D2B;
        Methods.AtAdding() = m_callApiMethod_D2B;
        if(0 < m_domain.Length())
            m_remote = Remote::ConnectForClient(m_domain, m_port, Methods);
        else m_remote = Remote::ConnectForServer(m_port, Methods);
    }

    bool BriiDebuggerConnector::IsRunning()
    {
        return m_isRunning;
    }

    buffer BriiDebuggerConnector::RunScript(chars mainClassName)
    {
        if(!m_isRunning)
        {
            const sint32 NewRunID = ++m_lastRunID;
            m_isRunning = true;
            LinkStep_RunScript_DebuggerToBot(mainClassName, NewRunID);

            auto NewInitClass = (BriiEvent::InitClass*) Buffer::Alloc<BriiEvent::InitClass>(BOSS_DBG 1);
            NewInitClass->m_runID = NewRunID;
            return (buffer) NewInitClass;
        }
        return nullptr;
    }

    buffer BriiDebuggerConnector::ExitScript()
    {
        if(m_isRunning)
        {
            m_isRunning = false;
            LinkStep_ExitScript_DebuggerToBot(m_lastRunID);

            auto NewQuitClass = (BriiEvent::QuitClass*) Buffer::Alloc<BriiEvent::QuitClass>(BOSS_DBG 1);
            return (buffer) NewQuitClass;
        }
        return nullptr;
    }

    const Remote::Method BriiDebuggerConnector::CallApiMethod(sint32 classID, chars methodName)
    {
        return RunStep_CallApiMethod_DebuggerToBot(classID, methodName);
    }

    void BriiDebuggerConnector::EndEvent(sint32 eventID, bool success)
    {
        if(m_isRunning)
            RunStep_EndEvent_DebuggerToBot(eventID, (sint32) success);
    }

    void BriiDebuggerConnector::OnAddIcon(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerConnector* Self = (BriiDebuggerConnector*) data;

        auto NewIconPack = (BriiEvent::IconPack*) Buffer::Alloc<BriiEvent::IconPack>(BOSS_DBG 1);
        NewIconPack->m_botName = params[0].Drain();
        NewIconPack->m_iconBinary = params[1].Drain();

        const bool Result = Self->AddAnswer((buffer) NewIconPack);
        BOSS_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiDebuggerConnector::OnKickScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerConnector* Self = (BriiDebuggerConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiDebuggerConnector::OnAddTrace(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerConnector* Self = (BriiDebuggerConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiDebuggerConnector::OnAssertBreak(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerConnector* Self = (BriiDebuggerConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiDebuggerConnector::OnBeginEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerConnector* Self = (BriiDebuggerConnector*) data;

        auto NewCallEvent = (BriiEvent::CallEvent*) Buffer::Alloc<BriiEvent::CallEvent>(BOSS_DBG 1);
        NewCallEvent->m_methodName = params[0].Drain();
        NewCallEvent->m_eventID = (sint32) Remote::IntParam(params[1].Drain()).ConstValue();
        for(sint32 i = 2, iend = params.Count(); i < iend; ++i)
            NewCallEvent->m_params.AtAdding().Store(params[i].Drain());

        const bool Result = Self->AddAnswer((buffer) NewCallEvent);
        BOSS_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiDebuggerConnector::OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("봇이 시나리오에 맞지 않는 잘못된 호출을 하였습니다", localcall);
    }

    sint32 BriiDebuggerConnector::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        BriiDebuggerConnector* Self = (BriiDebuggerConnector*) self;
        Self->BindTaskQueue(&answer);
        if(!Self->m_remote) Self->InitForThread();
        const bool IsConnected = Remote::CommunicateOnce(Self->m_remote);
        Self->UnbindTaskQueue();

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
    // BriiBotConnector
    ////////////////////////////////////////////////////////////////////////////////
    BriiBotConnector::BriiBotConnector()
    {
        m_needSighUp = true;
        m_isRunning = false;
        m_isEventing = false;
        m_lastEventID = -1;
        m_icon = nullptr;
    }

    BriiBotConnector::~BriiBotConnector()
    {
        Buffer::Free(m_icon);
    }

    BriiBotConnector& BriiBotConnector::operator=(const BriiBotConnector&)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void BriiBotConnector::SetIcon(chars name, chars filename)
    {
        m_name = name;

        id_asset_read File = Asset::OpenForRead(filename);
        sint32 FileSize = Asset::Size(File);
        Buffer::Free(m_icon);

        m_icon = Buffer::Alloc(BOSS_DBG FileSize);
        Asset::Read(File, (uint08*) m_icon, FileSize);
        Asset::Close(File);
    }

    bool BriiBotConnector::IsRunningAndNotEventing()
    {
        return (m_isRunning && !m_isEventing);
    }

    void BriiBotConnector::AddApiMethod(chars apiMethodName, Remote::Method::GlueSubCB cb)
    {
        m_apiMethod(apiMethodName) = cb;
    }

    const Remote::Method BriiBotConnector::BeginEvent(chars methodName)
    {
        if(m_isRunning && !m_isEventing)
        {
            m_isEventing = true;
            return RunStep_BeginEvent_BotToDebugger(methodName, ++m_lastEventID);
        }
        return Remote::Method();
    }

    void BriiBotConnector::EndEventProc(EndEventResult result)
    {
        if(m_isEventing)
        {
            m_isEventing = false;

            /////////////////////////////////////////////////////
            /////////////////////////////////////////////////////
            /////////////////////////////////////////////////////

            bool Result = false;
            switch(result)
            {
            case EER_Done:
                Result = AddAnswer(Buffer::Alloc<BriiEvent::EventResult_Done>(BOSS_DBG 1));
                break;
            case EER_ExitScript:
                Result = AddAnswer(Buffer::Alloc<BriiEvent::EventResult_ExitScript>(BOSS_DBG 1));
                break;
            case EER_NotFound:
                Result = AddAnswer(Buffer::Alloc<BriiEvent::EventResult_NotFound>(BOSS_DBG 1));
                break;
            }
            BOSS_ASSERT("잘못된 시나리오입니다", Result);
        }
    }

    void BriiBotConnector::InitForThread()
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_remote == nullptr);
        m_addIcon_B2D = new Remote::Method("OnAddIcon", OnNull, this);
        m_runScript_D2B = new Remote::Method("OnRunScript", OnRunScript, this);
        m_exitScript_D2B = new Remote::Method("OnExitScript", OnExitScript, this);
        m_kickScript_B2D = new Remote::Method("OnKickScript", OnNull, this);
        m_addTrace_B2D = new Remote::Method("OnAddTrace", OnNull, this);
        m_assertBreak_B2D = new Remote::Method("OnAssertBreak", OnNull, this);
        m_getApiClasses_D2B = new Remote::Method("OnGetApiClasses", OnGetApiClasses, this);
        m_getApiClassDetail_D2B = new Remote::Method("OnGetApiClassDetail", OnGetApiClassDetail, this);
        m_getApiMethods_D2B = new Remote::Method("OnGetApiMethods", OnGetApiMethods, this);
        m_getApiMethodDetail_D2B = new Remote::Method("OnGetApiMethodDetail", OnGetApiMethodDetail, this);
        m_beginEvent_B2D = new Remote::Method("OnBeginEvent", OnNull, this);
        m_endEvent_D2B = new Remote::Method("OnEndEvent", OnEndEvent, this);
        m_createApiClass_D2B = new Remote::Method("OnCreateApiClass", OnCreateApiClass, this);
        m_removeApiClass_D2B = new Remote::Method("OnRemoveApiClass", OnRemoveApiClass, this);
        m_callApiMethod_D2B = new Remote::Method("OnCallApiMethod", OnCallApiMethod, this);

        BOSS_ASSERT("접속정보가 없습니다", m_port != 0);
        Remote::MethodPtrs Methods;
        Methods.AtAdding() = m_addIcon_B2D;
        Methods.AtAdding() = m_runScript_D2B;
        Methods.AtAdding() = m_exitScript_D2B;
        Methods.AtAdding() = m_kickScript_B2D;
        Methods.AtAdding() = m_addTrace_B2D;
        Methods.AtAdding() = m_assertBreak_B2D;
        Methods.AtAdding() = m_getApiClasses_D2B;
        Methods.AtAdding() = m_getApiClassDetail_D2B;
        Methods.AtAdding() = m_getApiMethods_D2B;
        Methods.AtAdding() = m_getApiMethodDetail_D2B;
        Methods.AtAdding() = m_beginEvent_B2D;
        Methods.AtAdding() = m_endEvent_D2B;
        Methods.AtAdding() = m_createApiClass_D2B;
        Methods.AtAdding() = m_removeApiClass_D2B;
        Methods.AtAdding() = m_callApiMethod_D2B;
        if(0 < m_domain.Length())
            m_remote = Remote::ConnectForClient(m_domain, m_port, Methods);
        else m_remote = Remote::ConnectForServer(m_port, Methods);
    }

    void BriiBotConnector::OnRunScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        Self->m_isRunning = true;
        Self->AddAnswer(Buffer::Alloc<BriiEvent::RunScript>(BOSS_DBG 1));
    }

    void BriiBotConnector::OnExitScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        Self->m_isRunning = false;
        Self->EndEventProc(EER_ExitScript);
    }

    void BriiBotConnector::OnGetApiClasses(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotConnector::OnGetApiClassDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotConnector::OnGetApiMethods(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotConnector::OnGetApiMethodDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotConnector::OnEndEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        Remote::IntParam EventID = params[0].Drain();
        Remote::IntParam Success = params[1].Drain();
        Self->EndEventProc((Success.ConstValue())? EER_Done : EER_NotFound);
    }

    void BriiBotConnector::OnCreateApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotConnector::OnRemoveApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        BOSS_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotConnector::OnCallApiMethod(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", data);
        BriiBotConnector* Self = (BriiBotConnector*) data;

        Remote::IntParam ClassID = params[0].Drain();
        Remote::StrParam MethodName = params[1].Drain();
        if(auto CurMethod = Self->m_apiMethod.Access(MethodName))
            (*CurMethod)(data, &params[2], params.Count() - 2, ret);
    }

    void BriiBotConnector::OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BOSS_ASSERT("디버거가 시나리오에 맞지 않는 잘못된 호출을 하였습니다", localcall);
    }

    sint32 BriiBotConnector::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        BriiBotConnector* Self = (BriiBotConnector*) self;
        Self->BindTaskQueue(&answer);
        if(!Self->m_remote) Self->InitForThread();
        const bool IsConnected = Remote::CommunicateOnce(Self->m_remote);
        Self->UnbindTaskQueue();

        if(IsConnected && Self->m_needSighUp)
        {
            Self->m_needSighUp = false;
            Self->LinkStep_AddIcon_BotToDebugger(Self->m_name, (bytes) Self->m_icon, Buffer::CountOf(Self->m_icon));
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
