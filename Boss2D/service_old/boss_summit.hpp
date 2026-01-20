#pragma once
#include <boss.hpp>

namespace BOSS
{
    /// @brief 프로그램인증
    class Summit
    {
    public:
        static void SetProgramID(char a, char b, char c, char d, char e, char f);
        static void SetDevicePrivate(id_curl curl = nullptr);

    public:
        static bool CheckPermission(chars name);
        static void UpdateBuildTag();

    public:
        static bool ConnectToRelay();
        static void SendToRelay(chars devicedomain, const void* data, sint32 length);

    public:
        inline static chars GetProgramPrivate()
        {return ST().mProgramPrivate;}
        inline static chars GetProgramVersion()
        {return ST().mProgramVersion;}
        inline static chars GetDeviceDomain()
        {return ST().mDeviceDomain;}

    private:
        Summit();
        ~Summit();
        static Summit& ST() {static Summit _; return _;}

    private:
        sint64 mProgramToken;
        String mProgramPrivate;
        String mProgramVersion;
        Map<bool> mPermissions;
        sint64 mAccessToken;
        String mDeviceDomain;
        String mDomainThumbnailUrl;
        String mDomainComment;
        sint64 mRelayToken;
        id_socket mRelaySocket;
        sint32 mRelayProcedure;
    };
}
