#include <boss.hpp>
#include "boss_updater.hpp"

namespace BOSS
{
    Updater::Updater(Updater* neighbor, bool atprev)
        : m_uigroup(nullptr)
    {
        if(neighbor)
        {
            if(atprev) BindAtPrev(neighbor);
            else BindAtNext(neighbor);
        }
        else Init();
    }

    Updater::Updater(chars uigroup_literal, Updater* neighbor, bool atprev)
        : m_uigroup(uigroup_literal)
    {
        if(neighbor)
        {
            if(atprev) BindAtPrev(neighbor);
            else BindAtNext(neighbor);
        }
        else Init();
    }

    Updater::Updater(const Updater& rhs)
        : m_uigroup(rhs.m_uigroup)
    {
        operator=(rhs);
    }

    Updater::~Updater()
    {
        Unbind();
    }

    Updater& Updater::operator=(const Updater& rhs)
    {
        Unbind();
        Updater* neighbor = (Updater*) &rhs;
        BindAtNext(neighbor);
        return *this;
    }

    void Updater::BindUpdater(Updater* neighbor, bool atprev)
    {
        Unbind();
        if(atprev) BindAtPrev(neighbor);
        else BindAtNext(neighbor);
    }

    Updater* Updater::GoPrev(Updater* forefront)
    {
        if(m_prev != forefront)
        {
            Updater* OldPrev = m_prev;
            Unbind();
            BindAtPrev(OldPrev);
            return OldPrev;
        }
        return nullptr;
    }

    Updater* Updater::GoNext(Updater* rearmost)
    {
        if(m_next != rearmost)
        {
            Updater* OldNext = m_next;
            Unbind();
            BindAtNext(OldNext);
            return OldNext;
        }
        return nullptr;
    }

    Updater* Updater::GetNext() const
    {
        return m_next;
    }

    void Updater::Init()
    {
        m_prev = this;
        m_next = this;
    }

    void Updater::BindAtPrev(Updater* neighbor)
    {
        BOSS_ASSERT("neighbor가 nullptr입니다", neighbor);
        m_prev = neighbor->m_prev;
        m_next = neighbor;
        neighbor->m_prev->m_next = this;
        neighbor->m_prev = this;
    }

    void Updater::BindAtNext(Updater* neighbor)
    {
        BOSS_ASSERT("neighbor가 nullptr입니다", neighbor);
        m_prev = neighbor;
        m_next = neighbor->m_next;
        neighbor->m_next->m_prev = this;
        neighbor->m_next = this;
    }

    void Updater::Unbind()
    {
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
    }

    ////////////////////////////////////////////////////////////////////////////////
    FrameUpdater::FrameUpdater()
    {
    }

    FrameUpdater::FrameUpdater(const FrameUpdater& rhs)
    {
        operator=(rhs);
    }

    FrameUpdater::~FrameUpdater()
    {
    }

    FrameUpdater& FrameUpdater::operator=(const FrameUpdater& rhs)
    {
        Updater::operator=(rhs);
        return *this;
    }

    void FrameUpdater::Flush(void (*invalidator)(payload, chars), payload data)
    {
        Updater* CurUpdater = GetNext();
        while(CurUpdater != this)
        {
            Updater* NextUpdater = CurUpdater->GetNext();
            if(CurUpdater->UpdateForRender())
                invalidator(data, CurUpdater->m_uigroup);
            CurUpdater = NextUpdater;
        }
    }

    void FrameUpdater::WakeUp(void (*invalidator)(payload, chars), payload data)
    {
        Updater* CurUpdater = GetNext();
        while(CurUpdater != this)
        {
            Updater* NextUpdater = CurUpdater->GetNext();
            if(CurUpdater->UpdateForTick())
                invalidator(data, CurUpdater->m_uigroup);
            CurUpdater = NextUpdater;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    OrderUpdater::OrderUpdater()
    {
    }

    OrderUpdater::OrderUpdater(const OrderUpdater& rhs)
    {
        operator=(rhs);
    }

    OrderUpdater::~OrderUpdater()
    {
    }

    OrderUpdater& OrderUpdater::operator=(const OrderUpdater& rhs)
    {
        Updater::operator=(rhs);
        return *this;
    }

    OrderUpdater* OrderUpdater::Next() const
    {
        return (OrderUpdater*) GetNext();
    }

    void OrderUpdater::Sort(Updater* mover, SortCB cb) const
    {
        while(mover->m_prev != this && cb(mover->m_prev, mover))
        {
            Updater* First = mover->m_prev->m_prev;
            Updater* Target = mover->m_prev;
            Updater* Last = mover->m_next;
            First->m_next = mover;
            mover->m_prev = First;
            mover->m_next = Target;
            Target->m_prev = mover;
            Target->m_next = Last;
            Last->m_prev = Target;
        }
        while(mover->m_next != this && cb(mover, mover->m_next))
        {
            Updater* First = mover->m_prev;
            Updater* Target = mover->m_next;
            Updater* Last = mover->m_next->m_next;
            First->m_next = Target;
            Target->m_prev = First;
            Target->m_next = mover;
            mover->m_prev = Target;
            mover->m_next = Last;
            Last->m_prev = mover;
        }
    }
}
