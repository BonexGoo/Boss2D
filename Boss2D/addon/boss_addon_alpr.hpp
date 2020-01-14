#pragma once
#include <boss.hpp>

namespace BOSS
{
    class AlprInstance;

    class AlprClass
    {
    public:
        AlprClass();
        AlprClass(const AlprClass& rhs);
        ~AlprClass();
        AlprClass& operator=(const AlprClass& rhs);

    public:
        bool Init(chars country, h_view listener);
        inline h_view GetListener() {return m_listener;}
        inline AlprInstance* GetInstance() {return m_instance;}

    private:
        h_view m_listener;
        AlprInstance* m_instance;
    };
}
