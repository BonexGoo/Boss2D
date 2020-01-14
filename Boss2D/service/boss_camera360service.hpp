#pragma once
#include <boss.hpp>

namespace BOSS
{
    /// @brief 서비스-Camera360
    class Camera360Service
    {
    BOSS_DECLARE_NONCOPYABLE_CLASS(Camera360Service)

    public:
        Camera360Service();
        ~Camera360Service();
        Camera360Service(Camera360Service&& rhs);
        Camera360Service& operator=(Camera360Service&& rhs);

    public:
        void Init(chars servicename);

    public:
        inline chars service_name() const {return mServiceName;}

    private:
        static sint32 OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

    private:
        String mServiceName;
        id_tasking mTasking;
    };
}
