#include <boss.h>
#if BOSS_NEED_ADDON_WEBRTC

#include "boss_integration_webrtc-jumpingyang001-20210616.h"

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

#define CURSOR_SUPPRESSED 0x00000002
#define PW_RENDERFULLCONTENT 2

////////////////////////////////////////////////////////////////////////////////
// rtc_base
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/async_invoker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/async_packet_socket.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/async_resolver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/async_resolver_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/async_socket.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/async_tcp_socket.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/async_udp_socket.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/bit_buffer.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/boringssl_certificate.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/boringssl_identity.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/buffer_queue.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/byte_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/callback_list.cc>
#define fprintf(TYPE, FORMAT, ...) BOSS_ASSERT(BOSS::String::Format(FORMAT, __VA_ARGS__), false)
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/checks.cc>
#undef fprintf
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/copy_on_write_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/cpu_time.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/crc32.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/crypt_string.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/data_rate_limiter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/event.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/event_tracer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/fake_clock.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/fake_ssl_identity.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/file_rotating_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/firewall_socket_server.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/helpers.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/http_common.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/ifaddrs_android.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/ifaddrs_converter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/ip_address.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/location.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/logging.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/log_sinks.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/mac_ifaddrs_converter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/memory_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/memory_usage.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/message_digest.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/message_handler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/nat_server.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/nat_socket_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/nat_types.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/net_helper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/net_helpers.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/network.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/network_constants.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/network_monitor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/network_monitor_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/network_route.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/null_socket_server.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/openssl_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/openssl_certificate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/openssl_digest.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/openssl_identity.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/openssl_key_pair.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/openssl_session_cache.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/openssl_stream_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/openssl_utility.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/operations_chain.cc>
#define PACKET_MAXIMUMS PACKET_MAXIMUMS_physicalsocketserver_BOSS
#define IP_HEADER_SIZE IP_HEADER_SIZE_physicalsocketserver_BOSS
#define ICMP_HEADER_SIZE ICMP_HEADER_SIZE_physicalsocketserver_BOSS
#define ICMP_PING_TIMEOUT_MILLIS ICMP_PING_TIMEOUT_MILLIS_physicalsocketserver_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/physical_socket_server.cc>
#undef PACKET_MAXIMUMS
#undef IP_HEADER_SIZE
#undef ICMP_HEADER_SIZE
#undef ICMP_PING_TIMEOUT_MILLIS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/platform_thread.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/platform_thread_types.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/proxy_info.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/proxy_server.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/race_checker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/random.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/rate_limiter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/rate_statistics.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/rate_tracker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/rtc_certificate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/rtc_certificate_generator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/server_socket_adapters.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/socket.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/socket_adapters.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/socket_address.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/socket_address_pair.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/socket_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/ssl_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/ssl_certificate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/ssl_fingerprint.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/ssl_identity.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/ssl_stream_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/string_encode.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/string_to_number.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/string_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/system_time.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/task_queue.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/task_queue_gcd.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/task_queue_libevent.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/task_queue_stdlib.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/task_queue_win.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/test_client.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/test_echo_server.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/test_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/thread.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/time_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/timestamp_aligner.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/unique_id_generator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/virtual_socket_server.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/weak_ptr.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/win32.cc>
#define PACKET_MAXIMUMS PACKET_MAXIMUMS_win32socketserver_BOSS
#define IP_HEADER_SIZE IP_HEADER_SIZE_win32socketserver_BOSS
#define ICMP_HEADER_SIZE ICMP_HEADER_SIZE_win32socketserver_BOSS
#define ICMP_PING_TIMEOUT_MILLIS ICMP_PING_TIMEOUT_MILLIS_win32socketserver_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/win32_socket_server.cc>
#undef PACKET_MAXIMUMS
#undef IP_HEADER_SIZE
#undef ICMP_HEADER_SIZE
#undef ICMP_PING_TIMEOUT_MILLIS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/win32_window.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/zero_memory.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/deprecated/recursive_critical_section.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/alr_experiment.cc>
#define kFieldTrial kFieldTrial_balanced_degradation_settings_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/balanced_degradation_settings.cc>
#undef kFieldTrial
#define kFieldTrial kFieldTrial_cpu_speed_experiment_BOSS
#define kMinSetting kMinSetting_cpu_speed_experiment_BOSS
#define kMaxSetting kMaxSetting_cpu_speed_experiment_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/cpu_speed_experiment.cc>
#undef kFieldTrial
#undef kMinSetting
#undef kMaxSetting
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/encoder_info_settings.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/field_trial_list.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/field_trial_parser.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/field_trial_units.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/jitter_upper_bound_experiment.cc>
#define kFieldTrialName kFieldTrialName_keyframe_interval_settings_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/keyframe_interval_settings.cc>
#undef kFieldTrialName
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/min_video_bitrate_experiment.cc>
#define kFieldTrial kFieldTrial_normalize_simulcast_size_experiment_BOSS
#define kMinSetting kMinSetting_normalize_simulcast_size_experiment_BOSS
#define kMaxSetting kMaxSetting_normalize_simulcast_size_experiment_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/normalize_simulcast_size_experiment.cc>
#undef kFieldTrial
#undef kMinSetting
#undef kMaxSetting
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/quality_rampup_experiment.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/quality_scaler_settings.cc>
#define kFieldTrial kFieldTrial_quality_scaling_experiment_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/quality_scaling_experiment.cc>
#undef kFieldTrial
#define IsEnabled IsEnabled_rate_control_settings_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/rate_control_settings.cc>
#undef IsEnabled
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/rtt_mult_experiment.cc>
#define kFieldTrialName kFieldTrialName_stable_target_rate_experiment_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/stable_target_rate_experiment.cc>
#undef kFieldTrialName
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/experiments/struct_parameters_parser.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/internal/default_socket_server.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/memory/aligned_malloc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/memory/fifo_buffer.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/network/sent_packet.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/numerics/event_based_exponential_moving_average.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/numerics/event_rate_counter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/numerics/exp_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/numerics/histogram_percentile_counter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/numerics/moving_average.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/numerics/sample_counter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/numerics/sample_stats.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/strings/audio_format_to_string.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/strings/json.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/strings/string_builder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/strings/string_format.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/synchronization/mutex.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/synchronization/sequence_checker_internal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/synchronization/yield.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/synchronization/yield_policy.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/system/file_wrapper.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/system/thread_registry.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/system/warn_current_thread_is_deadlocked.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/task_utils/pending_task_safety_flag.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/task_utils/repeating_task.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/third_party/base64/base64.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/third_party/sigslot/sigslot.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/time/timestamp_extrapolator.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/win/create_direct3d_device.cc>
#define LoadComBaseFunction LoadComBaseFunction_get_activation_factory_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/win/get_activation_factory.cc>
#undef LoadComBaseFunction
#define LoadComBaseFunction LoadComBaseFunction_hstring_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/win/hstring.cc>
#undef LoadComBaseFunction
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/win/scoped_com_initializer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/rtc_base/win/windows_version.cc>

////////////////////////////////////////////////////////////////////////////////
// api
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_options.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/candidate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/create_peerconnection_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/data_channel_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/dtls_transport_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/ice_transport_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/jsep.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/jsep_ice_candidate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/media_stream_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/media_types.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/peer_connection_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtc_error.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtc_event_log_output_file.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtp_headers.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtp_packet_info.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtp_parameters.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtp_receiver_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtp_sender_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtp_transceiver_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/sctp_transport_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/stats_types.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/adaptation/resource.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio/audio_frame.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio/channel_layout.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio/echo_canceller3_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio/echo_canceller3_config_json.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio/echo_canceller3_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio/echo_detector_creator.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/audio_codec_pair_id.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/audio_decoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/audio_encoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/audio_format.cc>
#define NotAdvertised NotAdvertised_builtin_audio_decoder_factory_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/builtin_audio_decoder_factory.cc>
#undef NotAdvertised
#define NotAdvertised NotAdvertised_builtin_audio_encoder_factory_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/builtin_audio_encoder_factory.cc>
#undef NotAdvertised
#define NotAdvertised NotAdvertised_opus_audio_decoder_factory_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/opus_audio_decoder_factory.cc>
#undef NotAdvertised
#define NotAdvertised NotAdvertised_opus_audio_encoder_factory_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/opus_audio_encoder_factory.cc>
#undef NotAdvertised

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/g711/audio_decoder_g711.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/g711/audio_encoder_g711.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/g722/audio_decoder_g722.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/g722/audio_encoder_g722.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/ilbc/audio_decoder_ilbc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/ilbc/audio_encoder_ilbc.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/isac/audio_decoder_isac_fix.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/isac/audio_decoder_isac_float.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/isac/audio_encoder_isac_fix.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/isac/audio_encoder_isac_float.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/L16/audio_decoder_L16.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/L16/audio_encoder_L16.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/opus/audio_decoder_multi_channel_opus.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/opus/audio_decoder_opus.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/opus/audio_encoder_multi_channel_opus.cc>
#define kDefaultComplexity kDefaultComplexity_audio_encoder_multi_channel_opus_config_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/opus/audio_encoder_multi_channel_opus_config.cc>
#undef kDefaultComplexity
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/opus/audio_encoder_opus.cc>
#define kDefaultComplexity kDefaultComplexity_audio_encoder_opus_config_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/audio_codecs/opus/audio_encoder_opus_config.cc>
#undef kDefaultComplexity

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/call/transport.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/crypto/crypto_options.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/neteq/custom_neteq_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/neteq/default_neteq_controller_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/neteq/neteq.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/neteq/tick_timer.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/numerics/samples_stats_counter.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtc_event_log/rtc_event.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtc_event_log/rtc_event_log.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/rtc_event_log/rtc_event_log_factory.cc>

//#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/task_queue/default_task_queue_factory_gcd.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/task_queue/default_task_queue_factory_libevent.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/task_queue/default_task_queue_factory_stdlib.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/task_queue/default_task_queue_factory_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/task_queue/task_queue_base.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/transport/bitrate_settings.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/transport/field_trial_based_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/transport/goog_cc_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/transport/network_types.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/transport/stun.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/transport/rtp/dependency_descriptor.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/units/data_rate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/units/data_size.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/units/frequency.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/units/time_delta.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/units/timestamp.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/builtin_video_bitrate_allocator_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/color_space.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/encoded_frame.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/encoded_image.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/hdr_metadata.cc>
#define kBufferAlignment kBufferAlignment_i010_buffer_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/i010_buffer.cc>
#undef kBufferAlignment
#define kBufferAlignment kBufferAlignment_i420_buffer_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/i420_buffer.cc>
#undef kBufferAlignment
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/nv12_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_adaptation_counters.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_bitrate_allocation.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_bitrate_allocator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_content_type.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_frame.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_frame_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_frame_metadata.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_source_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_stream_decoder_create.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video/video_timing.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/builtin_video_decoder_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/builtin_video_encoder_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/h264_profile_level_id.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/sdp_video_format.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/spatial_layer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/video_codec.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/video_decoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/video_decoder_software_fallback_wrapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/video_encoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/video_encoder_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/video_encoder_software_fallback_wrapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/vp8_frame_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/vp8_temporal_layers.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/vp8_temporal_layers_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/video_codecs/vp9_profile.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/api/voip/voip_engine_factory.cc>

////////////////////////////////////////////////////////////////////////////////
// audio
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/audio_level.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/audio_receive_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/audio_send_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/audio_state.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/audio_transport_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/channel_receive.cc>
#define TransformableAudioFrame TransformableAudioFrame_channel_receive_frame_transformer_delegate_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/channel_receive_frame_transformer_delegate.cc>
#undef TransformableAudioFrame
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/channel_send.cc>
#define TransformableAudioFrame TransformableAudioFrame_channel_send_frame_transformer_delegate_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/channel_send_frame_transformer_delegate.cc>
#undef TransformableAudioFrame
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/null_audio_poller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/remix_resample.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/utility/audio_frame_operations.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/utility/channel_mixer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/utility/channel_mixing_matrix.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/voip/audio_channel.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/voip/audio_egress.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/voip/audio_ingress.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/audio/voip/voip_core.cc>

////////////////////////////////////////////////////////////////////////////////
// call
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/audio_receive_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/audio_send_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/audio_state.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/bitrate_allocator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/call.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/call_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/call_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/degraded_call.cc>
#define kDefaultProcessIntervalMs kDefaultProcessIntervalMs_fake_network_pipe_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/fake_network_pipe.cc>
#undef kDefaultProcessIntervalMs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/flexfec_receive_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/flexfec_receive_stream_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/receive_time_calculator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/rtp_bitrate_configurator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/rtp_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/rtp_demuxer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/rtp_payload_params.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/rtp_stream_receiver_controller.cc>
#define kRetransmitWindowSizeMs kRetransmitWindowSizeMs_rtp_transport_controller_send_BOSS
#define IsEnabled IsEnabled_rtp_transport_controller_send_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/rtp_transport_controller_send.cc>
#undef kRetransmitWindowSizeMs
#undef IsEnabled
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/rtp_video_sender.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/rtx_receive_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/simulated_network.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/syncable.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/version.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/video_receive_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/video_send_stream.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/adaptation_constraint.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/broadcast_resource_listener.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/degradation_preference_provider.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/encoder_settings.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/resource_adaptation_processor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/resource_adaptation_processor_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/video_source_restrictions.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/video_stream_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/video_stream_input_state.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/call/adaptation/video_stream_input_state_provider.cc>

////////////////////////////////////////////////////////////////////////////////
// common_audio
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/audio_converter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/audio_util.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/channel_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/fir_filter_avx2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/fir_filter_c.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/fir_filter_factory.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/fir_filter_neon.cc> 최적화시 다시 고려!
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/fir_filter_sse.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/real_fourier.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/real_fourier_ooura.cc>
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/ring_buffer.c>
}
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/smoothing_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/wav_file.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/wav_header.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/window_generator.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/resampler/push_resampler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/resampler/push_sinc_resampler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/resampler/resampler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/resampler/sinc_resampler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/resampler/sinc_resampler_avx2.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/resampler/sinc_resampler_neon.cc> 최적화시 다시 고려!
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/resampler/sinc_resampler_sse.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/resampler/sinusoidal_linear_chirp_source.cc>

extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/auto_corr_to_refl_coef.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/auto_correlation.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/complex_bit_reverse.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/complex_bit_reverse_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/complex_fft.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/complex_fft_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/copy_set_operations.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/cross_correlation.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/cross_correlation_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/cross_correlation_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/division_operations.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/dot_product_with_scale.cc>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/downsample_fast.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/downsample_fast_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/downsample_fast_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/energy.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/filter_ar.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/filter_ar_fast_q12.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/filter_ar_fast_q12_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/filter_ma_fast_q12.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/get_hanning_window.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/get_scaling_square.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/ilbc_specific_functions.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/levinson_durbin.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/lpc_to_refl_coef.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/min_max_operations.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/min_max_operations_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/min_max_operations_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/randomization_functions.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/real_fft.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/refl_coef_to_lpc.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/resample.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/resample_48khz.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/resample_by_2.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/resample_by_2_internal.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/resample_by_2_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/resample_fractional.c>
    #define InitFunctionPointers InitFunctionPointers_spl_init_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/spl_init.c>
    #undef InitFunctionPointers
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/spl_inl.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/spl_sqrt.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/splitting_filter.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/sqrt_of_one_minus_x_squared.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/vector_scaling_operations.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/signal_processing/vector_scaling_operations_mips.c> 최적화시 다시 고려!
}

#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/third_party/ooura/fft_size_128/ooura_fft.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/third_party/ooura/fft_size_128/ooura_fft_mips.cc> 최적화시 다시 고려!
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/third_party/ooura/fft_size_128/ooura_fft_neon.cc> 최적화시 다시 고려!
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/third_party/ooura/fft_size_128/ooura_fft_sse2.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/third_party/ooura/fft_size_256/fft4g.cc>

extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor_mips.c> 최적화시 다시 고려!
}

#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/vad/vad.cc>
extern "C"
{
    #define kInitCheck kInitCheck_vad_core_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/vad/vad_core.c>
    #undef kInitCheck
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/vad/vad_filterbank.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/vad/vad_gmm.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/vad/vad_sp.c>
    #define kInitCheck kInitCheck_webrtc_vad_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/common_audio/vad/webrtc_vad.c>
    #undef kInitCheck
}

////////////////////////////////////////////////////////////////////////////////
// common_video
////////////////////////////////////////////////////////////////////////////////

#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/bitrate_adjuster.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/frame_rate_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/incoming_video_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/video_frame_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/video_frame_buffer_pool.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/video_render_frames.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/generic_frame_descriptor/generic_frame_info.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/h264/h264_bitstream_parser.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/h264/h264_common.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/h264/pps_parser.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/h264/sps_parser.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/h264/sps_vui_rewriter.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/common_video/libyuv/webrtc_libyuv.cc>

////////////////////////////////////////////////////////////////////////////////
// logging
////////////////////////////////////////////////////////////////////////////////
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/fake_rtc_event_log.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/fake_rtc_event_log_factory.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/ice_logger.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/logged_events.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/rtc_event_log2rtp_dump.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/rtc_event_log_impl.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/rtc_event_log_parser.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/rtc_event_processor.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/rtc_stream_config.cc>

//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/encoder/blob_encoding.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/encoder/delta_encoding.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/encoder/rtc_event_log_encoder_common.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/encoder/rtc_event_log_encoder_legacy.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/encoder/rtc_event_log_encoder_new_format.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/encoder/var_int.cc>

//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_alr_state.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_audio_network_adaptation.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_audio_playout.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_audio_receive_stream_config.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_audio_send_stream_config.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_bwe_update_delay_based.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_bwe_update_loss_based.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_dtls_transport_state.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_dtls_writable_state.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_frame_decoded.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_generic_ack_received.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_generic_packet_received.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_generic_packet_sent.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_ice_candidate_pair.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_ice_candidate_pair_config.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_probe_cluster_created.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_probe_result_failure.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_probe_result_success.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_route_change.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_rtcp_packet_incoming.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_rtcp_packet_outgoing.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_rtp_packet_incoming.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_rtp_packet_outgoing.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_video_receive_stream_config.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/logging/rtc_event_log/events/rtc_event_video_send_stream_config.cc>

////////////////////////////////////////////////////////////////////////////////
// media
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/adapted_video_track_source.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/codec.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/fake_frame_source.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/fake_media_engine.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/fake_rtp.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/fake_video_renderer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/h264_profile_level_id.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/media_channel.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/media_constants.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/media_engine.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/rid_description.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/rtp_utils.cc>
#define kProfileLevelId kProfileLevelId_sdp_video_format_utils_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/sdp_video_format_utils.cc>
#undef kProfileLevelId
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/stream_params.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/turn_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/video_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/video_broadcaster.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/video_common.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/base/video_source_base.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/adm_helpers.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/encoder_simulcast_proxy.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/fake_video_codec_factory.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/fake_webrtc_call.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/fake_webrtc_video_engine.cc>
#define IsFormatSupported IsFormatSupported_internaldecoderfactory_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/internal_decoder_factory.cc>
#undef IsFormatSupported
#define IsFormatSupported IsFormatSupported_internalencoderfactory_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/internal_encoder_factory.cc>
#undef IsFormatSupported
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/multiplex_codec_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/payload_type_mapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/simulcast.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/simulcast_encoder_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/unhandled_packets_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/webrtc_media_engine.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/webrtc_media_engine_defaults.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/webrtc_video_engine.cc>
#define ValidateStreamParams ValidateStreamParams_webrtc_voice_engine_BOSS
#define MinPositive MinPositive_webrtc_voice_engine_BOSS
#define IsEnabled IsEnabled_webrtc_voice_engine_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/engine/webrtc_voice_engine.cc>
#undef ValidateStreamParams
#undef MinPositive
#undef IsEnabled

#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/sctp/dcsctp_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/sctp/sctp_transport_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/media/sctp/usrsctp_transport.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/async_audio_processing
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/async_audio_processing/async_audio_processing.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/audio_coding
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/acm2/acm_receiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/acm2/acm_remixing.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/acm2/acm_resampler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/acm2/audio_coding_module.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/acm2/call_statistics.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/audio_network_adaptor_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/audio_network_adaptor_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/bitrate_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/channel_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/controller_manager.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/debug_dump_writer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/dtx_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/event_log_writer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/fec_controller_plr_based.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/frame_length_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/audio_network_adaptor/frame_length_controller_v2.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/legacy_encoded_audio_frame.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/cng/audio_encoder_cng.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/cng/webrtc_cng.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/g711/audio_decoder_pcm.cc>
#define CreateConfig CreateConfig_audio_encoder_pcm_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/g711/audio_encoder_pcm.cc>
#undef CreateConfig
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/g711/g711_interface.c>
}

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/g722/audio_decoder_g722.cc>
#define CreateConfig CreateConfig_audio_encoder_g722_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/g722/audio_encoder_g722.cc>
#undef CreateConfig
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/g722/g722_interface.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/abs_quant.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/abs_quant_loop.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/audio_decoder_ilbc.cc>
    #define kSampleRateHz kSampleRateHz_audio_encoder_ilbc_BOSS
    #define GetIlbcBitrate GetIlbcBitrate_audio_encoder_ilbc_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/audio_encoder_ilbc.cc>
    #undef kSampleRateHz
    #undef GetIlbcBitrate
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/augmented_cb_corr.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/bw_expand.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/cb_construct.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/cb_mem_energy.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/cb_mem_energy_augmentation.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/cb_mem_energy_calc.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/cb_search.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/cb_search_core.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/cb_update_best_index.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/chebyshev.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/comp_corr.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/constants.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/create_augmented_vec.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/decode.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/decode_residual.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/decoder_interpolate_lsf.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/do_plc.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/encode.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/energy_inverse.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/enh_upsample.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/enhancer.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/enhancer_interface.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/filtered_cb_vecs.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/frame_classify.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/gain_dequant.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/gain_quant.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/get_cd_vec.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/get_lsp_poly.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/get_sync_seq.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/hp_input.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/hp_output.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/ilbc.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/index_conv_dec.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/index_conv_enc.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/init_decode.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/init_encode.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/interpolate.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/interpolate_samples.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/lpc_encode.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/lsf_check.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/lsf_interpolate_to_poly_dec.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/lsf_interpolate_to_poly_enc.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/lsf_to_lsp.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/lsf_to_poly.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/lsp_to_lsf.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/my_corr.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/nearest_neighbor.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/pack_bits.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/poly_to_lsf.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/poly_to_lsp.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/refiner.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/simple_interpolate_lsf.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/simple_lpc_analysis.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/simple_lsf_dequant.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/simple_lsf_quant.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/smooth.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/smooth_out_data.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/sort_sq.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/split_vq.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/state_construct.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/state_search.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/swap_bytes.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/unpack_bits.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/vq3.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/vq4.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/window32_w32.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/ilbc/xcorr_coef.c>
}
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/empty.cc>

#define PreFiltBankstr PreFiltBankstr_fix_source_BOSS
#define PostFiltBankstr PostFiltBankstr_fix_source_BOSS
#define PitchFiltstr PitchFiltstr_fix_source_BOSS
#define PitchAnalysisStruct PitchAnalysisStruct_fix_source_BOSS
#define BwEstimatorstr BwEstimatorstr_fix_source_BOSS
#define RateModel RateModel_fix_source_BOSS
#define IsacSaveEncoderData IsacSaveEncoderData_fix_source_BOSS
#define transcode_obj transcode_obj_fix_source_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/audio_decoder_isacfix.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/audio_encoder_isacfix.cc>
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/arith_routines.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/arith_routines_hist.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/arith_routines_logist.c>
    #define clamp clamp_fix_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/bandwidth_estimator.c>
    #undef clamp
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/decode.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/decode_bwe.c>
    #define exp2_Q10_T exp2_Q10_T_decode_plc_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/decode_plc.c>
    #undef exp2_Q10_T
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/encode.c>
    #define CalcLrIntQ CalcLrIntQ_entropy_coding_BOSS
    #define kBwCdf kBwCdf_fix_BOSS
    #define kBwCdfPtr kBwCdfPtr_fix_BOSS
    #define kBwInitIndex kBwInitIndex_fix_BOSS
    #define OverflowingAddS32S32ToS32 OverflowingAddS32S32ToS32_entropy_coding_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/entropy_coding.c>
    #undef CalcLrIntQ
    #undef kBwCdf
    #undef kBwCdfPtr
    #undef kBwInitIndex
    #undef OverflowingAddS32S32ToS32
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/entropy_coding_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/entropy_coding_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/fft.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/filterbank_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/filterbanks.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/filterbanks_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/filterbanks_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/filters.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/filters_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/filters_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/initialize.c>
    #define InitFunctionPointers InitFunctionPointers_isacfix_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/isacfix.c>
    #undef InitFunctionPointers
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/lattice.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/lattice_c.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/lattice_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/lattice_neon.c> 최적화시 다시 고려!
    #define exp2_Q10_T exp2_Q10_T_lpc_masking_model_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/lpc_masking_model.c>
    #undef exp2_Q10_T
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/lpc_masking_model_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/lpc_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_estimator.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_estimator_c.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_estimator_mips.c> 최적화시 다시 고려!
    #define CalcLrIntQ CalcLrIntQ_pitch_filter_BOSS
    #define kIntrpCoef kIntrpCoef_fix_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_filter.c>
    #undef CalcLrIntQ
    #undef kIntrpCoef
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_filter_c.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_filter_mips.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_gain_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/pitch_lag_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/spectrum_ar_model_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/transform.c>
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/transform_mips.c> 최적화시 다시 고려!
    //#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/transform_neon.c> 최적화시 다시 고려!
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/fix/source/transform_tables.c>
}
#undef PreFiltBankstr
#undef PostFiltBankstr
#undef PitchFiltstr
#undef PitchAnalysisStruct
#undef BwEstimatorstr
#undef RateModel
#undef IsacSaveEncoderData
#undef transcode_obj

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/audio_decoder_isac.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/audio_encoder_isac.cc>
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/arith_routines.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/arith_routines_hist.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/arith_routines_logist.c>
    #define Thld_20_30 Thld_20_30_main_BOSS
    #define Thld_30_20 Thld_30_20_main_BOSS
    #define Thld_30_60 Thld_30_60_main_BOSS
    #define Thld_60_30 Thld_60_30_main_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/bandwidth_estimator.c>
    #undef Thld_20_30
    #undef Thld_30_20
    #undef Thld_30_60
    #undef Thld_60_30
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/crc.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/decode.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/decode_bwe.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/encode.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/encode_lpc_swb.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/entropy_coding.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/filter_functions.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/filterbanks.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/intialize.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/isac.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/isac_vad.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/lattice.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_analysis.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_gain_swb_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_shape_swb12_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_shape_swb16_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/lpc_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/pitch_estimator.c>
    #define kIntrpCoef kIntrpCoef_main_BOSS
    #define kDampFilter kDampFilter_pitch_filter_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/pitch_filter.c>
    #undef kIntrpCoef
    #undef kDampFilter
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/pitch_gain_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/pitch_lag_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/spectrum_ar_model_tables.c>
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/source/transform.c>
}

extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/isac/main/util/utility.c>
}

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/opus/audio_coder_opus_common.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/opus/audio_decoder_multi_channel_opus_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/opus/audio_decoder_opus.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/opus/audio_encoder_multi_channel_opus_impl.cc>
#define kOpusBitrateNbBps kOpusBitrateNbBps_audio_encoder_opus_BOSS
#define kOpusBitrateWbBps kOpusBitrateWbBps_audio_encoder_opus_BOSS
#define kOpusBitrateFbBps kOpusBitrateFbBps_audio_encoder_opus_BOSS
#define kDefaultMaxPlaybackRate kDefaultMaxPlaybackRate_audio_encoder_opus_BOSS
#define kOpusSupportedFrameLengths kOpusSupportedFrameLengths_audio_encoder_opus_BOSS
#define CalculateDefaultBitrate CalculateDefaultBitrate_audio_encoder_opus_BOSS
#define CalculateBitrate CalculateBitrate_audio_encoder_opus_BOSS
#define GetMaxPlaybackRate GetMaxPlaybackRate_audio_encoder_opus_BOSS
#define GetFrameSizeMs GetFrameSizeMs_audio_encoder_opus_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/opus/audio_encoder_opus.cc>
#undef kOpusBitrateNbBps
#undef kOpusBitrateWbBps
#undef kOpusBitrateFbBps
#undef kDefaultMaxPlaybackRate
#undef kOpusSupportedFrameLengths
#undef CalculateDefaultBitrate
#undef CalculateBitrate
#undef GetMaxPlaybackRate
#undef GetFrameSizeMs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/opus/opus_interface.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/pcm16b/audio_decoder_pcm16b.cc>
#define CreateConfig CreateConfig_audio_encoder_pcm16b_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/pcm16b/audio_encoder_pcm16b.cc>
#undef CreateConfig
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/pcm16b/pcm16b.c>
}
#define Pcm16BAppendSupportedCodecSpecs Pcm16BAppendSupportedCodecSpecs_pcm16b_common_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/pcm16b/pcm16b_common.cc>
#undef Pcm16BAppendSupportedCodecSpecs

#define kRedHeaderLength kRedHeaderLength_red_payload_splitter_BOSS
#define kRedLastHeaderLength kRedLastHeaderLength_red_payload_splitter_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/codecs/red/audio_encoder_copy_red.cc>
#undef kRedHeaderLength
#undef kRedLastHeaderLength

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/accelerate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/audio_multi_vector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/audio_vector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/background_noise.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/buffer_level_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/comfort_noise.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/cross_correlation.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/decision_logic.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/decoder_database.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/default_neteq_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/delay_manager.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/dsp_helper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/dtmf_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/dtmf_tone_generator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/expand.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/expand_uma_logger.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/histogram.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/merge.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/nack_tracker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/neteq_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/normal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/packet.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/packet_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/post_decode_vad.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/preemptive_expand.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/random_vector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/red_payload_splitter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/statistics_calculator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/sync_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/time_stretch.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/timestamp_scaler.cc>

//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/audio_loop.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/audio_sink.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/constant_pcm_packet_source.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/encode_neteq_input.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/fake_decode_from_file.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/initial_packet_inserter_neteq_input.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/input_audio_file.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/neteq_delay_analyzer.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/neteq_event_log_input.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/neteq_input.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/neteq_packet_source_input.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/neteq_replacement_input.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/neteq_rtpplay.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/neteq_stats_getter.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/neteq_stats_plotter.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/packet.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/packet_source.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/resample_input_audio_file.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/rtc_event_log_source.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/rtp_analyze.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/rtp_encode.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/rtp_file_source.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/rtp_generator.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/rtp_jitter.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_coding/neteq/tools/rtpcat.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/audio_device
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/audio_device_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/audio_device_data_observer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/audio_device_generic.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/audio_device_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/audio_device_name.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/fine_audio_buffer.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/dummy/audio_device_dummy.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/dummy/file_audio_device.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/dummy/file_audio_device_factory.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/win/audio_device_core_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/win/audio_device_module_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/win/core_audio_base_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/win/core_audio_input_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/win/core_audio_output_win.cc>
#define RoleToString RoleToString_core_audio_utility_win_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_device/win/core_audio_utility_win.cc>
#undef RoleToString

////////////////////////////////////////////////////////////////////////////////
// modules/audio_mixer
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_mixer/audio_frame_manipulator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_mixer/audio_mixer_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_mixer/default_output_rate_calculator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_mixer/frame_combiner.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_mixer/gain_change_calculator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_mixer/sine_wave_generator.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/audio_processing
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/audio_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/audio_processing_builder_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/audio_processing_impl.cc>
#define MapError MapError_echo_control_mobile_impl_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/echo_control_mobile_impl.cc>
#undef MapError
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/gain_control_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/gain_controller2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/high_pass_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/level_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/optionally_built_submodule_creators.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/residual_echo_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/rms_level.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/splitting_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/three_band_filter_bank.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/typing_detection.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/voice_detection.cc>

//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec_dump/aec_dump_impl.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec_dump/capture_stream_info.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec_dump/mock_aec_dump.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec_dump/null_aec_dump_factory.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec_dump/write_to_file_task.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/adaptive_fir_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/adaptive_fir_filter_avx2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/adaptive_fir_filter_erl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/adaptive_fir_filter_erl_avx2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/aec3_common.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/aec3_fft.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/aec_state.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/alignment_mixer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/api_call_jitter_metrics.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/block_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/block_delay_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/block_framer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/block_processor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/block_processor_metrics.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/clockdrift_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/coarse_filter_update_gain.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/comfort_noise_generator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/decimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/dominant_nearend_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/downsampled_render_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/echo_audibility.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/echo_canceller3.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/echo_path_delay_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/echo_path_variability.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/echo_remover.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/echo_remover_metrics.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/erl_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/erle_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/fft_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/fft_data_avx2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/filter_analyzer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/frame_blocker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/fullband_erle_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/matched_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/matched_filter_avx2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/matched_filter_lag_aggregator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/moving_average.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/refined_filter_update_gain.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/render_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/render_delay_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/render_delay_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/render_delay_controller_metrics.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/render_signal_analyzer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/residual_echo_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/reverb_decay_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/reverb_frequency_response.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/reverb_model.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/reverb_model_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/signal_dependent_erle_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/spectrum_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/stationarity_estimator.cc>
#define kX2BandEnergyThreshold kX2BandEnergyThreshold_subband_erle_estimator_BOSS
#define kBlocksToHoldErle kBlocksToHoldErle_subband_erle_estimator_BOSS
#define kPointsToAccumulate kPointsToAccumulate_subband_erle_estimator_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/subband_erle_estimator.cc>
#undef kX2BandEnergyThreshold
#undef kBlocksToHoldErle
#undef kPointsToAccumulate
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/subband_nearend_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/subtractor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/subtractor_output.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/subtractor_output_analyzer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/suppression_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/suppression_gain.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/transparent_mode.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aec3/vector_math_avx2.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aecm/aecm_core.cc>
#define ComfortNoise ComfortNoise_aecm_core_c_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aecm/aecm_core_c.cc>
#undef ComfortNoise
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aecm/aecm_core_mips.cc> 최적화시 다시 고려!
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aecm/aecm_core_neon.cc> 최적화시 다시 고려!
#define kInitCheck kInitCheck_echo_control_mobile_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/aecm/echo_control_mobile.cc>
#undef kInitCheck

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc/agc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc/agc_manager_direct.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc/clipping_predictor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc/clipping_predictor_level_buffer.cc>
#define kTransientWidthThreshold kTransientWidthThreshold_loudness_histogram_BOSS
#define kLowProbabilityThreshold kLowProbabilityThreshold_loudness_histogram_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc/loudness_histogram.cc>
#undef kTransientWidthThreshold
#undef kLowProbabilityThreshold
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc/utility.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc/legacy/analog_agc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc/legacy/digital_agc.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/adaptive_agc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/adaptive_digital_gain_applier.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/adaptive_mode_level_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/biquad_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/compute_interpolated_gain_curve.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/cpu_features.cc>
#define kChunkSizeMs kChunkSizeMs_down_sampler_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/down_sampler.cc>
#undef kChunkSizeMs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/fixed_digital_level_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/gain_applier.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/interpolated_gain_curve.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/limiter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/limiter_db_gain_curve.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/noise_level_estimator.cc>
#define kMinNoisePower kMinNoisePower_noise_spectrum_estimator_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/noise_spectrum_estimator.cc>
#undef kMinNoisePower
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/saturation_protector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/saturation_protector_buffer.cc>
#define IsSse2Available IsSse2Available_noise_spectrum_estimator_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/signal_classifier.cc>
#undef IsSse2Available
#define Vad Vad_vad_with_level_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/vad_with_level.cc>
#undef Vad
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/vector_float_frame.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/auto_correlation.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/features_extraction.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/lp_residual.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/pitch_search.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/pitch_search_internal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/rnn.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/rnn_fc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/rnn_gru.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/rnn_vad_tool.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/spectral_features.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/spectral_features_internal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/agc2/rnn_vad/vector_math_avx2.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/capture_levels_adjuster/audio_samples_scaler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/capture_levels_adjuster/capture_levels_adjuster.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/echo_detector/circular_buffer.cc>
#define kAlpha kAlpha_mean_variance_estimator_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/echo_detector/mean_variance_estimator.cc>
#undef kAlpha
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/echo_detector/moving_max.cc>
#define kAlpha kAlpha_normalized_covariance_estimator_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/echo_detector/normalized_covariance_estimator.cc>
#undef kAlpha

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/include/aec_dump.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/include/audio_frame_proxies.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/include/audio_processing.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/include/audio_processing_statistics.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/include/config.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/logging/apm_data_dumper.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/fast_math.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/histograms.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/noise_estimator.cc>
#define kMaxNumChannelsOnStack kMaxNumChannelsOnStack_noise_suppressor_BOSS
#define NumChannelsOnHeap NumChannelsOnHeap_noise_suppressor_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/noise_suppressor.cc>
#undef kMaxNumChannelsOnStack
#undef NumChannelsOnHeap
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/ns_fft.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/prior_signal_model.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/prior_signal_model_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/quantile_noise_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/signal_model.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/signal_model_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/speech_probability_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/suppression_params.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/ns/wiener_filter.cc>

//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/transient/click_annotate.cc> main이 있음
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/transient/file_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/transient/moving_moments.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/transient/transient_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/transient/transient_suppressor_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/transient/wpd_node.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/transient/wpd_tree.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/utility/cascaded_biquad_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/utility/delay_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/utility/delay_estimator_wrapper.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/utility/pffft_wrapper.cc> pffft서드파티가 필요

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/vad/gmm.cc>
#define kTransientWidthThreshold kTransientWidthThreshold_pitch_based_vad_BOSS
#define kLowProbabilityThreshold kLowProbabilityThreshold_pitch_based_vad_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/vad/pitch_based_vad.cc>
#undef kTransientWidthThreshold
#undef kLowProbabilityThreshold
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/vad/pitch_internal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/vad/pole_zero_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/vad/standalone_vad.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/vad/vad_audio_proc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/vad/vad_circular_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/audio_processing/vad/voice_activity_detector.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/congestion_controller
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/receive_side_congestion_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/remb_throttler.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/alr_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/bitrate_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/congestion_window_pushback_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/delay_based_bwe.cc>
#define CwndExperimentEnabled CwndExperimentEnabled_goog_cc_network_control_BOSS
#define ReadCwndExperimentParameter ReadCwndExperimentParameter_goog_cc_network_control_BOSS
#define kCwndExperiment kCwndExperiment_goog_cc_network_control_BOSS
#define kDefaultAcceptedQueueMs kDefaultAcceptedQueueMs_goog_cc_network_control_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/goog_cc_network_control.cc>
#undef CwndExperimentEnabled
#undef ReadCwndExperimentParameter
#undef kCwndExperiment
#undef kDefaultAcceptedQueueMs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/inter_arrival_delta.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/link_capacity_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/loss_based_bandwidth_estimation.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/probe_bitrate_estimator.cc>
#define kMinProbePacketsSent kMinProbePacketsSent_probe_controller_BOSS
#define kMinProbeDurationMs kMinProbeDurationMs_probe_controller_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/probe_controller.cc>
#undef kMinProbePacketsSent
#undef kMinProbeDurationMs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/robust_throughput_estimator.cc>
#define kMaxRtcpFeedbackInterval kMaxRtcpFeedbackInterval_send_side_bandwidth_estimation_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.cc>
#undef kMaxRtcpFeedbackInterval
#define kDeltaCounterMax kDeltaCounterMax_trendline_estimator_BOSS
#define kMaxAdaptOffsetMs kMaxAdaptOffsetMs_trendline_estimator_BOSS
#define kOverUsingTimeThreshold kOverUsingTimeThreshold_trendline_estimator_BOSS
#define kMinNumDeltas kMinNumDeltas_trendline_estimator_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/goog_cc/trendline_estimator.cc>
#undef kDeltaCounterMax
#undef kMaxAdaptOffsetMs
#undef kOverUsingTimeThreshold
#undef kMinNumDeltas

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/pcc/bitrate_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/pcc/monitor_interval.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/pcc/pcc_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/pcc/pcc_network_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/pcc/rtt_tracker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/pcc/utility_function.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/rtp/control_handler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/rtp/transport_feedback_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/congestion_controller/rtp/transport_feedback_demuxer.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/desktop_capture
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/blank_detector_desktop_capturer_wrapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/cropped_desktop_frame.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/cropping_window_capturer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/cropping_window_capturer_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_and_cursor_composer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_capture_metrics_helper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_capture_options.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_capturer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_capturer_differ_wrapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_capturer_wrapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_frame.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_frame_generator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_frame_rotation.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_frame_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_geometry.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/desktop_region.cc>
#define kBlockSize kBlockSize_differ_block_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/differ_block.cc>
#undef kBlockSize
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/differ_vector_sse2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/fake_desktop_capturer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/fallback_desktop_capturer_wrapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/full_screen_application_handler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/full_screen_window_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/mouse_cursor.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/mouse_cursor_monitor_linux.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/mouse_cursor_monitor_null.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/mouse_cursor_monitor_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/resolution_tracker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/rgba_color.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_capturer_helper.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_capturer_linux.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_capturer_null.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_capturer_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_drawer.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_drawer_linux.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_drawer_lock_posix.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_drawer_mac.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/screen_drawer_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/shared_desktop_frame.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/shared_memory.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/window_capturer_linux.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/window_capturer_null.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/window_capturer_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/window_finder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/window_finder_win.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/cursor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/d3d_device.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/desktop.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/desktop_capture_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/display_configuration_monitor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/dxgi_adapter_duplicator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/dxgi_context.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/dxgi_duplicator_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/dxgi_frame.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/dxgi_output_duplicator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/dxgi_texture.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/dxgi_texture_mapping.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/dxgi_texture_staging.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/full_screen_win_application_handler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/scoped_thread_desktop.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/screen_capture_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/screen_capturer_win_directx.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/screen_capturer_win_gdi.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/screen_capturer_win_magnifier.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/selected_window_context.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/wgc_capture_session.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/wgc_capture_source.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/wgc_capturer_win.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/wgc_desktop_frame.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/window_capture_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/desktop_capture/win/window_capturer_win_gdi.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/pacing
////////////////////////////////////////////////////////////////////////////////
#define kMinProbePacketsSent kMinProbePacketsSent_bitrate_prober_BOSS
#define kMinProbeDurationMs kMinProbeDurationMs_bitrate_prober_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/pacing/bitrate_prober.cc>
#undef kMinProbePacketsSent
#undef kMinProbeDurationMs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/pacing/interval_budget.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/pacing/paced_sender.cc>
#define IsEnabled IsEnabled_pacing_controller_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/pacing/pacing_controller.cc>
#undef IsEnabled
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/pacing/packet_router.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/pacing/round_robin_packet_queue.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/pacing/task_queue_paced_sender.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/remote_bitrate_estimator
////////////////////////////////////////////////////////////////////////////////
#define kDefaultRttMs kDefaultRttMs_aimd_rate_control_BOSS
#define IsEnabled IsEnabled_aimd_rate_control_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/aimd_rate_control.cc>
#undef kDefaultRttMs
#undef IsEnabled
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/bwe_defines.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/inter_arrival.cc>
#define kMaxAdaptOffsetMs kMaxAdaptOffsetMs_overuse_detector_BOSS
#define kOverUsingTimeThreshold kOverUsingTimeThreshold_overuse_detector_BOSS
#define kMinNumDeltas kMinNumDeltas_overuse_detector_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/overuse_detector.cc>
#undef kMaxAdaptOffsetMs
#undef kOverUsingTimeThreshold
#undef kMinNumDeltas
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/overuse_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/packet_arrival_map.cc>
#define kTimestampGroupLengthMs kTimestampGroupLengthMs_remote_bitrate_estimator_abs_send_time_BOSS
#define kTimestampToMs kTimestampToMs_remote_bitrate_estimator_abs_send_time_BOSS
#define kAbsSendTimeInterArrivalUpshift kAbsSendTimeInterArrivalUpshift_remote_bitrate_estimator_abs_send_time_BOSS
#define kInterArrivalShift kInterArrivalShift_remote_bitrate_estimator_abs_send_time_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/remote_bitrate_estimator_abs_send_time.cc>
#undef kTimestampGroupLengthMs
#undef kTimestampToMs
#undef kAbsSendTimeInterArrivalUpshift
#undef kInterArrivalShift
#define kTimestampGroupLengthMs kTimestampGroupLengthMs_remote_bitrate_estimator_single_stream_BOSS
#define kTimestampToMs kTimestampToMs_remote_bitrate_estimator_single_stream_BOSS
#define OptionalRateFromOptionalBps OptionalRateFromOptionalBps_remote_bitrate_estimator_single_stream_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/remote_bitrate_estimator_single_stream.cc>
#undef kTimestampGroupLengthMs
#undef kTimestampToMs
#undef OptionalRateFromOptionalBps
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/remote_estimator_proxy.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/tools/bwe_rtp.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/remote_bitrate_estimator/tools/rtp_to_text.cc> main이 있음

////////////////////////////////////////////////////////////////////////////////
// modules/rtp_rtcp
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/include/report_block_data.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/include/rtp_rtcp_defines.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/absolute_capture_time_interpolator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/absolute_capture_time_receiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/absolute_capture_time_sender.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/active_decode_targets_helper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/capture_clock_offset_updater.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/create_video_rtp_depacketizer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/dtmf_queue.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/fec_private_tables_bursty.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/fec_private_tables_random.cc>
#define kMaxMediaPackets kMaxMediaPackets_flexfec_header_reader_writer_BOSS
#define kMaxFecPackets kMaxFecPackets_flexfec_header_reader_writer_BOSS
#define kPacketMaskOffset kPacketMaskOffset_flexfec_header_reader_writer_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/flexfec_header_reader_writer.cc>
#undef kMaxMediaPackets
#undef kMaxFecPackets
#undef kPacketMaskOffset
#define kPacketLogIntervalMs kPacketLogIntervalMs_flexfec_receiver_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/flexfec_receiver.cc>
#undef kPacketLogIntervalMs
#define kPacketLogIntervalMs kPacketLogIntervalMs_flexfec_sender_BOSS
#define kMaxInitRtpSeqNumber kMaxInitRtpSeqNumber_flexfec_sender_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/flexfec_sender.cc>
#undef kPacketLogIntervalMs
#undef kMaxInitRtpSeqNumber
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/forward_error_correction.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/forward_error_correction_internal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/packet_loss_stats.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/packet_sequencer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/receive_statistics_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/remote_ntp_time_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_nack_stats.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet.cc>
#define kDefaultVideoReportInterval kDefaultVideoReportInterval_rtcp_receiver_BOSS
#define kDefaultAudioReportInterval kDefaultAudioReportInterval_rtcp_receiver_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_receiver.cc>
#undef kDefaultVideoReportInterval
#undef kDefaultAudioReportInterval
#define kDefaultVideoReportInterval kDefaultVideoReportInterval_rtcp_sender_BOSS
#define kDefaultAudioReportInterval kDefaultAudioReportInterval_rtcp_sender_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_sender.cc>
#undef kDefaultVideoReportInterval
#undef kDefaultAudioReportInterval
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_transceiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_transceiver_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_transceiver_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_dependency_descriptor_reader.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_dependency_descriptor_writer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_descriptor_authentication.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_format.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_format_h264.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_format_video_generic.cc>
#define kSBit kSBit_rtp_format_vp8_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_format_vp8.cc>
#undef kSBit
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_format_vp9.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_generic_frame_descriptor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_header_extension_map.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_header_extension_size.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_header_extensions.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_packet.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_packet_history.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_packet_received.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_packet_to_send.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_packetizer_av1.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_rtcp_impl.cc>
#define kRtpRtcpMaxIdleTimeProcessMs kRtpRtcpMaxIdleTimeProcessMs_rtp_rtcp_impl2_BOSS
#define kDefaultExpectedRetransmissionTimeMs kDefaultExpectedRetransmissionTimeMs_rtp_rtcp_impl2_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_rtcp_impl2.cc>
#undef kRtpRtcpMaxIdleTimeProcessMs
#undef kDefaultExpectedRetransmissionTimeMs
#define kMaxInitRtpSeqNumber kMaxInitRtpSeqNumber_rtp_sender_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_sender.cc>
#undef kMaxInitRtpSeqNumber
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_sender_audio.cc>
#define kTimestampTicksPerMs kTimestampTicksPerMs_rtp_sender_egress_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_sender_egress.cc>
#undef kTimestampTicksPerMs
#define kRedForFecHeaderLength kRedForFecHeaderLength_rtp_sender_video_BOSS
#define kIncludeCaptureClockOffset kIncludeCaptureClockOffset_rtp_sender_video_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_sender_video.cc>
#undef kRedForFecHeaderLength
#undef kIncludeCaptureClockOffset
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_sender_video_frame_transformer_delegate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_sequence_number_map.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_utility.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_video_header.cc>
#define WriteLeb128 WriteLeb128_rtp_video_layers_allocation_extension_BOSS
#define Leb128Size Leb128Size_rtp_video_layers_allocation_extension_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtp_video_layers_allocation_extension.cc>
#undef WriteLeb128
#undef Leb128Size
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/source_tracker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/time_util.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/tmmbr_help.cc>
#define kMaxInitRtpSeqNumber kMaxInitRtpSeqNumber_ulpfec_generator_BOSS
#define kRedForFecHeaderLength kRedForFecHeaderLength_ulpfec_generator_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/ulpfec_generator.cc>
#undef kMaxInitRtpSeqNumber
#undef kRedForFecHeaderLength
#define kMaxMediaPackets kMaxMediaPackets_ulpfec_header_reader_writer_BOSS
#define kMaxFecPackets kMaxFecPackets_ulpfec_header_reader_writer_BOSS
#define kPacketMaskOffset kPacketMaskOffset_ulpfec_header_reader_writer_BOSS
#define kMaxTrackedMediaPackets kMaxTrackedMediaPackets_ulpfec_header_reader_writer_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/ulpfec_header_reader_writer.cc>
#undef kMaxMediaPackets
#undef kMaxFecPackets
#undef kPacketMaskOffset
#undef kMaxTrackedMediaPackets
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/ulpfec_receiver_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/video_rtp_depacketizer.cc>
#define kObuSizePresentBit kObuSizePresentBit_video_rtp_depacketizer_av1_BOSS
#define ObuHasExtension ObuHasExtension_video_rtp_depacketizer_av1_BOSS
#define ObuHasSize ObuHasSize_video_rtp_depacketizer_av1_BOSS
#define WriteLeb128 WriteLeb128_video_rtp_depacketizer_av1_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/video_rtp_depacketizer_av1.cc>
#undef kObuSizePresentBit
#undef ObuHasExtension
#undef ObuHasSize
#undef WriteLeb128
#define kGenericHeaderLength kGenericHeaderLength_video_rtp_depacketizer_generic_BOSS
#define kExtendedHeaderLength kExtendedHeaderLength_video_rtp_depacketizer_generic_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/video_rtp_depacketizer_generic.cc>
#undef kGenericHeaderLength
#undef kExtendedHeaderLength
#define kNalHeaderSize kNalHeaderSize_video_rtp_depacketizer_h264_BOSS
#define kFuAHeaderSize kFuAHeaderSize_video_rtp_depacketizer_h264_BOSS
#define kLengthFieldSize kLengthFieldSize_video_rtp_depacketizer_h264_BOSS
#define NalDefs NalDefs_video_rtp_depacketizer_h264_BOSS
#define FuDefs FuDefs_video_rtp_depacketizer_h264_BOSS
#define kFBit kFBit_video_rtp_depacketizer_h264_BOSS
#define kNriMask kNriMask_video_rtp_depacketizer_h264_BOSS
#define kTypeMask kTypeMask_video_rtp_depacketizer_h264_BOSS
#define kSBit kSBit_video_rtp_depacketizer_h264_BOSS
#define kEBit kEBit_video_rtp_depacketizer_h264_BOSS
#define kRBit kRBit_video_rtp_depacketizer_h264_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/video_rtp_depacketizer_h264.cc>
#undef kNalHeaderSize
#undef kFuAHeaderSize
#undef kLengthFieldSize
#undef NalDefs
#undef FuDefs
#undef kFBit
#undef kNriMask
#undef kTypeMask
#undef kSBit
#undef kEBit
#undef kRBit
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/video_rtp_depacketizer_raw.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/video_rtp_depacketizer_vp8.cc>
#define kFailedToParse kFailedToParse_video_rtp_depacketizer_vp9_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/video_rtp_depacketizer_vp9.cc>
#undef kFailedToParse

#define kTimestampTicksPerMs kTimestampTicksPerMs_deprecated_rtp_sender_egress_BOSS
#define kSendSideDelayWindowMs kSendSideDelayWindowMs_deprecated_rtp_sender_egress_BOSS
#define kBitrateStatisticsWindowMs kBitrateStatisticsWindowMs_deprecated_rtp_sender_egress_BOSS
#define kRtpSequenceNumberMapMaxEntries kRtpSequenceNumberMapMaxEntries_deprecated_rtp_sender_egress_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/deprecated/deprecated_rtp_sender_egress.cc>
#undef kTimestampTicksPerMs
#undef kSendSideDelayWindowMs
#undef kBitrateStatisticsWindowMs
#undef kRtpSequenceNumberMapMaxEntries

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/app.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/bye.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/common_header.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/compound_packet.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/dlrr.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/extended_jitter_report.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/extended_reports.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/fir.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/loss_notification.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/nack.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/pli.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/psfb.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/rapid_resync_request.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/receiver_report.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/remb.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/remote_estimate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/report_block.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/rrtr.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/rtpfb.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/sdes.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/sender_report.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/target_bitrate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/tmmb_item.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/tmmbn.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/tmmbr.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/rtp_rtcp/source/rtcp_packet/transport_feedback.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/third_party
////////////////////////////////////////////////////////////////////////////////
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/third_party/fft/fft.c>
}
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/third_party/g711/g711.c>
}
extern "C"
{
    #define saturate saturate_g722_decode_BOSS
    #define block4 block4_g722_decode_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/third_party/g722/g722_decode.c>
    #undef saturate
    #undef block4

    #define saturate saturate_g722_encode_BOSS
    #define block4 block4_g722_encode_BOSS
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/third_party/g722/g722_encode.c>
    #undef saturate
    #undef block4
}
extern "C"
{
    #include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/third_party/portaudio/pa_ringbuffer.c>
}

////////////////////////////////////////////////////////////////////////////////
// modules/utility
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/utility/source/process_thread_impl.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/video_capture
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/device_info_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/video_capture_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/video_capture_impl.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/windows/device_info_ds.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/windows/help_functions_ds.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/windows/sink_filter_ds.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/windows/video_capture_ds.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_capture/windows/video_capture_factory_windows.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/video_coding
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/chain_diff_calculator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codec_timer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/decoder_database.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/decoding_state.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/encoded_frame.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/event_wrapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/fec_controller_default.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/frame_buffer.cc>
#undef abs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/frame_buffer2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/frame_dependencies_calculator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/frame_object.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/generic_decoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/h264_sprop_parameter_sets.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/h264_sps_pps_tracker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/histogram.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/inter_frame_delay.cc>
#define kDefaultRtt kDefaultRtt_jitter_buffer_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/jitter_buffer.cc>
#undef kDefaultRtt
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/jitter_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/loss_notification_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/media_opt_util.cc>
#define kDefaultRttMs kDefaultRttMs_nack_module_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/nack_module2.cc>
#undef kDefaultRttMs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/packet.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/packet_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/receiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/rtp_frame_id_only_ref_finder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/rtp_frame_reference_finder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/rtp_generic_ref_finder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/rtp_seq_num_only_ref_finder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/rtp_vp8_ref_finder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/rtp_vp9_ref_finder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/rtt_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/session_info.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/timestamp_map.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/timing.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/unique_timestamp_counter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/video_codec_initializer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/video_coding_defines.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/video_coding_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/video_receiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/video_receiver2.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/av1/av1_svc_config.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/av1/libaom_av1_decoder.cc> // libaom필요!
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/av1/libaom_av1_decoder_absent.cc>
//#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/av1/libaom_av1_encoder.cc> // libaom필요!
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/av1/libaom_av1_encoder_absent.cc>

/*#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/h264/h264.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/h264/h264_color_space.cc> // ffmpeg/libavcodec필요!
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/h264/h264_decoder_impl.cc> // ffmpeg/libavcodec필요!
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/h264/h264_encoder_impl.cc> // openh264필요!

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/interface/libvpx_interface.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/multiplex/augmented_video_frame_buffer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/multiplex/multiplex_decoder_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/multiplex/multiplex_encoded_image_packer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/multiplex/multiplex_encoder_adapter.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp8/default_temporal_layers.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp8/libvpx_vp8_decoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp8/libvpx_vp8_encoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp8/screenshare_layers.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp8/temporal_layers_checker.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp9/libvpx_vp9_decoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp9/libvpx_vp9_encoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp9/svc_config.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp9/vp9.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/codecs/vp9/vp9_frame_buffer_pool.cc> // vpx필요!

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/deprecated/nack_module.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/svc/create_scalability_structure.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/svc/scalability_structure_full_svc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/svc/scalability_structure_key_svc.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/svc/scalability_structure_l2t2_key_shift.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/svc/scalability_structure_simulcast.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/svc/scalable_video_controller_no_layering.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/svc/svc_rate_allocator.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/decoded_frames_history.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/frame_dropper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/framerate_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/ivf_file_reader.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/ivf_file_writer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/qp_parser.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/quality_scaler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/simulcast_rate_allocator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/simulcast_test_fixture_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/simulcast_utility.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/vp8_header_parser.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_coding/utility/vp9_uncompressed_header_parser.cc>

////////////////////////////////////////////////////////////////////////////////
// modules/video_processing
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_processing/video_denoiser.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_processing/util/denoiser_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_processing/util/denoiser_filter_c.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_processing/util/denoiser_filter_neon.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_processing/util/denoiser_filter_sse2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_processing/util/noise_estimation.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/modules/video_processing/util/skin_detection.cc>

////////////////////////////////////////////////////////////////////////////////
// net
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/fuzzers/dcsctp_fuzzers.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk_validators.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/crc32c.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/sctp_packet.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/tlv_trait.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/abort_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/cookie_ack_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/cookie_echo_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/data_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/error_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/forward_tsn_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/heartbeat_ack_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/heartbeat_request_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/idata_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/iforward_tsn_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/init_ack_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/init_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/reconfig_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/sack_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/shutdown_ack_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/shutdown_chunk.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/chunk/shutdown_complete_chunk.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/cookie_received_while_shutting_down_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/error_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/invalid_mandatory_parameter_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/invalid_stream_identifier_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/missing_mandatory_parameter_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/no_user_data_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/out_of_resource_error_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/protocol_violation_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/restart_of_an_association_with_new_address_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/stale_cookie_error_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/unrecognized_chunk_type_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/unrecognized_parameter_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/unresolvable_address_cause.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/error_cause/user_initiated_abort_cause.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/add_incoming_streams_request_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/add_outgoing_streams_request_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/forward_tsn_supported_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/heartbeat_info_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/incoming_ssn_reset_request_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/outgoing_ssn_reset_request_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/reconfiguration_response_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/ssn_tsn_reset_request_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/state_cookie_parameter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/packet/parameter/supported_extensions_parameter.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/rx/data_tracker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/rx/reassembly_queue.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/rx/traditional_reassembly_streams.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/socket/dcsctp_socket.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/socket/heartbeat_handler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/socket/state_cookie.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/socket/stream_reset_handler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/socket/transmission_control_block.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/timer/task_queue_timeout.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/timer/timer.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/tx/retransmission_error_counter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/tx/retransmission_queue.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/tx/retransmission_timeout.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/net/dcsctp/tx/rr_send_queue.cc>

////////////////////////////////////////////////////////////////////////////////
// p2p
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/async_stun_tcp_socket.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/basic_async_resolver_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/basic_ice_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/basic_packet_socket_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/connection.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/connection_info.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/default_ice_transport_factory.cc>
#define kMinRtpPacketLen kMinRtpPacketLen_dtls_transport_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/dtls_transport.cc>
#undef kMinRtpPacketLen
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/dtls_transport_internal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/ice_controller_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/ice_credentials_iterator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/ice_transport_internal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/p2p_constants.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/p2p_transport_channel.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/packet_transport_internal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/port.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/port_allocator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/port_interface.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/pseudo_tcp.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/regathering_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/stun_port.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/stun_request.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/stun_server.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/tcp_port.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/test_stun_server.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/transport_description.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/transport_description_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/turn_port.cc>
#define TURN_CHANNEL_HEADER_SIZE TURN_CHANNEL_HEADER_SIZE_turn_server_BOSS
#define IsTurnChannelData IsTurnChannelData_turn_server_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/base/turn_server.cc>
#undef TURN_CHANNEL_HEADER_SIZE
#undef IsTurnChannelData

#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/client/basic_port_allocator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/client/turn_port_factory.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/p2p/stunprober/stun_prober.cc>

////////////////////////////////////////////////////////////////////////////////
// pc
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/audio_rtp_receiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/audio_track.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/channel.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/channel_manager.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/connection_context.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/data_channel_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/data_channel_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/dtls_srtp_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/dtls_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/dtmf_sender.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/external_hmac.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/ice_server_parsing.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/ice_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/jitter_buffer_delay.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/jsep_ice_candidate.cc>
#define kPreferenceUnknown kPreferenceUnknown_jsep_session_description_BOSS
#define kPreferenceHost kPreferenceHost_jsep_session_description_BOSS
#define kPreferenceReflexive kPreferenceReflexive_jsep_session_description_BOSS
#define kPreferenceRelayed kPreferenceRelayed_jsep_session_description_BOSS
#define kDummyAddress kDummyAddress_jsep_session_description_BOSS
#define kDummyPort kDummyPort_jsep_session_description_BOSS
#define GetCandidatePreferenceFromType GetCandidatePreferenceFromType_jsep_session_description_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/jsep_session_description.cc>
#undef kPreferenceUnknown
#undef kPreferenceHost
#undef kPreferenceReflexive
#undef kPreferenceRelayed
#undef kDummyAddress
#undef kDummyPort
#undef GetCandidatePreferenceFromType
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/jsep_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/jsep_transport_collection.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/jsep_transport_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/local_audio_source.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/media_protocol_names.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/media_session.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/media_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/media_stream_observer.cc>
#define MSG_CREATE_SESSIONDESCRIPTION_FAILED MSG_CREATE_SESSIONDESCRIPTION_FAILED_peer_connection_BOSS
#define CreateSessionDescriptionMsg CreateSessionDescriptionMsg_peer_connection_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/peer_connection.cc>
#undef MSG_CREATE_SESSIONDESCRIPTION_FAILED
#undef CreateSessionDescriptionMsg
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/peer_connection_factory.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/peer_connection_message_handler.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/peer_connection_wrapper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/proxy.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/remote_audio_source.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtc_stats_collector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtc_stats_traversal.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtcp_mux_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtp_media_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtp_parameters_conversion.cc>
#define GenerateUniqueId GenerateUniqueId_rtp_receiver_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtp_receiver.cc>
#undef GenerateUniqueId
#define GenerateUniqueId GenerateUniqueId_rtp_sender_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtp_sender.cc>
#undef GenerateUniqueId
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtp_transceiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtp_transmission_manager.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/rtp_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/sctp_data_channel.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/sctp_data_channel_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/sctp_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/sctp_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/sdp_offer_answer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/sdp_serializer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/sdp_utils.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/session_description.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/simulcast_description.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/srtp_filter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/srtp_session.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/srtp_transport.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/stats_collector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/track_media_info_map.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/transceiver_list.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/transport_stats.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/usage_pattern.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/video_rtp_receiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/video_rtp_track_source.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/video_track.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/video_track_source.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/video_track_source_proxy.cc>
#define kPreferenceUnknown kPreferenceUnknown_webrtc_sdp_BOSS
#define kPreferenceHost kPreferenceHost_webrtc_sdp_BOSS
#define kPreferenceReflexive kPreferenceReflexive_webrtc_sdp_BOSS
#define kPreferenceRelayed kPreferenceRelayed_webrtc_sdp_BOSS
#define kDummyAddress kDummyAddress_webrtc_sdp_BOSS
#define kDummyPort kDummyPort_webrtc_sdp_BOSS
#define GetCandidatePreferenceFromType GetCandidatePreferenceFromType_webrtc_sdp_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/webrtc_sdp.cc>
#undef kPreferenceUnknown
#undef kPreferenceHost
#undef kPreferenceReflexive
#undef kPreferenceRelayed
#undef kDummyAddress
#undef kDummyPort
#undef GetCandidatePreferenceFromType
#define MSG_CREATE_SESSIONDESCRIPTION_FAILED MSG_CREATE_SESSIONDESCRIPTION_FAILED_webrtc_session_description_factory_BOSS
#define CreateSessionDescriptionMsg CreateSessionDescriptionMsg_webrtc_session_description_factory_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/pc/webrtc_session_description_factory.cc>
#undef MSG_CREATE_SESSIONDESCRIPTION_FAILED
#undef CreateSessionDescriptionMsg

////////////////////////////////////////////////////////////////////////////////
// stats
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/stats/rtc_stats.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/stats/rtc_stats_report.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/stats/rtcstats_objects.cc>

////////////////////////////////////////////////////////////////////////////////
// system_wrappers
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/system_wrappers/source/clock.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/system_wrappers/source/cpu_features.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/system_wrappers/source/cpu_info.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/system_wrappers/source/field_trial.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/system_wrappers/source/metrics.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/system_wrappers/source/rtp_to_ntp_estimator.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/system_wrappers/source/sleep.cc>

////////////////////////////////////////////////////////////////////////////////
// video
////////////////////////////////////////////////////////////////////////////////
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/alignment_adjuster.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/buffered_frame_decryptor.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/call_stats.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/call_stats2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/encoder_bitrate_adjuster.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/encoder_overshoot_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/encoder_rtcp_feedback.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/frame_dumping_decoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/frame_encode_metadata_writer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/quality_limitation_reason_tracker.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/quality_threshold.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/receive_statistics_proxy.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/receive_statistics_proxy2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/report_block_stats.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/rtp_streams_synchronizer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/rtp_streams_synchronizer2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/rtp_video_stream_receiver.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/rtp_video_stream_receiver2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/rtp_video_stream_receiver_frame_transformer_delegate.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/screenshare_loopback.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/send_delay_stats.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/send_statistics_proxy.cc>
#define kDefaultProcessIntervalMs kDefaultProcessIntervalMs_stats_counter_BOSS
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/stats_counter.cc>
#undef kDefaultProcessIntervalMs
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/stream_synchronization.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/sv_loopback.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/transport_adapter.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_analyzer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_loopback.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_loopback_main.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_quality_observer.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_quality_observer2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_receive_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_receive_stream2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_send_stream.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_send_stream_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_source_sink_controller.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_stream_decoder.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_stream_decoder2.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_stream_decoder_impl.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/video_stream_encoder.cc>

#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/balanced_constraint.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/bitrate_constraint.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/encode_usage_resource.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/overuse_frame_detector.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/pixel_limit_resource.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/quality_rampup_experiment_helper.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/quality_scaler_resource.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/video_stream_encoder_resource.cc>
#include <addon/webrtc-jumpingyang001-20210616_for_boss/video/adaptation/video_stream_encoder_resource_manager.cc>

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
        shutdown_event_(true, false),
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

    namespace videocapturemodule
    {
        VideoCaptureModule::DeviceInfo* VideoCaptureImpl::CreateDeviceInfo() {_ptr_;}
        rtc::scoped_refptr<VideoCaptureModule> VideoCaptureImpl::Create(const char* device_id) {_ptr_;}
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
}*/

#endif
