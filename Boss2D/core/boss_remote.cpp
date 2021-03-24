#include <boss.hpp>
#include "boss_remote.hpp"

#include <platform/boss_platform.hpp>

// 관리객체
class RemoteClass
{
private:
    class CallSpec
    {
    public:
        CallSpec(const bool localcall, const bool needreturn, const Remote::Method* method, Remote::Params* params)
            : m_localcall(localcall), m_needreturn(needreturn), m_method(method)
        {
            m_params = params;
        }
        ~CallSpec()
        {
            delete m_params;
        }
        CallSpec& operator=(const CallSpec&)
        {
            BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
            return *this;
        }

    public:
        const bool m_localcall;
        const bool m_needreturn;
        const Remote::Method* m_method;
        Remote::Params* m_params;
    };

public:
    RemoteClass(uint16 port, bool autopartner) : m_isServer(true), m_autoPartner(autopartner)
    {
        server.m_id = Platform::Server::CreateTCP(true);
        m_MutexPeerID = Mutex::Open();
        m_partnersPeerID = -1;
        Platform::Server::Listen(server.m_id, port);
    }
    RemoteClass(chars domain, uint16 port, bool autopartner) : m_isServer(false), m_autoPartner(autopartner)
    {
        client.m_id = Platform::Socket::OpenForTcp();
        client.m_domain = new String(domain);
        client.m_port = port;
        m_MutexPeerID = Mutex::Open();
        m_partnersPeerID = -1;
    }
    ~RemoteClass()
    {
        if(m_isServer)
            Platform::Server::Release(server.m_id);
        else
        {
            Platform::Socket::Close(client.m_id);
            delete client.m_domain;
        }
        Mutex::Close(m_MutexPeerID);
    }
    RemoteClass& operator=(const RemoteClass&)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

private:
    uint08s* CallSpecToMessage(CallSpec* spec)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", !spec->m_localcall);

        // 패킷사이즈 공간확보
        m_tempBuffer.SubtractionAll();
        m_tempBuffer.AtDumpingAdded(4);

        // 리턴타입 기록
        m_tempBuffer.AtAdding() = (spec->m_needreturn)? 1 : 0;

        // 함수명 기록
        const sint32 NameSizeWithNull = spec->m_method->func.m_name->Length() + 1;
        Memory::Copy(m_tempBuffer.AtDumpingAdded(4), &NameSizeWithNull, 4);
        Memory::Copy(m_tempBuffer.AtDumpingAdded(NameSizeWithNull), (chars) *spec->m_method->func.m_name, NameSizeWithNull);

        // 파라미터 기록
        const sint32 ParamCount = spec->m_params->Count();
        Memory::Copy(m_tempBuffer.AtDumpingAdded(4), &ParamCount, 4);
        for(sint32 i = 0; i < ParamCount; ++i)
        {
            id_cloned_share ClonedShare = (*spec->m_params)[i].Drain();
            if(((Share*) ClonedShare)->TypeID() == Buffer::TypeOf<sint64, datatype_pod_canmemcpy>())
            {
                m_tempBuffer.AtAdding() = 2;
                const Remote::IntParam Param(ClonedShare);
                Memory::Copy(m_tempBuffer.AtDumpingAdded(8), Param.ConstPtr(), 8);
            }
            else if(((Share*) ClonedShare)->TypeID() == Buffer::TypeOf<double, datatype_pod_canmemcpy>())
            {
                m_tempBuffer.AtAdding() = 3;
                const Remote::DecParam Param(ClonedShare);
                Memory::Copy(m_tempBuffer.AtDumpingAdded(8), Param.ConstPtr(), 8);
            }
            else if(((Share*) ClonedShare)->TypeID() == Buffer::TypeOf<uint08, datatype_pod_canmemcpy>())
            {
                m_tempBuffer.AtAdding() = 4;
                const Remote::HexParam Param(ClonedShare);
                const sint32 BytesCount = Param.Count();
                Memory::Copy(m_tempBuffer.AtDumpingAdded(4), &BytesCount, 4);
                Memory::Copy(m_tempBuffer.AtDumpingAdded(BytesCount), &Param[0], BytesCount);
            }
            else if(((Share*) ClonedShare)->TypeID() == Buffer::TypeOf<char, datatype_pod_canmemcpy>())
            {
                m_tempBuffer.AtAdding() = 5;
                const Remote::StrParam Param(ClonedShare);
                const sint32 CharsCountWithNull = Param.Length() + 1;
                Memory::Copy(m_tempBuffer.AtDumpingAdded(4), &CharsCountWithNull, 4);
                Memory::Copy(m_tempBuffer.AtDumpingAdded(CharsCountWithNull), (chars) Param, CharsCountWithNull);
            }
            else
            {
                BOSS_ASSERT("송신할 수 없는 파라미터가 있습니다", false);
                spec->m_params->At(i).Store(ClonedShare);
                return nullptr;
            }
        }

        // 패킷사이즈 기록
        const sint32 PacketSize = m_tempBuffer.Count() - 4;
        Memory::Copy(m_tempBuffer.AtDumping(0, 4), &PacketSize, 4);
        return &m_tempBuffer;
    }

    uint08s* ResultToMessage(const String MethodName, id_cloned_share result)
    {
        // 패킷사이즈 공간확보
        m_tempBuffer.SubtractionAll();
        m_tempBuffer.AtDumpingAdded(4);

        // 리턴값 기록
        if(((Share*) result)->TypeID() == Buffer::TypeOf<sint64, datatype_pod_canmemcpy>())
        {
            m_tempBuffer.AtAdding() = 2;
            const Remote::IntParam Param(result);
            Memory::Copy(m_tempBuffer.AtDumpingAdded(8), Param.ConstPtr(), 8);
        }
        else if(((Share*) result)->TypeID() == Buffer::TypeOf<double, datatype_pod_canmemcpy>())
        {
            m_tempBuffer.AtAdding() = 3;
            const Remote::DecParam Param(result);
            Memory::Copy(m_tempBuffer.AtDumpingAdded(8), Param.ConstPtr(), 8);
        }
        else if(((Share*) result)->TypeID() == Buffer::TypeOf<uint08, datatype_pod_canmemcpy>())
        {
            m_tempBuffer.AtAdding() = 4;
            const Remote::HexParam Param(result);
            const sint32 BytesCount = Param.Count();
            Memory::Copy(m_tempBuffer.AtDumpingAdded(4), &BytesCount, 4);
            Memory::Copy(m_tempBuffer.AtDumpingAdded(BytesCount), &Param[0], BytesCount);
        }
        else if(((Share*) result)->TypeID() == Buffer::TypeOf<char, datatype_pod_canmemcpy>())
        {
            m_tempBuffer.AtAdding() = 5;
            const Remote::StrParam Param(result);
            const sint32 CharsCountWithNull = Param.Length() + 1;
            Memory::Copy(m_tempBuffer.AtDumpingAdded(4), &CharsCountWithNull, 4);
            Memory::Copy(m_tempBuffer.AtDumpingAdded(CharsCountWithNull), (chars) Param, CharsCountWithNull);
        }
        else
        {
            BOSS_ASSERT("송신할 수 없는 리턴값입니다", false);
            Share::Destroy(result);
            return nullptr;
        }

        // 함수명 기록(사이즈필드가 없음)
        const sint32 NameSizeWithNull = MethodName.Length() + 1;
        Memory::Copy(m_tempBuffer.AtDumpingAdded(NameSizeWithNull), (chars) MethodName, NameSizeWithNull);

        // 패킷사이즈 기록
        const sint32 PacketSize = m_tempBuffer.Count() - 4;
        Memory::Copy(m_tempBuffer.AtDumping(0, 4), &PacketSize, 4);
        return &m_tempBuffer;
    }

    id_cloned_share MessageToCallOrReturn(bytes message, String& MethodName)
    {
        bool NeedReturn = true;
        id_cloned_share ReturnValue = nullptr;
        switch(*(message++))
        {
        case 0: // 리턴없는 함수
            NeedReturn = false;
        case 1: // 리턴있는 함수
            {
                // 함수명 수집
                sint32 NameSizeWithNull = 0;
                Memory::Copy(&NameSizeWithNull, message, 4); message += 4;
                chars RefName = (chars) message; message += NameSizeWithNull;
                // 함수명 전달
                MethodName = RefName;

                // 함수호출
                if(const Remote::Method** CurMethod = m_callMethods.Access(RefName))
                {
                    Remote::Method CallingMethod = (**CurMethod)();
                    // 파라미터 수집
                    sint32 ParamCount = 0;
                    Memory::Copy(&ParamCount, message, 4); message += 4;
                    for(sint32 i = 0; i < ParamCount; ++i)
                    {
                        switch(*(message++))
                        {
                        case 2: // IntParam
                            {
                                sint64 Param = 0;
                                Memory::Copy(&Param, message, 8); message += 8;
                                CallingMethod = CallingMethod(Param);
                            }
                            break;
                        case 3: // DecParam
                            {
                                double Param = 0;
                                Memory::Copy(&Param, message, 8); message += 8;
                                CallingMethod = CallingMethod(Param);
                            }
                            break;
                        case 4: // HexParam
                            {
                                sint32 BytesCount = 0;
                                Memory::Copy(&BytesCount, message, 4); message += 4;
                                CallingMethod = CallingMethod(message, BytesCount);
                                message += BytesCount;
                            }
                            break;
                        case 5: // StrParam
                            {
                                sint32 CharsCountWithNull = 0;
                                Memory::Copy(&CharsCountWithNull, message, 4); message += 4;
                                CallingMethod = CallingMethod((chars) message, CharsCountWithNull - 1);
                                message += CharsCountWithNull;
                            }
                            break;
                        default:
                            BOSS_ASSERT("수신내용에 알 수 없는 파라미터가 있어 생략합니다", false);
                            break;
                        }
                    }
                    // CallingMethod인스턴스가 사라지는 시점에 소멸자가 호출되면서 함수콜
                    CallingMethod.DirectCallOnce();
                    if(NeedReturn)
                    {
                        if(id_cloned_share Result = CallingMethod) return Result;
                        else return String("<Remote-method does not return a result value>");
                    }
                }
                else if(NeedReturn)
                    return String("<Remote-method is not found>");
            }
            break;
        case 2: // 리턴값 : IntParam
            {
                sint64 Param = 0;
                Memory::Copy(&Param, message, 8); message += 8;
                ReturnValue = Remote::IntParam(Param);
            }
            break;
        case 3: // 리턴값 : DecParam
            {
                double Param = 0;
                Memory::Copy(&Param, message, 8); message += 8;
                ReturnValue = Remote::DecParam(Param);
            }
            break;
        case 4: // 리턴값 : HexParam
            {
                sint32 BytesCount = 0;
                Memory::Copy(&BytesCount, message, 4); message += 4;
                Remote::HexParam NewHexParam;
                Memory::Copy(NewHexParam.AtDumpingAdded(BytesCount), message, BytesCount);
                ReturnValue = NewHexParam;
                message += BytesCount;
            }
            break;
        case 5: // 리턴값 : StrParam
            {
                sint32 CharsCountWithNull = 0;
                Memory::Copy(&CharsCountWithNull, message, 4); message += 4;
                ReturnValue = Remote::StrParam((chars) message, CharsCountWithNull - 1);
                message += CharsCountWithNull;
            }
            break;
        default:
            BOSS_ASSERT("알 수 없는 수신내용입니다", false);
            break;
        }

        if(ReturnValue)
        {
            // 함수호출
            if(const Remote::Method** CurMethod = m_callMethods.Access((chars) message))
                (*CurMethod)->SendResult(ReturnValue);
            else BOSS_ASSERT("수신된 리턴값을 받을 함수가 없습니다", false);
        }
        return nullptr;
    }

    sint32 CommunicateOnceForServer()
    {
        BOSS_ASSERT("잘못된 호출입니다", m_isServer);

        // 송신처리
        while(CallSpec* OneCallSpec = m_callQueue.Dequeue())
        {
            if(OneCallSpec->m_localcall || m_partnersPeerID == -1)
            {
                if(OneCallSpec->m_needreturn)
                {
                    id_cloned_share Result = nullptr;
                    OneCallSpec->m_method->func.m_cb(OneCallSpec->m_method->func.m_data, *OneCallSpec->m_params, &Result, true);
                    if(!Result) Result = String("<Local-method does not return a result value>");
                    OneCallSpec->m_method->SendResult(Result);
                }
                else OneCallSpec->m_method->func.m_cb(OneCallSpec->m_method->func.m_data, *OneCallSpec->m_params, nullptr, true);
            }
            else if(uint08s* NewMessage = CallSpecToMessage(OneCallSpec))
                Platform::Server::SendToPeer(server.m_id, m_partnersPeerID,
                    NewMessage->AtDumping(0, NewMessage->Count()), NewMessage->Count());
            delete OneCallSpec;
        }

        // 수신처리
        Remote::GlobalValue::ExitPeerIDs().Clear();
        while(Platform::Server::TryNextPacket(server.m_id))
        {
            const sint32 PeerID = Platform::Server::GetPacketPeerID(server.m_id);
            Remote::GlobalValue::LastPeerID() = PeerID;
            switch(Platform::Server::GetPacketType(server.m_id))
            {
            case packettype_entrance:
                if(m_autoPartner)
                    m_partnersPeerID = PeerID;
                m_peerIDs.AtAdding() = PeerID;
                break;
            case packettype_message:
                {
                    // 호출 또는 결과전달
                    String MethodName;
                    if(id_cloned_share Result = MessageToCallOrReturn(Platform::Server::GetPacketBuffer(server.m_id), MethodName))
                    if(uint08s* NewMessage = ResultToMessage(MethodName, Result))
                        Platform::Server::SendToPeer(server.m_id, PeerID,
                            NewMessage->AtDumping(0, NewMessage->Count()), NewMessage->Count());
                }
                break;
            case packettype_leaved:
            case packettype_kicked:
                if(m_partnersPeerID == PeerID)
                    m_partnersPeerID = -1;
                for(sint32 i = m_peerIDs.Count() - 1; 0 <= i; --i)
                    if(m_peerIDs[i] == PeerID)
                        m_peerIDs.SubtractionSection(i);
                Remote::GlobalValue::ExitPeerIDs().AtAdding() = PeerID;
                break;
            }
            Remote::GlobalValue::LastPeerID() = -1;
        }
        return (0 < m_peerIDs.Count());
    }

    bool CommunicateOnceForClient()
    {
        BOSS_ASSERT("잘못된 호출입니다", !m_isServer);

        // 접속시도
        if(m_peerIDs.Count() == 0)
        if(Platform::Socket::Connect(client.m_id, *client.m_domain, client.m_port))
        {
            if(m_autoPartner)
                m_partnersPeerID = 0;
            m_peerIDs.AtAdding() = 0;
        }

        // 송신처리
        while(CallSpec* OneCallSpec = m_callQueue.Dequeue())
        {
            if(OneCallSpec->m_localcall || m_partnersPeerID == -1)
            {
                if(OneCallSpec->m_needreturn)
                {
                    id_cloned_share Result = nullptr;
                    OneCallSpec->m_method->func.m_cb(OneCallSpec->m_method->func.m_data, *OneCallSpec->m_params, &Result, true);
                    if(!Result) Result = String("<Local-method does not return a result value>");
                    OneCallSpec->m_method->SendResult(Result);
                }
                else OneCallSpec->m_method->func.m_cb(OneCallSpec->m_method->func.m_data, *OneCallSpec->m_params, nullptr, true);
            }
            else if(uint08s* NewMessage = CallSpecToMessage(OneCallSpec))
                Platform::Socket::Send(client.m_id,
                    NewMessage->AtDumping(0, NewMessage->Count()), NewMessage->Count());
            delete OneCallSpec;
        }

        // 수신처리
        Remote::GlobalValue::ExitPeerIDs().Clear();
        if(0 < m_peerIDs.Count())
        {
            sint32 RecvAvailableSize = Platform::Socket::RecvAvailable(client.m_id);
            if(RecvAvailableSize < 0)
                m_peerIDs.Clear();
            else if(4 < RecvAvailableSize)
            {
                sint32 PacketSize = 0;
                RecvAvailableSize = Platform::Socket::Recv(client.m_id, (uint08*) &PacketSize, 4);
                if(RecvAvailableSize < 0)
                    m_peerIDs.Clear();
                else if(0 < PacketSize)
                {
                    sint32 RecvFocus = 0, RecvSize = 0;
                    do
                    {
                        RecvFocus += RecvSize;
                        RecvSize = Platform::Socket::Recv(client.m_id, m_tempBuffer.AtDumping(RecvFocus, PacketSize), PacketSize);
                        if(RecvSize < 0)
                        {
                            m_peerIDs.Clear();
                            break;
                        }
                        PacketSize -= RecvSize;
                    }
                    while(0 < PacketSize);

                    // 호출 또는 결과전달
                    if(PacketSize == 0)
                    {
                        String MethodName;
                        if(id_cloned_share Result = MessageToCallOrReturn(&m_tempBuffer[0], MethodName))
                        if(uint08s* NewMessage = ResultToMessage(MethodName, Result))
                            Platform::Socket::Send(client.m_id,
                                NewMessage->AtDumping(0, NewMessage->Count()), NewMessage->Count());
                    }
                }
            }

            // 서버와의 연결해제 상황
            if(m_peerIDs.Count() == 0)
            {
                m_partnersPeerID = -1;
                Remote::GlobalValue::ExitPeerIDs().AtAdding() = 0;
            }
        }
        return (0 < m_peerIDs.Count());
    }

public:
    bool CommunicateOnce()
    {
        bool Result = false;
        Mutex::Lock(m_MutexPeerID);
        {
            // 킥요청 처리
            sint32 KickedPeerID = m_kickedPeerIDs.Dequeue(-1);
            while(KickedPeerID != -1)
            {
                if(m_partnersPeerID == KickedPeerID)
                    m_partnersPeerID = -1;
                for(sint32 i = m_peerIDs.Count() - 1; 0 <= i; --i)
                    if(m_peerIDs[i] == KickedPeerID)
                        m_peerIDs.SubtractionSection(i);
                if(m_isServer)
                    Platform::Server::KickPeer(server.m_id, KickedPeerID);
                else Platform::Socket::Disconnect(client.m_id);
                KickedPeerID = m_kickedPeerIDs.Dequeue(-1);
            }

            // 시나리오 진행
            if(m_isServer)
                Result = (0 < CommunicateOnceForServer());
            else Result = CommunicateOnceForClient();
        }
        Mutex::Unlock(m_MutexPeerID);
        return Result;
    }

    void BindMethod(const String name, const Remote::Method* method) const
    {
        m_callMethods(name) = method;
    }

    void CallMethod(const bool localcall, const bool needreturn, const Remote::Method* method, Remote::Params** params) const
    {
        CallSpec* NewCallSpec = new CallSpec(localcall, needreturn, method, *params);
        m_callQueue.Enqueue(NewCallSpec);
        *params = nullptr;
    }

    sint32 CountOfPartners() const
    {
        sint32 Result = 0;
        Mutex::Lock(m_MutexPeerID);
        {
            Result = m_peerIDs.Count();
        }
        Mutex::Unlock(m_MutexPeerID);
        return Result;
    }

    bool SelectPartnerByIndex(sint32 index)
    {
        bool Result = false;
        Mutex::Lock(m_MutexPeerID);
        {
            if(0 <= index && index < m_peerIDs.Count())
            {
                m_partnersPeerID = m_peerIDs[index];
                Result = true;
            }
            else m_partnersPeerID = -1;
        }
        Mutex::Unlock(m_MutexPeerID);
        return Result;
    }

    bool SelectPartnerByPeerID(sint32 peerid)
    {
        bool Result = false;
        Mutex::Lock(m_MutexPeerID);
        {
            m_partnersPeerID = -1;
            for(sint32 i = 0, iend = m_peerIDs.Count(); i < iend; ++i)
            {
                if(m_peerIDs[i] == peerid)
                {
                    m_partnersPeerID = peerid;
                    Result = true;
                    break;
                }
            }
        }
        Mutex::Unlock(m_MutexPeerID);
        return false;
    }

    void KickPartner(sint32 peerid)
    {
        if(peerid != -1)
            m_kickedPeerIDs.Enqueue(peerid);
    }

private:
    const bool m_isServer;
    const bool m_autoPartner;
    union
    {
        struct
        {
            id_server m_id;
        } server;
        struct
        {
            id_socket m_id;
            String* m_domain;
            uint16 m_port;
        } client;
    };
    id_mutex m_MutexPeerID;
    sint32 m_partnersPeerID;
    sint32s m_peerIDs;
    Queue<sint32> m_kickedPeerIDs;
    mutable Map<const Remote::Method*> m_callMethods;
    mutable Queue<CallSpec*> m_callQueue;
    uint08s m_tempBuffer;
};

namespace BOSS
{
    id_remote Remote::ConnectForServer(uint16 port, MethodPtrs methods, bool autopartner)
    {
        RemoteClass* NewRemote = new RemoteClass(port, autopartner);
        for(sint32 i = 0, iend = methods.Count(); i < iend; ++i)
            methods[i]->SetRemote((id_remote_read) NewRemote);
        return (id_remote) NewRemote;
    }

    id_remote Remote::ConnectForClient(chars domain, uint16 port, MethodPtrs methods, bool autopartner)
    {
        RemoteClass* NewRemote = new RemoteClass(domain, port, autopartner);
        for(sint32 i = 0, iend = methods.Count(); i < iend; ++i)
            methods[i]->SetRemote((id_remote_read) NewRemote);
        return (id_remote) NewRemote;
    }

    void Remote::Disconnect(id_remote_read remote)
    {
        delete (RemoteClass*) remote;
    }

    bool Remote::CommunicateOnce(id_remote_read remote)
    {
        if(RemoteClass* CurRemote = (RemoteClass*) remote)
            return CurRemote->CommunicateOnce();
        return false;
    }

    sint32 Remote::CountOfPartners(id_remote_read remote)
    {
        if(RemoteClass* CurRemote = (RemoteClass*) remote)
            return CurRemote->CountOfPartners();
        return 0;
    }

    bool Remote::SelectPartnerByIndex(id_remote remote, sint32 index)
    {
        if(RemoteClass* CurRemote = (RemoteClass*) remote)
            return CurRemote->SelectPartnerByIndex(index);
        return false;
    }

    bool Remote::SelectPartnerByPeerID(id_remote remote, sint32 peerid)
    {
        if(RemoteClass* CurRemote = (RemoteClass*) remote)
            return CurRemote->SelectPartnerByPeerID(peerid);
        return false;
    }

    void Remote::KickPartner(id_remote remote, sint32 peerid)
    {
        if(RemoteClass* CurRemote = (RemoteClass*) remote)
            CurRemote->KickPartner(peerid);
    }

    void Remote::Param::Store(id_cloned_share rhs)
    {
        m_share = rhs;
    }

    id_cloned_share Remote::Param::Drain() const
    {
        id_cloned_share Result = m_share;
        m_share = nullptr;
        return Result;
    }

    bool Remote::Param::WhatTypeInt() const
    {
        return (((Share*) m_share)->TypeID() == Buffer::TypeOf<sint64, datatype_pod_canmemcpy>());
    }

    bool Remote::Param::WhatTypeDec() const
    {
        return (((Share*) m_share)->TypeID() == Buffer::TypeOf<double, datatype_pod_canmemcpy>());
    }

    bool Remote::Param::WhatTypeHex() const
    {
        return (((Share*) m_share)->TypeID() == Buffer::TypeOf<uint08, datatype_pod_canmemcpy>());
    }

    bool Remote::Param::WhatTypeStr() const
    {
        return (((Share*) m_share)->TypeID() == Buffer::TypeOf<char, datatype_pod_canmemcpy>());
    }

    Remote::Param::Param()
    {
        m_share = nullptr;
    }

    Remote::Param::~Param()
    {
        Share::Destroy(m_share);
    }

    Remote::Param& Remote::Param::operator=(const Param&)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    Remote::Method& Remote::Method::operator=(const Method& rhs)
    {
        BOSS_ASSERT("복사대상과의 타입이 일치하지 않습니다", m_calling == rhs.m_calling);
        if(this != &rhs)
        {
            if(m_calling)
            {
                call.m_parent = rhs.call.m_parent;
                call.m_needCall = rhs.call.m_needCall;
                call.m_callType = rhs.call.m_callType;
                call.m_params = rhs.call.m_params;
                // 이관된 내역
                rhs.call.m_needCall = false;
                rhs.call.m_params = nullptr;
            }
            else
            {
                BOSS_ASSERT("잘못된 시나리오입니다", !func.m_remote);
                *func.m_name = *rhs.func.m_name;
                func.m_remote = rhs.func.m_remote;
                func.m_cb = rhs.func.m_cb;
                func.m_data = rhs.func.m_data;
                func.m_result = rhs.func.m_result;
                // 이관된 내역
                rhs.func.m_remote = nullptr;
                rhs.func.m_result = nullptr;
                ((const RemoteClass*) func.m_remote)->BindMethod(*func.m_name, this);
            }
        }
        return *this;
    }

    const Remote::Method Remote::Method::operator()() const
    {
        BOSS_ASSERT("잘못된 호출입니다", !m_calling);
        return Method(this, nullptr);
    }

    const Remote::Method Remote::Method::operator()(sint32 value) const
    {
        return operator()((sint64) value);
    }

    const Remote::Method Remote::Method::operator()(sint64 value) const
    {
        if(m_calling)
        {
            AddParam(IntParam(value));
            return *this;
        }
        return Method(this, IntParam(value));
    }

    const Remote::Method Remote::Method::operator()(float value) const
    {
        return operator()((double) value);
    }

    const Remote::Method Remote::Method::operator()(double value) const
    {
        if(m_calling)
        {
            AddParam(DecParam(value));
            return *this;
        }
        return Method(this, DecParam(value));
    }

    const Remote::Method Remote::Method::operator()(bytes value, sint32 length) const
    {
        BOSS_ASSERT("bytes타입의 length는 -1이 될 수 없습니다", length != -1);
        HexParam NewHex;
        Memory::Copy(NewHex.AtDumping(0, length), value, length);

        if(m_calling)
        {
            AddParam(NewHex);
            return *this;
        }
        return Method(this, NewHex);
    }

    const Remote::Method Remote::Method::operator()(chars value, sint32 length) const
    {
        if(m_calling)
        {
            AddParam(StrParam(value, length));
            return *this;
        }
        return Method(this, StrParam(value, length));
    }

    const Remote::Method Remote::Method::operator()(wchars value, sint32 length) const
    {
        String NewString = String::FromWChars(value, length);
        return operator()((chars) NewString, NewString.Length());
    }

    const Remote::Method Remote::Method::operator()(id_cloned_share value) const
    {
        BOSS_ASSERT("value는 nullptr이 될 수 없습니다", value);
        if(m_calling)
        {
            AddParam(value);
            return *this;
        }
        return Method(this, value);
    }

    Remote::Method::operator id_cloned_share() const
    {
        id_cloned_share Result = nullptr;
        if(m_calling && call.m_needCall)
        {
            call.m_needCall = false;
            CallFlush(&Result);
        }
        return Result;
    }

    Remote::Method::Method() : m_calling(false)
    {
        func.m_name = new String();
        func.m_remote = nullptr;
        func.m_cb = nullptr;
        func.m_data = nullptr;
        func.m_result = nullptr;
        func.m_resultMutex = Mutex::Open();
    }

    Remote::Method::Method(const Method& rhs) : m_calling(rhs.m_calling)
    {
        if(!m_calling)
        {
            func.m_name = new String();
            func.m_resultMutex = Mutex::Open();
        }
        operator=(rhs);
    }

    Remote::Method::Method(chars name, GlueCB cb, payload data) : m_calling(false)
    {
        func.m_name = new String(name);
        func.m_remote = nullptr;
        func.m_cb = cb;
        func.m_data = data;
        func.m_result = nullptr;
        func.m_resultMutex = Mutex::Open();
    }

    Remote::Method::~Method()
    {
        if(m_calling)
        {
            if(call.m_needCall)
                CallFlush(nullptr);
            delete call.m_params;
        }
        else
        {
            delete func.m_name;
            Share::Destroy(func.m_result);
            Mutex::Close(func.m_resultMutex);
        }
    }

    Remote::Method::Method(const Method* parent, id_cloned_share value) : m_calling(true)
    {
        call.m_parent = parent;
        call.m_needCall = true;
        call.m_callType = CT_NeedRemote;
        call.m_params = new Params();
        if(value) AddParam(value);
    }

    void Remote::Method::SetRemote(id_remote_read remote) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", !m_calling);
        func.m_remote = remote;
        ((const RemoteClass*) remote)->BindMethod(*func.m_name, this);
    }

    void Remote::Method::AddParam(id_cloned_share value) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_calling);
        call.m_params->AtAdding().Store(value);
    }

    void Remote::Method::CallFlush(id_cloned_share* out) const
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_calling);
        if(call.m_parent->func.m_remote && call.m_callType != CT_DirectForRemote)
        {
            const RemoteClass* CurRemote = (const RemoteClass*) call.m_parent->func.m_remote;
            if(out) call.m_parent->LockForResult();
            CurRemote->CallMethod(call.m_callType == CT_NeedLocal, out != nullptr, call.m_parent, &call.m_params);
            if(out) *out = call.m_parent->UnlockForResult();
        }
        else if(call.m_parent->func.m_cb)
            call.m_parent->func.m_cb(call.m_parent->func.m_data, *call.m_params, out,
                call.m_callType != CT_DirectForRemote);
    }

    void Remote::Method::LockForResult() const
    {
        Mutex::Lock(func.m_resultMutex);
    }

    void Remote::Method::SendResult(id_cloned_share value) const
    {
        func.m_result = value;
        Mutex::Unlock(func.m_resultMutex);
    }

    id_cloned_share Remote::Method::UnlockForResult() const
    {
        id_cloned_share Result = nullptr;
        Mutex::Lock(func.m_resultMutex);
        {
            Result = func.m_result;
            func.m_result = nullptr;
        }
        Mutex::Unlock(func.m_resultMutex);
        return Result;
    }

    void Remote::Method::DirectCallOnce() const
    {
        BOSS_ASSERT("잘못된 호출입니다", m_calling);
        call.m_callType = CT_DirectForRemote;
    }

    void Remote::Method::LocalCallOnce() const
    {
        BOSS_ASSERT("잘못된 호출입니다", m_calling);
        call.m_callType = CT_NeedLocal;
    }
}
