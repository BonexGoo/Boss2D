#include <boss.h>
#if BOSS_NEED_ADDON_CURL

#if BOSS_WINDOWS
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "wldap32.lib")
#endif

#pragma warning(disable : 4090)

#if BOSS_ARM
    #define _M_ARM
#endif

#if !BOSS_WINDOWS | BOSS_NEED_FORCED_FAKEWIN
    #define __pragma(...)
    #define struct_stat_BOSS struct boss_fakewin_struct_stat64
#else
    #define struct_stat_BOSS struct stat
#endif

#include "boss_integration_curl-7.51.0.h"

#define delay Sleep

#include <addon/curl-7.51.0_for_boss/lib/amigaos.c>
#include <addon/curl-7.51.0_for_boss/lib/asyn-ares.c>
#include <addon/curl-7.51.0_for_boss/lib/asyn-thread.c>
#include <addon/curl-7.51.0_for_boss/lib/base64.c>
#include <addon/curl-7.51.0_for_boss/lib/conncache.c>
#include <addon/curl-7.51.0_for_boss/lib/connect.c>
#include <addon/curl-7.51.0_for_boss/lib/content_encoding.c>
#include <addon/curl-7.51.0_for_boss/lib/cookie.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_addrinfo.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_des.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_endian.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_fnmatch.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_gethostname.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_gssapi.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_memrchr.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_multibyte.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_ntlm_core.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_ntlm_wb.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_rtmp.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_sasl.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_sspi.c>
#include <addon/curl-7.51.0_for_boss/lib/curl_threads.c>
#include <addon/curl-7.51.0_for_boss/lib/dict.c>
#include <addon/curl-7.51.0_for_boss/lib/dotdot.c>
#include <addon/curl-7.51.0_for_boss/lib/easy.c>
#include <addon/curl-7.51.0_for_boss/lib/escape.c>
#define struct_stat struct_stat_BOSS
#include <addon/curl-7.51.0_for_boss/lib/file.c>
#undef struct_stat
#include <addon/curl-7.51.0_for_boss/lib/fileinfo.c>
#define struct_stat struct_stat_BOSS
#include <addon/curl-7.51.0_for_boss/lib/formdata.c>
#undef struct_stat
#include <addon/curl-7.51.0_for_boss/lib/ftp.c>
#include <addon/curl-7.51.0_for_boss/lib/ftplistparser.c>
#include <addon/curl-7.51.0_for_boss/lib/getenv.c>
#include <addon/curl-7.51.0_for_boss/lib/getinfo.c>
#include <addon/curl-7.51.0_for_boss/lib/gopher.c>
#include <addon/curl-7.51.0_for_boss/lib/hash.c>
#include <addon/curl-7.51.0_for_boss/lib/hmac.c>
#include <addon/curl-7.51.0_for_boss/lib/hostasyn.c>
#include <addon/curl-7.51.0_for_boss/lib/hostcheck.c>
#include <addon/curl-7.51.0_for_boss/lib/hostip.c>
#include <addon/curl-7.51.0_for_boss/lib/hostip4.c>
#include <addon/curl-7.51.0_for_boss/lib/hostip6.c>
#include <addon/curl-7.51.0_for_boss/lib/hostsyn.c>
#include <addon/curl-7.51.0_for_boss/lib/http.c>
#include <addon/curl-7.51.0_for_boss/lib/http2.c>
#include <addon/curl-7.51.0_for_boss/lib/http_chunks.c>
#include <addon/curl-7.51.0_for_boss/lib/http_digest.c>
#include <addon/curl-7.51.0_for_boss/lib/http_negotiate.c>
#include <addon/curl-7.51.0_for_boss/lib/http_ntlm.c>
#include <addon/curl-7.51.0_for_boss/lib/http_proxy.c>
#include <addon/curl-7.51.0_for_boss/lib/idn_win32.c>
#include <addon/curl-7.51.0_for_boss/lib/if2ip.c>
#include <addon/curl-7.51.0_for_boss/lib/imap.c>
#include <addon/curl-7.51.0_for_boss/lib/inet_ntop.c>
#include <addon/curl-7.51.0_for_boss/lib/inet_pton.c>
#include <addon/curl-7.51.0_for_boss/lib/krb5.c>
#include <addon/curl-7.51.0_for_boss/lib/ldap.c>
#include <addon/curl-7.51.0_for_boss/lib/llist.c>
#include <addon/curl-7.51.0_for_boss/lib/md4.c>
#include <addon/curl-7.51.0_for_boss/lib/md5.c>
#include <addon/curl-7.51.0_for_boss/lib/memdebug.c>
#include <addon/curl-7.51.0_for_boss/lib/mprintf.c>
#include <addon/curl-7.51.0_for_boss/lib/multi.c>
#include <addon/curl-7.51.0_for_boss/lib/netrc.c>
#include <addon/curl-7.51.0_for_boss/lib/non-ascii.c>
#include <addon/curl-7.51.0_for_boss/lib/nonblock.c>
#include <addon/curl-7.51.0_for_boss/lib/nwlib.c>
#include <addon/curl-7.51.0_for_boss/lib/nwos.c>
#include <addon/curl-7.51.0_for_boss/lib/openldap.c>
#include <addon/curl-7.51.0_for_boss/lib/parsedate.c>
#include <addon/curl-7.51.0_for_boss/lib/pingpong.c>
#include <addon/curl-7.51.0_for_boss/lib/pipeline.c>
#include <addon/curl-7.51.0_for_boss/lib/pop3.c>
#include <addon/curl-7.51.0_for_boss/lib/progress.c>
#include <addon/curl-7.51.0_for_boss/lib/rtsp.c>
#include <addon/curl-7.51.0_for_boss/lib/security.c>
#include <addon/curl-7.51.0_for_boss/lib/select.c>
#include <addon/curl-7.51.0_for_boss/lib/sendf.c>
#include <addon/curl-7.51.0_for_boss/lib/share.c>
#include <addon/curl-7.51.0_for_boss/lib/slist.c>
#include <addon/curl-7.51.0_for_boss/lib/smb.c>
#include <addon/curl-7.51.0_for_boss/lib/smtp.c>
#include <addon/curl-7.51.0_for_boss/lib/socks.c>
#include <addon/curl-7.51.0_for_boss/lib/socks_gssapi.c>
#include <addon/curl-7.51.0_for_boss/lib/socks_sspi.c>
#include <addon/curl-7.51.0_for_boss/lib/speedcheck.c>
#include <addon/curl-7.51.0_for_boss/lib/splay.c>
#include <addon/curl-7.51.0_for_boss/lib/ssh.c>
#include <addon/curl-7.51.0_for_boss/lib/strcase.c>
#include <addon/curl-7.51.0_for_boss/lib/strdup.c>
#include <addon/curl-7.51.0_for_boss/lib/strerror.c>
#include <addon/curl-7.51.0_for_boss/lib/strtok.c>
#include <addon/curl-7.51.0_for_boss/lib/strtoofft.c>
#include <addon/curl-7.51.0_for_boss/lib/system_win32.c>
#include <addon/curl-7.51.0_for_boss/lib/telnet.c>
#include <addon/curl-7.51.0_for_boss/lib/tftp.c>
#include <addon/curl-7.51.0_for_boss/lib/timeval.c>
#include <addon/curl-7.51.0_for_boss/lib/transfer.c>
#include <addon/curl-7.51.0_for_boss/lib/url.c>
#define protocols protocols_version_BOSS
#include <addon/curl-7.51.0_for_boss/lib/version.c>
#undef protocols
#undef read
#undef write
#include <addon/curl-7.51.0_for_boss/lib/warnless.c>
#include <addon/curl-7.51.0_for_boss/lib/wildcard.c>
#include <addon/curl-7.51.0_for_boss/lib/x509asn1.c>

#include <addon/curl-7.51.0_for_boss/lib/vauth/cleartext.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/cram.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/digest.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/digest_sspi.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/krb5_gssapi.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/krb5_sspi.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/ntlm.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/ntlm_sspi.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/oauth2.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/spnego_gssapi.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/spnego_sspi.c>
#include <addon/curl-7.51.0_for_boss/lib/vauth/vauth.c>

#include <addon/curl-7.51.0_for_boss/lib/vtls/axtls.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/cyassl.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/darwinssl.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/gskit.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/gtls.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/mbedtls.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/nss.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/openssl.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/polarssl.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/polarssl_threadlock.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/schannel.c>
#include <addon/curl-7.51.0_for_boss/lib/vtls/vtls.c>

#endif
