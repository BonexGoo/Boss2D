#include <boss.h>
#if BOSS_NEED_ADDON_WEBRTC

#include "boss_integration_webrtc-qt5.11.2.h"

#if BOSS_WINDOWS
    #pragma comment(lib, "secur32.lib")
    #pragma comment(lib, "psapi.lib")
#endif

static void SSL_enable_ocsp_stapling(SSL* ssl)
{
    BOSS_ASSERT("미구현, 확인필요!", false);
}

static void SSL_enable_signed_cert_timestamps(SSL* ssl)
{
    BOSS_ASSERT("미구현, 확인필요!", false);
}

// jsoncpp
#include <addon/webrtc-qt5.11.2_for_boss/[boss]/jsoncpp/jsoncpp.cpp>

#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/asyncinvoker.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/asyncpacketsocket.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/asyncresolverinterface.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/asyncsocket.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/asynctcpsocket.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/asynctcpsocket_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/asyncudpsocket.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/asyncudpsocket_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/atomicops_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/base64.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/base64_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/basictypes_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bind_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bitbuffer.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bitbuffer_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bitrateallocationstrategy.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bitrateallocationstrategy_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/buffer_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bufferqueue.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bufferqueue_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bytebuffer.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/bytebuffer_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/byteorder_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/callback_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/checks.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/copyonwritebuffer.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/copyonwritebuffer_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/cpu_time.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/cpu_time_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/crc32.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/crc32_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/criticalsection.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/criticalsection_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/cryptstring.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/event.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/event_tracer.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/event_tracer_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/event_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/fakeclock.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/fakesslidentity.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/file.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/file_posix.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/file_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/file_win.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/filerotatingstream.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/filerotatingstream_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/fileutils.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/firewallsocketserver.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/flags.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/function_view_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/helpers.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/helpers_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/httpbase.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/httpbase_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/httpcommon.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/httpcommon_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/httpserver.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/httpserver_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ifaddrs-android.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ifaddrs_converter.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ipaddress.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ipaddress_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/json.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/json_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/location.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/logging.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/logging_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/logsinks.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/macifaddrs_converter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/macutils.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/md5.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/md5digest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/md5digest_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/memory_usage.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/memory_usage_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/messagedigest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/messagedigest_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/messagehandler.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/messagequeue.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/messagequeue_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/nat_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/natserver.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/natsocketfactory.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/nattypes.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/nethelper.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/nethelpers.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/network.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/network_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/networkmonitor.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/noop.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/nullsocketserver.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/nullsocketserver_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/onetimeevent_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/openssladapter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/openssladapter_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/openssldigest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/opensslidentity.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/opensslstreamadapter.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/optionsfile.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/optionsfile_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/pathutils.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/pathutils_unittest.cc>

#define PACKET_MAXIMUMS PACKET_MAXIMUMS_physicalsocketserver_BOSS
#define IP_HEADER_SIZE IP_HEADER_SIZE_physicalsocketserver_BOSS
#define ICMP_HEADER_SIZE ICMP_HEADER_SIZE_physicalsocketserver_BOSS
#define ICMP_PING_TIMEOUT_MILLIS ICMP_PING_TIMEOUT_MILLIS_physicalsocketserver_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/physicalsocketserver.cc>
#undef PACKET_MAXIMUMS
#undef IP_HEADER_SIZE
#undef ICMP_HEADER_SIZE
#undef ICMP_PING_TIMEOUT_MILLIS

//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/physicalsocketserver_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/platform_file.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/platform_thread.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/platform_thread_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/proxy_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/proxyinfo.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/proxyserver.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ptr_util_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/race_checker.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/random.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/random_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rate_limiter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rate_limiter_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rate_statistics.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rate_statistics_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ratelimiter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ratelimiter_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ratetracker.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ratetracker_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/refcountedobject_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rollingaccumulator_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rtccertificate.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rtccertificate_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rtccertificategenerator.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/rtccertificategenerator_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sequenced_task_checker_impl.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sequenced_task_checker_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sha1.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sha1digest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sha1digest_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/signalthread.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/signalthread_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sigslot.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sigslot_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sigslottester_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/socket_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/socketadapters.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/socketaddress.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/socketaddress_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/socketaddresspair.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/socketstream.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ssladapter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/ssladapter_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sslfingerprint.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sslidentity.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sslidentity_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sslstreamadapter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/sslstreamadapter_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/stream.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/stream_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/string_to_number.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/string_to_number_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/stringencode.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/stringencode_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/stringize_macros_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/stringutils.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/stringutils_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/swap_queue_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/task_queue_gcd.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/task_queue_libevent.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/task_queue_posix.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/task_queue_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/task_queue_win.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/testclient.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/testclient_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/testechoserver.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/testutils.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/thread.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/thread_annotations_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/thread_checker_impl.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/thread_checker_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/thread_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/timestampaligner.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/timestampaligner_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/timeutils.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/timeutils_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/transformadapter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/unittest_main.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/unixfilesystem.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/virtualsocket_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/virtualsocketserver.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/weak_ptr.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/weak_ptr_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32filesystem.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32securityerrors.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32socketinit.cc>

#define PACKET_MAXIMUMS PACKET_MAXIMUMS_win32socketserver_BOSS
#define IP_HEADER_SIZE IP_HEADER_SIZE_win32socketserver_BOSS
#define ICMP_HEADER_SIZE ICMP_HEADER_SIZE_win32socketserver_BOSS
#define ICMP_PING_TIMEOUT_MILLIS ICMP_PING_TIMEOUT_MILLIS_win32socketserver_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32socketserver.cc>
#undef PACKET_MAXIMUMS
#undef IP_HEADER_SIZE
#undef ICMP_HEADER_SIZE
#undef ICMP_PING_TIMEOUT_MILLIS

//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32socketserver_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32window.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/rtc_base/win32window_unittest.cc>

//#include <addon/webrtc-qt5.11.2_for_boss/api/array_view_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/candidate.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/fakemetricsobserver.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/jsep.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/mediaconstraintsinterface.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/mediastreaminterface.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/mediatypes.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/optional.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/api/optional_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/proxy.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/rtcerror.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/api/rtcerror_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/rtp_headers.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/rtpparameters.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/api/rtpparameters_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/statstypes.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/umametrics.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/videosourceinterface.cc>

#include <addon/webrtc-qt5.11.2_for_boss/api/audio_codecs/audio_decoder.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/audio_codecs/audio_encoder.cc>
#include <addon/webrtc-qt5.11.2_for_boss/api/audio_codecs/audio_format.cc>

#define NotAdvertised NotAdvertised_builtin_audio_decoder_factory_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/api/audio_codecs/builtin_audio_decoder_factory.cc>
#undef NotAdvertised

#define NotAdvertised NotAdvertised_builtin_audio_encoder_factory_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/api/audio_codecs/builtin_audio_encoder_factory.cc>
#undef NotAdvertised

#define MSG_MONITOR_POLL MSG_MONITOR_POLL_audiomonitor_BOSS
#define MSG_MONITOR_START MSG_MONITOR_START_audiomonitor_BOSS
#define MSG_MONITOR_STOP MSG_MONITOR_STOP_audiomonitor_BOSS
#define MSG_MONITOR_SIGNAL MSG_MONITOR_SIGNAL_audiomonitor_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/pc/audiomonitor.cc>
#undef MSG_MONITOR_POLL
#undef MSG_MONITOR_START
#undef MSG_MONITOR_STOP
#undef MSG_MONITOR_SIGNAL

#include <addon/webrtc-qt5.11.2_for_boss/pc/audiotrack.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/bundlefilter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/bundlefilter_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/channel.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/channel_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/channelmanager.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/channelmanager_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/createpeerconnectionfactory.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/currentspeakermonitor.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/currentspeakermonitor_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/datachannel.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/datachannel_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/dtlssrtptransport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/dtlssrtptransport_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/dtmfsender.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/dtmfsender_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/externalhmac.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/iceserverparsing.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/iceserverparsing_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/jsepicecandidate.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/jsepsessiondescription.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/jsepsessiondescription_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/jseptransport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/jseptransport_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/localaudiosource.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/localaudiosource_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/mediaconstraintsinterface_unittest.cc>

#define MSG_MONITOR_POLL MSG_MONITOR_POLL_mediamonitor_BOSS
#define MSG_MONITOR_START MSG_MONITOR_START_mediamonitor_BOSS
#define MSG_MONITOR_STOP MSG_MONITOR_STOP_mediamonitor_BOSS
#define MSG_MONITOR_SIGNAL MSG_MONITOR_SIGNAL_mediamonitor_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/pc/mediamonitor.cc>
#undef MSG_MONITOR_POLL
#undef MSG_MONITOR_START
#undef MSG_MONITOR_STOP
#undef MSG_MONITOR_SIGNAL

#include <addon/webrtc-qt5.11.2_for_boss/pc/mediasession.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/mediasession_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/mediastream.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/mediastream_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/mediastreamobserver.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_bundle_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_crypto_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_datachannel_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_ice_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_integrationtest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_jsep_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_media_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_rtp_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnection_signaling_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnectionendtoend_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnectionfactory.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnectionfactory_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnectioninterface_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/peerconnectionwrapper.cc>///////////////////////////////////////////////    ?
//#include <addon/webrtc-qt5.11.2_for_boss/pc/proxy_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/remoteaudiosource.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/rtcpmuxfilter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/rtcpmuxfilter_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/rtcstats_integrationtest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/rtcstatscollector.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/rtcstatscollector_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/rtpmediautils.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/rtpmediautils_unittest.cc>

#define GenerateUniqueId GenerateUniqueId_rtpreceiver_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/pc/rtpreceiver.cc>
#undef GenerateUniqueId

#define GenerateUniqueId GenerateUniqueId_rtpsender_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/pc/rtpsender.cc>
#undef GenerateUniqueId

//#include <addon/webrtc-qt5.11.2_for_boss/pc/rtpsenderreceiver_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/rtptransceiver.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/rtptransport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/rtptransport_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/sctputils.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/sctputils_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/sdputils.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/sessiondescription.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/srtpfilter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/srtpfilter_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/srtpsession.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/srtpsession_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/srtptransport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/srtptransport_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/statscollector.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/statscollector_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/trackmediainfomap.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/trackmediainfomap_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/transportcontroller.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/transportcontroller_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/videocapturertracksource.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/videocapturertracksource_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/videotrack.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/videotrack_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/pc/videotracksource.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/webrtcsdp.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/webrtcsdp_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/pc/webrtcsessiondescriptionfactory.cc>

#include <addon/webrtc-qt5.11.2_for_boss/stats/rtcstats.cc>
#include <addon/webrtc-qt5.11.2_for_boss/stats/rtcstats_objects.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/stats/rtcstats_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/stats/rtcstatsreport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/stats/rtcstatsreport_unittest.cc>

#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/asyncstuntcpsocket.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/asyncstuntcpsocket_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/basicpacketsocketfactory.cc>

#define kMinRtpPacketLen kMinRtpPacketLen_dtlstransport_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/dtlstransport.cc>
#undef kMinRtpPacketLen

//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/dtlstransport_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/dtlstransportinternal.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/icetransportinternal.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/p2pconstants.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/p2ptransportchannel.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/p2ptransportchannel_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/packetlossestimator.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/packetlossestimator_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/packetsocketfactory.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/packettransportinternal.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/port.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/port_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/portallocator.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/portallocator_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/portinterface.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/pseudotcp.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/pseudotcp_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/relayport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/relayport_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/relayserver.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/relayserver_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/session.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/stun.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/stun_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/stunport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/stunport_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/stunrequest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/stunrequest_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/stunserver.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/stunserver_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/tcpport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/tcpport_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/teststunserver.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/transportdescription.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/transportdescriptionfactory.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/transportdescriptionfactory_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/turnport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/turnport_unittest.cc>

#define TURN_CHANNEL_HEADER_SIZE TURN_CHANNEL_HEADER_SIZE_turnserver_BOSS
#define IsTurnChannelData IsTurnChannelData_turnserver_BOSS
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/turnserver.cc>
#undef TURN_CHANNEL_HEADER_SIZE
#undef IsTurnChannelData

//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/turnserver_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/udptransport.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/p2p/base/udptransport_unittest.cc>

#include <addon/webrtc-qt5.11.2_for_boss/media/base/adaptedvideotracksource.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/codec.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/codec_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/fakertp.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/h264_profile_level_id.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/mediaconstants.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/mediaengine.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/rtpdataengine.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/rtpdataengine_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/rtputils.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/rtputils_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/streamparams.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/streamparams_unittest.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/testutils.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/turnutils.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/turnutils_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/videoadapter.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/videoadapter_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/videobroadcaster.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/videobroadcaster_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/videocapturer.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/videocapturer_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/videocommon.cc>
//#include <addon/webrtc-qt5.11.2_for_boss/media/base/videocommon_unittest.cc>
#include <addon/webrtc-qt5.11.2_for_boss/media/base/videosourcebase.cc>

#endif
