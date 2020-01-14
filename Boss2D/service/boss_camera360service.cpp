#include <boss.hpp>
#include "boss_camera360service.hpp"

namespace BOSS
{
    Camera360Service::Camera360Service()
    {
        mTasking = nullptr;
    }

    Camera360Service::~Camera360Service()
    {
        Tasking::Release(mTasking, true);
    }

    Camera360Service::Camera360Service(Camera360Service&& rhs)
    {
        operator=(rhs);
    }

    Camera360Service& Camera360Service::operator=(Camera360Service&& rhs)
    {
        mServiceName = ToReference(rhs.mServiceName);
        mTasking = rhs.mTasking;
        rhs.mTasking = nullptr;
        return *this;
    }

    void Camera360Service::Init(chars servicename)
    {
        BOSS_ASSERT("본 객체는 여러번 초기화될 수 없습니다", !mTasking);
        mServiceName = servicename;
        mTasking = Tasking::Create(OnTask);
    }

    sint32 Camera360Service::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        return 100;
    }
}
