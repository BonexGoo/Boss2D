#pragma once
#include <platform/boss_platform.hpp>
#include <functional>

namespace BOSS
{
    class Whisper
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(Whisper)
    public:
        Whisper();
        ~Whisper();

    public:
        enum ConnectFlag {CF_Connect, CF_Disconnect};
        typedef std::function<void(chars spotid, ConnectFlag flag)> ConnectCB;
        typedef std::function<void(chars type)> UnknownCB;
        typedef std::function<void(const Context& data)> RecvCB;

    public:
        static bool ConnectTest(chars spotid);
        Strings EnumSpots(rect128 windowrect) const;
        void SetEventCB(ConnectCB ccb, UnknownCB ucb = nullptr);
        void AddRecvCB(chars type, RecvCB cb);

    public:
        String OpenForSpot();
        bool OpenForConnector(chars spotid);
        void Convey(Whisper& target);
        void Close();
        void Send(chars type, const Context& data = Context()) const;

    private:
        void OnEvent(Platform::Pipe::EventType type, bool isspot);

    private:
        id_pipe mPipe {nullptr};
        String mConnectedSpotID;
        ConnectCB mConnectCB {nullptr};
        UnknownCB mUnknownCB {nullptr};
        Map<RecvCB> mRecvCBs;
    };
}
