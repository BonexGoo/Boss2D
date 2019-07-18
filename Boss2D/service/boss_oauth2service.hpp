#pragma once
#include <element/boss_image.hpp>

namespace BOSS
{
    //! \brief 서비스-OAuth2
    class OAuth2Service
    {
    public:
        OAuth2Service();
        OAuth2Service(const OAuth2Service& rhs);
        virtual ~OAuth2Service();
        OAuth2Service& operator=(const OAuth2Service& rhs);

    // 스타팅함수
    public:
        static const Strings GetValidServices();
        static OAuth2Service Create(chars service);
        static OAuth2Service* CreatePtr(chars service);
        static void Remove(OAuth2Service* service);

    // 서비스함수
    public:
        virtual void Signin(chars option, bool dialog, bool clearcookies, id_bitmap clipper = nullptr) {}
        virtual void Signout() {}
        virtual bool IsSigning(bool needDestroy, bool* destroyResult = nullptr) {return false;}
        virtual bool IsSigned() const {return false;}
        virtual id_image_read GetWebImage(sint32 width, sint32 height) {return nullptr;}
        virtual void SendTouchToWeb(TouchType type, sint32 x, sint32 y) {}
        virtual void SendKeyToWeb(sint32 code, chars text, bool pressed) {}
        virtual chars GetServiceName() const {return "";}
        virtual const String& GetServiceId() const {static const String _; return _;}
        virtual const String& GetName() const {static const String _; return _;}
        virtual const String& GetComment() const {static const String _; return _;}
        virtual const Image& GetPicture() const {static const Image _; return _;}
        virtual const Image& GetBackground() const {static const Image _; return _;}
        virtual chars GetPictureUrl() const {return "";}
        virtual chars GetBackgroundUrl() const {return "";}

    // 데이터(공유모델)
    protected:
        const Share* mShare;
    };
}
