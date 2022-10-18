#include <boss.hpp>
#include "boss_tasking.hpp"

#include <platform/boss_platform.hpp>

class CommonClass
{
public:
    buffer Lock()
    {
        Mutex::Lock(m_mutex);
        return m_buffer;
    }

    void Unlock(buffer buf)
    {
        if(m_buffer != buf)
        {
            Buffer::Free(m_buffer);
            m_buffer = buf;
        }
        Mutex::Unlock(m_mutex);
    }

public:
    void SetParent(id_tasking parent) {m_ref_parent = parent;}
    id_tasking parent() const {return m_ref_parent;}

public:
    CommonClass()
    {
        m_buffer = nullptr;
        m_mutex = Mutex::Open();
        m_ref_parent = nullptr;
    }

    ~CommonClass()
    {
        Buffer::Free(m_buffer);
        Mutex::Close(m_mutex);
    }

private:
    buffer m_buffer;
    id_mutex m_mutex;
    id_tasking m_ref_parent;
};

class TaskingClass
{
public:
    enum BindingState {BS_Both, BS_OnlyTask, BS_OnlyUser, BS_WaitForTask};

public:
    bool IsAlive()
    {
        Mutex::Lock(m_mutex);
        bool Result = m_alived;
        Mutex::Unlock(m_mutex);
        return Result;
    }

    bool IsPause()
    {
        Mutex::Lock(m_mutex);
        bool Result = m_paused;
        Mutex::Unlock(m_mutex);
        return Result;
    }

    BindingState GetState()
    {
        Mutex::Lock(m_mutex);
        BindingState Result = m_state;
        Mutex::Unlock(m_mutex);
        return Result;
    }

    void DoDie()
    {
        Mutex::Lock(m_mutex);
        m_alived = false;
        AliveCounter().Set(false);
        Mutex::Unlock(m_mutex);
    }

    void SetPause(bool on)
    {
        Mutex::Lock(m_mutex);
        m_paused = on;
        Mutex::Unlock(m_mutex);
    }

    bool SetStateByCheck(BindingState check, BindingState state)
    {
        Mutex::Lock(m_mutex);
        bool Result = (m_state == check);
        if(!Result) m_state = state;
        Mutex::Unlock(m_mutex);
        return Result;
    }

public:
    static sint32 GetAliveCount()
    {
        return AliveCounter().Get();
    }

public:
    TaskingClass()
    {
        m_cb = nullptr;
        m_self = nullptr;
        m_alived = true;
        m_paused = false;
        m_state = BS_Both;
        m_mutex = Mutex::Open();
        AliveCounter().Set(true);
    }

    ~TaskingClass()
    {
        Buffer::Free(m_self);
        while(buffer RemBuffer = m_query.Dequeue())
            Buffer::Free(RemBuffer);
        while(buffer RemBuffer = m_answer.Dequeue())
            Buffer::Free(RemBuffer);
        Mutex::Close(m_mutex);
    }

public:
    Tasking::TaskCB m_cb;
    buffer m_self;
    Queue<buffer> m_query;
    Queue<buffer> m_answer;
    CommonClass m_common;

private:
    bool m_alived;
    bool m_paused;
    BindingState m_state;
    id_mutex m_mutex;

private:
    static SafeCounter& AliveCounter() {static SafeCounter _; return _;}
};

static void _TaskCore(void* arg)
{
    TaskingClass* This = (TaskingClass*) arg;
    const buffer FirstSelf = This->m_self;

    sint32 NextSleep = 0;
    while(0 <= NextSleep && This->GetState() == TaskingClass::BS_Both)
    {
        // true로 설정해야 같은 스레드의 소켓이 작동함
        const bool ProcessSocket = true;
        if(This->IsPause())
        {
            Platform::Utility::Sleep(100, false, ProcessSocket, false);
            continue;
        }
        else Platform::Utility::Sleep(NextSleep, false, ProcessSocket, false);
        NextSleep = This->m_cb(This->m_self, This->m_query, This->m_answer, (id_common) &This->m_common);
    }

    // 스레드 내부에서 만든 버퍼라면 스레드종료전 삭제
    if(FirstSelf != This->m_self)
    {
        Buffer::Free(This->m_self);
        This->m_self = nullptr;
    }

    This->DoDie();
    if(This->SetStateByCheck(TaskingClass::BS_OnlyTask, TaskingClass::BS_OnlyUser))
        Buffer::Free((buffer) This);

    // 스토리지(TLS) 영구제거
    Storage::ClearAll(SCL_SystemAndUser);
}

namespace BOSS
{
    id_tasking Tasking::Create(TaskCB cb, buffer self, prioritytype priority)
    {
        TaskingClass* NewTasking = (TaskingClass*) Buffer::Alloc<TaskingClass>(BOSS_DBG 1);
        NewTasking->m_cb = cb;
        NewTasking->m_self = self;
        NewTasking->m_common.SetParent((id_tasking) NewTasking);
        Platform::Utility::Threading(_TaskCore, NewTasking, priority);
        return (id_tasking) NewTasking;
    }

    void Tasking::Release(id_tasking tasking, bool doWait)
    {
        if(!tasking) return;
        if(((TaskingClass*) tasking)->SetStateByCheck(TaskingClass::BS_OnlyUser,
            (doWait)? TaskingClass::BS_WaitForTask : TaskingClass::BS_OnlyTask))
            Buffer::Free((buffer) tasking);
        else if(doWait)
        {
            while(((TaskingClass*) tasking)->IsAlive())
                Platform::Utility::Sleep(10, false, false, false);
            Buffer::Free((buffer) tasking);
        }
    }

    bool Tasking::IsReleased(id_common common)
    {
        if(!common) return true;
        return (((TaskingClass*) ((CommonClass*) common)->parent())->GetState() != TaskingClass::BS_Both);
    }

    void Tasking::Pause(id_tasking tasking)
    {
        if(!tasking) return;
        ((TaskingClass*) tasking)->SetPause(true);
    }

    void Tasking::Resume(id_tasking tasking)
    {
        if(!tasking) return;
        ((TaskingClass*) tasking)->SetPause(false);
    }

    bool Tasking::IsAlive(id_tasking tasking)
    {
        if(!tasking) return false;
        return ((TaskingClass*) tasking)->IsAlive();
    }

    sint32 Tasking::GetAliveCount()
    {
        return TaskingClass::GetAliveCount();
    }

    void Tasking::SendQuery(id_tasking tasking, buffer query)
    {
        if(!tasking) {Buffer::Free(query); return;}
        ((TaskingClass*) tasking)->m_query.Enqueue(query);
    }

    sint32 Tasking::GetQueryCount(id_tasking tasking)
    {
        if(!tasking) return 0;
        return ((TaskingClass*) tasking)->m_query.Count();
    }

    buffer Tasking::GetAnswer(id_tasking tasking)
    {
        if(!tasking) return nullptr;
        return ((TaskingClass*) tasking)->m_answer.Dequeue();
    }

    sint32 Tasking::GetAnswerCount(id_tasking tasking)
    {
        if(!tasking) return 0;
        return ((TaskingClass*) tasking)->m_answer.Count();
    }

    void Tasking::KeepAnswer(id_tasking tasking, buffer answer)
    {
        if(!tasking) {Buffer::Free(answer); return;}
        ((TaskingClass*) tasking)->m_answer.Enqueue(answer);
    }

    buffer Tasking::LockCommon(id_tasking tasking, bool autounlock)
    {
        if(!tasking) return nullptr;
        buffer Result = ((TaskingClass*) tasking)->m_common.Lock();
        if(autounlock && !Result)
            ((TaskingClass*) tasking)->m_common.Unlock(nullptr);
        return Result;
    }

    nullbuffer Tasking::UnlockCommon(id_tasking tasking, buffer buf)
    {
        if(!tasking) {Buffer::Free(buf); return nullptr;}
        ((TaskingClass*) tasking)->m_common.Unlock(buf);
        return nullptr;
    }

    buffer Tasking::LockCommonForTask(id_common common, bool autounlock)
    {
        if(!common) return nullptr;
        buffer Result = ((CommonClass*) common)->Lock();
        if(autounlock && !Result)
            ((CommonClass*) common)->Unlock(nullptr);
        return Result;
    }

    nullbuffer Tasking::UnlockCommonForTask(id_common common, buffer buf)
    {
        if(!common) {Buffer::Free(buf); return nullptr;}
        ((CommonClass*) common)->Unlock(buf);
        return nullptr;
    }
}
