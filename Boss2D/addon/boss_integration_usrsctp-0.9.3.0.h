#pragma once
#include <boss.h>
#include <addon/boss_fakewin.h>

#define __Userspace__
#define __Userspace_os_Windows
#define SCTP_SIMPLE_ALLOCATOR
#define SCTP_PROCESS_LEVEL_LOCKS
#define INET
#define INET6

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BOSS_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BOSS_USRSCTP_", "restore-comment" : " //original-code:"}
#define BOSS_USRSCTP_V_user_atomic_h                                                "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_atomic.h"
#define BOSS_USRSCTP_V_user_environment_h                                           "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_environment.h"
#define BOSS_USRSCTP_V_user_inpcb_h                                                 "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_inpcb.h"
#define BOSS_USRSCTP_V_user_ip6_var_h                                               "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_ip6_var.h"
#define BOSS_USRSCTP_V_user_ip_icmp_h                                               "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_ip_icmp.h"
#define BOSS_USRSCTP_V_user_malloc_h                                                "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_malloc.h"
#define BOSS_USRSCTP_V_user_mbuf_h                                                  "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_mbuf.h"
#define BOSS_USRSCTP_V_user_queue_h                                                 "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_queue.h"
#define BOSS_USRSCTP_V_user_recv_thread_h                                           "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_recv_thread.h"
#define BOSS_USRSCTP_V_user_route_h                                                 "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_route.h"
#define BOSS_USRSCTP_V_user_socketvar_h                                             "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_socketvar.h"
#define BOSS_USRSCTP_V_user_uma_h                                                   "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_uma.h"
#define BOSS_USRSCTP_V_usrsctp_h                                                    "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/usrsctp.h"

#define BOSS_USRSCTP_V_netinet__sctp_h                                              "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp.h"
#define BOSS_USRSCTP_V_netinet__sctp_asconf_h                                       "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_asconf.h"
#define BOSS_USRSCTP_V_netinet__sctp_auth_h                                         "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_auth.h"
#define BOSS_USRSCTP_V_netinet__sctp_bsd_addr_h                                     "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_bsd_addr.h"
#define BOSS_USRSCTP_V_netinet__sctp_callout_h                                      "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_callout.h"
#define BOSS_USRSCTP_V_netinet__sctp_constants_h                                    "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_constants.h"
#define BOSS_USRSCTP_V_netinet__sctp_crc32_h                                        "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_crc32.h"
#define BOSS_USRSCTP_V_netinet__sctp_header_h                                       "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_header.h"
#define BOSS_USRSCTP_V_netinet__sctp_indata_h                                       "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_indata.h"
#define BOSS_USRSCTP_V_netinet__sctp_input_h                                        "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_input.h"
#define BOSS_USRSCTP_V_netinet__sctp_lock_userspace_h                               "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_lock_userspace.h"
#define BOSS_USRSCTP_V_netinet__sctp_os_h                                           "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_os.h"
#define BOSS_USRSCTP_V_netinet__sctp_os_userspace_h                                 "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_os_userspace.h"
#define BOSS_USRSCTP_V_netinet__sctp_output_h                                       "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_output.h"
#define BOSS_USRSCTP_V_netinet__sctp_pcb_h                                          "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_pcb.h"
#define BOSS_USRSCTP_V_netinet__sctp_peeloff_h                                      "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_peeloff.h"
#define BOSS_USRSCTP_V_netinet__sctp_process_lock_h                                 "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_process_lock.h"
#define BOSS_USRSCTP_V_netinet__sctp_sha1_h                                         "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_sha1.h"
#define BOSS_USRSCTP_V_netinet__sctp_structs_h                                      "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_structs.h"
#define BOSS_USRSCTP_V_netinet__sctp_sysctl_h                                       "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_sysctl.h"
#define BOSS_USRSCTP_V_netinet__sctp_timer_h                                        "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_timer.h"
#define BOSS_USRSCTP_V_netinet__sctp_uio_h                                          "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_uio.h"
#define BOSS_USRSCTP_V_netinet__sctp_var_h                                          "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctp_var.h"
#define BOSS_USRSCTP_V_netinet__sctputil_h                                          "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet/sctputil.h"

#define BOSS_USRSCTP_V_netinet6__sctp6_var_h                                        "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/netinet6/sctp6_var.h"

#define BOSS_USRSCTP_U_user_ip6_var_h                                               "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/user_ip6_var.h"

// 외부지원
#define BOSS_USRSCTP_U_usrsctplib__usrsctp_h                                        "addon/usrsctp-0.9.3.0_for_boss/usrsctplib/usrsctp.h"
