#include <boss.hpp>
#include "boss_whisper.hpp"

#include "platform/boss_platform.hpp"

namespace BOSS
{
    static String gSpotID;

    Whisper::Whisper()
    {
        if(gSpotID.Length() == 0)
        {
            const String ProgramPath = Platform::Utility::GetProgramPath();
            for(sint32 i = ProgramPath.Length() - 1; 0 <= i; --i)
                if(ProgramPath[i] == '/')
                {
                    gSpotID = ProgramPath.Offset(i + 1);
                    gSpotID += String::Format("[%lld]", Platform::Utility::GetProcessID());
                    break;
                }
        }
    }

    Whisper::~Whisper()
    {
        Close();
    }

    bool Whisper::ConnectTest(chars spotid)
    {
        id_pipe OneTimePipe = Platform::Pipe::Open(String(spotid).Upper());
        const bool Result = (Platform::Pipe::Status(OneTimePipe) == CS_Connected);
        Platform::Pipe::Close(OneTimePipe);
        return Result;
    }

    Strings Whisper::EnumSpots(rect128 windowrect) const
    {
        Strings SpotCollector;
        Context Programs;
        const sint32 Count = Platform::Utility::EnumPrograms(Programs, true);
        for(sint32 i = Count - 1; 0 <= i; --i)
        {
            auto& CurProgram = Programs[i];
            const String FilePath = CurProgram("filepath").GetText();
            for(sint32 j = FilePath.Length() - 1; 0 <= j; --j)
                if(FilePath[j] == '/')
                {
                    String NewSpotID = FilePath.Offset(j + 1);
                    if(!!NewSpotID.CompareNoCase("explorer.exe"))
                    if(!!NewSpotID.CompareNoCase("SystemSettings.exe"))
                    if(!!NewSpotID.CompareNoCase("ApplicationFrameHost.exe"))
                    if(!!NewSpotID.CompareNoCase("TextInputHost.exe"))
                    if(!!NewSpotID.CompareNoCase("devenv.exe"))
                    {
                        NewSpotID += String::Format("[%lld]", CurProgram("processid").GetInt());
                        if(!NewSpotID.Compare(gSpotID)) // 자기자신의 순서를 만나면 탐색종료(실행초반에는 자기없음)
                            return SpotCollector;
                        const sint32 Left = CurProgram("windowrect")("left").GetInt();
                        const sint32 Top = CurProgram("windowrect")("top").GetInt();
                        const sint32 Right = CurProgram("windowrect")("right").GetInt();
                        const sint32 Bottom = CurProgram("windowrect")("bottom").GetInt();
                        if(Left < Right && Top < Bottom) // 제로사이즈(0, 0)는 제외
                        if(Left < windowrect.r && Top < windowrect.b && windowrect.l < Right && windowrect.t < Bottom)
                            SpotCollector.AtAdding() = NewSpotID;
                    }
                    break;
                }
        }
        return SpotCollector;
    }

    void Whisper::SetEventCB(ConnectCB ccb, UnknownCB ucb)
    {
        mConnectCB = ccb;
        mUnknownCB = ucb;
    }

    void Whisper::AddRecvCB(chars type, RecvCB cb)
    {
        mRecvCBs(type) = cb;
    }

    String Whisper::OpenForSpot()
    {
        Close();
        mPipe = Platform::Pipe::Open(gSpotID.Upper(),
            [](Platform::Pipe::EventType type, payload data)->void
            {((Whisper*) data)->OnEvent(type, true);}, this);
        return gSpotID;
    }

    bool Whisper::OpenForConnector(chars spotid)
    {
        Close();
        mPipe = Platform::Pipe::Open(String(spotid).Upper(),
            [](Platform::Pipe::EventType type, payload data)->void
            {((Whisper*) data)->OnEvent(type, false);}, this);
        if(Platform::Pipe::Status(mPipe) == CS_Connected)
        {
            const String Packet = String::Format("<#spot:%s#>{}", spotid);
            Platform::Pipe::Send(mPipe, (bytes)(chars) Packet, Packet.Length());
            return true;
        }
        Close();
        return false;
    }

    void Whisper::Convey(Whisper& target)
    {
        target.Close();
        target.mPipe = mPipe;
        mPipe = nullptr;
    }

    void Whisper::Close()
    {
        if(mPipe)
        {
            Platform::Pipe::Close(mPipe);
            mPipe = nullptr;
        }
    }

    void Whisper::Send(chars type, const Context& data) const
    {
        const String Packet = String::Format("<@%s@>%s", type, (chars) data.SaveJson());
        Platform::Pipe::Send(mPipe, (bytes)(chars) Packet, Packet.Length());
    }

    void Whisper::OnEvent(Platform::Pipe::EventType type, bool isspot)
    {
        switch(type)
        {
        case Platform::Pipe::Connected:
            mConnectedSpotID.Empty();
            break;
        case Platform::Pipe::Disconnected:
            if(0 < mConnectedSpotID.Length())
            if(mConnectCB)
                mConnectCB(mConnectedSpotID, CF_Disconnect);
            break;
        case Platform::Pipe::Received:
            {
                uint08 Data[40960];
                const sint32 Count = Platform::Pipe::Recv(mPipe, Data, 40960);
                if(Data[0] == '<')
                {
                    char HeadType = Data[1];
                    for(sint32 i = 3; i < Count; ++i)
                        if(Data[i] == '>' && Data[i - 1] == HeadType)
                        {
                            if(HeadType == '#') // <#spot:Window[1234]#>{...}
                            {
                                if(!Memory::Compare(&Data[1], strpair("#spot:")))
                                {
                                    mConnectedSpotID = String((chars) &Data[7], i - 8);
                                    if(mConnectCB)
                                        mConnectCB(mConnectedSpotID, CF_Connect);
                                    if(isspot)
                                    {
                                        const String Packet = String::Format("<#spot:%s#>{}", (chars) gSpotID);
                                        Platform::Pipe::Send(mPipe, (bytes)(chars) Packet, Packet.Length());
                                    }
                                }
                            }
                            else if(HeadType == '@') // <@typename@>{...}
                            {
                                const String TypeName = String((chars) &Data[2], i - 3);
                                if(auto CurRecvCB = mRecvCBs.Access(TypeName))
                                {
                                    const Context Message(ST_Json, SO_OnlyReference, (chars) &Data[i + 1], Count - i - 1);
                                    (*CurRecvCB)(Message);
                                }
                                else if(mUnknownCB)
                                    mUnknownCB(TypeName);
                            }
                            break;
                        }
                }
            }
            break;
        }
    }
}
