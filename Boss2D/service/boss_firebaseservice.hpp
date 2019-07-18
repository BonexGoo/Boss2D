#pragma once
#include <boss.hpp>
#include <functional>

namespace BOSS
{
    //! \brief 서비스-Firebase
    class FirebaseService
    {
    public:
        class RequestPack
        {
        friend class FirebaseService;
        private: RequestPack(chars userId = nullptr);
        public: ~RequestPack();
        public: RequestPack& SetBirthday(sint32 year, sint32 month, sint32 day);
        private: void* mRequest;
        };
        typedef std::function<void(bool, chars)> ResultCB;

    public:
        FirebaseService();
        ~FirebaseService();

    public:
        RequestPack& SetRequest(chars userId);
        void PlayAdMob(chars appId, chars unitId, ResultCB cb);

    public:
        void* mFirebase;
        void* mAd;
        RequestPack* mRequestPack;
        ResultCB mResultCB;
    };
}
