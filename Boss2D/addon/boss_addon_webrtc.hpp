#pragma once
#include <boss.h>

#include "boss_integration_webrtc-jumpingyang001-20210616.h"
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/builtin_audio_decoder_factory.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/builtin_audio_encoder_factory.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/media_stream_interface.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/peer_connection_interface.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/data_channel_interface.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/create_peerconnection_factory.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/examples/peerconnection/client/defaults.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/examples/peerconnection/client/peer_connection_client.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/video_capture_factory.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/checks.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/strings/json.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/logging.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/internal_decoder_factory.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/internal_encoder_factory.h>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/multiplex_codec_factory.h>

#include <boss.hpp>

using namespace webrtc;

class WebRtcConnector
{
public:
    WebRtcConnector();
    ~WebRtcConnector();

public:
    void SetMute(bool on);

private:
    class PCO : public PeerConnectionObserver
    {
    public:
        PCO(WebRtcConnector& parent);
        ~PCO();
    private:
        WebRtcConnector& mParent;

    public:
        void OnSignalingChange(PeerConnectionInterface::SignalingState new_state) override;
        void OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams) override;
        void OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver) override;
        void OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream) override;
        void OnRemoveStream(rtc::scoped_refptr<MediaStreamInterface> stream) override;
        void OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel) override;
        void OnRenegotiationNeeded() override;
        void OnIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state) override;
        void OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state) override;
        void OnIceCandidate(const IceCandidateInterface* candidate) override;
        void OnIceConnectionReceivingChange(bool receiving) override;
    };

private:
    class DCO : public DataChannelObserver
    {
    public:
        DCO(WebRtcConnector& parent);
        ~DCO();
    private:
        WebRtcConnector& mParent;

    public:
        void OnStateChange() override;
        void OnMessage(const DataBuffer& buffer) override;
    };

private:
    class CSDO : public CreateSessionDescriptionObserver
    {
    public:
        CSDO(WebRtcConnector& parent);
        ~CSDO();
    private:
        WebRtcConnector& mParent;

    public:
        void OnSuccess(SessionDescriptionInterface* desc) override;
        void OnFailure(RTCError error) override;
    };

private:
    class SSDO : public SetSessionDescriptionObserver
    {
    public:
        SSDO(WebRtcConnector& parent);
        ~SSDO();
    private:
        WebRtcConnector& mParent;

    public:
        void OnSuccess() override;
        void OnFailure(RTCError error) override;
    };

private:
    class ATSI : public AudioTrackSinkInterface
    {
    public:
        ATSI(WebRtcConnector& parent);
        ~ATSI();
    private:
        WebRtcConnector& mParent;
        id_sound mSound;

    public:
        void OnData(const void* audio_data, int bits_per_sample, int sample_rate,
            size_t number_of_channels, size_t number_of_frames) override;
    };

public:
    rtc::scoped_refptr<PeerConnectionInterface> mPeerConnection;
    rtc::scoped_refptr<MediaStreamInterface> mMediaStream;
    rtc::scoped_refptr<DataChannelInterface> mDataChannel;
    Context mSessionInfo;

public:
    PCO mPCO;
    DCO mDCO;
    rtc::scoped_refptr<CSDO> mCSDO;
    rtc::scoped_refptr<SSDO> mSSDO;
    ATSI mATSI;
};

class WebRtcManager
{
public:
    WebRtcManager();
    ~WebRtcManager();

public:
    bool CreateOffer(bool video, bool audio, bool data);
    bool CreateAnswer(const Context& offer_sdp);
    bool BindSdp(const Context& answer_sdp);
    bool AddIce(const Context& offer_sdp);
    void SetMute(bool on);
    void Send(bytes data, sint32 len);

private:
    std::unique_ptr<rtc::Thread> mWorker;
    std::unique_ptr<rtc::Thread> mSignaling;
    rtc::PhysicalSocketServer mSocketServer;

    rtc::scoped_refptr<PeerConnectionFactoryInterface> mConnectionFactory;
    PeerConnectionInterface::RTCConfiguration mConnectionConfig;
    std::unique_ptr<WebRtcConnector> mConnector;
};
