#include <boss.h>

#if !defined(BOSS_NEED_ADDON_WEBRTC) || (BOSS_NEED_ADDON_WEBRTC != 0 && BOSS_NEED_ADDON_WEBRTC != 1)
    #error BOSS_NEED_ADDON_WEBRTC macro is invalid use
#endif
bool __LINK_ADDON_WEBRTC__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_WEBRTC

#include "boss_addon_webrtc.hpp"

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/ssladapter.h>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/win32socketserver.h>
#include <addon/webrtc-jumpingyang001_for_boss/api/test/fakeconstraints.h>

#include <boss.hpp>
#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(WebRtc, OpenForOffer, id_webrtc, bool, bool)
    BOSS_DECLARE_ADDON_FUNCTION(WebRtc, OpenForAnswer, id_webrtc, chars)
    BOSS_DECLARE_ADDON_FUNCTION(WebRtc, BindWithAnswer, bool, id_webrtc, chars)
    BOSS_DECLARE_ADDON_FUNCTION(WebRtc, Close, void, id_webrtc)
    BOSS_DECLARE_ADDON_FUNCTION(WebRtc, SetMute, void, id_webrtc, bool)
    BOSS_DECLARE_ADDON_FUNCTION(WebRtc, SendData, void, id_webrtc, bytes, sint32)

    static autorun Bind_AddOn_WebRtc()
    {
        Core_AddOn_WebRtc_OpenForOffer() = Customized_AddOn_WebRtc_OpenForOffer;
        Core_AddOn_WebRtc_OpenForAnswer() = Customized_AddOn_WebRtc_OpenForAnswer;
        Core_AddOn_WebRtc_BindWithAnswer() = Customized_AddOn_WebRtc_BindWithAnswer;
        Core_AddOn_WebRtc_Close() = Customized_AddOn_WebRtc_Close;
        Core_AddOn_WebRtc_SetMute() = Customized_AddOn_WebRtc_SetMute;
        Core_AddOn_WebRtc_SendData() = Customized_AddOn_WebRtc_SendData;
        return true;
    }
    static autorun _ = Bind_AddOn_WebRtc();
}

// 구현부
namespace BOSS
{
    id_webrtc Customized_AddOn_WebRtc_OpenForOffer(bool audio, bool data)
    {
        auto NewWebRtcManager = new WebRtcManager();
        if(NewWebRtcManager)
        {
            if(NewWebRtcManager->CreateOffer(audio, data))
                return (id_webrtc) NewWebRtcManager;
            delete NewWebRtcManager;
        }
        return (id_webrtc) nullptr;
    }

    id_webrtc Customized_AddOn_WebRtc_OpenForAnswer(chars offer_sdp)
    {
        auto NewWebRtcManager = new WebRtcManager();
        if(NewWebRtcManager)
        {
            const Context Sdp(ST_Json, SO_OnlyReference, offer_sdp);
            if(NewWebRtcManager->CreateAnswer(Sdp))
                return (id_webrtc) NewWebRtcManager;
            delete NewWebRtcManager;
        }
        return (id_webrtc) nullptr;
    }

    bool Customized_AddOn_WebRtc_BindWithAnswer(id_webrtc offer_webrtc, chars answer_sdp)
    {
        if(auto CurWebRtcManager = (WebRtcManager*) offer_webrtc)
        {
            const Context Sdp(ST_Json, SO_OnlyReference, answer_sdp);
            return CurWebRtcManager->BindSdp(Sdp);
        }
        return false;
    }

    void Customized_AddOn_WebRtc_Close(id_webrtc webrtc)
    {
        auto OldWebRtcManager = (WebRtcManager*) webrtc;
        delete OldWebRtcManager;
    }

    void Customized_AddOn_WebRtc_SetMute(id_webrtc answer_webrtc, bool on)
    {
        if(auto CurWebRtcManager = (WebRtcManager*) answer_webrtc)
            CurWebRtcManager->SetMute(on);
    }

    void Customized_AddOn_WebRtc_SendData(id_webrtc webrtc, bytes data, sint32 len)
    {
        if(auto CurWebRtcManager = (WebRtcManager*) webrtc)
            CurWebRtcManager->Send(data, len);
    }
}

////////////////////////////////////////////////////////////////////////////////
// WebRtcConnector
////////////////////////////////////////////////////////////////////////////////

WebRtcConnector::WebRtcConnector() :
    mPCO(*this),
    mDCO(*this),
    mCSDO(new rtc::RefCountedObject<CSDO>(*this)),
    mSSDO(new rtc::RefCountedObject<SSDO>(*this)),
    mATSI(*this)
{
}

WebRtcConnector::~WebRtcConnector()
{
    if(mDataChannel.get())
        mDataChannel.release();
    if(mMediaStream.get())
        mMediaStream.release();
    if(mPeerConnection.get())
        mPeerConnection.release();
}

void WebRtcConnector::SetMute(bool on)
{
    auto AllTracks = mMediaStream->GetAudioTracks();
    if(!AllTracks.empty())
    {
        if(on)
        {
            AllTracks[0]->RemoveSink(&mATSI);
            // ATSI::OnData를 단일채널로 하기 위하여 기존 트랙출력은 끔
            for(auto& CurTrack : AllTracks)
                CurTrack->set_enabled(false);
        }
        else AllTracks[0]->AddSink(&mATSI);
    }
}

////////////////////////////////////////////////////////////////////////////////
// PCO
////////////////////////////////////////////////////////////////////////////////

WebRtcConnector::PCO::PCO(WebRtcConnector& parent) : mParent(parent)
{
}

WebRtcConnector::PCO::~PCO()
{
}

void WebRtcConnector::PCO::OnSignalingChange(PeerConnectionInterface::SignalingState new_state)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnSignalingChange(%d)", (sint32) new_state);
}

void WebRtcConnector::PCO::OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnAddTrack(%s)", receiver->id().c_str());
}

void WebRtcConnector::PCO::OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnRemoveTrack()");
}

void WebRtcConnector::PCO::OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnAddStream()");
    mParent.mMediaStream = stream;
    mParent.SetMute(true);
}

void WebRtcConnector::PCO::OnRemoveStream(rtc::scoped_refptr<MediaStreamInterface> stream)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnRemoveStream()");
}

void WebRtcConnector::PCO::OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnDataChannel()");
    mParent.mDataChannel = data_channel;
    mParent.mDataChannel->RegisterObserver(&mParent.mDCO);
}

void WebRtcConnector::PCO::OnRenegotiationNeeded()
{
    BOSS_TRACE("WebRtcConnector::PCO::OnRenegotiationNeeded()");
}

void WebRtcConnector::PCO::OnIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnIceConnectionChange(%d)", (sint32) new_state);
}

void WebRtcConnector::PCO::OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnIceGatheringChange(%d)", (sint32) new_state);
}

void WebRtcConnector::PCO::OnIceCandidate(const IceCandidateInterface* candidate)
{
    std::string SdpName;
    candidate->ToString(&SdpName);

    auto& NewCandidate = mParent.mSessionInfo.At("Candidate").AtAdding();
    NewCandidate.At("Sdp").Set(SdpName.c_str());
    NewCandidate.At("SdpMid").Set(candidate->sdp_mid().c_str());
    NewCandidate.At("SdpMLineIndex").Set(String::FromInteger(candidate->sdp_mline_index()));

    const String NewSDP = mParent.mSessionInfo.SaveJson();
    Platform::BroadcastNotify("WebRtc:CreateSdp", NewSDP, NT_AddOn);
    BOSS_TRACE("WebRtcConnector::PCO::OnIceCandidate() - %s", (chars) NewSDP);
}

void WebRtcConnector::PCO::OnIceConnectionReceivingChange(bool receiving)
{
    BOSS_TRACE("WebRtcConnector::PCO::OnIceConnectionReceivingChange(%d)", (sint32) receiving);
}

////////////////////////////////////////////////////////////////////////////////
// DCO
////////////////////////////////////////////////////////////////////////////////

WebRtcConnector::DCO::DCO(WebRtcConnector& parent) : mParent(parent)
{
}

WebRtcConnector::DCO::~DCO()
{
}

void WebRtcConnector::DCO::OnStateChange()
{
    if(mParent.mDataChannel.get())
    {
        auto CurState = mParent.mDataChannel->state();
        BOSS_TRACE("WebRtcConnector::DCO::OnStateChange(%d)", CurState);

        if(CurState == DataChannelInterface::kOpen)
            Platform::BroadcastNotify("WebRtc:DataChannelOpen", nullptr, NT_AddOn);
    }
    else BOSS_TRACE("WebRtcConnector::DCO::OnStateChange()");
}

void WebRtcConnector::DCO::OnMessage(const DataBuffer& buffer)
{
    BOSS_TRACE("WebRtcConnector::DCO::OnMessage()");

    const sint32 MessageSize = (sint32) buffer.data.size();
    uint08s NewMessage;
    Memory::Copy(NewMessage.AtDumpingAdded(MessageSize), buffer.data.data(), MessageSize);
    Platform::BroadcastNotify("WebRtc:OnMessage", NewMessage, NT_AddOn);
}

////////////////////////////////////////////////////////////////////////////////
// CSDO
////////////////////////////////////////////////////////////////////////////////

WebRtcConnector::CSDO::CSDO(WebRtcConnector& parent) : mParent(parent)
{
}

WebRtcConnector::CSDO::~CSDO()
{
}

void WebRtcConnector::CSDO::OnSuccess(SessionDescriptionInterface* desc)
{
    mParent.mPeerConnection->SetLocalDescription(mParent.mSSDO, desc);

    std::string SdpName;
    desc->ToString(&SdpName);
    mParent.mSessionInfo.At("Sdp").At("Contents").Set(String(SdpName.c_str()).ToUrlString());

    const String NewSDP = mParent.mSessionInfo.SaveJson();
    Platform::BroadcastNotify("WebRtc:CreateSdp", NewSDP, NT_AddOn);
    BOSS_TRACE("WebRtcConnector::CSDO::OnSuccess() - %s", (chars) NewSDP);
}

void WebRtcConnector::CSDO::OnFailure(const std::string& error)
{
    BOSS_TRACE("WebRtcConnector::CSDO::OnFailure(%s)", error.c_str());
}

////////////////////////////////////////////////////////////////////////////////
// SSDO
////////////////////////////////////////////////////////////////////////////////

WebRtcConnector::SSDO::SSDO(WebRtcConnector& parent) : mParent(parent)
{
}

WebRtcConnector::SSDO::~SSDO()
{
}

void WebRtcConnector::SSDO::OnSuccess()
{
    BOSS_TRACE("WebRtcConnector::SSDO::OnSuccess()");
}

void WebRtcConnector::SSDO::OnFailure(const std::string& error)
{
    BOSS_TRACE("WebRtcConnector::SSDO::OnFailure(%s)", error.c_str());
}

////////////////////////////////////////////////////////////////////////////////
// ATSI
////////////////////////////////////////////////////////////////////////////////

WebRtcConnector::ATSI::ATSI(WebRtcConnector& parent) : mParent(parent)
{
    mSound = nullptr;
}

WebRtcConnector::ATSI::~ATSI()
{
    Platform::Sound::Close(mSound);
}

void WebRtcConnector::ATSI::OnData(const void* audio_data, int bits_per_sample, int sample_rate,
    size_t number_of_channels, size_t number_of_frames)
{
    if(!mSound)
    {
        BOSS_TRACE("WebRtcConnector::ATSI::OnData({%02d%02d%02d...}, %d, %d, %d, %d)",
            ((uint08*) audio_data)[0], ((uint08*) audio_data)[1], ((uint08*) audio_data)[2],
            bits_per_sample, sample_rate, (sint32) number_of_channels, (sint32) number_of_frames);

        mSound = Platform::Sound::OpenForStream((sint32) number_of_channels, sample_rate, bits_per_sample);
        Platform::Sound::Play(mSound, 1.0f);
    }
    const sint32 DataSize = (sint32) (number_of_channels * number_of_frames * (bits_per_sample / 8));
    Platform::Sound::AddStreamForPlay(mSound, (bytes) audio_data, DataSize);

    // 그래프
    if(bits_per_sample == 16 && 4 <= DataSize)
    {
        const short Value0 = ((const short*) audio_data)[0];
        const short Value1 = ((const short*) audio_data)[1];
        Platform::BroadcastNotify("WebRtc:OnAudioGraph",
            floato(Math::ClampF(Math::AbsF(Value0 + Value1) / (float) 0x7FFF, 0, 1)), NT_AddOn);
    }
}

////////////////////////////////////////////////////////////////////////////////
// WebRtcManager
////////////////////////////////////////////////////////////////////////////////

WebRtcManager::WebRtcManager() : mFactory(*this)
{
    PeerConnectionInterface::IceServer StunServer1;
    StunServer1.uri = "stun:stun.l.google.com:19302";
    mConnectionConfig.servers.push_back(StunServer1);

    PeerConnectionInterface::IceServer StunServer2;//////////////////////
    StunServer2.uri = "stun:stun01.sipphone.com";
    mConnectionConfig.servers.push_back(StunServer2);

    PeerConnectionInterface::IceServer StunServer3;//////////////////////
    StunServer3.uri = "stun:stun.ekiga.net";
    mConnectionConfig.servers.push_back(StunServer3);

    PeerConnectionInterface::IceServer TurnServer1;//////////////////////
    TurnServer1.urls.push_back("turn:192.158.29.39:3478?transport=udp");
    TurnServer1.username = "28224511:1379330808";
    TurnServer1.password = "JZEOEt2V3Qb0y27GRntt2u2PAYA=";
    mConnectionConfig.servers.push_back(TurnServer1);

    PeerConnectionInterface::IceServer TurnServer2;//////////////////////
    TurnServer2.urls.push_back("turn:192.158.29.39:3478?transport=tcp");
    TurnServer2.username = "28224511:1379330808";
    TurnServer2.password = "JZEOEt2V3Qb0y27GRntt2u2PAYA=";
    mConnectionConfig.servers.push_back(TurnServer2);

    rtc::InitializeSSL();
    mThread.reset(new rtc::Thread(&mSocketServer));
    mThread->Start(&mFactory);
}

WebRtcManager::~WebRtcManager()
{
    mConnector.reset();
    mThread.reset();
    rtc::CleanupSSL();
}

bool WebRtcManager::CreateOffer(bool audio, bool data)
{
    while(!mConnectionFactory.get())
        Platform::Utility::Sleep(1, false, true);

    mConnectionConfig.sdp_semantics = SdpSemantics::kUnifiedPlan;
    mConnectionConfig.enable_dtls_srtp = true;

    mConnector.reset(new WebRtcConnector());
    mConnector->mPeerConnection = mConnectionFactory->CreatePeerConnection(
        mConnectionConfig, nullptr, nullptr, &mConnector->mPCO);

    if(mConnector->mPeerConnection.get())
    {
        if(audio)
        {
            auto NewAudioSource = mConnectionFactory->CreateAudioSource(cricket::AudioOptions());
            auto NewAudioTrack = mConnectionFactory->CreateAudioTrack("AudioChannel", NewAudioSource);
            if(!mConnector->mPeerConnection->AddTrack(NewAudioTrack, {"AudioStreamID"}).ok())
                BOSS_TRACE("AddTrack이 실패하였습니다");
        }

        if(data)
        {
            DataChannelInit Config;
            mConnector->mDataChannel = mConnector->mPeerConnection->CreateDataChannel("DataChannel", &Config);
            mConnector->mDataChannel->RegisterObserver(&mConnector->mDCO);
        }

        mConnector->mSessionInfo.At("Sdp").At("Type").Set("offer");
        FakeConstraints NewConstraints;
        NewConstraints.SetMandatoryReceiveAudio(true);
        mConnector->mPeerConnection->CreateOffer(mConnector->mCSDO, &NewConstraints);
        return true;
    }
    else BOSS_ASSERT("CreatePeerConnection이 실패하였습니다", false);
    return false;
}

bool WebRtcManager::CreateAnswer(const Context& offer_sdp)
{
    while(!mConnectionFactory.get())
        Platform::Utility::Sleep(1, false, true);

    if(!String::Compare("offer", offer_sdp("Sdp")("Type").GetText()))
    {
        mConnectionConfig.sdp_semantics = SdpSemantics::kUnifiedPlan;
        mConnectionConfig.enable_dtls_srtp = true;

        mConnector.reset(new WebRtcConnector());
        mConnector->mPeerConnection = mConnectionFactory->CreatePeerConnection(
            mConnectionConfig, nullptr, nullptr, &mConnector->mPCO);

        if(mConnector->mPeerConnection.get())
        {
            const String SdpText = String::FromUrlString(offer_sdp("Sdp")("Contents").GetText());
            SdpParseError SdpError;
            if(auto NewSessionDesc = CreateSessionDescription("offer", (chars) SdpText, &SdpError))
            {
                mConnector->mPeerConnection->SetRemoteDescription(mConnector->mSSDO, NewSessionDesc);
                AddIce(offer_sdp);

                mConnector->mSessionInfo.At("Sdp").At("Type").Set("answer");
                FakeConstraints NewConstraints;
                NewConstraints.SetMandatoryReceiveAudio(true);
                mConnector->mPeerConnection->CreateAnswer(mConnector->mCSDO, &NewConstraints);
                return true;
            }
            else BOSS_ASSERT("CreateSessionDescription이 실패하였습니다", false);
        }
        else BOSS_ASSERT("CreatePeerConnection이 실패하였습니다", false);
    }
    else BOSS_ASSERT("상대방의 sdp가 offer가 아닙니다", false);
    return false;
}

bool WebRtcManager::BindSdp(const Context& answer_sdp)
{
    if(!String::Compare("answer", answer_sdp("Sdp")("Type").GetText()))
    {
        const String SdpText = String::FromUrlString(answer_sdp("Sdp")("Contents").GetText());
        SdpParseError SdpError;
        if(auto NewSessionDesc = CreateSessionDescription("answer", (chars) SdpText, &SdpError))
        {
            mConnector->mPeerConnection->SetRemoteDescription(mConnector->mSSDO, NewSessionDesc);
            return true;
        }
        else BOSS_ASSERT("CreateSessionDescription이 실패하였습니다", false);
    }
    else BOSS_ASSERT("상대방의 sdp가 answer가 아닙니다", false);
    return false;
}

bool WebRtcManager::AddIce(const Context& offer_sdp)
{
    hook(offer_sdp("Candidate"))
    for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
    {
        hook(fish[i])
        {
            SdpParseError SdpError;
            if(auto NewIceCandidate = CreateIceCandidate((chars) fish("SdpMid").GetText(),
                fish("SdpMLineIndex").GetInt(), (chars) fish("Sdp").GetText(), &SdpError))
            {
                mConnector->mPeerConnection->AddIceCandidate(NewIceCandidate);
            }
            else
            {
                BOSS_ASSERT("CreateIceCandidate가 실패하였습니다", false);
                return false;
            }
        }
    }
    return true;
}

void WebRtcManager::SetMute(bool on)
{
    if(mConnector->mMediaStream.get())
        mConnector->SetMute(on);
}

void WebRtcManager::Send(bytes data, sint32 len)
{
    if(mConnector->mDataChannel.get())
    {
        DataBuffer NewBuffer(rtc::CopyOnWriteBuffer(data, len), true);
        mConnector->mDataChannel->Send(NewBuffer);
    }
}

////////////////////////////////////////////////////////////////////////////////
// WebRtcManager
////////////////////////////////////////////////////////////////////////////////

WebRtcManager::FactoryRunnable::FactoryRunnable(WebRtcManager& parent) : mParent(parent)
{
}

WebRtcManager::FactoryRunnable::~FactoryRunnable()
{
}

void WebRtcManager::FactoryRunnable::Run(rtc::Thread* subthread)
{
    mParent.mConnectionFactory = CreatePeerConnectionFactory(
        CreateBuiltinAudioEncoderFactory(),
        CreateBuiltinAudioDecoderFactory());

    if(mParent.mConnectionFactory.get())
        subthread->Run();
    else BOSS_ASSERT("CreatePeerConnectionFactory가 실패하였습니다", false);
}

#endif
