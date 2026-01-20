#include <boss.hpp>
#include "boss_firebaseservice.hpp"

#if defined(BOSS_PLUGINS_FIREBASE) & !BOSS_WINDOWS_MINGW
    #include "firebase/admob.h"
    #include "firebase/admob/types.h"
    #include "firebase/admob/banner_view.h"
    #include "firebase/admob/native_express_ad_view.h"
    #include "firebase/admob/interstitial_ad.h"
    #include "firebase/admob/rewarded_video.h"

    #if BOSS_ANDROID
        extern JNIEnv* GetAndroidJNIEnv();
        extern jobject GetAndroidApplicationActivity();
    #elif BOSS_IPHONE
        void* GetIOSApplicationUIView();
    #endif
#endif

namespace BOSS
{
    FirebaseService::RequestPack::RequestPack(chars userId)
    {
        #if defined(BOSS_PLUGINS_FIREBASE) & !BOSS_WINDOWS_MINGW
            mRequest = new firebase::admob::AdRequest();
            auto Request = (firebase::admob::AdRequest*) mRequest;
            Memory::Set(Request, 0, sizeof(firebase::admob::AdRequest));
            if(userId)
            {
                Request->test_device_id_count = 1;
                Request->test_device_ids = new chars[1];
                Request->test_device_ids[0] = userId;
            }
        #else
            mRequest = nullptr;
        #endif
    }

    FirebaseService::RequestPack::~RequestPack()
    {
        #if defined(BOSS_PLUGINS_FIREBASE) & !BOSS_WINDOWS_MINGW
            auto Request = (firebase::admob::AdRequest*) mRequest;
            delete[] Request->test_device_ids;
            delete (firebase::admob::AdRequest*) mRequest;
        #endif
    }

    FirebaseService::RequestPack& FirebaseService::RequestPack::SetBirthday(sint32 year, sint32 month, sint32 day)
    {
        #if defined(BOSS_PLUGINS_FIREBASE) & !BOSS_WINDOWS_MINGW
            auto Request = (firebase::admob::AdRequest*) mRequest;
            Request->birthday_year = year;
            Request->birthday_month = month;
            Request->birthday_day = day;
            /*Request->gender = firebase::admob::kGenderUnknown;
            Request->birthday_day = 10;
            Request->birthday_month = 11;
            Request->birthday_year = 1976;
            static const char* kKeywords[] = {"AdMob", "C++", "Fun"};
            Request->keyword_count = sizeof(kKeywords) / sizeof(kKeywords[0]);
            Request->keywords = kKeywords;
            static const firebase::admob::KeyValuePair kRequestExtras[] = {{"the_name_of_an_extra", "the_value_for_that_extra"}};
            Request->extras_count = sizeof(kRequestExtras) / sizeof(kRequestExtras[0]);
            Request->extras = kRequestExtras;
            static const char* kTestDeviceIDs[] = {"2077ef9a63d2b398840261c8221a0c9b", "098fe087d987c9a878965454a65654d7"};
            Request->test_device_id_count = sizeof(kTestDeviceIDs) / sizeof(kTestDeviceIDs[0]);
            Request->test_device_ids = kTestDeviceIDs;*/
        #endif
        return *this;
    }

    FirebaseService::FirebaseService()
    {
        #if defined(BOSS_PLUGINS_FIREBASE) & !BOSS_WINDOWS_MINGW
            #if BOSS_ANDROID
                mFirebase = firebase::App::Create(firebase::AppOptions(), GetAndroidJNIEnv(), GetAndroidApplicationActivity());
            #else
                mFirebase = firebase::App::Create(firebase::AppOptions());
            #endif
        #else
            mFirebase = nullptr;
        #endif
        mAd = nullptr;
        mRequestPack = new RequestPack();
    }

    FirebaseService::~FirebaseService()
    {
        #if defined(BOSS_PLUGINS_FIREBASE) & !BOSS_WINDOWS_MINGW
            if(mAd)
            {
                delete (firebase::admob::InterstitialAd*) mAd;
                firebase::admob::Terminate();
            }
            delete (firebase::App*) mFirebase;
        #endif
        delete mRequestPack;
    }

    FirebaseService::RequestPack& FirebaseService::SetRequest(chars userId)
    {
        delete mRequestPack;
        mRequestPack = new RequestPack(userId);
        return *mRequestPack;
    }

    void FirebaseService::PlayAdMob(chars appId, chars unitId, ResultCB cb)
    {
        mResultCB = cb;
        #if defined(BOSS_PLUGINS_FIREBASE) & !BOSS_WINDOWS_MINGW
            if(mAd)
            {
                delete (firebase::admob::InterstitialAd*) mAd;
                firebase::admob::Terminate();
            }
            if(firebase::admob::Initialize(*((firebase::App*) mFirebase), appId) == firebase::InitResult::kInitResultSuccess)
            {
                mAd = new firebase::admob::InterstitialAd();
                auto Ad = (firebase::admob::InterstitialAd*) mAd;
                #if BOSS_ANDROID
                    Ad->Initialize((firebase::admob::AdParent) GetAndroidApplicationActivity(), unitId);
                #elif BOSS_IPHONE
                    Ad->Initialize((firebase::admob::AdParent) GetIOSApplicationUIView(), unitId);
                #else
                    firebase::admob::AdParent NewParent = {};
                    Ad->Initialize(NewParent, unitId);
                #endif

                Ad->InitializeLastResult().OnCompletion(
                    [this](const firebase::Future<void>& completed_future)->void
                    {
                        auto Ad = (firebase::admob::InterstitialAd*) mAd;
                        if(completed_future.status() == firebase::kFutureStatusComplete
                            && completed_future.error() == firebase::admob::kAdMobErrorNone)
                        {
                            Ad->LoadAd(*((const firebase::admob::AdRequest*) mRequestPack->mRequest));
                            Ad->LoadAdLastResult().OnCompletion(
                                [this](const firebase::Future<void>& completed_future)->void
                                {
                                    auto Ad = (firebase::admob::InterstitialAd*) mAd;
                                    if(completed_future.status() == firebase::kFutureStatusComplete
                                        && completed_future.error() == firebase::admob::kAdMobErrorNone)
                                    {
                                        Ad->Show();
                                        mResultCB(true, "");
                                    }
                                    else mResultCB(false, "Load Error");
                                });
                        }
                        else mResultCB(false, "Initialize Error(UnitId)");
                    });
            }
            else mResultCB(false, "Initialize Error(AppId)");
        #else
            mResultCB(false, "BOSS_PLUGINS_FIREBASE is required");
        #endif
    }
}
