#include <boss.hpp>
#include "boss_tween.hpp"

#include <platform/boss_platform.hpp>

namespace BOSS
{
    Tween1D::Tween1D(Updater* neighbor, float value, TweenUpdateCB cb)
        : Updater(neighbor)
    {
        Reset(value);
        m_updateCB = cb;
        m_needRepaint = false;
    }

    Tween1D::Tween1D(chars uigroup_literal, Updater* neighbor, float value, TweenUpdateCB cb)
        : Updater(uigroup_literal, neighbor)
    {
        Reset(value);
        m_updateCB = cb;
        m_needRepaint = false;
    }

    Tween1D::Tween1D(const Tween1D& rhs)
    {
        operator=(rhs);
    }

    Tween1D::~Tween1D()
    {
        Reset(0);
    }

    Tween1D& Tween1D::operator=(const Tween1D& rhs)
    {
        const float OldValue = m_value;
        m_value = rhs.m_value;
        m_valueStart = rhs.m_valueStart;
        m_valueEnd = rhs.m_valueEnd;
        m_timeStart = rhs.m_timeStart;
        m_timeEnd = rhs.m_timeEnd;
        m_updateCB = rhs.m_updateCB;
        m_needRepaint = (m_value != OldValue);
        return *this;
    }

    void Tween1D::Reset(float value)
    {
        const float OldValue = m_value;
        m_value = value;
        m_valueStart = value;
        m_valueEnd = value;
        m_timeStart = 0;
        m_timeEnd = 0;
        m_needRepaint = (m_value != OldValue);
    }

    void Tween1D::MoveTo(float value, float second)
    {
        m_valueStart = m_value;
        m_valueEnd = value;
        m_timeStart = Platform::Utility::CurrentTimeMsec();
        m_timeEnd = m_timeStart + (uint64) (Math::MaxF(0, second) * 1000);
        m_needRepaint = (m_value != m_valueEnd);
    }

    bool Tween1D::IsArrived()
    {
        const uint64 CurTime = Platform::Utility::CurrentTimeMsec();
        return (m_timeEnd < CurTime)? true : false;
    }

    void Tween1D::SetUpdateCB(TweenUpdateCB cb)
    {
        BOSS_ASSERT("cb는 nullptr가 될 수 없습니다", cb != nullptr);
        m_updateCB = cb;
    }

    bool Tween1D::UpdateForRender()
    {
        m_needRepaint = false;
        const uint64 CurTime = Platform::Utility::CurrentTimeMsec();
        const float OldValue = m_value;
        if(m_timeEnd < CurTime) m_value = m_valueEnd;
        else if(CurTime < m_timeStart) m_value = m_valueStart;
        else m_value = m_valueStart + (m_valueEnd - m_valueStart)
            * (CurTime - m_timeStart) / (m_timeEnd - m_timeStart);
        return m_updateCB(m_value != OldValue);
    }

    bool Tween1D::UpdateForTick()
    {
        const bool Result = m_needRepaint;
        m_needRepaint = false;
        return Result;
    }

    Tween2D::Tween2D(Updater* neighbor)
        : Updater(neighbor)
    {
        m_curpath = nullptr;
        m_lastmsec = 0;
        m_needRepaint = false;
    }

    Tween2D::Tween2D(chars uigroup_literal, Updater* neighbor)
        : Updater(uigroup_literal, neighbor)
    {
        m_curpath = nullptr;
        m_lastmsec = 0;
        m_needRepaint = false;
    }

    Tween2D::Tween2D(const Tween2D& rhs)
    {
        operator=(rhs);
    }

    Tween2D::~Tween2D()
    {
        Reset(0, 0);
    }

    Tween2D& Tween2D::operator=(const Tween2D& rhs)
    {
        Reset(rhs.m_curpos.x, rhs.m_curpos.y);
        return *this;
    }

    void Tween2D::Reset(float x, float y)
    {
        m_curpos.x = x;
        m_curpos.y = y;
        ResetPathes();
    }

    void Tween2D::ResetPathes()
    {
        Buffer::Free(m_curpath);
        m_curpath = nullptr;
        while(buffer RemBuffer = m_pathes.Dequeue())
            Buffer::Free(RemBuffer);
        m_lastpos.x = m_curpos.x;
        m_lastpos.y = m_curpos.y;
        m_lastmsec = 0;
        m_needRepaint = true;
    }

    class Tween2DPath
    {
    public:
        Tween2DPath()
        {
            m_begin_msec = 0;
            m_period_msec = 0;
        }
        ~Tween2DPath()
        {
        }

        Tween2DPath& operator=(const Tween2DPath& rhs)
        {
            m_begin = rhs.m_begin;
            m_end = rhs.m_end;
            m_begin_msec = rhs.m_begin_msec;
            m_period_msec = rhs.m_period_msec;
            return *this;
        }

    public:
        void Init(const Point& begin, const Point& end, uint64 msec, sint32 period)
        {
            m_begin = begin;
            m_end = end;
            m_begin_msec = msec;
            m_period_msec = period;
        }

        const Point NextStep(buffer& This)
        {
            const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
            if(m_period_msec <= 0 || m_begin_msec + m_period_msec <= CurMsec)
            {
                const Point SavedPos(m_end);
                Buffer::Free(This);
                This = nullptr;
                return SavedPos;
            }
            const float EndRate = Math::MaxF(0, (CurMsec - m_begin_msec) / (float) m_period_msec);
            const float BeginRate = 1 - EndRate;
            const float NewX = m_begin.x * BeginRate + m_end.x * EndRate;
            const float NewY = m_begin.y * BeginRate + m_end.y * EndRate;
            return Point(NewX, NewY);
        }

    private:
        Point m_begin;
        Point m_end;
        uint64 m_begin_msec;
        sint32 m_period_msec;
    };

    void Tween2D::MoveTo(float x, float y, float second)
    {
        if(m_pathes.Count() == 0)
            m_lastmsec = Platform::Utility::CurrentTimeMsec();
        const sint32 addmsec = (sint32) (second * 1000);

        Point NewPos = Point(x, y);
        Tween2DPath* NewPath = (Tween2DPath*) Buffer::Alloc<Tween2DPath, datatype_class_canmemcpy>(BOSS_DBG 1);
        NewPath->Init(m_lastpos, NewPos, m_lastmsec, addmsec);
        m_pathes.Enqueue((buffer) NewPath);
        m_lastpos = NewPos;
        m_lastmsec += addmsec;
        m_needRepaint = true;
    }

    void Tween2D::JumpTo(float x, float y)
    {
        MoveTo(x, y, 0);
    }

    bool Tween2D::IsArrived()
    {
        return (!m_curpath && m_pathes.Count() == 0);
    }

    bool Tween2D::IsArrivedAlmost(float second)
    {
        if(IsArrived()) return true;
        return ((m_lastmsec - Platform::Utility::CurrentTimeMsec()) * 0.001f <= second);
    }

    bool Tween2D::UpdateForRender()
    {
        m_needRepaint = false;
        if(!m_curpath && !(m_curpath = m_pathes.Dequeue()))
            return false;
        Tween2DPath* CurPath = (Tween2DPath*) m_curpath;
        m_curpos = CurPath->NextStep(m_curpath);
        return true;
    }

    bool Tween2D::UpdateForTick()
    {
        const bool Result = m_needRepaint;
        m_needRepaint = false;
        return Result;
    }
}
