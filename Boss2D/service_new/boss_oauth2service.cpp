#include <boss.hpp>
#include "boss_oauth2service.hpp"

#include <element/boss_image.hpp>
#include <format/boss_bmp.hpp>
#include <platform/boss_platform.hpp>

namespace BOSS
{
    class OAuth2ServiceImpl : public OAuth2Service
    {
    protected:
        class Data
        {
        public:
            Data()
            {
                mSigned = false;
                mNeedDestroyWeb = false;
                mCurl = AddOn::Curl::Create();
                mPictureClipper = nullptr;
            }
            ~Data()
            {
                Platform::Web::Release(mWeb);
                AddOn::Curl::Release(mCurl);
                Bmp::Remove(mPictureClipper);
            }
            Data(const Data& rhs) {operator=(rhs);}
            Data& operator=(const Data& rhs)
            {BOSS_ASSERT("This class is not allowed to be copied", false); return *this;}
        public:
            h_web mWeb;
            h_web_native mWebNative;
            bool mSigned;
            bool mNeedDestroyWeb;
            id_curl mCurl;
            String mClientId;
            String mClientSecret;
            String mAccessToken;
            String mRefreshToken;
            String mServiceId;
            String mName;
            String mComment;
            String mPictureUrl;
            String mBackgroundUrl;
            Image mPicture;
            Image mBackground;
            id_bitmap mPictureClipper;
        };

    protected:
        OAuth2ServiceImpl(buffer gift)
        {
            mShare = Share::Create(gift);
        }
        ~OAuth2ServiceImpl() override
        {
        }

    private:
        void Signin(chars option, bool dialog, bool clearcookies, id_bitmap clipper) override
        {
            Context ClientInfo(ST_Json, SO_OnlyReference, option);
            data().mClientId = ClientInfo[0].GetText();
            data().mClientSecret = ClientInfo[1].GetText();
            Bmp::Remove(data().mPictureClipper);
            data().mPictureClipper = clipper;

            data().mSigned = false;
            data().mNeedDestroyWeb = false;
            String ResultUrl = AddOn::Curl::GetRedirectUrl(data().mCurl, SigninCore(data_const().mClientId), 302);
            if(dialog) data().mWebNative = Platform::Web::CreateNative(ResultUrl, clearcookies, OnEvent, (payload) this);
            else data().mWeb = Platform::Web::Create(ResultUrl, 0, 0, clearcookies, OnEvent, (payload) this);
        }
        void Signout() override
        {
            Share::Remove(mShare);
            if(auto NewBuffer = CreateShare())
                mShare = Share::Create(NewBuffer);
            else mShare = nullptr;
        }
        bool IsSigning(bool needDestroy, bool* destroyResult = nullptr) override
        {
            if(needDestroy)
            {
                if(data().mNeedDestroyWeb)
                {
                    data().mNeedDestroyWeb = false;
                    Platform::Web::Release(data().mWeb);
                    Platform::Web::ReleaseNative(data().mWebNative);
                    if(destroyResult) *destroyResult = true;
                    return false;
                }
                else if(destroyResult) *destroyResult = false;
            }
            auto& CurWeb = data_const().mWeb;
            auto& CurWebNative = data_const().mWebNative;
            return (CurWeb.get() || CurWebNative.get());
        }
        bool IsSigned() const override
        {
            return data_const().mSigned;
        }
        id_image_read GetWebImage(sint32 width, sint32 height) override
        {
            if(IsSigning(false))
            {
                Platform::Web::Resize(data().mWeb, width, height);
                return Platform::Web::GetPageImage(data().mWeb);
            }
            return nullptr;
        }
        void SendTouchToWeb(TouchType type, sint32 x, sint32 y) override
        {
            if(IsSigning(false))
                Platform::Web::SendTouchEvent(data().mWeb, type, x, y);
        }
        void SendKeyToWeb(sint32 code, chars text, bool pressed) override
        {
            if(IsSigning(false))
                Platform::Web::SendKeyEvent(data().mWeb, code, text, pressed);
        }
        chars GetServiceName() const override
        {
            return service_name();
        }
        const String& GetServiceId() const override
        {
            return data_const().mServiceId;
        }
        const String& GetName() const override
        {
            return data_const().mName;
        }
        const String& GetComment() const override
        {
            return data_const().mComment;
        }
        const Image& GetPicture() const override
        {
            return data_const().mPicture;
        }
        const Image& GetBackground() const override
        {
            return data_const().mBackground;
        }
        chars GetPictureUrl() const override
        {
            return data_const().mPictureUrl;
        }
        chars GetBackgroundUrl() const override
        {
            return data_const().mBackgroundUrl;
        }

    protected:
        virtual buffer CreateShare()
        {
            return Buffer::Alloc<Data>(BOSS_DBG 1);
        }

    private:
        static void OnEvent(payload self, chars type, chars text)
        {
            auto Self = (OAuth2ServiceImpl*) self;
            if(!String::Compare(type, "UrlChanged"))
            {
                chars CallbackUrl = "http://" "localhost/oauth2callback?";
                static const sint32 CallbackUrlLen = boss_strlen(CallbackUrl);
                if(!String::Compare(text, CallbackUrl, CallbackUrlLen))
                    OnEventUrlChanged(self, text + CallbackUrlLen);
            }
        }
        static void OnEventUrlChanged(payload self, chars text)
        {
            auto Self = (OAuth2ServiceImpl*) self;

            String Code = text;
            const sint32 FindBegin = Code.Find(0, "code=");
            if(FindBegin != -1) Code = Code.Right(Code.Length() - (FindBegin + 5));
            const sint32 FindEnd = Code.Find(0, "&");
            if(FindEnd != -1) Code = Code.Left(FindEnd);
            else
            {
                const sint32 FindEnd2 = Code.Find(0, " ");
                if(FindEnd2 != -1) Code = Code.Left(FindEnd2);
            }

            Self->OnEventCore(Code);

            // 서명됨
            Self->data().mSigned = true;
            // 웹제거요청
            Self->data().mNeedDestroyWeb = true;
        }

    protected:
        void ReloadPicture(chars url)
        {
            if(url && *url)
            {
                sint32 GetSize = 0;
                bytes Result = AddOn::Curl::GetBytes(data().mCurl, url, &GetSize);
                if(id_bitmap NewBitmap = AddOn::Jpg::ToBmp(Result, GetSize))
                {
                    data().mPicture.LoadBitmap(NewBitmap);
                    if(data().mPictureClipper)
                        data().mPicture.ReplaceAlphaChannelBy(data().mPictureClipper);
                    Bmp::Remove(NewBitmap);
                }
                else data().mPicture.Clear();
            }
            else data().mPicture.Clear();
        }
        void ReloadBackground(chars url)
        {
            if(url && *url)
            {
                sint32 GetSize = 0;
                bytes Result = AddOn::Curl::GetBytes(data().mCurl, url, &GetSize);
                if(id_bitmap NewBitmap = AddOn::Jpg::ToBmp(Result, GetSize))
                {
                    data().mBackground.LoadBitmap(NewBitmap);
                    Bmp::Remove(NewBitmap);
                }
                else data().mBackground.Clear();
            }
            else data().mBackground.Clear();
        }

    private:
        virtual String SigninCore(chars clientId) = 0;
        virtual void OnEventCore(chars code) = 0;

    protected:
        inline Data& data() {return mShare->At<Data>(0);}
        inline const Data& data_const() const {return mShare->At<Data>(0);}
        virtual chars service_name() const = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Google+
    ////////////////////////////////////////////////////////////////////////////////
    class OAuth2GoogleService : public OAuth2ServiceImpl
    {
    private:
        chars service_name() const override {return "google";}

    public:
        OAuth2GoogleService() : OAuth2ServiceImpl(CreateShare())
        {
        }
        ~OAuth2GoogleService() override
        {
        }

    private:
        String SigninCore(chars clientId) override
        {
            return String::Format(
                "https://" "accounts.google.com/o/oauth2/auth?"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "scope=https://" "www.googleapis.com/auth/plus.me&"
                "response_type=code&"
                "access_type=offline", clientId);
        }
        void OnEventCore(chars code) override
        {
            // 접근토큰/갱신토큰 얻기
            const String PostData = String::Format(
                "code=%s&"
                "client_id=%s&"
                "client_secret=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "grant_type=authorization_code",
                code, (chars) data_const().mClientId, (chars) data_const().mClientSecret);
            chars ResultA = AddOn::Curl::GetString(data().mCurl,
                "https://" "accounts.google.com/o/oauth2/token", nullptr, AddOn::Curl::ST_Post, PostData);
            const Context ResultAJson(ST_Json, SO_OnlyReference, ResultA);
            data().mAccessToken = ResultAJson("access_token").GetText();
            data().mRefreshToken = ResultAJson("refresh_token").GetText();

            // 회원정보 얻기(https://developers.google.com/apis-explorer/?hl=ko#p/plus/v1/ 에 방문하여 슬라이드를 ON으로 변경)
            chars ResultB = AddOn::Curl::GetString(data().mCurl,
                String::Format("https://" "www.googleapis.com/plus/v1/people/me?access_token=%s", (chars) data_const().mAccessToken));
            const Context ResultBJson(ST_Json, SO_OnlyReference, ResultB);
            data().mName = ResultBJson("displayName").GetText();
            data().mServiceId = String("google_") + ResultBJson("id").GetText();
            data().mComment = String::Format("팔로워 %d명 - %s",
                (sint32) ResultBJson("circledByCount").GetInt(), (chars) ResultBJson("tagline").GetText());

            // 사진/배경 얻기
            data().mPictureUrl = ResultBJson("image")("url").GetText();
            ReloadPicture(data().mPictureUrl);
            data().mBackgroundUrl = ResultBJson("cover")("coverPhoto")("url").GetText();
            ReloadBackground(data().mBackgroundUrl);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Facebook
    ////////////////////////////////////////////////////////////////////////////////
    class OAuth2FacebookService : public OAuth2ServiceImpl
    {
    private:
        chars service_name() const override {return "facebook";}

    public:
        OAuth2FacebookService() : OAuth2ServiceImpl(CreateShare())
        {
        }
        ~OAuth2FacebookService() override
        {
        }

    private:
        String SigninCore(chars clientId) override
        {
            // 스코프는 App Review에서 변경, developers.facebook.com/apps/[앱ID]/review-status
            return String::Format(
                "https://" "www.facebook.com/dialog/oauth?"
                "client_id=%s&"
                "display=popup&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "scope=public_profile,user_photos", clientId);
        }
        void OnEventCore(chars code) override
        {
            // 접근토큰 얻기
            const String Url = String::Format(
                "https://" "graph.facebook.com/oauth/access_token?"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "client_secret=%s&"
                "code=%s",
                (chars) data_const().mClientId, (chars) data_const().mClientSecret, code);
            chars ResultA = AddOn::Curl::GetString(data().mCurl, Url);
            const Context ResultAJson(ST_Json, SO_OnlyReference, ResultA);
            data().mAccessToken = ResultAJson("access_token").GetText();

            // 회원정보 얻기
            chars ResultB = AddOn::Curl::GetString(data().mCurl, String::Format(
                "https://" "graph.facebook.com/me?access_token=%s&"
                "fields=id,name,picture,cover,context", (chars) data().mAccessToken));
            const Context ResultBJson(ST_Json, SO_OnlyReference, ResultB);
            data().mName = ResultBJson("name").GetText();
            data().mServiceId = String("facebook_") + ResultBJson("id").GetText();
            data().mComment = String::Format("좋아요 총 %d건",
                (sint32) ResultBJson("context")("mutual_likes")("summary")("total_count").GetInt());

            // 사진/배경 얻기
            data().mPictureUrl = ResultBJson("picture")("data")("url").GetText();
            if(0 < data().mPictureUrl.Length())
                ReloadPicture(data().mPictureUrl);
            data().mBackgroundUrl = ResultBJson("cover")("source").GetText();
            if(0 < data().mBackgroundUrl.Length())
                ReloadBackground(data().mBackgroundUrl);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Kakao
    ////////////////////////////////////////////////////////////////////////////////
    class OAuth2KakaoService : public OAuth2ServiceImpl
    {
    private:
        chars service_name() const override {return "kakao";}

    public:
        OAuth2KakaoService() : OAuth2ServiceImpl(CreateShare())
        {
        }
        ~OAuth2KakaoService() override
        {
        }

    private:
        String SigninCore(chars clientId) override
        {
            // 사이트 도메인과 Redirect Path를 선행하여 수정할 것(https://developers.kakao.com/apps/[앱번호]/settings/general)
            return String::Format(
                "https://" "kauth.kakao.com/oauth/authorize?"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "response_type=code", clientId);
        }
        void OnEventCore(chars code) override
        {
            // 접근토큰 얻기
            const String PostData = String::Format(
                "grant_type=authorization_code&"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "code=%s",
                (chars) data_const().mClientId, code);
            chars ResultA = AddOn::Curl::GetString(data().mCurl,
                "https://" "kauth.kakao.com/oauth/token", nullptr, AddOn::Curl::ST_Post, PostData);
            const Context ResultAJson(ST_Json, SO_OnlyReference, ResultA);
            data().mAccessToken = ResultAJson("access_token").GetText();
            data().mRefreshToken = ResultAJson("refresh_token").GetText();

            // 회원정보 얻기
            chars ResultB = AddOn::Curl::GetString(data().mCurl,
                "https://" "kapi.kakao.com/v1/api/story/profile",
                String::Format("Authorization: Bearer %s", (chars) data().mAccessToken));
            const Context ResultBJson(ST_Json, SO_OnlyReference, ResultB);
            data().mName = ResultBJson("nickName").GetText();
            chars BirthType = "";
            if(!String::Compare(ResultBJson("birthdayType").GetText(), "SOLAR")) BirthType = "(양력)";
            else if(!String::Compare(ResultBJson("birthdayType").GetText(), "LUNAR")) BirthType = "(음력)";
            const sint32 BirthDay = (sint32) ResultBJson("birthday").GetInt();
            data().mComment = String::Format("생일%s %d월 %d일", BirthType, BirthDay / 100, BirthDay % 100);

            // 사진/배경 얻기
            data().mPictureUrl = ResultBJson("thumbnailURL").GetText();
            ReloadPicture(data().mPictureUrl);
            data().mBackgroundUrl = ResultBJson("bgImageURL").GetText();
            ReloadBackground(data().mBackgroundUrl);

            // ID정보 얻기
            chars ResultC = AddOn::Curl::GetString(data().mCurl,
                "https://" "kapi.kakao.com/v1/user/me",
                String::Format("Authorization: Bearer %s", (chars) data().mAccessToken));
            const Context ResultCJson(ST_Json, SO_OnlyReference, ResultC);
            data().mServiceId = String("kakao_") + ResultCJson("id").GetText();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // OAuth2Service
    ////////////////////////////////////////////////////////////////////////////////
    OAuth2Service::OAuth2Service() : mShare(nullptr)
    {
    }

    OAuth2Service::OAuth2Service(const OAuth2Service& rhs) : mShare(nullptr)
    {
        operator=(rhs);
    }

    OAuth2Service::~OAuth2Service()
    {
        Share::Remove(mShare);
    }

    OAuth2Service& OAuth2Service::operator=(const OAuth2Service& rhs)
    {
        Share::Remove(mShare);
        if(rhs.mShare) mShare = rhs.mShare->Clone();
        Memory::ReplaceVPTR(this, &rhs);
        return *this;
    }

    const Strings OAuth2Service::GetValidServices()
    {
        static Strings Services = []()
        {
            Strings Collector;
            Collector.AtAdding() = "Google+";
            Collector.AtAdding() = "Facebook";
            Collector.AtAdding() = "Kakao";
            return Collector;
        }();
        return Services;
    }

    OAuth2Service OAuth2Service::Create(chars service)
    {
        if(!String::CompareNoCase(service, "Google+"))
            return OAuth2GoogleService();
        if(!String::CompareNoCase(service, "Facebook"))
            return OAuth2FacebookService();
        if(!String::CompareNoCase(service, "Kakao"))
            return OAuth2KakaoService();
        return OAuth2Service();
    }

    OAuth2Service* OAuth2Service::CreatePtr(chars service)
    {
        if(!String::CompareNoCase(service, "Google+"))
            return new OAuth2GoogleService();
        if(!String::CompareNoCase(service, "Facebook"))
            return new OAuth2FacebookService();
        if(!String::CompareNoCase(service, "Kakao"))
            return new OAuth2KakaoService();
        return new OAuth2Service();
    }

    void OAuth2Service::Remove(OAuth2Service* service)
    {
        delete service;
    }
}
