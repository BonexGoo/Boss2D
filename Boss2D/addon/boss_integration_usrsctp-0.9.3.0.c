#include <boss.h>
#if BOSS_NEED_ADDON_WEBRTC

#if BOSS_WINDOWS
    #pragma comment(lib, "ws2_32.lib")
#endif

#include <errno.h>
#undef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#undef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#undef EALREADY
#define EALREADY WSAEALREADY
#undef ENOTSOCK
#define ENOTSOCK WSAENOTSOCK
#undef EDESTADDRREQ
#define EDESTADDRREQ WSAEDESTADDRREQ
#undef EMSGSIZE
#define EMSGSIZE WSAEMSGSIZE
#undef EPROTOTYPE
#define EPROTOTYPE WSAEPROTOTYPE
#undef ENOPROTOOPT
#define ENOPROTOOPT WSAENOPROTOOPT
#undef EPROTONOSUPPORT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#undef ESOCKTNOSUPPORT
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#undef EOPNOTSUPP
#define EOPNOTSUPP WSAEOPNOTSUPP
#undef EPFNOSUPPORT
#define EPFNOSUPPORT WSAEPFNOSUPPORT
#undef EAFNOSUPPORT
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#undef EADDRINUSE
#define EADDRINUSE WSAEADDRINUSE
#undef EADDRNOTAVAIL
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#undef ENETDOWN
#define ENETDOWN WSAENETDOWN
#undef ENETUNREACH
#define ENETUNREACH WSAENETUNREACH
#undef ENETRESET
#define ENETRESET WSAENETRESET
#undef ECONNABORTED
#define ECONNABORTED WSAECONNABORTED
#undef ECONNRESET
#define ECONNRESET WSAECONNRESET
#undef ENOBUFS
#define ENOBUFS WSAENOBUFS
#undef EISCONN
#define EISCONN WSAEISCONN
#undef ENOTCONN
#define ENOTCONN WSAENOTCONN
#undef ESHUTDOWN
#define ESHUTDOWN WSAESHUTDOWN
#undef ETOOMANYREFS
#define ETOOMANYREFS WSAETOOMANYREFS
#undef ETIMEDOUT
#define ETIMEDOUT WSAETIMEDOUT
#undef ECONNREFUSED
#define ECONNREFUSED WSAECONNREFUSED
#undef ELOOP
#define ELOOP WSAELOOP
#undef ENAMETOOLONG
#define ENAMETOOLONG WSAENAMETOOLONG
#undef EHOSTDOWN
#define EHOSTDOWN WSAEHOSTDOWN
#undef EHOSTUNREACH
#define EHOSTUNREACH WSAEHOSTUNREACH
#undef ENOTEMPTY
#define ENOTEMPTY WSAENOTEMPTY
#undef EPROCLIM
#define EPROCLIM WSAEPROCLIM
#undef EUSERS
#define EUSERS WSAEUSERS
#undef EDQUOT
#define EDQUOT WSAEDQUOT
#undef ESTALE
#define ESTALE WSAESTALE
#undef EREMOTE
#define EREMOTE WSAEREMOTE
#define SOCKET_EACCES WSAEACCES

#include "boss_integration_usrsctp-0.9.3.0.h"

#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_environment.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_mbuf.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_recv_thread.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_socket.c>

#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_pcb.h>

#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_asconf.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_auth.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_bsd_addr.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_callout.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_cc_functions.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_crc32.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_indata.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_input.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_output.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_pcb.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_peeloff.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_sha1.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_ss_functions.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_sysctl.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_timer.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_userspace.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_usrreq.c>
#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctputil.c>

#include <addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet6/sctp6_usrreq.c>

#endif
