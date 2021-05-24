#include <boss.h>
#if BOSS_NEED_ADDON_WEBRTC

#include "boss_integration_webrtc-jumpingyang001.h"

#define BOSS_WEBRTC_FORCE_DISABLE_DMO 1

#include <boss.hpp>

#if BOSS_WINDOWS
    #pragma comment(lib, "secur32.lib")
    #pragma comment(lib, "psapi.lib")
    #pragma comment(lib, "iphlpapi.lib")
    #pragma comment(lib, "msdmo.lib")
    #pragma comment(lib, "dmoguids.lib") 
    #pragma comment(lib, "wmcodecdspuuid.lib")
    #pragma comment(lib, "strmiids.lib")
    #pragma comment(lib, "quartz.lib")
    #pragma comment(lib, "winmm.lib")
#endif

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/third_party/base64/base64.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/third_party/sigslot/sigslot.cc>

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/asyncinvoker.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/asyncpacketsocket.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/asyncresolverinterface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/asyncsocket.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/asynctcpsocket.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/asyncudpsocket.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/bitbuffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/bitrateallocationstrategy.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/bufferqueue.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/bytebuffer.cc>

#define fprintf(TYPE, FORMAT, ...) BOSS_ASSERT(BOSS::String::Format(FORMAT, __VA_ARGS__), false)
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/checks.cc>
#undef fprintf

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/copyonwritebuffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/cpu_time.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/crc32.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/criticalsection.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/cryptstring.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/data_rate_limiter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/event.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/event_tracer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/fakeclock.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/fakesslidentity.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/file.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/file_posix.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/file_win.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/filerotatingstream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/fileutils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/firewallsocketserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/flags.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/helpers.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/httpbase.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/httpcommon.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/httpserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/ifaddrs-android.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/ifaddrs_converter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/ipaddress.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/json.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/location.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/logging.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/logsinks.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/macifaddrs_converter.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/macutils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/memory_usage.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/messagedigest.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/messagehandler.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/messagequeue.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/natserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/natsocketfactory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/nattypes.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/nethelper.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/nethelpers.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/network.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/networkmonitor.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/noop.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/nullsocketserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/openssladapter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/opensslcertificate.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/openssldigest.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/opensslidentity.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/opensslsessioncache.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/opensslstreamadapter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/opensslutility.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/optionsfile.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/pathutils.cc>
#define PACKET_MAXIMUMS PACKET_MAXIMUMS_physicalsocketserver_BOSS
#define IP_HEADER_SIZE IP_HEADER_SIZE_physicalsocketserver_BOSS
#define ICMP_HEADER_SIZE ICMP_HEADER_SIZE_physicalsocketserver_BOSS
#define ICMP_PING_TIMEOUT_MILLIS ICMP_PING_TIMEOUT_MILLIS_physicalsocketserver_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/physicalsocketserver.cc>
#undef PACKET_MAXIMUMS
#undef IP_HEADER_SIZE
#undef ICMP_HEADER_SIZE
#undef ICMP_PING_TIMEOUT_MILLIS

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/platform_file.cc>

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/platform_thread.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/platform_thread_types.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/proxyinfo.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/proxyserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/race_checker.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/random.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/rate_limiter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/rate_statistics.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/ratetracker.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/rtccertificate.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/rtccertificategenerator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/sequenced_task_checker_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/signalthread.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/socket.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/socketadapters.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/socketaddress.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/socketaddresspair.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/socketstream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/ssladapter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/sslcertificate.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/sslfingerprint.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/sslidentity.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/sslstreamadapter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/string_to_number.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/stringencode.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/stringutils.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/task_queue_gcd.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/task_queue_libevent.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/task_queue_posix.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/task_queue_win.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/testclient.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/testechoserver.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/testutils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/thread.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/thread_checker_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/timestampaligner.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/timeutils.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/unixfilesystem.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/virtualsocketserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/weak_ptr.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/win32.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/win32filesystem.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/win32socketinit.cc>

#define PACKET_MAXIMUMS PACKET_MAXIMUMS_win32socketserver_BOSS
#define IP_HEADER_SIZE IP_HEADER_SIZE_win32socketserver_BOSS
#define ICMP_HEADER_SIZE ICMP_HEADER_SIZE_win32socketserver_BOSS
#define ICMP_PING_TIMEOUT_MILLIS ICMP_PING_TIMEOUT_MILLIS_win32socketserver_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/win32socketserver.cc>
#undef PACKET_MAXIMUMS
#undef IP_HEADER_SIZE
#undef ICMP_HEADER_SIZE
#undef ICMP_PING_TIMEOUT_MILLIS

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/win32window.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/zero_memory.cc>

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/experiments/alr_experiment.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/experiments/congestion_controller_experiment.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/experiments/field_trial_parser.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/experiments/field_trial_units.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/experiments/quality_scaling_experiment.cc>

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/memory/aligned_malloc.cc>

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/numerics/exp_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/numerics/histogram_percentile_counter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/numerics/sample_counter.cc>

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/strings/audio_format_to_string.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/strings/string_builder.cc>

//#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/synchronization/rw_lock_posix.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/synchronization/rw_lock_win.cc>
#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/synchronization/rw_lock_wrapper.cc>

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/system/file_wrapper.cc>

#include <addon/webrtc-jumpingyang001_for_boss/rtc_base/time/timestamp_extrapolator.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/audio_options.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/candidate.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/datachannelinterface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/jsep.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/jsepicecandidate.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/mediaconstraintsinterface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/mediastreaminterface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/mediatypes.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/peerconnectioninterface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/proxy.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/rtcerror.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/rtp_headers.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/rtpparameters.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/rtpreceiverinterface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/rtptransceiverinterface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/statstypes.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/audio/audio_frame.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/api/audio/echo_canceller3_config.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/api/audio/echo_canceller3_factory.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/audio_codec_pair_id.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/audio_decoder.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/audio_encoder.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/audio_format.cc>

#define NotAdvertised NotAdvertised_builtin_audio_decoder_factory_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/builtin_audio_decoder_factory.cc>
#undef NotAdvertised

#define NotAdvertised NotAdvertised_builtin_audio_encoder_factory_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/builtin_audio_encoder_factory.cc>
#undef NotAdvertised

#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/g711/audio_decoder_g711.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/g711/audio_encoder_g711.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/g722/audio_decoder_g722.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/g722/audio_encoder_g722.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/ilbc/audio_decoder_ilbc.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/ilbc/audio_encoder_ilbc.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/isac/audio_decoder_isac_fix.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/isac/audio_decoder_isac_float.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/isac/audio_encoder_isac_fix.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/isac/audio_encoder_isac_float.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/L16/audio_decoder_L16.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/L16/audio_encoder_L16.cc>

//#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/opus/audio_decoder_opus.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/opus/audio_encoder_opus.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/api/audio_codecs/opus/audio_encoder_opus_config.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/call/transport.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/video/color_space.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video/encoded_frame.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/api/video/i010_buffer.cc> libyuv필요!
//#include <addon/webrtc-jumpingyang001_for_boss/api/video/i420_buffer.cc> libyuv필요!
#include <addon/webrtc-jumpingyang001_for_boss/api/video/video_bitrate_allocation.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video/video_content_type.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video/video_frame.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video/video_frame_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video/video_source_interface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video/video_stream_decoder_create.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video/video_stream_encoder_create.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video/video_timing.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/builtin_video_decoder_factory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/builtin_video_encoder_factory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/sdp_video_format.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/video_codec.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/video_decoder.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/video_decoder_software_fallback_wrapper.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/video_encoder.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/video_encoder_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/video_codecs/video_encoder_software_fallback_wrapper.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/transport/bitrate_settings.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/transport/network_types.cc>

#include <addon/webrtc-jumpingyang001_for_boss/api/units/data_rate.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/units/data_size.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/units/time_delta.cc>
#include <addon/webrtc-jumpingyang001_for_boss/api/units/timestamp.cc>

#include <addon/webrtc-jumpingyang001_for_boss/audio/audio_level.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/audio_receive_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/audio_send_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/audio_state.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/audio_transport_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/channel.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/channel_proxy.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/null_audio_poller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/remix_resample.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/time_interval.cc>
#include <addon/webrtc-jumpingyang001_for_boss/audio/transport_feedback_packet_loss_tracker.cc>

#include <addon/webrtc-jumpingyang001_for_boss/audio/utility/audio_frame_operations.cc>

#include <addon/webrtc-jumpingyang001_for_boss/call/audio_receive_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/audio_send_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/audio_state.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/bitrate_allocator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/call.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/call_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/callfactory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/degraded_call.cc>

#define kDefaultProcessIntervalMs kDefaultProcessIntervalMs_fake_network_pipe_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/call/fake_network_pipe.cc>
#undef kDefaultProcessIntervalMs

#include <addon/webrtc-jumpingyang001_for_boss/call/flexfec_receive_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/flexfec_receive_stream_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/packet_receiver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/receive_time_calculator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/rtcp_demuxer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/rtp_bitrate_configurator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/rtp_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/rtp_demuxer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/rtp_payload_params.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/rtp_rtcp_demuxer_helper.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/rtp_stream_receiver_controller.cc>

#define kRetransmitWindowSizeMs kRetransmitWindowSizeMs_rtp_transport_controller_send_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/call/rtp_transport_controller_send.cc>
#undef kRetransmitWindowSizeMs

#include <addon/webrtc-jumpingyang001_for_boss/call/rtp_video_sender.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/rtx_receive_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/simulated_network.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/syncable.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/video_receive_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/call/video_send_stream.cc>

#include <addon/webrtc-jumpingyang001_for_boss/common_audio/audio_converter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/audio_ring_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/audio_util.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/blocker.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/channel_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/fir_filter_c.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/fir_filter_factory.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/common_audio/fir_filter_neon.cc> 최적화시 다시 고려!
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/fir_filter_sse.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/lapped_transform.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/real_fourier.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/real_fourier_ooura.cc>

extern "C"
{
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/ring_buffer.c>
}

#include <addon/webrtc-jumpingyang001_for_boss/common_audio/smoothing_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/sparse_fir_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/wav_file.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/wav_header.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/window_generator.cc>

#include <addon/webrtc-jumpingyang001_for_boss/common_audio/resampler/push_resampler.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/resampler/push_sinc_resampler.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/resampler/resampler.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/resampler/sinc_resampler.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/common_audio/resampler/sinc_resampler_neon.cc> 최적화시 다시 고려!
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/resampler/sinc_resampler_sse.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_audio/resampler/sinusoidal_linear_chirp_source.cc>

extern "C"
{
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/third_party/fft4g/fft4g.c>

    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor_mips.c> 최적화시 다시 고려!

    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/auto_corr_to_refl_coef.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/auto_correlation.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/complex_bit_reverse.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/complex_bit_reverse_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/complex_fft.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/complex_fft_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/copy_set_operations.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/cross_correlation.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/cross_correlation_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/cross_correlation_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/division_operations.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/dot_product_with_scale.cc>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/downsample_fast.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/downsample_fast_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/downsample_fast_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/energy.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/filter_ar.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/filter_ar_fast_q12.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/filter_ar_fast_q12_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/filter_ma_fast_q12.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/get_hanning_window.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/get_scaling_square.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/ilbc_specific_functions.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/levinson_durbin.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/lpc_to_refl_coef.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/min_max_operations.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/min_max_operations_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/min_max_operations_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/randomization_functions.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/real_fft.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/refl_coef_to_lpc.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/resample.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/resample_48khz.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/resample_by_2.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/resample_by_2_internal.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/resample_by_2_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/resample_fractional.c>

    #define InitFunctionPointers InitFunctionPointers_spl_init_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/spl_init.c>
    #undef InitFunctionPointers

    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/spl_inl.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/spl_sqrt.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/splitting_filter.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/sqrt_of_one_minus_x_squared.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/vector_scaling_operations.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/common_audio/signal_processing/vector_scaling_operations_mips.c> 최적화시 다시 고려!
}

#include <addon/webrtc-jumpingyang001_for_boss/common_audio/vad/vad.cc>

extern "C"
{
    #define kInitCheck kInitCheck_vad_core_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/vad/vad_core.c>
    #undef kInitCheck

    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/vad/vad_filterbank.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/vad/vad_gmm.c>
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/vad/vad_sp.c>

    #define kInitCheck kInitCheck_webrtc_vad_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/common_audio/vad/webrtc_vad.c>
    #undef kInitCheck
}

#include <addon/webrtc-jumpingyang001_for_boss/common_video/bitrate_adjuster.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/common_video/i420_buffer_pool.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_video/incoming_video_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_video/video_frame.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/common_video/video_frame_buffer.cc> libyuv필요!
#include <addon/webrtc-jumpingyang001_for_boss/common_video/video_render_frames.cc>

#include <addon/webrtc-jumpingyang001_for_boss/common_video/h264/h264_bitstream_parser.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_video/h264/h264_common.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_video/h264/pps_parser.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_video/h264/sps_parser.cc>
#include <addon/webrtc-jumpingyang001_for_boss/common_video/h264/sps_vui_rewriter.cc>

#include <addon/webrtc-jumpingyang001_for_boss/pc/audiotrack.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/pc/bundlefilter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/channel.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/channelmanager.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/createpeerconnectionfactory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/datachannel.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/dtlssrtptransport.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/dtmfsender.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/externalhmac.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/iceserverparsing.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/jsepicecandidate.cc>

#define kPreferenceUnknown kPreferenceUnknown_jsepsessiondescription_BOSS
#define kPreferenceHost kPreferenceHost_jsepsessiondescription_BOSS
#define kPreferenceReflexive kPreferenceReflexive_jsepsessiondescription_BOSS
#define kPreferenceRelayed kPreferenceRelayed_jsepsessiondescription_BOSS
#define kDummyAddress kDummyAddress_jsepsessiondescription_BOSS
#define kDummyPort kDummyPort_jsepsessiondescription_BOSS
#define GetCandidatePreferenceFromType GetCandidatePreferenceFromType_jsepsessiondescription_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/pc/jsepsessiondescription.cc>
#undef kPreferenceUnknown
#undef kPreferenceHost
#undef kPreferenceReflexive
#undef kPreferenceRelayed
#undef kDummyAddress
#undef kDummyPort
#undef GetCandidatePreferenceFromType

#include <addon/webrtc-jumpingyang001_for_boss/pc/jseptransport.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/jseptransportcontroller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/localaudiosource.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/mediasession.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/mediastream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/mediastreamobserver.cc>

#define MSG_CREATE_SESSIONDESCRIPTION_FAILED MSG_CREATE_SESSIONDESCRIPTION_FAILED_peerconnection_BOSS
#define CreateSessionDescriptionMsg CreateSessionDescriptionMsg_peerconnection_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/pc/peerconnection.cc>
#undef MSG_CREATE_SESSIONDESCRIPTION_FAILED
#undef CreateSessionDescriptionMsg

#include <addon/webrtc-jumpingyang001_for_boss/pc/peerconnectionfactory.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/pc/peerconnectionwrapper.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/remoteaudiosource.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/rtcpmuxfilter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/rtcstatscollector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/rtcstatstraversal.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/rtpmediautils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/rtpparametersconversion.cc>

#define GenerateUniqueId GenerateUniqueId_rtpreceiver_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/pc/rtpreceiver.cc>
#undef GenerateUniqueId

#define GenerateUniqueId GenerateUniqueId_rtpsender_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/pc/rtpsender.cc>
#undef GenerateUniqueId

#include <addon/webrtc-jumpingyang001_for_boss/pc/rtptransceiver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/rtptransport.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/sctputils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/sdputils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/sessiondescription.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/srtpfilter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/srtpsession.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/srtptransport.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/statscollector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/trackmediainfomap.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/transportstats.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/videocapturertracksource.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/videotrack.cc>
#include <addon/webrtc-jumpingyang001_for_boss/pc/videotracksource.cc>

#define kPreferenceUnknown kPreferenceUnknown_webrtcsdp_BOSS
#define kPreferenceHost kPreferenceHost_webrtcsdp_BOSS
#define kPreferenceReflexive kPreferenceReflexive_webrtcsdp_BOSS
#define kPreferenceRelayed kPreferenceRelayed_webrtcsdp_BOSS
#define kDummyAddress kDummyAddress_webrtcsdp_BOSS
#define kDummyPort kDummyPort_webrtcsdp_BOSS
#define GetCandidatePreferenceFromType GetCandidatePreferenceFromType_webrtcsdp_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/pc/webrtcsdp.cc>
#undef kPreferenceUnknown
#undef kPreferenceHost
#undef kPreferenceReflexive
#undef kPreferenceRelayed
#undef kDummyAddress
#undef kDummyPort
#undef GetCandidatePreferenceFromType

#define MSG_CREATE_SESSIONDESCRIPTION_FAILED MSG_CREATE_SESSIONDESCRIPTION_FAILED_webrtcsessiondescriptionfactory_BOSS
#define CreateSessionDescriptionMsg CreateSessionDescriptionMsg_webrtcsessiondescriptionfactory_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/pc/webrtcsessiondescriptionfactory.cc>
#undef MSG_CREATE_SESSIONDESCRIPTION_FAILED
#undef CreateSessionDescriptionMsg

#include <addon/webrtc-jumpingyang001_for_boss/stats/rtcstats.cc>
#include <addon/webrtc-jumpingyang001_for_boss/stats/rtcstats_objects.cc>
#include <addon/webrtc-jumpingyang001_for_boss/stats/rtcstatsreport.cc>

#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/asyncstuntcpsocket.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/basicpacketsocketfactory.cc>

#define kMinRtpPacketLen kMinRtpPacketLen_dtlstransport_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/dtlstransport.cc>
#undef kMinRtpPacketLen

#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/dtlstransportinternal.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/icetransportinternal.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/p2pconstants.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/p2ptransportchannel.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/packetlossestimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/packetsocketfactory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/packettransportinternal.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/port.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/portallocator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/portinterface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/pseudotcp.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/regatheringcontroller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/relayport.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/relayserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/stun.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/stunport.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/stunrequest.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/stunserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/tcpport.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/teststunserver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/transportdescription.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/transportdescriptionfactory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/turnport.cc>

#define TURN_CHANNEL_HEADER_SIZE TURN_CHANNEL_HEADER_SIZE_turnserver_BOSS
#define IsTurnChannelData IsTurnChannelData_turnserver_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/turnserver.cc>
#undef TURN_CHANNEL_HEADER_SIZE
#undef IsTurnChannelData

#include <addon/webrtc-jumpingyang001_for_boss/p2p/base/udptransport.cc>

#include <addon/webrtc-jumpingyang001_for_boss/p2p/client/basicportallocator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/p2p/client/turnportfactory.cc>

#include <addon/webrtc-jumpingyang001_for_boss/media/base/adaptedvideotracksource.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/codec.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/fakeframesource.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/base/fakertp.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/fakevideocapturer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/fakevideorenderer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/h264_profile_level_id.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/mediachannel.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/mediaconstants.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/mediaengine.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/rtpdataengine.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/rtputils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/streamparams.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/turnutils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/videoadapter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/videobroadcaster.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/videocapturer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/videocommon.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/videosourcebase.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/base/vp9_profile.cc>

#include <addon/webrtc-jumpingyang001_for_boss/media/engine/adm_helpers.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/engine/apm_helpers.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/engine/constants.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/convert_legacy_video_factory.cc>

#define IsFormatSupported IsFormatSupported_internaldecoderfactory_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/media/engine/internaldecoderfactory.cc>
#undef IsFormatSupported

#define IsFormatSupported IsFormatSupported_internalencoderfactory_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/media/engine/internalencoderfactory.cc>
#undef IsFormatSupported

//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/multiplexcodecfactory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/engine/payload_type_mapper.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/scopedvideodecoder.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/scopedvideoencoder.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/simulcast.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/simulcast_encoder_adapter.cc> libyuv필요!
#include <addon/webrtc-jumpingyang001_for_boss/media/engine/vp8_encoder_simulcast_proxy.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/engine/webrtcmediaengine.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/webrtcvideocapturer.cc> libyuv필요!
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/webrtcvideocapturerfactory.cc> libyuv필요!
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/webrtcvideodecoderfactory.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/webrtcvideoencoderfactory.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/media/engine/webrtcvideoengine.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/engine/webrtcvoiceengine.cc>

#include <addon/webrtc-jumpingyang001_for_boss/media/sctp/noop.cc>
#include <addon/webrtc-jumpingyang001_for_boss/media/sctp/sctptransport.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/acm2/acm_codec_database.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/acm2/acm_receiver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/acm2/acm_resampler.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/acm2/audio_coding_module.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/acm2/call_statistics.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/acm2/codec_manager.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/acm2/rent_a_codec.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/audio_network_adaptor_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/audio_network_adaptor_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/bitrate_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/channel_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/controller_manager.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/debug_dump_writer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/dtx_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/event_log_writer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/fec_controller_plr_based.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/fec_controller_rplr_based.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/audio_network_adaptor/frame_length_controller.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/audio_format_conversion.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/legacy_encoded_audio_frame.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/cng/audio_encoder_cng.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/cng/webrtc_cng.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/g711/audio_decoder_pcm.cc>

#define CreateConfig CreateConfig_audio_encoder_pcm_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/g711/audio_encoder_pcm.cc>
#undef CreateConfig

extern "C"
{
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/g711/g711_interface.c>
}

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/g722/audio_decoder_g722.cc>

#define CreateConfig CreateConfig_audio_encoder_g722_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/g722/audio_encoder_g722.cc>
#undef CreateConfig

extern "C"
{
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/g722/g722_interface.c>
}

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/empty.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/locked_bandwidth_info.cc>

#define PreFiltBankstr PreFiltBankstr_fix_source_BOSS
#define PostFiltBankstr PostFiltBankstr_fix_source_BOSS
#define PitchFiltstr PitchFiltstr_fix_source_BOSS
#define PitchAnalysisStruct PitchAnalysisStruct_fix_source_BOSS
#define BwEstimatorstr BwEstimatorstr_fix_source_BOSS
#define RateModel RateModel_fix_source_BOSS
#define IsacSaveEncoderData IsacSaveEncoderData_fix_source_BOSS
#define transcode_obj transcode_obj_fix_source_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/audio_decoder_isacfix.cc>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/audio_encoder_isacfix.cc>

    extern "C"
    {
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/arith_routines.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/arith_routines_hist.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/arith_routines_logist.c>

        #define clamp clamp_fix_BOSS
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/bandwidth_estimator.c>
        #undef clamp

        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/decode.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/decode_bwe.c>

        #define exp2_Q10_T exp2_Q10_T_decode_plc_BOSS
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/decode_plc.c>
        #undef exp2_Q10_T

        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/encode.c>

        #define CalcLrIntQ CalcLrIntQ_entropy_coding_BOSS
        #define kBwCdf kBwCdf_fix_BOSS
        #define kBwCdfPtr kBwCdfPtr_fix_BOSS
        #define kBwInitIndex kBwInitIndex_fix_BOSS
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/entropy_coding.c>
        #undef CalcLrIntQ
        #undef kBwCdf
        #undef kBwCdfPtr
        #undef kBwInitIndex

        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/entropy_coding_mips.c> 최적화시 다시 고려!
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/entropy_coding_neon.c> 최적화시 다시 고려!
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/fft.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/filterbank_tables.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/filterbanks.c>
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/filterbanks_mips.c> 최적화시 다시 고려!
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/filterbanks_neon.c> 최적화시 다시 고려!
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/filters.c>
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/filters_mips.c> 최적화시 다시 고려!
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/filters_neon.c> 최적화시 다시 고려!
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/initialize.c>

        #define InitFunctionPointers InitFunctionPointers_isacfix_BOSS
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/isacfix.c>
        #undef InitFunctionPointers

        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/lattice.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/lattice_c.c>
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/lattice_mips.c> 최적화시 다시 고려!
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/lattice_neon.c> 최적화시 다시 고려!

        #define exp2_Q10_T exp2_Q10_T_lpc_masking_model_BOSS
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/lpc_masking_model.c>
        #undef exp2_Q10_T

        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/lpc_masking_model_mips.c> 최적화시 다시 고려!
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/lpc_tables.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_estimator.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_estimator_c.c>
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_estimator_mips.c> 최적화시 다시 고려!

        #define CalcLrIntQ CalcLrIntQ_pitch_filter_BOSS
        #define kIntrpCoef kIntrpCoef_fix_BOSS
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_filter.c>
        #undef CalcLrIntQ
        #undef kIntrpCoef

        #define kDampFilter kDampFilter_pitch_filter_c_BOSS
        #define kIntrpCoef kIntrpCoef_pitch_filter_c_BOSS
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_filter_c.c>
        #undef kDampFilter
        #undef kIntrpCoef

        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_filter_mips.c> 최적화시 다시 고려!
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_gain_tables.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_lag_tables.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/spectrum_ar_model_tables.c>
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/transform.c>
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/transform_mips.c> 최적화시 다시 고려!
        //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/transform_neon.c> 최적화시 다시 고려!
        #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/fix/source/transform_tables.c>
    }
#undef PreFiltBankstr
#undef PostFiltBankstr
#undef PitchFiltstr
#undef PitchAnalysisStruct
#undef BwEstimatorstr
#undef RateModel
#undef IsacSaveEncoderData
#undef transcode_obj

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/audio_decoder_isac.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/audio_encoder_isac.cc>

extern "C"
{
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/arith_routines.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/arith_routines_hist.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/arith_routines_logist.c>

    #define Thld_20_30 Thld_20_30_main_BOSS
    #define Thld_30_20 Thld_30_20_main_BOSS
    #define Thld_30_60 Thld_30_60_main_BOSS
    #define Thld_60_30 Thld_60_30_main_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/bandwidth_estimator.c>
    #undef Thld_20_30
    #undef Thld_30_20
    #undef Thld_30_60
    #undef Thld_60_30

    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/crc.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/decode.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/decode_bwe.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/encode.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/encode_lpc_swb.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/entropy_coding.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/filter_functions.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/filterbanks.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/intialize.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/isac.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/isac_vad.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/lattice.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_analysis.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_gain_swb_tables.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_shape_swb12_tables.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_shape_swb16_tables.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_tables.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/pitch_estimator.c>

    #define kIntrpCoef kIntrpCoef_main_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/pitch_filter.c>
    #undef kIntrpCoef

    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/pitch_gain_tables.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/pitch_lag_tables.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/spectrum_ar_model_tables.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/isac/main/source/transform.c>
}

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/pcm16b/audio_decoder_pcm16b.cc>

#define CreateConfig CreateConfig_audio_encoder_pcm16b_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/pcm16b/audio_encoder_pcm16b.cc>
#undef CreateConfig

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/pcm16b/pcm16b.c>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/codecs/pcm16b/pcm16b_common.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/accelerate.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/audio_multi_vector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/audio_vector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/background_noise.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/buffer_level_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/comfort_noise.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/cross_correlation.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/decision_logic.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/decoder_database.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/delay_manager.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/delay_peak_detector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/dsp_helper.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/dtmf_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/dtmf_tone_generator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/expand.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/expand_uma_logger.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/merge.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/nack_tracker.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/neteq.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/neteq_decoder_enum.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/neteq_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/normal.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/packet.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/packet_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/post_decode_vad.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/preemptive_expand.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/random_vector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/red_payload_splitter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/rtcp.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/statistics_calculator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/sync_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/tick_timer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/time_stretch.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_coding/neteq/timestamp_scaler.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/audio_device_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/audio_device_data_observer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/audio_device_generic.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/audio_device_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/audio_device_name.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/fine_audio_buffer.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/dummy/audio_device_dummy.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/dummy/file_audio_device.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/dummy/file_audio_device_factory.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/win/audio_device_core_win.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/win/audio_device_module_win.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/win/core_audio_base_win.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/win/core_audio_input_win.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/win/core_audio_output_win.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_device/win/core_audio_utility_win.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_mixer/audio_frame_manipulator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_mixer/audio_mixer_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_mixer/default_output_rate_calculator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_mixer/frame_combiner.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_mixer/gain_change_calculator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_mixer/sine_wave_generator.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/audio_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/audio_processing_impl.cc>

#define MapError MapError_echo_cancellation_impl_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/echo_cancellation_impl.cc>
#undef MapError

#define MapError MapError_echo_control_mobile_impl_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/echo_control_mobile_impl.cc>
#undef MapError

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/gain_control_for_experimental_agc.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/gain_control_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/gain_controller2.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/level_estimator_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/low_cut_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/noise_suppression_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/residual_echo_detector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/rms_level.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/splitting_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/three_band_filter_bank.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/typing_detection.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/voice_detection_impl.cc>

#define MulRe MulRe_aec_core_BOSS
#define MulIm MulIm_aec_core_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aec/aec_core.cc>
#undef MulRe
#undef MulIm

//#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aec/aec_core_mips.cc> 최적화시 다시 고려!
//#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aec/aec_core_neon.cc> 최적화시 다시 고려!

#define MulRe MulRe_aec_core_sse2_BOSS
#define MulIm MulIm_aec_core_sse2_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aec/aec_core_sse2.cc>
#undef MulRe
#undef MulIm

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aec/aec_resampler.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aec/echo_cancellation.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aec_dump/null_aec_dump_factory.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aecm/aecm_core.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aecm/aecm_core_c.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aecm/aecm_core_mips.cc> 최적화시 다시 고려!
//#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aecm/aecm_core_neon.cc> 최적화시 다시 고려!

#define kInitCheck kInitCheck_echo_control_mobile_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/aecm/echo_control_mobile.cc>
#undef kInitCheck

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc/agc.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc/agc_manager_direct.cc>

#define kTransientWidthThreshold kTransientWidthThreshold_loudness_histogram_BOSS
#define kLowProbabilityThreshold kLowProbabilityThreshold_loudness_histogram_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc/loudness_histogram.cc>
#undef kTransientWidthThreshold
#undef kLowProbabilityThreshold

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc/utility.cc>

extern "C"
{
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc/legacy/analog_agc.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc/legacy/digital_agc.c>
}

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/adaptive_agc.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/adaptive_digital_gain_applier.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/adaptive_mode_level_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/adaptive_mode_level_estimator_agc.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/agc2_testing_common.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/biquad_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/compute_interpolated_gain_curve.cc>

#define kChunkSizeMs kChunkSizeMs_down_sampler_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/down_sampler.cc>
#undef kChunkSizeMs

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/fixed_digital_level_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/fixed_gain_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/gain_applier.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/gain_curve_applier.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/interpolated_gain_curve.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/limiter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/noise_level_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/noise_spectrum_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/saturation_protector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/signal_classifier.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/vad_with_level.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/vector_float_frame.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/rnn_vad/features_extraction.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/rnn_vad/fft_util.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/rnn_vad/lp_residual.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/rnn_vad/pitch_search.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/rnn_vad/pitch_search_internal.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/rnn_vad/rnn.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/rnn_vad/spectral_features.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/agc2/rnn_vad/spectral_features_internal.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/audio_generator/file_audio_generator.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/echo_detector/circular_buffer.cc>

#define kAlpha kAlpha_mean_variance_estimator_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/echo_detector/mean_variance_estimator.cc>
#undef kAlpha

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/echo_detector/moving_max.cc>

#define kAlpha kAlpha_normalized_covariance_estimator_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/echo_detector/normalized_covariance_estimator.cc>
#undef kAlpha

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/include/aec_dump.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/include/audio_generator_factory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/include/audio_processing.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/include/audio_processing_statistics.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/include/config.cc>

#define kChunkSizeMs kChunkSizeMs_intelligibility_enhancer_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/intelligibility/intelligibility_enhancer.cc>
#undef kChunkSizeMs

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/intelligibility/intelligibility_utils.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/logging/apm_data_dumper.cc>

extern "C"
{
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/ns/noise_suppression.c>
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/ns/noise_suppression_x.c>

    #define NoiseEstimation NoiseEstimation_ns_core_BOSS
    #define UpdateNoiseEstimate UpdateNoiseEstimate_ns_core_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/ns/ns_core.c>
    #undef NoiseEstimation
    #undef UpdateNoiseEstimate

    #define UpdateNoiseEstimate UpdateNoiseEstimate_nsx_core_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/ns/nsx_core.c>
    #undef UpdateNoiseEstimate

    #include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/ns/nsx_core_c.c>
    //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/ns/nsx_core_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/ns/nsx_core_neon.c> 최적화시 다시 고려!
}

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/transient/file_utils.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/transient/moving_moments.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/transient/transient_detector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/transient/transient_suppressor.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/transient/wpd_node.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/transient/wpd_tree.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/utility/block_mean_calculator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/utility/delay_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/utility/delay_estimator_wrapper.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/utility/ooura_fft.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/utility/ooura_fft_mips.cc> 최적화시 다시 고려!
//#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/utility/ooura_fft_neon.cc> 최적화시 다시 고려!
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/utility/ooura_fft_sse2.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/vad/gmm.cc>

#define kTransientWidthThreshold kTransientWidthThreshold_pitch_based_vad_BOSS
#define kLowProbabilityThreshold kLowProbabilityThreshold_pitch_based_vad_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/vad/pitch_based_vad.cc>
#undef kTransientWidthThreshold
#undef kLowProbabilityThreshold

#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/vad/pitch_internal.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/vad/pole_zero_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/vad/standalone_vad.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/vad/vad_audio_proc.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/vad/vad_circular_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/audio_processing/vad/voice_activity_detector.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/bitrate_controller/bitrate_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/bitrate_controller/bitrate_controller_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/bitrate_controller/send_side_bandwidth_estimation.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/congestion_window_pushback_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/receive_side_congestion_controller.cc>

#define kRetransmitWindowSizeMs kRetransmitWindowSizeMs_send_side_congestion_controller_BOSS
#define CwndExperimentEnabled CwndExperimentEnabled_send_side_congestion_controller_BOSS
#define ReadCwndExperimentParameter ReadCwndExperimentParameter_send_side_congestion_controller_BOSS
#define kCwndExperiment kCwndExperiment_send_side_congestion_controller_BOSS
#define kDefaultAcceptedQueueMs kDefaultAcceptedQueueMs_send_side_congestion_controller_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/send_side_congestion_controller.cc>
#undef kRetransmitWindowSizeMs
#undef CwndExperimentEnabled
#undef ReadCwndExperimentParameter
#undef kCwndExperiment
#undef kDefaultAcceptedQueueMs

#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/transport_feedback_adapter.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/bbr/bandwidth_sampler.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/bbr/bbr_factory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/bbr/bbr_network_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/bbr/data_transfer_tracker.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/bbr/loss_rate_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/bbr/rtt_stats.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/alr_detector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/bitrate_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/delay_based_bwe.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/goog_cc_factory.cc>

#define CwndExperimentEnabled CwndExperimentEnabled_goog_cc_network_control_BOSS
#define ReadCwndExperimentParameter ReadCwndExperimentParameter_goog_cc_network_control_BOSS
#define kCwndExperiment kCwndExperiment_goog_cc_network_control_BOSS
#define kDefaultAcceptedQueueMs kDefaultAcceptedQueueMs_goog_cc_network_control_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/goog_cc_network_control.cc>
#undef CwndExperimentEnabled
#undef ReadCwndExperimentParameter
#undef kCwndExperiment
#undef kDefaultAcceptedQueueMs

#define kDeltaCounterMax kDeltaCounterMax_median_slope_estimator_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/median_slope_estimator.cc>
#undef kDeltaCounterMax

#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/probe_bitrate_estimator.cc>

#define kMinProbePacketsSent kMinProbePacketsSent_probe_controller_BOSS
#define kMinProbeDurationMs kMinProbeDurationMs_probe_controller_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/probe_controller.cc>
#undef kMinProbePacketsSent
#undef kMinProbeDurationMs

#define kDeltaCounterMax kDeltaCounterMax_trendline_estimator_BOSS
#define kMaxAdaptOffsetMs kMaxAdaptOffsetMs_trendline_estimator_BOSS
#define kOverUsingTimeThreshold kOverUsingTimeThreshold_trendline_estimator_BOSS
#define kMinNumDeltas kMinNumDeltas_trendline_estimator_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/goog_cc/trendline_estimator.cc>
#undef kDeltaCounterMax
#undef kMaxAdaptOffsetMs
#undef kOverUsingTimeThreshold
#undef kMinNumDeltas

#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/rtp/pacer_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/rtp/send_side_congestion_controller.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/rtp/send_time_history.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/congestion_controller/rtp/transport_feedback_adapter.cc>

#define kMinProbePacketsSent kMinProbePacketsSent_bitrate_prober_BOSS
#define kMinProbeDurationMs kMinProbeDurationMs_bitrate_prober_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/pacing/bitrate_prober.cc>
#undef kMinProbePacketsSent
#undef kMinProbeDurationMs

#include <addon/webrtc-jumpingyang001_for_boss/modules/pacing/interval_budget.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/pacing/paced_sender.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/pacing/packet_queue.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/pacing/packet_queue_interface.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/pacing/packet_router.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/pacing/round_robin_packet_queue.cc>

#define kDefaultRttMs kDefaultRttMs_aimd_rate_control_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/aimd_rate_control.cc>
#undef kDefaultRttMs

#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/bwe_defines.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/inter_arrival.cc>

#define kMaxAdaptOffsetMs kMaxAdaptOffsetMs_overuse_detector_BOSS
#define kOverUsingTimeThreshold kOverUsingTimeThreshold_overuse_detector_BOSS
#define kMinNumDeltas kMinNumDeltas_overuse_detector_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/overuse_detector.cc>
#undef kMaxAdaptOffsetMs
#undef kOverUsingTimeThreshold
#undef kMinNumDeltas

#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/overuse_estimator.cc>

#define kTimestampGroupLengthMs kTimestampGroupLengthMs_remote_bitrate_estimator_abs_send_time_BOSS
#define kTimestampToMs kTimestampToMs_remote_bitrate_estimator_abs_send_time_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/remote_bitrate_estimator_abs_send_time.cc>
#undef kTimestampGroupLengthMs
#undef kTimestampToMs

#define kTimestampGroupLengthMs kTimestampGroupLengthMs_remote_bitrate_estimator_single_stream_BOSS
#define kTimestampToMs kTimestampToMs_remote_bitrate_estimator_single_stream_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/remote_bitrate_estimator_single_stream.cc>
#undef kTimestampGroupLengthMs
#undef kTimestampToMs

#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/remote_estimator_proxy.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/remote_bitrate_estimator/test/bwe_test_framework.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/include/rtp_rtcp_defines.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/contributing_sources.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/dtmf_queue.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/fec_private_tables_bursty.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/fec_private_tables_random.cc>

#define kMaxMediaPackets kMaxMediaPackets_flexfec_header_reader_writer_BOSS
#define kMaxFecPackets kMaxFecPackets_flexfec_header_reader_writer_BOSS
#define kPacketMaskOffset kPacketMaskOffset_flexfec_header_reader_writer_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/flexfec_header_reader_writer.cc>
#undef kMaxMediaPackets
#undef kMaxFecPackets
#undef kPacketMaskOffset

#define kPacketLogIntervalMs kPacketLogIntervalMs_flexfec_receiver_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/flexfec_receiver.cc>
#undef kPacketLogIntervalMs

#define kPacketLogIntervalMs kPacketLogIntervalMs_flexfec_sender_BOSS
#define kMaxInitRtpSeqNumber kMaxInitRtpSeqNumber_flexfec_sender_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/flexfec_sender.cc>
#undef kPacketLogIntervalMs
#undef kMaxInitRtpSeqNumber

#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/forward_error_correction.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/forward_error_correction_internal.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/packet_loss_stats.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/playout_delay_oracle.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/receive_statistics_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/remote_ntp_time_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_nack_stats.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_receiver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_sender.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_transceiver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_transceiver_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_transceiver_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_format.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_format_h264.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_format_video_generic.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_format_vp8.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_format_vp9.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_generic_frame_descriptor.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_header_extension_map.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_header_extensions.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_header_parser.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_packet.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_packet_history.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_packet_received.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_packet_to_send.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_payload_registry.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_receiver_audio.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_receiver_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_receiver_strategy.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_receiver_video.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_rtcp_impl.cc>

#define kMaxInitRtpSeqNumber kMaxInitRtpSeqNumber_rtp_sender_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_sender.cc>
#undef kMaxInitRtpSeqNumber

#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_sender_audio.cc>

#define kRedForFecHeaderLength kRedForFecHeaderLength_rtp_sender_video_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_sender_video.cc>
#undef kRedForFecHeaderLength

#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_utility.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtp_video_header.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/time_util.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/tmmbr_help.cc>

#define kMaxInitRtpSeqNumber kMaxInitRtpSeqNumber_ulpfec_generator_BOSS
#define kRedForFecHeaderLength kRedForFecHeaderLength_ulpfec_generator_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/ulpfec_generator.cc>
#undef kMaxInitRtpSeqNumber
#undef kRedForFecHeaderLength

#define kMaxMediaPackets kMaxMediaPackets_ulpfec_header_reader_writer_BOSS
#define kMaxFecPackets kMaxFecPackets_ulpfec_header_reader_writer_BOSS
#define kPacketMaskOffset kPacketMaskOffset_ulpfec_header_reader_writer_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/ulpfec_header_reader_writer.cc>
#undef kMaxMediaPackets
#undef kMaxFecPackets
#undef kPacketMaskOffset

#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/ulpfec_receiver_impl.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/app.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/bye.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/common_header.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/compound_packet.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/dlrr.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/extended_jitter_report.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/extended_reports.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/fir.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/nack.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/pli.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/psfb.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/rapid_resync_request.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/receiver_report.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/remb.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/report_block.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/rrtr.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/rtpfb.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/sdes.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/sender_report.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/target_bitrate.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/tmmb_item.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/tmmbn.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/tmmbr.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/transport_feedback.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/rtp_rtcp/source/rtcp_packet/voip_metric.cc>

extern "C"
{
    #include <addon/webrtc-jumpingyang001_for_boss/modules/third_party/fft/fft.c>
}

extern "C"
{
    #define saturate saturate_g722_decode_BOSS
    #define block4 block4_g722_decode_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/modules/third_party/g722/g722_decode.c>
    #undef saturate
    #undef block4

    #define saturate saturate_g722_encode_BOSS
    #define block4 block4_g722_encode_BOSS
    #include <addon/webrtc-jumpingyang001_for_boss/modules/third_party/g722/g722_encode.c>
    #undef saturate
    #undef block4
}

//#include <addon/webrtc-jumpingyang001_for_boss/modules/utility/source/helpers_android.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/modules/utility/source/jvm_android.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/utility/source/process_thread_impl.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codec_timer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/decoder_database.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/decoding_state.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/encoded_frame.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/encoder_database.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/fec_controller_default.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/frame_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/frame_buffer2.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/frame_object.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/generic_decoder.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/generic_encoder.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/h264_sprop_parameter_sets.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/h264_sps_pps_tracker.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/histogram.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/inter_frame_delay.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/jitter_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/jitter_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/media_opt_util.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/media_optimization.cc>

#define kDefaultRttMs kDefaultRttMs_nack_module_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/nack_module.cc>
#undef kDefaultRttMs

#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/packet.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/packet_buffer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/qp_parser.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/receiver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/rtp_frame_reference_finder.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/rtt_filter.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/session_info.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/timestamp_map.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/timing.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/video_codec_initializer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/video_coding_defines.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/video_coding_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/video_receiver.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/video_sender.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/h264/h264.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/h264/h264_color_space.cc> ffmpeg/libavcodec필요!
//#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/h264/h264_decoder_impl.cc> ffmpeg/libavcodec필요!
//#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/h264/h264_encoder_impl.cc> openh264필요!

#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp8/default_temporal_layers.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp8/libvpx_vp8_decoder.cc> libyuv필요!
//#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp8/libvpx_vp8_encoder.cc> libyuv필요!
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp8/screenshare_layers.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp8/temporal_layers.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp9/svc_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp9/svc_rate_allocator.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp9/vp9_frame_buffer_pool.cc> vpx필요!
//#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp9/vp9_impl.cc> vpx필요!
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/codecs/vp9/vp9_noop.cc>

#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/default_video_bitrate_allocator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/frame_dropper.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/ivf_file_writer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/moving_average.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/quality_scaler.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/simulcast_rate_allocator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/simulcast_utility.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/vp8_header_parser.cc>
#include <addon/webrtc-jumpingyang001_for_boss/modules/video_coding/utility/vp9_uncompressed_header_parser.cc>

#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/fake_rtc_event_log.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/fake_rtc_event_log_factory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/icelogger.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_event_log.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_event_log2rtp_dump.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_event_log2stats.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_event_log2text.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_event_log_factory.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_event_log_impl.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_event_log_parser.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_event_log_parser_new.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/rtc_stream_config.cc>

#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_alr_state.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_audio_network_adaptation.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_audio_playout.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_audio_receive_stream_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_audio_send_stream_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_bwe_update_delay_based.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_bwe_update_loss_based.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_ice_candidate_pair.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_ice_candidate_pair_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_probe_cluster_created.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_probe_result_failure.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_probe_result_success.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_rtcp_packet_incoming.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_rtcp_packet_outgoing.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_rtp_packet_incoming.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_rtp_packet_outgoing.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_video_receive_stream_config.cc>
#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/events/rtc_event_video_send_stream_config.cc>

#include <addon/webrtc-jumpingyang001_for_boss/logging/rtc_event_log/output/rtc_event_log_output_file.cc>

#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/clock.cc>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/cpu_features.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/cpu_features_android.c>
//#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/cpu_features_linux.c>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/cpu_info.cc>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/event.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/event_timer_posix.cc>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/event_timer_win.cc>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/field_trial_default.cc>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/metrics_default.cc>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/rtp_to_ntp_estimator.cc>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/runtime_enabled_features_default.cc>
#include <addon/webrtc-jumpingyang001_for_boss/system_wrappers/source/sleep.cc>

#include <addon/webrtc-jumpingyang001_for_boss/video/call_stats.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/encoder_rtcp_feedback.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/overuse_frame_detector.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/quality_threshold.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/receive_statistics_proxy.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/video/replay.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/report_block_stats.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/rtp_streams_synchronizer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/rtp_video_stream_receiver.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/video/screenshare_loopback.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/send_delay_stats.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/send_statistics_proxy.cc>

#define kDefaultProcessIntervalMs kDefaultProcessIntervalMs_stats_counter_BOSS
#include <addon/webrtc-jumpingyang001_for_boss/video/stats_counter.cc>
#undef kDefaultProcessIntervalMs

#include <addon/webrtc-jumpingyang001_for_boss/video/stream_synchronization.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/video/sv_loopback.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/transport_adapter.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/video/video_analyzer.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/video/video_loopback.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/video_quality_observer.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/video_receive_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/video_send_stream.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/video_send_stream_impl.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/video_stream_decoder.cc>
#include <addon/webrtc-jumpingyang001_for_boss/video/video_stream_decoder_impl.cc>
//#include <addon/webrtc-jumpingyang001_for_boss/video/video_stream_encoder.cc>

////////////////////////////////////////////////////////////////////////////////
// 사용하지 않는 기능을 분리하기 위해 만든 페이크계층
#define _void_  BOSS_ASSERT("미구현!", false)
#define _int_  BOSS_ASSERT("미구현!", false); return 0
#define _bool_  BOSS_ASSERT("미구현!", false); return false
#define _ptr_  BOSS_ASSERT("미구현!", false); return nullptr
#define _str_  BOSS_ASSERT("미구현!", false); return ""

namespace webrtc
{
    AudioDecoderIlbcImpl::AudioDecoderIlbcImpl() {_void_;}
    AudioEncoderIlbcImpl::AudioEncoderIlbcImpl(const AudioEncoderIlbcConfig&, int) :
        frame_size_ms_(0),
        payload_type_(0),
        num_10ms_frames_per_packet_(0),
        encoder_(nullptr) {_void_;}
    AudioDecoderIlbcImpl::~AudioDecoderIlbcImpl() {_void_;}

    rtc::scoped_refptr<I420Buffer> I420Buffer::Create(int, int) {_ptr_;}
    rtc::scoped_refptr<I420Buffer> I420Buffer::Rotate(const I420BufferInterface&, VideoRotation) {_ptr_;}
    void I420Buffer::SetBlack(I420Buffer*) {_void_;}
    void I420Buffer::InitializeData() {_void_;}

    bool AudioDecoderIlbcImpl::HasDecodePlc() const {_bool_;}
    unsigned __int64 AudioDecoderIlbcImpl::DecodePlc(unsigned __int64, short*) {_int_;}
    void AudioDecoderIlbcImpl::Reset() {_void_;}
    std::vector<AudioDecoder::ParseResult, std::allocator<AudioDecoder::ParseResult> > AudioDecoderIlbcImpl::ParsePayload(rtc::BufferT<unsigned char, 0> &&, unsigned int)
    {_void_; return std::vector<AudioDecoder::ParseResult, std::allocator<AudioDecoder::ParseResult> >();}
    int AudioDecoderIlbcImpl::SampleRateHz() const {_int_;}
    unsigned __int64 AudioDecoderIlbcImpl::Channels() const {_int_;}
    int AudioDecoderIlbcImpl::DecodeInternal(const unsigned char*, unsigned __int64, int, short*, AudioDecoder::SpeechType*) {_int_;}

    AudioEncoderIlbcImpl::~AudioEncoderIlbcImpl() {_void_;}
    int AudioEncoderIlbcImpl::SampleRateHz() const {_int_;}
    unsigned __int64 AudioEncoderIlbcImpl::NumChannels() const {_int_;}
    unsigned __int64 AudioEncoderIlbcImpl::Num10MsFramesInNextPacket() const {_int_;}
    unsigned __int64 AudioEncoderIlbcImpl::Max10MsFramesInAPacket() const {_int_;}
    int AudioEncoderIlbcImpl::GetTargetBitrate() const {_int_;}
    AudioEncoder::EncodedInfo AudioEncoderIlbcImpl::EncodeImpl(unsigned int, rtc::ArrayView<short const , -4711>, rtc::BufferT<unsigned char, 0>*)
    {_void_; return AudioEncoder::EncodedInfo();}
    void AudioEncoderIlbcImpl::Reset() {_void_;}

    VideoCodecType PayloadStringToCodecType(std::basic_string<char, std::char_traits<char>, const std::allocator<char> >&)
    {_void_; return VideoCodecType();}

    class VideoStreamEncoder::VideoSourceProxy {};
    VideoStreamEncoder::VideoStreamEncoder(
        uint32_t number_of_cores,
        VideoStreamEncoderObserver* encoder_stats_observer,
        const VideoStreamEncoderSettings& settings,
        rtc::VideoSinkInterface<VideoFrame>* pre_encode_callback,
        std::unique_ptr<OveruseFrameDetector> overuse_detector) :
        shutdown_event_(true /* manual_reset */, false),
        number_of_cores_(number_of_cores),
        initial_rampup_(0),
        quality_scaling_experiment_enabled_(QualityScalingExperiment::Enabled()),
        source_proxy_(nullptr),
        sink_(nullptr),
        settings_(settings),
        video_sender_(Clock::GetRealTimeClock(), this),
        overuse_detector_(std::move(overuse_detector)),
        encoder_stats_observer_(encoder_stats_observer),
        pre_encode_callback_(pre_encode_callback),
        max_framerate_(-1),
        pending_encoder_reconfiguration_(false),
        pending_encoder_creation_(false),
        encoder_start_bitrate_bps_(0),
        max_data_payload_length_(0),
        last_observed_bitrate_bps_(0),
        encoder_paused_and_dropped_frame_(false),
        clock_(Clock::GetRealTimeClock()),
        degradation_preference_(DegradationPreference::DISABLED),
        posted_frames_waiting_for_encode_(0),
        last_captured_timestamp_(0),
        delta_ntp_internal_ms_(clock_->CurrentNtpInMilliseconds() - clock_->TimeInMilliseconds()),
        last_frame_log_ms_(clock_->TimeInMilliseconds()),
        captured_frame_count_(0),
        dropped_frame_count_(0),
        bitrate_observer_(nullptr),
        encoder_queue_("EncoderQueue") {_void_;}
    VideoStreamEncoder::~VideoStreamEncoder() {_void_;}
    void VideoStreamEncoder::SetSource(rtc::VideoSourceInterface<VideoFrame>*, const DegradationPreference&) {_void_;}
    void VideoStreamEncoder::SetSink(VideoStreamEncoderInterface::EncoderSink*, bool) {_void_;}
    void VideoStreamEncoder::SetStartBitrate(int) {_void_;}
    void VideoStreamEncoder::SetBitrateAllocationObserver(VideoBitrateAllocationObserver*) {_void_;}
    void VideoStreamEncoder::ConfigureEncoder(VideoEncoderConfig, unsigned __int64) {_void_;}
    void VideoStreamEncoder::Stop() {_void_;}
    void VideoStreamEncoder::SendKeyFrame() {_void_;}
    void VideoStreamEncoder::OnBitrateUpdated(unsigned int, unsigned char, __int64) {_void_;}
    void VideoStreamEncoder::AdaptUp(AdaptationObserverInterface::AdaptReason) {_void_;}
    void VideoStreamEncoder::AdaptDown(AdaptationObserverInterface::AdaptReason) {_void_;}
    void VideoStreamEncoder::OnFrame(const VideoFrame&) {_void_;}
    void VideoStreamEncoder::OnDiscardedFrame() {_void_;}
    EncodedImageCallback::Result VideoStreamEncoder::OnEncodedImage(const EncodedImage&, const CodecSpecificInfo*, const RTPFragmentationHeader*)
    {_void_; return EncodedImageCallback::Result(EncodedImageCallback::Result::Error::OK);}
    void VideoStreamEncoder::OnDroppedFrame(EncodedImageCallback::DropReason) {_void_;}
    VideoStreamEncoder::AdaptCounter::~AdaptCounter() {_void_;}

    std::unique_ptr<VP8Encoder, std::default_delete<VP8Encoder> > VP8Encoder::Create()
    {_ptr_;}

    std::unique_ptr<VP8Decoder, std::default_delete<VP8Decoder> > VP8Decoder::Create()
    {_ptr_;}

    SimulcastEncoderAdapter::SimulcastEncoderAdapter(VideoEncoderFactory* factory, const SdpVideoFormat& format) :
        inited_(0),
        factory_(factory),
        video_format_(format),
        encoded_complete_callback_(nullptr),
        implementation_name_("SimulcastEncoderAdapter") {_void_;}
    SimulcastEncoderAdapter::~SimulcastEncoderAdapter() {_void_;}
    int SimulcastEncoderAdapter::Release() {_int_;}
    int SimulcastEncoderAdapter::InitEncode(const VideoCodec*, int, unsigned __int64) {_int_;}
    int SimulcastEncoderAdapter::Encode(const VideoFrame&, const CodecSpecificInfo*, const std::vector<FrameType, std::allocator<FrameType> >*) {_int_;}
    int SimulcastEncoderAdapter::RegisterEncodeCompleteCallback(EncodedImageCallback*) {_int_;}
    int SimulcastEncoderAdapter::SetChannelParameters(unsigned int, __int64) {_int_;}
    int SimulcastEncoderAdapter::SetRateAllocation(const VideoBitrateAllocation&, unsigned int) {_int_;}
    VideoEncoder::ScalingSettings SimulcastEncoderAdapter::GetScalingSettings() const
    {_void_; return VideoEncoder::ScalingSettings(VideoEncoder::ScalingSettings::kOff);}
    bool SimulcastEncoderAdapter::SupportsNativeHandle() const {_bool_;}
    const char* SimulcastEncoderAdapter::ImplementationName() const {_str_;}

    namespace testing
    {
        namespace bwe
        {
            Packet::Packet() {_void_;}
            Packet::Packet(int, __int64, unsigned __int64) {_void_;}
            Packet::~Packet() {_void_;}
            bool Packet::operator<(const Packet&) const {_bool_;}
            int Packet::flow_id() const  {_int_;}
            void Packet::set_send_time_us(__int64) {_void_;}
            __int64 Packet::send_time_us() const {_int_;}
            __int64 Packet::sender_timestamp_us() const {_int_;}
            unsigned __int64 Packet::payload_size() const {_int_;}
            void Packet::set_sender_timestamp_us(__int64) {_void_;}
            __int64 Packet::creation_time_ms() const {_int_;}
            __int64 Packet::sender_timestamp_ms() const {_int_;}
            __int64 Packet::send_time_ms() const {_int_;}
            Packet::Type MediaPacket::GetPacketType() const
            {_void_; return Packet::Type();}
            Packet::Type FeedbackPacket::GetPacketType() const
            {_void_; return Packet::Type();}

            BbrBweFeedback::~BbrBweFeedback() {_void_;}
            SendSideBweFeedback::~SendSideBweFeedback() {_void_;}
        }
    }
}

namespace rtc
{
    namespace test
    {
        TaskQueueForTest::TaskQueueForTest(const char* queue_name, TaskQueue::Priority priority) :
            TaskQueue(queue_name, priority) {_void_;}
        TaskQueueForTest::~TaskQueueForTest() {_void_;}
    }
}

#endif
