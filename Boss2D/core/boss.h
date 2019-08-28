#pragma once
/*!
\mainpage Boss2D AppEngine
\section information 목적
    - Qt기반 2D앱엔진
\section advanced 소개
    - addon그룹: 오픈소스연동
    - core그룹: 코어라이브러리
    - element그룹: 단위모델지원
    - format그룹: 파일포맷지원
    - platform그룹: 하이브리드 환경제공
    - service그룹: 서비스레이어 기능제공
\section developer 개발자
    - Bonex Goo
\section contact 연락처
    - bonexgoo@gmail.com
*/

// About disable warning
#pragma warning(disable:4117) // 미리 정의된 매크로를 정의 또는 정의 해제하려고 합니다
#pragma warning(disable:4244) // type1에서 type2로 변환하면서 데이터가 손실될 수 있습니다
#pragma warning(disable:4273) // dll 링크가 일치하지 않습니다
#pragma warning(disable:4800) // true 또는 false로 bool 값을 강제하고 있습니다
#pragma warning(disable:4996) // 컴파일러가 사용되지 않는 선언을 발견했습니다
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

// About literal incoding
#pragma execution_character_set("utf-8")

// About debug macro
#if !defined(BOSS_NDEBUG) || (BOSS_NDEBUG != 0 && BOSS_NDEBUG != 1)
    #error BOSS_NDEBUG macro is invalid use
#endif

// About platform macro
#if defined(_WIN32) || defined(_WIN64)
    #define BOSS_WINDOWS 1
    #if defined(__MINGW32__)
        #define BOSS_WINDOWS_MINGW 1
    #else
        #define BOSS_WINDOWS_MINGW 0
    #endif
#else
    #define BOSS_WINDOWS 0
    #define BOSS_WINDOWS_MINGW 0
#endif

#if defined(__linux__) && !defined(ANDROID)
    #define BOSS_LINUX 1
#else
    #define BOSS_LINUX 0
#endif

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC && !TARGET_OS_IPHONE
        #define BOSS_MAC_OSX 1
    #else
        #define BOSS_MAC_OSX 0
    #endif
    #if TARGET_OS_MAC && TARGET_OS_IPHONE
        #define BOSS_IPHONE 1
    #else
        #define BOSS_IPHONE 0
    #endif
#else
    #define BOSS_MAC_OSX 0
    #define BOSS_IPHONE 0
#endif

#if defined(ANDROID)
    #define BOSS_ANDROID 1
#else
    #define BOSS_ANDROID 0
#endif

#if (BOSS_WINDOWS + BOSS_LINUX + BOSS_MAC_OSX + BOSS_IPHONE + BOSS_ANDROID != 1)
    #error Unknown platform
#endif

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || \
    defined(__ppc64__) || defined(_WIN64) || defined(__LP64__) || defined(_LP64)
    #define BOSS_X64 1
#else
    #define BOSS_X64 0
#endif

#if defined(arm) || defined(__arm__) || defined(ARM) || defined(_ARM_)
    #define BOSS_ARM 1
#else
    #define BOSS_ARM 0
#endif

// About type_t
#if BOSS_WINDOWS
    #if BOSS_X64
        typedef unsigned long long boss_size_t;
        typedef long long boss_ssize_t;
    #else
        typedef unsigned int boss_size_t;
        typedef int boss_ssize_t;
    #endif
    #ifndef __cplusplus
        typedef unsigned short wchar_t;
    #endif
#elif BOSS_LINUX
    #if BOSS_X64
        typedef long unsigned int boss_size_t;
        typedef long int boss_ssize_t;
    #else
        typedef unsigned int boss_size_t;
        typedef int boss_ssize_t;
    #endif
    #ifndef __cplusplus
        typedef unsigned short wchar_t;
    #endif
#elif BOSS_MAC_OSX || BOSS_IPHONE
    typedef unsigned long boss_size_t;
    typedef long boss_ssize_t;
    #ifndef __cplusplus
        typedef int wchar_t;
    #endif
#elif BOSS_ANDROID
    typedef unsigned int boss_size_t;
    typedef long int boss_ssize_t;
    #ifndef __cplusplus
        typedef unsigned int wchar_t;
    #endif
#endif

// About user config
#include <boss_config.h>

// About assert
#if !BOSS_NDEBUG
    #define BOSS_DBG __FILE__,__LINE__,__FUNCTION__,
    #define BOSS_DBG_PRM const char* file,const int line,const char* func,
    #define BOSS_DBG_VAL const char* File;const int Line;const char* Func;
    #define BOSS_DBG_INT File(file),Line(line),Func(func),
    #define BOSS_DBG_ARG file,line,func,
    #define BOSS_DBG_FILE file
    #define BOSS_DBG_LINE line
    #define BOSS_DBG_FUNC func
    #define BOSS_ASSERT(TEXT, QUERY) do{ \
        static int IsIgnore = 0; \
        if(IsIgnore || (QUERY) != 0) break; \
        switch(boss_platform_assert(BOSS_DBG TEXT, #QUERY)) \
        { \
        case 0: BOSS_DBG_BREAK; break; \
        case 1: IsIgnore = 1; break; \
        }} while(0)
    #define BOSS_ASSERT_PRM(TEXT, QUERY) do{ \
        static int IsIgnore = 0; \
        if(IsIgnore || (QUERY) != 0) break; \
        switch(boss_platform_assert(BOSS_DBG_ARG TEXT, #QUERY)) \
        { \
        case 0: BOSS_DBG_BREAK; break; \
        case 1: IsIgnore = 1; break; \
        }} while(0)
    #define BOSS_ASSERT_ARG(TEXT, QUERY, ...) do{ \
        static int IsIgnore = 0; \
        if(IsIgnore || (QUERY) != 0) break; \
        switch(boss_platform_assert(__VA_ARGS__, TEXT, #QUERY)) \
        { \
        case 0: BOSS_DBG_BREAK; break; \
        case 1: IsIgnore = 1; break; \
        }} while(0)
    #if BOSS_WINDOWS && defined(_MSC_VER)
        #define BOSS_DBG_BREAK do{__debugbreak();} while(0)
    #elif BOSS_WINDOWS || BOSS_LINUX || BOSS_MAC_OSX || BOSS_IPHONE
        #define BOSS_DBG_BREAK do{__builtin_trap();} while(0)
    #else
        #define BOSS_DBG_BREAK do{} while(0)
    #endif
#elif BOSS_NEED_RELEASE_TRACE
    #define BOSS_DBG
    #define BOSS_DBG_PRM
    #define BOSS_DBG_VAL
    #define BOSS_DBG_INT
    #define BOSS_DBG_ARG
    #define BOSS_DBG_FILE ""
    #define BOSS_DBG_LINE -1
    #define BOSS_DBG_FUNC ""
    #define BOSS_ASSERT(TEXT, QUERY) do{ \
        if((QUERY) == 0) \
            boss_platform_trace("[BOSS_ASSERT] %s [%s] %s(%dLn, %s())", \
                (const char*) TEXT, #QUERY, __FILE__, __LINE__, __FUNCTION__); \
        } while(0)
    #define BOSS_ASSERT_PRM(TEXT, QUERY)
    #define BOSS_ASSERT_ARG(TEXT, QUERY, ...)
#else
    #define BOSS_DBG
    #define BOSS_DBG_PRM
    #define BOSS_DBG_VAL
    #define BOSS_DBG_INT
    #define BOSS_DBG_ARG
    #define BOSS_DBG_FILE ""
    #define BOSS_DBG_LINE -1
    #define BOSS_DBG_FUNC ""
    #define BOSS_ASSERT(TEXT, QUERY)
    #define BOSS_ASSERT_PRM(TEXT, QUERY)
    #define BOSS_ASSERT_ARG(TEXT, QUERY, ...)
#endif

// About trace
#if !BOSS_NDEBUG | BOSS_NEED_RELEASE_TRACE
    #define BOSS_TRACE(...) boss_platform_trace(__VA_ARGS__)
#else
    #define BOSS_TRACE(...) do{} while(0)
#endif

// About global function
#ifdef __cplusplus
    extern "C" {
#endif
    #if defined(__GNUC__) && __GNUC__ >= 3
        typedef __builtin_va_list boss_va_list;
    #else
        typedef char* boss_va_list;
    #endif

    int boss_platform_assert(BOSS_DBG_PRM const char* name, const char* query);
    void boss_platform_trace(const char* text, ...);

    ////////////////////////////////////////////////////////////////////////////////
    // String
    ////////////////////////////////////////////////////////////////////////////////
    int boss_isalpha(int c);
    int boss_isdigit(int c);
    int boss_isalnum(int c);
    double boss_atof(const char* str);
    char* boss_strcpy(char* dest, const char* src);
    char* boss_strncpy(char* dest, const char* src, boss_size_t n);
    wchar_t* boss_wcscpy(wchar_t* dest, const wchar_t* src);
    wchar_t* boss_wcsncpy(wchar_t* dest, const wchar_t* src, boss_size_t n);
    boss_size_t boss_strlen(const char* str);
    boss_size_t boss_wcslen(const wchar_t* str);
    int boss_snprintf(char* str, boss_size_t n, const char* format, ...);
    int boss_vsnprintf(char* str, boss_size_t n, const char* format, boss_va_list args);
    int boss_snwprintf(wchar_t* str, boss_size_t n, const wchar_t* format, ...);
    int boss_vsnwprintf(wchar_t* str, boss_size_t n, const wchar_t* format, boss_va_list args);
    int boss_strcmp(const char* str1, const char* str2);
    int boss_strncmp(const char* str1, const char* str2, boss_size_t maxcount);
    int boss_stricmp(const char* str1, const char* str2);
    int boss_strnicmp(const char* str1, const char* str2, boss_size_t maxcount);
    char* boss_strpbrk(const char* str1, const char* str2);
    int boss_wcscmp(const wchar_t* str1, const wchar_t* str2);
    int boss_wcsncmp(const wchar_t* str1, const wchar_t* str2, boss_size_t maxcount);
    int boss_wcsicmp(const wchar_t* str1, const wchar_t* str2);
    int boss_wcsnicmp(const wchar_t* str1, const wchar_t* str2, boss_size_t maxcount);
    wchar_t* boss_wcspbrk(const wchar_t* str1, const wchar_t* str2);

    ////////////////////////////////////////////////////////////////////////////////
    // File
    ////////////////////////////////////////////////////////////////////////////////
    typedef void* boss_file;
    typedef void* boss_dir;
    typedef void* boss_dirent;
    typedef enum boss_drive
    {
        drive_error = -1,
        drive_absolute = 0, drive_relative, drive_assets, drive_memory
    } boss_drive;
    const char* boss_normalpath(const char* itemname, boss_drive* result);
    boss_file boss_fopen(const char* filename, const char* mode);
    int boss_fclose(boss_file file);
    int boss_feof(boss_file file);
    int boss_fseek(boss_file file, long int offset, int origin);
    long int boss_ftell(boss_file file);
    boss_size_t boss_fread(void* ptr, boss_size_t size, boss_size_t count, boss_file file);
    boss_size_t boss_fwrite(const void* ptr, boss_size_t size, boss_size_t count, boss_file file);
    int boss_fgetc(boss_file file);
    int boss_ungetc(int character, boss_file file);
    int boss_fprintf(boss_file file, const char* format, boss_va_list args);
    void boss_rewind(boss_file file);
    char* boss_fgets(char* str, int num, boss_file file);
    boss_dir boss_opendir(const char* dirname);
    boss_dirent boss_readdir(boss_dir dir);
    const char* boss_getdirname(boss_dirent dirent);
    int boss_closedir(boss_dir dir);

    ////////////////////////////////////////////////////////////////////////////////
    // Socket
    ////////////////////////////////////////////////////////////////////////////////
    typedef struct boss_fd_set
    {
        unsigned int fd_count;
        union
        {
            int fd_array[64];
            unsigned long fds_bits[1024 / (8 * sizeof(unsigned long))];
        };
    } boss_fd_set;
    int boss_socket(int domain, int type, int protocol);
    int boss_connect(int sockfd, const void* addr, int addrlen);
    int boss_bind(int sockfd, const void* addr, int addrlen);
    int boss_accept(int sockfd, void* addr, int* addrlen);
    int boss_listen(int sockfd, int backlog);
    int boss_shutdown(int sockfd, int how);
    int boss_getsockopt(int sockfd, int level, int optname, void* optval, int* optlen);
    int boss_setsockopt(int sockfd, int level, int optname, const void* optval, int optlen);
    int boss_getsockname(int sockfd, void* addr, int* addrlen);
    int boss_getpeername(int sockfd, void* addr, int* addrlen);
    boss_ssize_t boss_recv(int sockfd, void* buf, boss_size_t len, int flags);
    boss_ssize_t boss_recvfrom(int sockfd, void* buf, boss_size_t len, int flags, void* src_addr, int* addrlen);
    boss_ssize_t boss_send(int sockfd, const void* buf, boss_size_t len, int flags);
    boss_ssize_t boss_sendto(int sockfd, const void* buf, boss_size_t len, int flags, const void* dest_addr, int addrlen);
    int boss_ioctlsocket(int sockfd, long cmd, unsigned long* argp);
    int boss_select(int nfds, boss_fd_set* readfds, boss_fd_set* writefds, boss_fd_set* exceptfds, void* timeout);
    int boss_closesocket(int sockfd);
    void* boss_gethostbyaddr(const void* addr, int len, int type);
    void* boss_gethostbyname(const char* name);
    int boss_getaddrinfo(const char* node, const char* service, const void* hints, void** res);
    void boss_freeaddrinfo(void* res);
    const char* boss_gai_strerror(int errcode);
    void* boss_getservbyname(const char* name, const char* proto);
    void* boss_getservbyport(int port, const char* proto);
    char* boss_inet_ntoa(void* in);
    unsigned int boss_inet_addr(const char* cp);
    unsigned int boss_htonl(unsigned int hostlong);
    unsigned short boss_htons(unsigned short hostshort);
    unsigned int boss_ntohl(unsigned int netlong);
    unsigned short boss_ntohs(unsigned short netshort);
    void boss_FD_ZERO(boss_fd_set* fdset);
    void boss_FD_SET(int fd, boss_fd_set* fdset);
    void boss_FD_CLR(int fd, boss_fd_set* fdset);
    int boss_FD_ISSET(int fd, boss_fd_set* fdset);
    int boss_geterrno();
    void boss_seterrno(int err);
#ifdef __cplusplus
    }
#endif
