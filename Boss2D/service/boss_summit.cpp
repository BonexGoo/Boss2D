#include <boss.hpp>
#include "boss_summit.hpp"

#include <platform/boss_platform.hpp>
#include <boss_buildtag.hpp>

namespace BOSS
{
    void Summit::SetProgramID(char a, char b, char c, char d, char e, char f)
    {
        ST().mProgramPrivate = String::Format("%c%c%c%c%c%c" BOSS_BUILDTAG_TIME, a, b, c, d, e, f);

        #if !HU_WASM
            if(id_curl NewCurl = AddOn::Curl::Create(5000))
            {
                const String UrlResult = AddOn::Curl::GetString(NewCurl, String::Format(
                    "http://" BOSS_SUMMIT_DOMAIN "/get_program_info?program_private=%s",
                    (chars) ST().mProgramPrivate));

                Context UrlParser;
                UrlParser.LoadPrm(UrlResult);
                if(!String::Compare(UrlParser("result").GetText(), "ok"))
                {
                    ST().mProgramToken = UrlParser("program_token").GetInt(-1);
                    ST().mProgramVersion = UrlParser("program_comment").GetText();

                    String Permissions = UrlParser("permissions").GetText();
                    if(0 < Permissions.Length())
                    {
                        Permissions.AddTail("|");
                        sint32 FindPos = Permissions.Find(0, "|");
                        while(FindPos != -1)
                        {
                            const String NewPermission = Permissions.Left(FindPos);
                            ST().mPermissions(NewPermission) = true;
                            Permissions = Permissions.Offset(FindPos + 1);
                            FindPos = Permissions.Find(0, "|");
                        }
                    }
                }

                SetDevicePrivate(NewCurl);
                AddOn::Curl::Release(NewCurl);
            }
        #endif
    }

    void Summit::SetDevicePrivate(id_curl curl)
    {
        id_curl NewCurl = (curl)? nullptr : AddOn::Curl::Create(5000);
        if(!curl) curl = NewCurl;

        const String DevicePrivate = String("Hg") + Platform::Utility::GetDeviceID();
        const String UrlResult = AddOn::Curl::GetString(curl, String::Format(
            "http://" BOSS_SUMMIT_DOMAIN "/get_device_info?device_private=%s&program_token=%d",
            (chars) DevicePrivate, ST().mProgramToken));

        Context UrlParser;
        UrlParser.LoadPrm(UrlResult);
        if(!String::Compare(UrlParser("result").GetText(), "ok"))
        {
            ST().mAccessToken = UrlParser("access_token").GetInt(-1);
            ST().mDeviceDomain = UrlParser("device_domain").GetText();
            ST().mDomainThumbnailUrl = UrlParser("domain_thumbnail_url").GetText();
            ST().mDomainComment = UrlParser("domain_comment").GetText();
            ST().mRelayToken = UrlParser("relay_token").GetInt(-1);

            String Permissions = UrlParser("permissions").GetText();
            if(0 < Permissions.Length())
            {
                Permissions.AddTail("|");
                sint32 FindPos = Permissions.Find(0, "|");
                while(FindPos != -1)
                {
                    const String NewPermission = Permissions.Left(FindPos);
                    ST().mPermissions(NewPermission) = true;
                    Permissions = Permissions.Offset(FindPos + 1);
                    FindPos = Permissions.Find(0, "|");
                }
            }
        }

        AddOn::Curl::Release(NewCurl);
    }

    bool Summit::CheckPermission(chars name)
    {
        if(auto CurPermission = ST().mPermissions.Access(name))
            return *CurPermission;
        return false;
    }

    void Summit::UpdateBuildTag()
    {
        if(auto GenFile = Platform::File::OpenForWrite(Platform::File::RootForAssets() + "../generate/hu_buildtag.hpp"))
        {
            String TimeTag;
            if(id_clock NewClock = Platform::Clock::CreateAsCurrent())
            {
                sint32 Year = 0, Month = 0, Day = 0, Hour = 0, Min = 0, Sec = 0;
                Platform::Clock::GetDetail(NewClock, nullptr, &Sec, &Min, &Hour, &Day, &Month, &Year);
                TimeTag = String::Format("_%04d%02d%02d_%02d%02d%02d", Year, Month, Day, Hour, Min, Sec);
                Platform::Clock::Release(NewClock);
            }

            String GenText;
            GenText += "#include <hugine.hpp>\n";
            GenText += "\n";
            GenText += String::Format("#define HU_BUILDTAG_TIME  \"%s\"\n", (chars) TimeTag);
            GenText += "#define HU_BUILDTAG_SEEDS { \\\n";
            for(sint32 i = 0; i < 16; ++i)
            {
                const uint32 HiCode = Platform::Utility::Random();
                const uint32 LoCode = Platform::Utility::Random();
                GenText += String::Format("    Unsigned64(0x%08X%08X)%s \\\n", HiCode, LoCode, (i < 15)? "," : "");
            }
            GenText += "}\n";

            Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
            Platform::File::Close(GenFile);
        }

        // 소켓관련 정리
        Platform::SubWindowProcedure(ST().mRelayProcedure);
        Platform::Socket::Close(ST().mRelaySocket);
        ST().mRelayProcedure = 0;
        ST().mRelaySocket = nullptr;
    }

    bool Summit::ConnectToRelay()
    {
        if(ST().mRelayToken == -1)
            return false;

        const String DomainAndPort = BOSS_SUMMIT_DOMAIN;
        const sint32 ColonPos = DomainAndPort.Find(0, ":");
        if(ColonPos == -1)
            return false;

        Platform::Socket::Disconnect(ST().mRelaySocket);
        const uint16 RelayPort = (uint16) (Parser::GetInt(DomainAndPort.Offset(ColonPos + 1)) - 1);
        if(Platform::Socket::Connect(ST().mRelaySocket, DomainAndPort.Left(ColonPos), RelayPort))
        {
            const sint32 PacketSize = 8;
            Platform::Socket::Send(ST().mRelaySocket, (bytes) &PacketSize, 4);
            Platform::Socket::Send(ST().mRelaySocket, (bytes) &ST().mRelayToken, 8);
            return true;
        }
        return false;
    }

    void Summit::SendToRelay(chars devicedomain, const void* data, sint32 length)
    {
        char DeviceDomainUB[6];
        for(sint32 i = 0; i < 6; ++i)
            DeviceDomainUB[i] = (*devicedomain != '\0')? *devicedomain++ : '_';

        const sint32 PacketSize = 6 + 4 + length;
        Platform::Socket::Send(ST().mRelaySocket, (bytes) &PacketSize, 4);
        Platform::Socket::Send(ST().mRelaySocket, (bytes) DeviceDomainUB, 6);
        Platform::Socket::Send(ST().mRelaySocket, (bytes) &length, 4);
        Platform::Socket::Send(ST().mRelaySocket, (bytes) data, length);
    }

    Summit::Summit()
    {
        mProgramToken = -1;
        mAccessToken = -1;
        mRelayToken = -1;
        mRelaySocket = Platform::Socket::OpenForTCP();
        mRelayProcedure = Platform::AddWindowProcedure(WE_Tick,
            [](payload data)->void
            {
                id_socket& RelaySocket = *((id_socket*) data);
                static sint32 DataSize = 0;

                if(DataSize == 0)
                {
                    if(4 < Platform::Socket::RecvAvailable(RelaySocket))
                    {
                        Platform::Socket::Recv(RelaySocket, (uint08*) &DataSize, 4);
                        // 이상상황 발생시 처리
                        if(4096 < DataSize)
                        {
                            DataSize = 0;
                            Summit::ConnectToRelay();
                        }
                    }
                }
                if(0 < DataSize && DataSize <= Platform::Socket::RecvAvailable(RelaySocket))
                {
                    uint08s Data;
                    Platform::Socket::Recv(RelaySocket, Data.AtDumpingAdded(DataSize), DataSize);
                    Platform::BroadcastNotify("summit:Relay", Data, NT_Summit);
                    DataSize = 0;
                }
            }, &mRelaySocket);
    }

    Summit::~Summit()
    {
    }
}
