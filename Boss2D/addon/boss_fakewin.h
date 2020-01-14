#pragma once
#include <boss.h>

#pragma warning(disable:4005) // macro redefinition

#if !BOSS_LINUX && (!defined(_MSC_VER) || (_MSC_VER < 1900)) && !BOSS_WINDOWS_MINGW
    #define snprintf(...) boss_snprintf(__VA_ARGS__)
#endif
#define WIN32
#define _WIN32
#define __WIN32__
#define _WIN32_WCE 0x0600
#define _WIN32_WINNT 0x0600
#define WINDOWS
#define _WINDOWS
#if BOSS_NDEBUG
    #define NDEBUG
#endif
#if !BOSS_WINDOWS
    #define _MSC_VER 1900
    #undef __linux__
    #undef __APPLE__
    #undef ANDROID
    #undef __ANDROID__
#endif

#define BOSS_FAKEWIN_DECLSPEC_DLLEXPORT //__declspec(dllexport)
#define BOSS_FAKEWIN_DECLSPEC_DLLIMPORT //__declspec(dllimport)
#define BOSS_FAKEWIN_STDCALL            //__stdcall

#if BOSS_LINUX | BOSS_MAC_OSX | BOSS_WINDOWS_MINGW
    #define alloca(N) __builtin_alloca(N)
    #define _alloca(N) __builtin_alloca(N)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BOSS_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BOSS_FAKEWIN_", "restore-comment" : " //original-code:"}
#if BOSS_WINDOWS & !BOSS_NEED_FORCED_FAKEWIN
    #define BOSS_FAKEWIN_V_windows_h                       <windows.h>
    #define BOSS_FAKEWIN_V_io_h                            <io.h>
    #define BOSS_FAKEWIN_V_basetsd_h                       <basetsd.h>
    #define BOSS_FAKEWIN_V_winsock_h                       <winsock.h>
    #define BOSS_FAKEWIN_V_winsock2_h                      <winsock2.h>
    #define BOSS_FAKEWIN_V_direct_h                        <direct.h>
    #define BOSS_FAKEWIN_V_ws2tcpip_h                      <ws2tcpip.h>
    #define BOSS_FAKEWIN_V_malloc_h                        <malloc.h>
    #define BOSS_FAKEWIN_V_tchar_h                         <tchar.h>
    #define BOSS_FAKEWIN_V_shlwapi_h                       <shlwapi.h>
    #define BOSS_FAKEWIN_V_wincon_h                        <wincon.h>
    #define BOSS_FAKEWIN_V_conio_h                         <conio.h>
    #define BOSS_FAKEWIN_V_wincrypt_h                      <wincrypt.h>
    #define BOSS_FAKEWIN_V_tlhelp32_h                      <tlhelp32.h>
    #define BOSS_FAKEWIN_V_lmcons_h                        <lmcons.h>
    #define BOSS_FAKEWIN_V_lmstats_h                       <lmstats.h>
    #define BOSS_FAKEWIN_V_stdint_h                        <stdint.h>
    #define BOSS_FAKEWIN_U_stdint_h                        "stdint.h"
    #define BOSS_FAKEWIN_V_winbase_h                       <winbase.h>
    #define BOSS_FAKEWIN_V_windef_h                        <windef.h>
    #define BOSS_FAKEWIN_V_process_h                       <process.h>
    #define BOSS_FAKEWIN_V_mbstring_h                      <mbstring.h>
    #define BOSS_FAKEWIN_V_search_h                        <search.h>
    #define BOSS_FAKEWIN_V_intrin_h                        <intrin.h>
    #define BOSS_FAKEWIN_V_winldap_h                       <winldap.h>
    #define BOSS_FAKEWIN_V_winber_h                        <winber.h>
    #define BOSS_FAKEWIN_V_sys__timeb_h                    <sys/timeb.h>

    // 선행참조효과
    #include <winsock2.h>
    #include <windows.h>
    #include <stdio.h>
    #include <sys/stat.h>
    #include <malloc.h>

    typedef struct fd_set boss_fakewin_struct_fd_set;
    #define GetCurrentDirectoryW boss_fakewin_GetCurrentDirectoryW
    typedef unsigned short mode_t;
#else
    #define BOSS_FAKEWIN_IS_ENABLED
    #define BOSS_FAKEWIN_V_windows_h                       <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_io_h                            <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_basetsd_h                       <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_winsock_h                       <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_winsock2_h                      <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_direct_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_ws2tcpip_h                      <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_malloc_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_tchar_h                         <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_shlwapi_h                       <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_wincon_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_conio_h                         <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_wincrypt_h                      <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_tlhelp32_h                      <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_lmcons_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_lmstats_h                       <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_stdint_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_U_stdint_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_winbase_h                       <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_windef_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_process_h                       <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_mbstring_h                      <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_search_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_intrin_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_winldap_h                       <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_winber_h                        <addon/boss_fakewin.h>
    #define BOSS_FAKEWIN_V_sys__timeb_h                    <addon/boss_fakewin.h>

    #define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
    DECLARE_HANDLE(HWINSTA);
    DECLARE_HANDLE(HINSTANCE);
    DECLARE_HANDLE(HKEY);
    DECLARE_HANDLE(HDC);
    DECLARE_HANDLE(HGDIOBJ);
    #define HBITMAP HGDIOBJ

    typedef char CHAR;
    typedef CHAR *PCHAR, *LPCH, *PCH, *PSTR;
    typedef PSTR *PZPSTR;
    typedef char TCHAR, *PTCHAR;
    typedef char* LPSTR;
    typedef const char *LPCSTR, *PCSTR;
    typedef unsigned char UCHAR;
    typedef unsigned short USHORT;
    typedef wchar_t* LPWSTR;
    typedef const wchar_t* LPCWSTR;
    typedef unsigned char BYTE, *LPBYTE;
    typedef wchar_t WCHAR;
    typedef WCHAR *PWCHAR, *LPWCH, *PWCH, *PWSTR;
    typedef PWSTR *PZPWSTR;
    typedef LPCSTR LPCTSTR;
    typedef LPSTR LPTSTR;
    typedef unsigned short WORD;
    typedef long LONG, *PLONG, *LPLONG;
    typedef long long LONGLONG;
    typedef unsigned long long ULONGLONG;
    typedef unsigned long ULONG;
    typedef int BOOL, *LPBOOL;
    typedef int INT, *LPINT;
    typedef unsigned int UINT;
    typedef long long INT64;
    typedef unsigned long long UINT64;
    #if BOSS_X64
        typedef long long INT_PTR;
        typedef unsigned long long UINT_PTR;
        typedef long long LONG_PTR;
        typedef unsigned long long ULONG_PTR;
    #else
        typedef int INT_PTR;
        typedef unsigned int UINT_PTR;
        typedef long LONG_PTR;
        typedef unsigned long ULONG_PTR;
    #endif
    typedef unsigned long DWORD, *LPDWORD;
    typedef unsigned long DWORD_PTR, *PDWORD_PTR;
    typedef unsigned long long DWORDLONG;
    typedef ULONG_PTR SIZE_T, *PSIZE_T;
    typedef LONG_PTR SSIZE_T, *PSSIZE_T;
    typedef boss_size_t rsize_t;
    typedef void VOID;
    typedef void* PVOID;
    typedef void* LPVOID;
    typedef const void* LPCVOID;
    typedef void* HANDLE;
    typedef int SOCKET;
    typedef USHORT ADDRESS_FAMILY;
    typedef int socklen_t;
    #define __socklen_t_defined
    typedef HANDLE HWND;
    typedef HANDLE HLOCAL;
    typedef HINSTANCE HMODULE;
    typedef HKEY* PHKEY;
    typedef DWORD ACCESS_MASK;
    typedef DWORD REGSAM;
    typedef UINT_PTR WPARAM;
    typedef LONG_PTR LPARAM;
    typedef LONG_PTR LRESULT;
    typedef int UNKNOWN;
    typedef int (*FARPROC)();
    typedef BYTE  BOOLEAN;
    typedef BOOLEAN *PBOOLEAN;
    typedef DWORD (*LPTHREAD_START_ROUTINE) (LPVOID);

    #define __int8 char
    #define __int16 short
    #define __int32 int
    #define __int64 long long
    #define u_short USHORT
    typedef unsigned int u_int;
    typedef int errno_t;
    #if !BOSS_LINUX
        typedef unsigned short mode_t;
    #endif
    typedef unsigned int _dev_t;
    typedef unsigned short _ino_t;
    typedef long _off_t;
    typedef long __time32_t;
    typedef __int64 __time64_t;
    typedef unsigned __int32 uid_t;
    typedef unsigned __int32 gid_t;
    #if !BOSS_ANDROID & !BOSS_LINUX
        typedef unsigned __int16 nlink_t;
        typedef __int32 blksize_t;
        typedef __int64 blkcnt_t;
    #endif

    // 선행참조효과(공통)
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>
    #include <ctype.h>
    #include <errno.h>
    #include <sys/stat.h>
    #include <wchar.h>
    #include <float.h>
    #if !BOSS_MAC_OSX & !BOSS_IPHONE & !BOSS_ANDROID
        #include <malloc.h>
    #endif
    #if BOSS_LINUX
        #include <sys/types.h>
        #include <sys/socket.h>
    #elif BOSS_MAC_OSX | BOSS_IPHONE | BOSS_ANDROID
        #include <sys/select.h>
    #endif
    #if BOSS_WINDOWS
        #include <direct.h>
        #include <io.h>
        #include <conio.h>
    #else
        #include <unistd.h>
    #endif
    #include <stdint.h>
    #include <fcntl.h>

    //#define _CMATH_
    //#define _CSTDLIB_

    #ifdef __cplusplus
        #if BOSS_WINDOWS
            //#include <iostream>
            //#include <fstream>
            //#include <sstream>
        #endif
    #endif

    #define socket boss_fakewin_socket
    #define connect boss_fakewin_connect
    #define getsockopt boss_fakewin_getsockopt
    #define setsockopt boss_fakewin_setsockopt
    #define select boss_fakewin_select
    #define recv boss_fakewin_recv
    #define send boss_fakewin_send
    #define closesocket boss_fakewin_closesocket
    #define ioctlsocket boss_fakewin_ioctlsocket
    #define gethostbyname boss_fakewin_gethostbyname
    #define htons boss_fakewin_htons
    #define getaddrinfo boss_fakewin_getaddrinfo
    #define freeaddrinfo boss_fakewin_freeaddrinfo
    #define gai_strerror boss_fakewin_gai_strerror
    #define inet_addr boss_fakewin_inet_addr
    #define bind boss_fakewin_bind
    #define getsockname boss_fakewin_getsockname
    #define getpeername boss_fakewin_getpeername
    #define sendto boss_fakewin_sendto
    #define recvfrom boss_fakewin_recvfrom
    #define accept boss_fakewin_accept
    #define listen boss_fakewin_listen
    #define getservbyname boss_fakewin_getservbyname
    #define inet_ntoa boss_fakewin_inet_ntoa
    #define shutdown boss_fakewin_shutdown
    #define _exit boss_fakewin_exit
    #define getpid boss_fakewin_getpid
    #define _getpid boss_fakewin_getpid
    #define mbstowcs_s boss_fakewin_mbstowcs_s
    #define wcstombs_s boss_fakewin_wcstombs_s

    #define GetCurrentDirectoryW boss_fakewin_GetCurrentDirectoryW
    #define CreateFileW boss_fakewin_CreateFileW
    #define CreateFileMapping boss_fakewin_CreateFileMapping
    #define CreateDirectory boss_fakewin_CreateDirectory
    #define RemoveDirectory boss_fakewin_RemoveDirectory
    #define CloseHandle boss_fakewin_CloseHandle
    #define MapViewOfFile boss_fakewin_MapViewOfFile
    #define UnmapViewOfFile boss_fakewin_UnmapViewOfFile
    #define CreateThread boss_fakewin_CreateThread
    #define DeregisterEventSource boss_fakewin_DeregisterEventSource
    #define DeviceIoControl boss_fakewin_DeviceIoControl
    #define ExpandEnvironmentStringsA boss_fakewin_ExpandEnvironmentStringsA
    #define ExpandEnvironmentStringsW boss_fakewin_ExpandEnvironmentStringsW
    #define FindClose boss_fakewin_FindClose
    #define FindFirstFileA boss_fakewin_FindFirstFileA
    #define FindFirstFileW boss_fakewin_FindFirstFileW
    #define FindFirstFileExA boss_fakewin_FindFirstFileExA
    #define FindNextFileA boss_fakewin_FindNextFileA
    #define FindNextFileW boss_fakewin_FindNextFileW
    #define FindWindow boss_fakewin_FindWindow
    #define FlushConsoleInputBuffer boss_fakewin_FlushConsoleInputBuffer
    #define FlushFileBuffers boss_fakewin_FlushFileBuffers
    #define FormatMessageA boss_fakewin_FormatMessageA
    #define FormatMessageW boss_fakewin_FormatMessageW
    #define GetCurrentProcess boss_fakewin_GetCurrentProcess
    #define GetCurrentThread boss_fakewin_GetCurrentThread
    #define GetCurrentThreadId boss_fakewin_GetCurrentThreadId
    #define GetExitCodeThread boss_fakewin_GetExitCodeThread
    #define GetFileAttributes boss_fakewin_GetFileAttributes
    #define GetFileAttributesW boss_fakewin_GetFileAttributesW
    #define GetFileAttributesExA boss_fakewin_GetFileAttributesExA
    #define GetFileAttributesExW boss_fakewin_GetFileAttributesExW
    #define GetFullPathNameA boss_fakewin_GetFullPathNameA
    #define GetFullPathNameW boss_fakewin_GetFullPathNameW
    #define GetLastError boss_fakewin_GetLastError
    #define SetLastError boss_fakewin_SetLastError
    #define LoadLibraryA boss_fakewin_LoadLibraryA
    #define LoadLibraryW boss_fakewin_LoadLibraryW
    #define FreeLibrary boss_fakewin_FreeLibrary
    #define GetModuleHandleA boss_fakewin_GetModuleHandleA
    #define GetModuleHandleW boss_fakewin_GetModuleHandleW
    #define GetModuleFileName boss_fakewin_GetModuleFileName
    #define GetProcAddress boss_fakewin_GetProcAddress
    #define GetProcessAffinityMask boss_fakewin_GetProcessAffinityMask
    #define GetProcessWindowStation boss_fakewin_GetProcessWindowStation
    #define GetProcessTimes boss_fakewin_GetProcessTimes
    #define GetShortPathNameW boss_fakewin_GetShortPathNameW
    #define GetStdHandle boss_fakewin_GetStdHandle
    #define GetSystemInfo boss_fakewin_GetSystemInfo
    #define GetNativeSystemInfo boss_fakewin_GetNativeSystemInfo
    #define GetSystemTimeAsFileTime boss_fakewin_GetSystemTimeAsFileTime
    #define SystemTimeToFileTime boss_fakewin_SystemTimeToFileTime
    #define GetTickCount boss_fakewin_GetTickCount
    #define GetUserObjectInformationW boss_fakewin_GetUserObjectInformationW
    #define LocalFree boss_fakewin_LocalFree
    #define MessageBox boss_fakewin_MessageBox
    #define CopyFile boss_fakewin_CopyFile
    #define DeleteFileA boss_fakewin_DeleteFileA
    #define MoveFileEx boss_fakewin_MoveFileEx
    #define MoveFileExW boss_fakewin_MoveFileExW
    #define GetTempPathA boss_fakewin_GetTempPathA
    #define GetTempFileNameA boss_fakewin_GetTempFileNameA
    #define MultiByteToWideChar boss_fakewin_MultiByteToWideChar
    #define WideCharToMultiByte boss_fakewin_WideCharToMultiByte
    #define RegCloseKey boss_fakewin_RegCloseKey
    #define RegisterEventSource boss_fakewin_RegisterEventSource
    #define RegOpenKeyExW boss_fakewin_RegOpenKeyExW
    #define RegQueryValueExW boss_fakewin_RegQueryValueExW
    #define ReportEvent boss_fakewin_ReportEvent
    #define SecureZeroMemory boss_fakewin_SecureZeroMemory
    #define SendMessage boss_fakewin_SendMessage
    #define SetFileAttributesW boss_fakewin_SetFileAttributesW
    #define Sleep boss_fakewin_Sleep
    #define SleepEx boss_fakewin_SleepEx
    #define VerifyVersionInfo boss_fakewin_VerifyVersionInfo
    #define SetEvent boss_fakewin_SetEvent
    #define ResetEvent boss_fakewin_ResetEvent
    #define CreateEvent boss_fakewin_CreateEvent
    #define CreateEventW boss_fakewin_CreateEventW
    #define WaitForSingleObject boss_fakewin_WaitForSingleObject
    #define WaitForMultipleObjects boss_fakewin_WaitForMultipleObjects
    #define DeleteCriticalSection boss_fakewin_DeleteCriticalSection
    #define TryEnterCriticalSection boss_fakewin_TryEnterCriticalSection
    #define EnterCriticalSection boss_fakewin_EnterCriticalSection
    #define InitializeCriticalSection boss_fakewin_InitializeCriticalSection
    #define LeaveCriticalSection boss_fakewin_LeaveCriticalSection
    #define WSAGetLastError boss_fakewin_WSAGetLastError
    #define WSASetLastError boss_fakewin_WSASetLastError
    #define WSACleanup boss_fakewin_WSACleanup
    #define WSAStartup boss_fakewin_WSAStartup
    #define WSAStringToAddressA boss_fakewin_WSAStringToAddressA
    #define QueryPerformanceFrequency boss_fakewin_QueryPerformanceFrequency
    #define QueryPerformanceCounter boss_fakewin_QueryPerformanceCounter
    #define ZeroMemory boss_fakewin_ZeroMemory
    #define GetObject boss_fakewin_GetObject
    #define DeleteObject boss_fakewin_DeleteObject
    #define GetDC boss_fakewin_GetDC
    #define ReleaseDC boss_fakewin_ReleaseDC
    #define CreateDIBSection boss_fakewin_CreateDIBSection
    #define TlsAlloc boss_fakewin_TlsAlloc
    #define TlsFree boss_fakewin_TlsFree
    #define TlsGetValue boss_fakewin_TlsGetValue
    #define TlsSetValue boss_fakewin_TlsSetValue
    #define CreateMutexA boss_fakewin_CreateMutexA
    #define ReleaseMutex boss_fakewin_ReleaseMutex
    #define CreateSemaphoreA boss_fakewin_CreateSemaphoreA
    #define ReleaseSemaphore boss_fakewin_ReleaseSemaphore
    #define GetStartupInfo boss_fakewin_GetStartupInfo
    #define CreateProcessA boss_fakewin_CreateProcessA
    #define GetEnvironmentVariableA boss_fakewin_GetEnvironmentVariableA
    #define GetEnvironmentVariableW boss_fakewin_GetEnvironmentVariableW
    #define GetEnvironmentStrings boss_fakewin_GetEnvironmentStrings
    #define GetEnvironmentStringsW boss_fakewin_GetEnvironmentStringsW
    #define GetSystemDirectoryA boss_fakewin_GetSystemDirectoryA
    #define GetSystemDirectoryW boss_fakewin_GetSystemDirectoryW
    #define ReadConsoleA boss_fakewin_ReadConsoleA
    #define ReadConsoleW boss_fakewin_ReadConsoleW
    #define GetSystemTime boss_fakewin_GetSystemTime
    #define SwitchToFiber boss_fakewin_SwitchToFiber
    #define DeleteFiber boss_fakewin_DeleteFiber
    #define ConvertFiberToThread boss_fakewin_ConvertFiberToThread
    #define ConvertThreadToFiber boss_fakewin_ConvertThreadToFiber
    #define CreateFiberEx boss_fakewin_CreateFiberEx
    #define ConvertThreadToFiberEx boss_fakewin_ConvertThreadToFiberEx
    #define CreateFiber boss_fakewin_CreateFiber
    #define ConvertThreadToFiber boss_fakewin_ConvertThreadToFiber
    #define GetConsoleMode boss_fakewin_GetConsoleMode
    #define SetConsoleMode boss_fakewin_SetConsoleMode
    #define ReadFile boss_fakewin_ReadFile
    #define ReadFileEx boss_fakewin_ReadFileEx
    #define GetFileType boss_fakewin_GetFileType
    #define PeekNamedPipe boss_fakewin_PeekNamedPipe
    #define timeGetTime boss_fakewin_timeGetTime
    #define GetFiberData boss_fakewin_GetFiberData
    #define GetCurrentFiber boss_fakewin_GetCurrentFiber
    #define GetVersionExA boss_fakewin_GetVersionExA
    #define GetVersionExW boss_fakewin_GetVersionExW
    #define WSAIoctl boss_fakewin_WSAIoctl
    #define ldap_err2stringW boss_fakewin_ldap_err2stringW
    #define ldap_err2stringA boss_fakewin_ldap_err2stringA
    #define ldap_get_option boss_fakewin_ldap_get_option
    #define ldap_get_optionW boss_fakewin_ldap_get_optionW
    #define ldap_set_option boss_fakewin_ldap_set_option
    #define ldap_set_optionW boss_fakewin_ldap_set_optionW
    #define ldap_initW boss_fakewin_ldap_initW
    #define ldap_init boss_fakewin_ldap_init
    #define ldap_simple_bindW boss_fakewin_ldap_simple_bindW
    #define ldap_simple_bindA boss_fakewin_ldap_simple_bindA
    #define ldap_simple_bind_sW boss_fakewin_ldap_simple_bind_sW
    #define ldap_simple_bind_sA boss_fakewin_ldap_simple_bind_sA
    #define ldap_searchW boss_fakewin_ldap_searchW
    #define ldap_searchA boss_fakewin_ldap_searchA
    #define ldap_search_sW boss_fakewin_ldap_search_sW
    #define ldap_search_sA boss_fakewin_ldap_search_sA
    #define ldap_first_entry boss_fakewin_ldap_first_entry
    #define ldap_get_dnW boss_fakewin_ldap_get_dnW
    #define ldap_get_dn boss_fakewin_ldap_get_dn
    #define ldap_memfreeW boss_fakewin_ldap_memfreeW
    #define ldap_memfree boss_fakewin_ldap_memfree
    #define ldap_first_attributeW boss_fakewin_ldap_first_attributeW
    #define ldap_first_attribute boss_fakewin_ldap_first_attribute
    #define ldap_next_attributeW boss_fakewin_ldap_next_attributeW
    #define ldap_next_attribute boss_fakewin_ldap_next_attribute
    #define ldap_get_values_lenW boss_fakewin_ldap_get_values_lenW
    #define ldap_get_values_len boss_fakewin_ldap_get_values_len
    #define ldap_value_free_len boss_fakewin_ldap_value_free_len
    #define ldap_next_entry boss_fakewin_ldap_next_entry
    #define ldap_msgfree boss_fakewin_ldap_msgfree
    #define ldap_unbind boss_fakewin_ldap_unbind
    #define ldap_unbind_s boss_fakewin_ldap_unbind_s

    // 유니코드여부관련
    #ifdef UNICODE
        #define DeleteFile DeleteFileW
        #define GetTempPath GetTempPathW
        #define FindFirstFile FindFirstFileW
        #define FindNextFile FindNextFileW
        #define CreateMutex CreateMutexW
        #define CreateSemaphore CreateSemaphoreW
        #define CreateProcess CreateProcessW
        #define GetModuleHandle GetModuleHandleW
    #else
        #define DeleteFile DeleteFileA
        #define GetTempPath GetTempPathA
        #define FindFirstFile FindFirstFileA
        #define FindNextFile FindNextFileA
        #define CreateMutex CreateMutexA
        #define CreateSemaphore CreateSemaphoreA
        #define CreateProcess CreateProcessA
        #define GetModuleHandle GetModuleHandleA
    #endif

    #define _access boss_fakewin_access
    #define _waccess boss_fakewin_waccess
    #define _chmod boss_fakewin_chmod
    #define _wchmod boss_fakewin_wchmod
    #define _wfopen boss_fakewin_wfopen
    #define _wfopen_s boss_fakewin_wfopen_s
    #define _fopen boss_fakewin_fopen
    #define _fopen_s boss_fakewin_fopen_s
    #define _fseek boss_fakewin_fseek
    #define _ftell boss_fakewin_ftell
    #define _rewind boss_fakewin_rewind
    #define _fread boss_fakewin_fread
    #define _fwrite boss_fakewin_fwrite
    #define _fgetc boss_fakewin_fgetc
    #define _fgets boss_fakewin_fgets
    #define _ungetc boss_fakewin_ungetc
    #define _fprintf boss_fakewin_fprintf
    #define _fclose boss_fakewin_fclose
    #define _feof boss_fakewin_feof
    #define _ftime_s boss_fakewin_ftime_s
    #define _localtime_s boss_fakewin_localtime_s
    #define _wopen boss_fakewin_wopen
    #define _open boss_fakewin_open
    #define _close boss_fakewin_close
    #define _read boss_fakewin_read
    #define _write boss_fakewin_write
    #define _get_osfhandle boss_fakewin_get_osfhandle
    #define _lseek boss_fakewin_lseek
    #define _lseeki64 boss_fakewin_lseeki64
    #define _lfind boss_fakewin_lfind
    #define _chsize_s boss_fakewin_chsize_s
    #define _stat boss_fakewin_stat
    #define _stat64 boss_fakewin_stat64
    #define _fstat boss_fakewin_fstat
    #define _fstat64 boss_fakewin_fstat64
    #define _wchdir boss_fakewin_wchdir
    #define _wmkdir boss_fakewin_wmkdir
    #define _wrmdir boss_fakewin_wrmdir
    #define _unlink boss_fakewin_unlink
    #define _wunlink boss_fakewin_wunlink
    #define _mktemp_s boss_fakewin_mktemp_s
    #define _wgetenv boss_fakewin_wgetenv
    #define _wgetcwd boss_fakewin_wgetcwd
    #define FD_SET boss_fakewin_FD_SET
    #define FD_ZERO boss_fakewin_FD_ZERO
    #define FD_ISSET boss_fakewin_FD_ISSET
    #define _fileno boss_fakewin_fileno
    #define _getch boss_fakewin_getch
    #define _getdrive boss_fakewin_getdrive
    #define _lrotl boss_fakewin_lrotl
    #define _lrotr boss_fakewin_lrotr
    #define _rotl boss_fakewin_rotl
    #define _setmode boss_fakewin_setmode
    #define stricmp boss_fakewin_stricmp
    #define _stricmp boss_fakewin_stricmp
    #define strnicmp boss_fakewin_strnicmp
    #define _strnicmp boss_fakewin_strnicmp
    #define _mbsicmp boss_fakewin_mbsicmp
    #define _wcsicmp boss_fakewin_wcsicmp
    #define _vscprintf boss_fakewin_vscprintf
    #define vsnprintf_s boss_fakewin_vsnprintf_s
    #define _vsnprintf_s boss_fakewin_vsnprintf_s
    #define _vsnprintf boss_fakewin_vsnprintf
    #define _vsnwprintf boss_fakewin_vsnwprintf
    #define _beginthreadex boss_fakewin_beginthreadex
    #define _fullpath boss_fakewin_fullpath
    #define _ltoa boss_fakewin_ltoa
    #define _ultoa boss_fakewin_ultoa
    #define _isnan boss_fakewin_isnan
    #define _isinf boss_fakewin_isinf
    #define _finite boss_fakewin_finite
    #define _splitpath boss_fakewin_splitpath
    #define _strtoi64 boss_fakewin_strtoi64
    #define _strtoui64 boss_fakewin_strtoui64
    #define _set_errno boss_fakewin_set_errno
    #define _byteswap_ushort boss_fakewin_byteswap_ushort
    #define _byteswap_ulong boss_fakewin_byteswap_ulong
    #define _byteswap_uint64 boss_fakewin_byteswap_uint64
    #define strlen boss_fakewin_strlen
    #define wcslen boss_fakewin_wcslen
    #define strerror boss_fakewin_strerror
    #define strerror_s boss_fakewin_strerror_s
    #define strcpy_s boss_fakewin_strcpy_s
    #define strncpy_s boss_fakewin_strncpy_s
    #define wcscpy_s boss_fakewin_wcscpy_s
    #define wcsncpy_s boss_fakewin_wcsncpy_s
    #define strcpy boss_fakewin_strcpy
    #define strncpy boss_fakewin_strncpy
    #define wcscpy boss_fakewin_wcscpy
    #define wcsncpy boss_fakewin_wcsncpy
    #define _strdup boss_fakewin_strdup
    #define _wcsdup boss_fakewin_wcsdup
    #define strpbrk boss_fakewin_strpbrk
    #define wcspbrk boss_fakewin_wcspbrk
    #define ber_free boss_fakewin_ber_free

    // 명칭재정의관련
    #define access _access
    #define wfopen _wfopen
    #define wfopen_s _wfopen_s
    #define fopen _fopen
    #define fopen_s _fopen_s
    #define fseek _fseek
    #define ftell _ftell
    #define rewind _rewind
    #define fread _fread
    #define fwrite _fwrite
    #define fgetc _fgetc
    #define fgets _fgets
    #define ungetc _ungetc
    #define fprintf _fprintf
    #define fclose _fclose
    #define feof _feof
    #define ftime_s _ftime_s
    #define localtime_s _localtime_s
    #define lseek _lseek
    #define lseeki64 _lseeki64
    #define chsize_s _chsize_s
    #define fileno _fileno
    #define unlink _unlink
    #define ltoa _ltoa
    #define _snprintf boss_snprintf
    #ifdef UNICODE
        #define _vsntprintf _vsnwprintf
    #else
        #define _vsntprintf _vsnprintf
    #endif

    #define fd_set boss_fd_set

    struct boss_fakewin_struct_timeb {
        //long  time;
        //short millitm;
        //short timezone;
        //short dstflag;
        time_t time;
        unsigned short millitm;
    };
    #define _timeb boss_fakewin_struct_timeb

    #undef st_atime
    #undef st_mtime
    #undef st_ctime
    struct boss_fakewin_struct_stat {
        dev_t           st_dev;
        ino_t           st_ino;
        mode_t          st_mode;
        nlink_t         st_nlink;
        uid_t           st_uid;
        gid_t           st_gid;
        dev_t           st_rdev;
        off_t           st_size;
        blksize_t       st_blksize;
        blkcnt_t        st_blocks;
        time_t          st_atime;
        time_t          st_mtime;
        time_t          st_ctime;
    };
    struct boss_fakewin_struct_stat64 {
        _dev_t          st_dev;
        _ino_t          st_ino;
        unsigned short  st_mode;
        short           st_nlink;
        short           st_uid;
        short           st_gid;
        _dev_t          st_rdev;
        __int64         st_size;
        __time64_t      st_atime;
        __time64_t      st_mtime;
        __time64_t      st_ctime;
    };

    #ifdef __cplusplus
        extern "C" {
    #endif
    int boss_fakewin_access(const char*,int);
    int boss_fakewin_waccess(const wchar_t*,int);
    int boss_fakewin_chmod(const char*,int);
    int boss_fakewin_wchmod(const wchar_t*,int);
    FILE* boss_fakewin_wfopen(const wchar_t*,const wchar_t*);
    errno_t boss_fakewin_wfopen_s(FILE**, const wchar_t*, const wchar_t*);
    FILE* boss_fakewin_fopen(char const*, char const*);
    errno_t boss_fakewin_fopen_s(FILE**, char const*, char const*);
    int boss_fakewin_fseek(FILE*,long int,int);
    long int boss_fakewin_ftell(FILE*);
    void boss_fakewin_rewind(FILE*);
    size_t boss_fakewin_fread(void*,size_t,size_t,FILE*);
    size_t boss_fakewin_fwrite(const void*,size_t,size_t,FILE*);
    int boss_fakewin_fgetc(FILE* stream);
    char* boss_fakewin_fgets(char*,int,FILE*);
    int boss_fakewin_ungetc(int,FILE*);
    int boss_fakewin_fprintf(FILE*,const char*, ...);
    int boss_fakewin_fclose(FILE*);
    int boss_fakewin_feof(FILE*);
    errno_t boss_fakewin_ftime_s(struct boss_fakewin_struct_timeb*);
    errno_t boss_fakewin_localtime_s(struct tm*, const time_t*);
    int boss_fakewin_wopen(const wchar_t*, int, int);
    int boss_fakewin_open(const char*, int);
    int boss_fakewin_close(int);
    long boss_fakewin_read(int, void*, unsigned int);
    long boss_fakewin_write(int, const void*, unsigned int);
    intptr_t boss_fakewin_get_osfhandle(int);
    long boss_fakewin_lseek(int, long, int);
    __int64 boss_fakewin_lseeki64(int,__int64,int);
    void* boss_fakewin_lfind(const void*,const void*,unsigned int*,unsigned int,int (*)(const void*, const void*));
    errno_t boss_fakewin_chsize_s(int, __int64);
    int boss_fakewin_stat(const char*,struct boss_fakewin_struct_stat*);
    int boss_fakewin_stat64(const char*,struct boss_fakewin_struct_stat64*);
    int boss_fakewin_fstat(int,struct boss_fakewin_struct_stat*);
    int boss_fakewin_fstat64(int,struct boss_fakewin_struct_stat64*);
    int boss_fakewin_wchdir(const wchar_t*);
    int boss_fakewin_wmkdir(const wchar_t*);
    int boss_fakewin_wrmdir(const wchar_t*);
    int boss_fakewin_unlink(const char*);
    int boss_fakewin_wunlink(const wchar_t*);
    errno_t boss_fakewin_mktemp_s(char*,size_t);
    wchar_t* boss_fakewin_wgetenv(const wchar_t*);
    wchar_t* boss_fakewin_wgetcwd(wchar_t*,int);
    void boss_fakewin_FD_SET(int fd, boss_fd_set* fdset);
    void boss_fakewin_FD_ZERO(boss_fd_set* fdset);
    int boss_fakewin_FD_ISSET(int fd, boss_fd_set* set);
    int boss_fakewin_fileno(FILE*);
    int boss_fakewin_getch();
    int boss_fakewin_getdrive();
    unsigned long boss_fakewin_lrotl(unsigned long,int);
    unsigned long boss_fakewin_lrotr(unsigned long,int);
    unsigned int boss_fakewin_rotl(unsigned int,int);
    int boss_fakewin_setmode(int,int);
    int boss_fakewin_stricmp(const char*,const char*);
    int boss_fakewin_strnicmp(const char*,const char*,size_t);
    int boss_fakewin_mbsicmp(const unsigned char*, const unsigned char*);
    int boss_fakewin_wcsicmp(wchar_t const*,wchar_t const*);
    int boss_fakewin_vscprintf(const char*,va_list);
    int boss_fakewin_vsnprintf_s(char*,size_t,size_t,const char*,va_list);
    int boss_fakewin_vsnprintf(char*,size_t,const char*,va_list);
    int boss_fakewin_vsnwprintf(wchar_t*,size_t,const wchar_t*,va_list);
    uintptr_t boss_fakewin_beginthreadex(void*, unsigned, unsigned (*)(void*), void*, unsigned, unsigned*);
    char* boss_fakewin_fullpath(char*,const char*,size_t);
    char* boss_fakewin_ltoa(long,char*,int);
    char* boss_fakewin_ultoa(unsigned long,char*,int);
    int boss_fakewin_isnan(double);
    int boss_fakewin_isinf(double);
    int boss_fakewin_finite(double);
    void boss_fakewin_splitpath(const char*,char*,char*,char*,char*);
    __int64 boss_fakewin_strtoi64(const char*,char**,int);
    unsigned __int64 boss_fakewin_strtoui64(const char*,char**,int);
    errno_t boss_fakewin_set_errno(int);
    unsigned short boss_fakewin_byteswap_ushort(unsigned short);
    unsigned long boss_fakewin_byteswap_ulong(unsigned long);
    unsigned __int64 boss_fakewin_byteswap_uint64(unsigned __int64);
    size_t boss_fakewin_strlen(const char* str);
    size_t boss_fakewin_wcslen(const wchar_t* str);
    char* boss_fakewin_strerror(int errnum);
    errno_t boss_fakewin_strerror_s(char* buf, rsize_t bufsz, errno_t errnum);
    errno_t boss_fakewin_strcpy_s(char* strDestination, size_t numberOfElements, const char* strSource);
    errno_t boss_fakewin_strncpy_s(char* strDestination, size_t numberOfElements, const char* strSource, size_t count);
    errno_t boss_fakewin_wcscpy_s(wchar_t* strDestination, size_t numberOfElements, const wchar_t* strSource);
    errno_t boss_fakewin_wcsncpy_s(wchar_t* strDestination, size_t numberOfElements, const wchar_t* strSource, size_t count);
    char* boss_fakewin_strcpy(char* strDestination, const char* strSource);
    char* boss_fakewin_strncpy(char* strDestination, const char* strSource, size_t count);
    wchar_t* boss_fakewin_wcscpy(wchar_t* strDestination, const wchar_t* strSource);
    wchar_t* boss_fakewin_wcsncpy(wchar_t* strDestination, const wchar_t* strSource, size_t count);
    char* boss_fakewin_strdup(const char* strSource);
    wchar_t* boss_fakewin_wcsdup(const wchar_t* strSource);
    char* boss_fakewin_strpbrk(const char* str, const char* strCharSet);
    wchar_t* boss_fakewin_wcspbrk(const wchar_t* str, const wchar_t* strCharSet);
    #ifdef __cplusplus
        }
    #endif

    #if BOSS_WINDOWS
        typedef unsigned long u_long;
        typedef long long longlong_t;
        typedef unsigned long long u_longlong_t;
        //typedef __int8 int8_t;
        //typedef __int16 int16_t;
        //typedef __int32 int32_t;
        //typedef __int64 int64_t;
        //typedef unsigned __int8 uint8_t;
        //typedef unsigned __int16 uint16_t;
        //typedef unsigned __int32 uint32_t;
        //typedef unsigned __int64 uint64_t;
        typedef long long intmax_t;
        typedef unsigned long long uintmax_t;
        #define INT8_MIN         (-127i8 - 1)
        #define INT16_MIN        (-32767i16 - 1)
        #define INT32_MIN        (-2147483647i32 - 1)
        #define INT64_MIN        (-9223372036854775807i64 - 1)
        #define INT8_MAX         127i8
        #define INT16_MAX        32767i16
        #define INT32_MAX        2147483647i32
        #define INT64_MAX        9223372036854775807i64
        #define UINT8_MAX        0xffui8
        #define UINT16_MAX       0xffffui16
        #define UINT32_MAX       0xffffffffui32
        #define UINT64_MAX       0xffffffffffffffffui64

        struct timeval {
            long    tv_sec;         /* seconds */
            long    tv_usec;        /* and microseconds */
        };
        #define _TIMEVAL_DEFINED // for mingw32
        #  if defined(_UNICODE) || defined(__UNICODE__)
        #   define _vsntprintf _vsnwprintf
        #  else
        #   define _vsntprintf _vsnprintf
        #  endif
    #elif BOSS_MAC_OSX | BOSS_IPHONE | BOSS_ANDROID
        typedef unsigned long u_long;
        #define __POCC__ 0
        #define __POCC__OLDNAMES
    #endif

    #ifdef  UNICODE                     
        #define __TEXT(quote)                L##quote
        #define LoadLibrary                  LoadLibraryW
        #define GetEnvironmentStrings        GetEnvironmentStringsW
        #define GetSystemDirectory           GetSystemDirectoryW
        #define GetEnvironmentVariable       GetEnvironmentVariableW
        #define ReadConsole                  ReadConsoleW
        #define GetVersionEx                 GetVersionExW
        #define FormatMessage                FormatMessageW
        #define ldap_err2string              ldap_err2stringW
        #define ldap_get_option              ldap_get_optionW
        #define ldap_set_option              ldap_set_optionW
        #define ldap_init                    ldap_initW
        #define ldap_simple_bind             ldap_simple_bindW
        #define ldap_simple_bind_s           ldap_simple_bind_sW
        #define ldap_search                  ldap_searchW
        #define ldap_search_s                ldap_search_sW
        #define ldap_get_dn                  ldap_get_dnW
        #define ldap_memfree                 ldap_memfreeW
        #define ldap_first_attribute         ldap_first_attributeW
        #define ldap_next_attribute          ldap_next_attributeW
        #define ldap_get_values_len          ldap_get_values_lenW
        #define _tcslen                      wcslen
        #define _tcsclen                     wcslen
        #define _tcscpy_s                    wcscpy_s
        #define _tcscpy                      wcscpy
        #define _tcsdup                      _wcsdup
        #define _tcspbrk                     wcspbrk
    #else   /* UNICODE */               
        #define __TEXT(quote)                quote
        #define LoadLibrary                  LoadLibraryA
        #define GetEnvironmentStringsA       GetEnvironmentStrings
        #define GetSystemDirectory           GetSystemDirectoryA
        #define GetEnvironmentVariable       GetEnvironmentVariableA
        #define ReadConsole                  ReadConsoleA
        #define GetVersionEx                 GetVersionExA
        #define FormatMessage                FormatMessageA
        #define ldap_err2string              ldap_err2stringA
        #define ldap_simple_bind             ldap_simple_bindA
        #define ldap_simple_bind_s           ldap_simple_bind_sA
        #define ldap_search                  ldap_searchA
        #define ldap_search_s                ldap_search_sA    
        #define _tcslen                      strlen
        #define _tcsclen                     strlen
        #define _tcscpy_s                    strcpy_s
        #define _tcscpy                      strcpy
        #define _tcsdup                      _strdup
        #define _tcspbrk                     strpbrk
    #endif /* UNICODE */
    #define TEXT(quote)                      __TEXT(quote) 

    #define IN
    #define OUT
    #define WINAPI
    #define APIENTRY                         WINAPI
    #define CALLBACK
    #define _inline
    #define __cdecl
    #define _MAX_PATH                        260
    #define _MAX_FNAME                       256
    #define VER_MINORVERSION                 0x0000001
    #define VER_MAJORVERSION                 0x0000002
    #define VER_BUILDNUMBER                  0x0000004
    #define VER_PLATFORMID                   0x0000008
    #define VER_SERVICEPACKMINOR             0x0000010
    #define VER_SERVICEPACKMAJOR             0x0000020
    #define VER_SUITENAME                    0x0000040
    #define VER_PRODUCT_TYPE                 0x0000080
    #define VER_EQUAL                        1
    #define VER_GREATER                      2
    #define VER_GREATER_EQUAL                3
    #define VER_LESS                         4
    #define VER_LESS_EQUAL                   5
    #define VER_AND                          6
    #define VER_OR                           7
    #define VER_CONDITION_MASK               7
    #define VER_NUM_BITS_PER_CONDITION_MASK  3
    #define VER_SET_CONDITION(_m_, _t_, _c_)
    #define MAX_PATH                         260
    #define _TRUNCATE                        ((size_t) -1)
    #define INVALID_HANDLE_VALUE             ((HANDLE) -1)
    #define INVALID_SOCKET                   ((SOCKET) ~0)
    #define STD_INPUT_HANDLE                 ((DWORD) -10)
    #define SOCKET_ERROR                     (-1)
    #define AF_UNSPEC                        0
    #define AF_INET                          2
    #define AF_INET6                         23
    #define PF_INET                          AF_INET
    #define PF_INET6                         AF_INET6
    #define PF_UNSPEC                        AF_UNSPEC
    #define SOCK_STREAM                      1
    #define SOCK_DGRAM                       2
    #define IPPROTO_IP                       0
    #define IPPROTO_ICMP                     1
    #define IPPROTO_TCP                      6
    #define IPPROTO_UDP                      17
    #define WSADESCRIPTION_LEN               256
    #define WSASYS_STATUS_LEN                128
    #define FORMAT_MESSAGE_ALLOCATE_BUFFER   0x00000100
    #define FORMAT_MESSAGE_IGNORE_INSERTS    0x00000200
    #define FORMAT_MESSAGE_FROM_HMODULE      0x00000800
    #define FORMAT_MESSAGE_FROM_SYSTEM       0x00001000
    #define ERROR_FILE_NOT_FOUND             2L
    #define ERROR_ACCESS_DENIED              5L
    #define ERROR_INVALID_HANDLE             6L
    #define ERROR_NOT_ENOUGH_MEMORY          8L
    #define ERROR_NO_MORE_FILES              18L
    #define ERROR_SHARING_VIOLATION          32L
    #define ERROR_INSUFFICIENT_BUFFER        122L
    #define ERROR_DIRECTORY                  267L
    #define ERROR_INVALID_FLAGS              1004L
    #define ERROR_NO_UNICODE_TRANSLATION     1113L
    #define EVENTLOG_ERROR_TYPE              0x0001
    #define WM_COPYDATA                      0x004A
    #define SOL_SOCKET                       0xffff
    #define SO_SNDBUF                        0x1001
    #define SO_RCVBUF                        0x1002
    #define SO_SNDLOWAT                      0x1003
    #define SO_RCVLOWAT                      0x1004
    #define SO_SNDTIMEO                      0x1005
    #define SO_RCVTIMEO                      0x1006
    #define SO_ERROR                         0x1007
    #define SO_TYPE                          0x1008
    #define SO_KEEPALIVE                     0x0008
    #define WSAEINTR                         10004L
    #define WSAEBADF                         10009L
    #define WSAEACCES                        10013L
    #define WSAEINVAL                        10022L
    #define WSAEMFILE                        10024L
    #define WSAEFAULT                        10014L
    #define WSAEWOULDBLOCK                   10035L
    #define WSAEINPROGRESS                   10036L
    #define WSAEALREADY                      10037L
    #define WSAENOTSOCK                      10038L
    #define WSAEDESTADDRREQ                  10039L
    #define WSAEMSGSIZE                      10040L
    #define WSAEPROTOTYPE                    10041L
    #define WSAENOPROTOOPT                   10042L
    #define WSAEPROTONOSUPPORT               10043L
    #define WSAESOCKTNOSUPPORT               10044L
    #define WSAEOPNOTSUPP                    10045L
    #define WSAEPFNOSUPPORT                  10046L
    #define WSAEAFNOSUPPORT                  10047L
    #define WSAEADDRINUSE                    10048L
    #define WSAEADDRNOTAVAIL                 10049L
    #define WSAENETDOWN                      10050L
    #define WSAENETUNREACH                   10051L
    #define WSAENETRESET                     10052L
    #define WSAECONNABORTED                  10053L
    #define WSAECONNRESET                    10054L
    #define WSAENOBUFS                       10055L
    #define WSAEISCONN                       10056L
    #define WSAENOTCONN                      10057L
    #define WSAESHUTDOWN                     10058L
    #define WSAETOOMANYREFS                  10059L
    #define WSAETIMEDOUT                     10060L
    #define WSAECONNREFUSED                  10061L
    #define WSAELOOP                         10062L
    #define WSAENAMETOOLONG                  10063L
    #define WSAEHOSTDOWN                     10064L
    #define WSAEHOSTUNREACH                  10065L
    #define WSAENOTEMPTY                     10066L
    #define WSAEPROCLIM                      10067L
    #define WSAEUSERS                        10068L
    #define WSAEDQUOT                        10069L
    #define WSAESTALE                        10070L
    #define WSAEREMOTE                       10071L
    #define WSASYSNOTREADY                   10091L
    #define WSAVERNOTSUPPORTED               10092L
    #define WSANOTINITIALISED                10093L
    #define WSAHOST_NOT_FOUND                11001L
    #define WSATRY_AGAIN                     11002L
    #define WSANO_RECOVERY                   11003L
    #define WSANO_DATA                       11004L
    #define WSA_NOT_ENOUGH_MEMORY            (ERROR_NOT_ENOUGH_MEMORY)
    #define IOC_VENDOR                       0x18000000
    #define EAI_MEMORY                       (ERROR_NOT_ENOUGH_MEMORY)
    #define EAI_NODATA                       WSAHOST_NOT_FOUND
    #define MB_OK                            0x00000000L
    #define MB_ICONHAND                      0x00000010L
    #define MB_ICONERROR                     MB_ICONHAND
    #define MB_PRECOMPOSED                   0x00000001  // use precomposed chars
    #define MB_COMPOSITE                     0x00000002  // use composite chars
    #define MB_USEGLYPHCHARS                 0x00000004  // use glyph chars, not ctrl chars
    #define MB_ERR_INVALID_CHARS             0x00000008  // error for invalid chars
    #define WC_ERR_INVALID_CHARS             0x00000080
    #define IOCPARM_MASK                     0x7f            /* parameters must be < 128 bytes */
    #define IOC_IN                           0x80000000      /* copy in parameters */
    #define _IOW(x,y,t)                      (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
    #define FIONBIO                          _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
    #define LANG_NEUTRAL                     0x00
    #define SUBLANG_DEFAULT                  0x01    // user default
    #define READ_CONTROL                     (0x00020000L)
    #define STANDARD_RIGHTS_READ             (READ_CONTROL)
    #define SYNCHRONIZE                      (0x00100000L)
    #define KEY_QUERY_VALUE                  (0x0001)
    #define KEY_ENUMERATE_SUB_KEYS           (0x0008)
    #define KEY_NOTIFY                       (0x0010)
    #define KEY_READ                         ((STANDARD_RIGHTS_READ|KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS|KEY_NOTIFY) & (~SYNCHRONIZE))
    #define REG_SZ                           ( 1 )
    #define HKEY_CURRENT_USER                (( HKEY ) (ULONG_PTR)((LONG)0x80000001) )
    #define HKEY_LOCAL_MACHINE               (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
    #define PAGE_READWRITE                   0x04
    #define PAGE_READONLY                    0x02
    #define SECTION_MAP_WRITE                0x0002
    #define SECTION_MAP_READ                 0x0004
    #define FILE_MAP_WRITE                   SECTION_MAP_WRITE
    #define FILE_MAP_READ                    SECTION_MAP_READ
    #define MAXIMUM_REPARSE_DATA_BUFFER_SIZE ( 16 * 1024 )
    #define GENERIC_READ                     (0x80000000L)
    #define OPEN_EXISTING                    3
    #define IO_REPARSE_TAG_MOUNT_POINT       (0xA0000003L)
    #define FILE_SHARE_READ                  0x00000001
    #define FILE_SHARE_WRITE                 0x00000002
    #define FILE_SHARE_DELETE                0x00000004
    #define FILE_ATTRIBUTE_READONLY          0x00000001
    #define FILE_ATTRIBUTE_HIDDEN            0x00000002
    #define FILE_ATTRIBUTE_DIRECTORY         0x00000010
    #define FILE_ATTRIBUTE_REPARSE_POINT     0x00000400
    #define FILE_FLAG_OPEN_REPARSE_POINT     0x00200000
    #define FILE_FLAG_BACKUP_SEMANTICS       0x02000000
    #define INVALID_FILE_ATTRIBUTES          ((DWORD)-1)
    #define S_IFDIR                          0x4000          /* directory */
    #define S_IFREG                          0x8000          /* regular */
    #define S_IREAD                          0x0100          /* read permission, owner */
    #define S_IWRITE                         0x0080          /* write permission, owner */
    #define MOVEFILE_REPLACE_EXISTING        0x00000001
    #define MOVEFILE_COPY_ALLOWED            0x00000002
    #define INFINITE                         0xFFFFFFFF  // Infinite timeout
    #define STATUS_WAIT_0                    ((DWORD) 0x00000000L)
    #define WAIT_FAILED                      ((DWORD) 0xFFFFFFFF)
    #define WAIT_OBJECT_0                    ((STATUS_WAIT_0 ) + 0)
    #define FALSE                            0
    #define TRUE                             1
    #define CP_ACP                           0
    #define CP_UTF8                          65001
    #define UOI_NAME                         2
    #define O_BINARY                         0x8000
    #define O_NOINHERIT                      0x0080
    #define O_TEMPORARY                      0x0040  /* temporary file bit */
    #define O_RDONLY                         0x0000
    #define O_WRONLY                         0x0001  /* open for writing only */
    #define O_RDWR                           0x0002
    #define O_APPEND                         0x0008  /* writes done at eof */
    #define O_CREAT                          0x0100  /* create and open file */
    #define O_TRUNC                          0x0200  /* open and truncate */
    #define O_EXCL                           0x0400    /* Open only if the file does not exist. */
    #define O_UNKNOWN                        0x0800
    #define O_TEXT                           0x4000
    #define O_CLOEXEC                        0x80000
    #define _O_BINARY                        O_BINARY
    #define _O_NOINHERIT                     O_NOINHERIT
    #define _O_TEMPORARY                     O_TEMPORARY
    #define _O_RDONLY                        O_RDONLY
    #define _O_WRONLY                        O_WRONLY
    #define _O_RDWR                          O_RDWR
    #define _O_APPEND                        O_APPEND
    #define _O_CREAT                         O_CREAT
    #define _O_TRUNC                         O_TRUNC
    #define _O_EXCL                          O_EXCL
    #define _O_UNKNOWN                       O_UNKNOWN
    #define _O_TEXT                          O_TEXT
    #define _O_CLOEXEC                       O_CLOEXEC
    #define _SS_MAXSIZE                      128                 // Maximum size
    #define _SS_ALIGNSIZE                    (sizeof(__int64)) // Desired alignment
    #define _SS_PAD1SIZE                     (_SS_ALIGNSIZE - sizeof (short))
    #define _SS_PAD2SIZE                     (_SS_MAXSIZE - (sizeof (short) + _SS_PAD1SIZE + _SS_ALIGNSIZE))
    #define MAKEWORD(a, b)                   ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
    #define MAKELONG(a, b)                   ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
    #define LOWORD(l)                        ((WORD)(((DWORD_PTR)(l)) & 0xffff))
    #define HIWORD(l)                        ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
    #define LOBYTE(w)                        ((BYTE)(((DWORD_PTR)(w)) & 0xff))
    #define HIBYTE(w)                        ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
    #define MAKELANGID(p, s)                 ((((WORD)(s)) << 10) | (WORD)(p))
    #define PRIMARYLANGID(lgid)              ((WORD)(lgid) & 0x3ff)
    #define SUBLANGID(lgid)                  ((WORD)(lgid) >> 10)
    #define _T(STR)                          STR
    #define ANYSIZE_ARRAY                    1
    #define MAX_PROTOCOL_CHAIN               7
    #define WSAPROTOCOL_LEN                  255
    #define BI_RGB                           0L
    #define BI_RLE8                          1L
    #define BI_RLE4                          2L
    #define BI_BITFIELDS                     3L
    #define BI_JPEG                          4L
    #define BI_PNG                           5L
    #define DIB_RGB_COLORS                   0 /* color table in RGBs */
    #define DIB_PAL_COLORS                   1 /* color table in palette indices */
    #define CREATE_BREAKAWAY_FROM_JOB        0x01000000
    #define CREATE_PRESERVE_CODE_AUTHZ_LEVEL 0x02000000
    #define CREATE_DEFAULT_ERROR_MODE        0x04000000
    #define CREATE_NO_WINDOW                 0x08000000
    #define DEBUG_PROCESS                    0x00000001
    #define DEBUG_ONLY_THIS_PROCESS          0x00000002
    #define CREATE_SUSPENDED                 0x00000004
    #define DETACHED_PROCESS                 0x00000008
    #define INET_ADDRSTRLEN                  16
    #define INET6_ADDRSTRLEN                 46
    #define VER_PLATFORM_WIN32s              0
    #define VER_PLATFORM_WIN32_WINDOWS       1
    #define VER_PLATFORM_WIN32_NT            2
    #define INADDR_NONE                      0xffffffff
    #define TCP_NODELAY                      0x0001
    #define LDAP_PORT                        389
    #define LDAP_SSL_PORT                    636
    #define LDAP_GC_PORT                     3268
    #define LDAP_SSL_GC_PORT                 3269
    #define LDAP_VERSION1                    1
    #define LDAP_VERSION2                    2
    #define LDAP_VERSION3                    3
    #define LDAP_VERSION                     LDAP_VERSION2
    #define LDAP_API_INFO_VERSION            1
    #define LDAP_API_VERSION                 2004
    #define LDAP_VERSION_MIN                 2
    #define LDAP_VERSION_MAX                 3
    #define LDAP_VENDOR_NAME                 "Microsoft Corporation."
    #define LDAP_VENDOR_NAME_W               L"Microsoft Corporation."
    #define LDAP_VENDOR_VERSION              510
    #define LDAP_OPT_API_INFO                0x00
    #define LDAP_OPT_DESC                    0x01
    #define LDAP_OPT_DEREF                   0x02
    #define LDAP_OPT_SIZELIMIT               0x03
    #define LDAP_OPT_TIMELIMIT               0x04
    #define LDAP_OPT_THREAD_FN_PTRS          0x05
    #define LDAP_OPT_REBIND_FN               0x06
    #define LDAP_OPT_REBIND_ARG              0x07
    #define LDAP_OPT_REFERRALS               0x08
    #define LDAP_OPT_RESTART                 0x09
    #define LDAP_OPT_SSL                     0x0a
    #define LDAP_OPT_IO_FN_PTRS              0x0b
    #define LDAP_OPT_CACHE_FN_PTRS           0x0d
    #define LDAP_OPT_CACHE_STRATEGY          0x0e
    #define LDAP_OPT_CACHE_ENABLE            0x0f
    #define LDAP_OPT_REFERRAL_HOP_LIMIT      0x10
    #define LDAP_OPT_PROTOCOL_VERSION        0x11        // known by two names.
    #define LDAP_OPT_VERSION                 0x11
    #define LDAP_OPT_API_FEATURE_INFO        0x15
    #define LDAP_OPT_HOST_NAME               0x30
    #define LDAP_OPT_ERROR_NUMBER            0x31
    #define LDAP_OPT_ERROR_STRING            0x32
    #define LDAP_OPT_SERVER_ERROR            0x33
    #define LDAP_OPT_SERVER_EXT_ERROR        0x34
    #define LDAP_OPT_HOST_REACHABLE          0x3E
    #define LDAP_OPT_PING_KEEP_ALIVE         0x36
    #define LDAP_OPT_PING_WAIT_TIME          0x37
    #define LDAP_OPT_PING_LIMIT              0x38
    #define LDAP_OPT_DNSDOMAIN_NAME          0x3B    // return DNS name of domain
    #define LDAP_OPT_GETDSNAME_FLAGS         0x3D    // flags for DsGetDcName
    #define LDAP_OPT_PROMPT_CREDENTIALS      0x3F    // prompt for creds? currently
    #define LDAP_OPT_AUTO_RECONNECT          0x91    // enable/disable autoreconnect
    #define LDAP_OPT_SSPI_FLAGS              0x92    // flags to pass to InitSecurityContext
    #define LDAP_SCOPE_BASE                  0x00
    #define LDAP_SCOPE_ONELEVEL              0x01
    #define LDAP_SCOPE_SUBTREE               0x02
#ifndef RC_INVOKED
    #define _SECURECRT_ERRCODE_VALUES_DEFINED
    #define    EPERM                             1        /* Operation not permitted */
    #define    ENOENT                            2        /* No such file or directory */
    #define    ESRCH                             3        /* No such process */
    #define    EINTR                             4        /* Interrupted system call */
    #define    EIO                               5        /* Input/output error */
    #define    ENXIO                             6        /* Device not configured */
    #define    E2BIG                             7        /* Argument list too long */
    #define    ENOEXEC                           8        /* Exec format error */
    #define    EBADF                             9        /* Bad file descriptor */
    #define    ECHILD                           10        /* No child processes */
    #define    EDEADLK                          11        /* Resource deadlock avoided */
    #define    ENOMEM                           12        /* Cannot allocate memory */
    #define    EACCES                           13        /* Permission denied */
    #define    EFAULT                           14        /* Bad address */
    #define    ENOTBLK                          15        /* Block device required */
    #define    EBUSY                            16        /* Device busy */
    #define    EEXIST                           17        /* File exists */
    #define    EXDEV                            18        /* Cross-device link */
    #define    ENODEV                           19        /* Operation not supported by device */
    #define    ENOTDIR                          20        /* Not a directory */
    #define    EISDIR                           21        /* Is a directory */
    #define    EINVAL                           22        /* Invalid argument */
    #define    ENFILE                           23        /* Too many open files in system */
    #define    EMFILE                           24        /* Too many open files */
    #define    ENOTTY                           25        /* Inappropriate ioctl for device */
    #define    ETXTBSY                          26        /* Text file busy */
    #define    EFBIG                            27        /* File too large */
    #define    ENOSPC                           28        /* No space left on device */
    #define    ESPIPE                           29        /* Illegal seek */
    #define    EROFS                            30        /* Read-only file system */
    #define    EMLINK                           31        /* Too many links */
    #define    EPIPE                            32        /* Broken pipe */
    #define    EDOM                             33        /* Numerical argument out of domain */
    #define    ERANGE                           34        /* Result too large */
    #define    EAGAIN                           35        /* Resource temporarily unavailable */
    #define    EWOULDBLOCK                      EAGAIN    /* Operation would block */
    #define    EINPROGRESS                      36        /* Operation now in progress */
    #define    EALREADY                         37        /* Operation already in progress */
    #define    ENOTSOCK                         38        /* Socket operation on non-socket */
    #define    EDESTADDRREQ                     39        /* Destination address required */
    #define    EMSGSIZE                         40        /* Message too long */
    #define    EPROTOTYPE                       41        /* Protocol wrong type for socket */
    #define    ENOPROTOOPT                      42        /* Protocol not available */
    #define    EPROTONOSUPPORT                  43        /* Protocol not supported */
    #define    ESOCKTNOSUPPORT                  44        /* Socket type not supported */
    #define    EOPNOTSUPP                       45        /* Operation not supported on socket */
    #define    EPFNOSUPPORT                     46        /* Protocol family not supported */
    #define    EAFNOSUPPORT                     47        /* Address family not supported by protocol family */
    #define    EADDRINUSE                       48        /* Address already in use */
    #define    EADDRNOTAVAIL                    49        /* Can't assign requested address */
    #define    ENETDOWN                         50        /* Network is down */
    #define    ENETUNREACH                      51        /* Network is unreachable */
    #define    ENETRESET                        52        /* Network dropped connection on reset */
    #define    ECONNABORTED                     53        /* Software caused connection abort */
    #define    ECONNRESET                       54        /* Connection reset by peer */
    #define    ENOBUFS                          55        /* No buffer space available */
    #define    EISCONN                          56        /* Socket is already connected */
    #define    ENOTCONN                         57        /* Socket is not connected */
    #define    ESHUTDOWN                        58        /* Can't send after socket shutdown */
    #define    ETOOMANYREFS                     59        /* Too many references: can't splice */
    #define    ETIMEDOUT                        60        /* Connection timed out */
    #define    ECONNREFUSED                     61        /* Connection refused */
    #define    ELOOP                            62        /* Too many levels of symbolic links */
    #define    ENAMETOOLONG                     63        /* File name too long */
    #define    EHOSTDOWN                        64        /* Host is down */
    #define    EHOSTUNREACH                     65        /* No route to host */
    #define    ENOTEMPTY                        66        /* Directory not empty */
    #define    EPROCLIM                         67        /* Too many processes */
    #define    EUSERS                           68        /* Too many users */
    #define    EDQUOT                           69        /* Disc quota exceeded */
    #define    ESTALE                           70        /* Stale NFS file handle */
    #define    EREMOTE                          71        /* Too many levels of remote in path */
    #define    EBADRPC                          72        /* RPC struct is bad */
    #define    ERPCMISMATCH                     73        /* RPC version wrong */
    #define    EPROGUNAVAIL                     74        /* RPC prog. not avail */
    #define    EPROGMISMATCH                    75        /* Program version wrong */
    #define    EPROCUNAVAIL                     76        /* Bad procedure for program */
    #define    ENOLCK                           77        /* No locks available */
    #define    ENOSYS                           78        /* Function not implemented */
    #define    EFTYPE                           79        /* Inappropriate file type or format */
    #define EILSEQ                           42
    #define STRUNCATE                        80
#endif
#ifndef sys_nerr
    #define sys_nerr                         EILSEQ
#endif
    #define WSAEVENT                         HANDLE
    #define FD_MAX_EVENTS                    10
    #define FD_ALL_EVENTS                    ((1 << FD_MAX_EVENTS) - 1)
    #define FAR
    #define NEAR
    #define WSA_INVALID_EVENT                ((WSAEVENT)NULL)
    #define FD_READ_BIT                      0
    #define FD_READ                          (1 << FD_READ_BIT)
    #define FD_WRITE_BIT                     1
    #define FD_WRITE                         (1 << FD_WRITE_BIT)
    #define FD_OOB_BIT                       2
    #define FD_OOB                           (1 << FD_OOB_BIT)
    #define FD_ACCEPT_BIT                    3
    #define FD_ACCEPT                        (1 << FD_ACCEPT_BIT)
    #define FD_CONNECT_BIT                   4
    #define FD_CONNECT                       (1 << FD_CONNECT_BIT)
    #define FD_CLOSE_BIT                     5
    #define FD_CLOSE                         (1 << FD_CLOSE_BIT)
    #define FD_QOS_BIT                       6
    #define FD_QOS                           (1 << FD_QOS_BIT)
    #define FD_GROUP_QOS_BIT                 7
    #define FD_GROUP_QOS                     (1 << FD_GROUP_QOS_BIT)
    #define FD_ROUTING_INTERFACE_CHANGE_BIT  8
    #define FD_ROUTING_INTERFACE_CHANGE      (1 << FD_ROUTING_INTERFACE_CHANGE_BIT)
    #define FD_ADDRESS_LIST_CHANGE_BIT       9
    #define FD_ADDRESS_LIST_CHANGE           (1 << FD_ADDRESS_LIST_CHANGE_BIT)
    #define FD_MAX_EVENTS                    10
    #define FD_ALL_EVENTS                    ((1 << FD_MAX_EVENTS) - 1)
    #define FILE_TYPE_UNKNOWN                0x0000
    #define FILE_TYPE_DISK                   0x0001
    #define FILE_TYPE_CHAR                   0x0002
    #define FILE_TYPE_PIPE                   0x0003
    #define FILE_TYPE_REMOTE                 0x8000
    #define WAIT_TIMEOUT                     258L    // dderror
    #define INADDR_ANY                       (u_long)0x00000000
    #define INADDR_LOOPBACK                  0x7f000001
    #define INADDR_BROADCAST                 (u_long)0xffffffff
    #define INADDR_NONE                      0xffffffff
    #define ENABLE_PROCESSED_INPUT           0x0001
    #define ENABLE_LINE_INPUT                0x0002
    #define ENABLE_ECHO_INPUT                0x0004
    #define ENABLE_WINDOW_INPUT              0x0008
    #define ENABLE_MOUSE_INPUT               0x0010
    #define ENABLE_INSERT_MODE               0x0020
    #define ENABLE_QUICK_EDIT_MODE           0x0040
    #define ENABLE_EXTENDED_FLAGS            0x0080
    #define ENABLE_AUTO_POSITION             0x0100
    #define ENABLE_VIRTUAL_TERMINAL_INPUT    0x0200
    #define _WSAIO(x,y)                      (IOC_VOID|(x)|(y))
    #define _WSAIOR(x,y)                     (IOC_OUT|(x)|(y))
    #define _WSAIOW(x,y)                     (IOC_IN|(x)|(y))
    #define _WSAIORW(x,y)                    (IOC_INOUT|(x)|(y))

    #define bswap32(x) _byteswap_ulong(x)
    #define bswap16(x) _byteswap_ushort(x)
#ifdef bswap32
    #undef ntohl
    #undef htonl
    #define ntohl(x) bswap32(x)
    #define htonl(x) bswap32(x)
#endif
#ifdef bswap16
    #undef ntohs
    #undef htons
    #define ntohs(x) bswap16(x)
    #define htons(x) bswap16(x)
#endif

    typedef struct _WSANETWORKEVENTS {
           long lNetworkEvents;
           int iErrorCode[FD_MAX_EVENTS];
    } WSANETWORKEVENTS, FAR * LPWSANETWORKEVENTS;

    typedef struct _OSVERSIONINFOA {
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        CHAR   szCSDVersion[ 128 ];     // Maintenance string for PSS usage
    } OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;

    typedef struct _OSVERSIONINFOW {
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        WCHAR  szCSDVersion[ 128 ];     // Maintenance string for PSS usage
    } OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW, RTL_OSVERSIONINFOW, *PRTL_OSVERSIONINFOW;
     #ifdef UNICODE
    typedef OSVERSIONINFOW OSVERSIONINFO;
    typedef POSVERSIONINFOW POSVERSIONINFO;
    typedef LPOSVERSIONINFOW LPOSVERSIONINFO;
    #else
    typedef OSVERSIONINFOA OSVERSIONINFO;
    typedef POSVERSIONINFOA POSVERSIONINFO;
    typedef LPOSVERSIONINFOA LPOSVERSIONINFO;
    #endif // UNICODE

    typedef struct _OVERLAPPED {
        ULONG_PTR Internal;
        ULONG_PTR InternalHigh;
        union {
            struct {
                DWORD Offset;
                DWORD OffsetHigh;
            } DUMMYSTRUCTNAME;
            PVOID Pointer;
        } DUMMYUNIONNAME;
        HANDLE  hEvent;
    } OVERLAPPED, *LPOVERLAPPED;
    
    typedef struct _WSAOVERLAPPED {
        ULONG_PTR Internal;
        ULONG_PTR InternalHigh;
        DWORD Offset;
        DWORD OffsetHigh;
        HANDLE  hEvent;
    } WSAOVERLAPPED, *LPWSAOVERLAPPED;

    typedef enum {
        LDAP_SUCCESS                    =   0x00,
        LDAP_OPERATIONS_ERROR           =   0x01,
        LDAP_PROTOCOL_ERROR             =   0x02,
        LDAP_TIMELIMIT_EXCEEDED         =   0x03,
        LDAP_SIZELIMIT_EXCEEDED         =   0x04,
        LDAP_COMPARE_FALSE              =   0x05,
        LDAP_COMPARE_TRUE               =   0x06,
        LDAP_AUTH_METHOD_NOT_SUPPORTED  =   0x07,
        LDAP_STRONG_AUTH_REQUIRED       =   0x08,
        LDAP_REFERRAL_V2                =   0x09,
        LDAP_PARTIAL_RESULTS            =   0x09,
        LDAP_REFERRAL                   =   0x0a,
        LDAP_ADMIN_LIMIT_EXCEEDED       =   0x0b,
        LDAP_UNAVAILABLE_CRIT_EXTENSION =   0x0c,
        LDAP_CONFIDENTIALITY_REQUIRED   =   0x0d,
        LDAP_SASL_BIND_IN_PROGRESS      =   0x0e,
        LDAP_NO_SUCH_ATTRIBUTE          =   0x10,
        LDAP_UNDEFINED_TYPE             =   0x11,
        LDAP_INAPPROPRIATE_MATCHING     =   0x12,
        LDAP_CONSTRAINT_VIOLATION       =   0x13,
        LDAP_ATTRIBUTE_OR_VALUE_EXISTS  =   0x14,
        LDAP_INVALID_SYNTAX             =   0x15,
        LDAP_NO_SUCH_OBJECT             =   0x20,
        LDAP_ALIAS_PROBLEM              =   0x21,
        LDAP_INVALID_DN_SYNTAX          =   0x22,
        LDAP_IS_LEAF                    =   0x23,
        LDAP_ALIAS_DEREF_PROBLEM        =   0x24,
        LDAP_INAPPROPRIATE_AUTH         =   0x30,
        LDAP_INVALID_CREDENTIALS        =   0x31,
        LDAP_INSUFFICIENT_RIGHTS        =   0x32,
        LDAP_BUSY                       =   0x33,
        LDAP_UNAVAILABLE                =   0x34,
        LDAP_UNWILLING_TO_PERFORM       =   0x35,
        LDAP_LOOP_DETECT                =   0x36,
        LDAP_SORT_CONTROL_MISSING       =   0x3C,
        LDAP_OFFSET_RANGE_ERROR         =   0x3D,
        LDAP_NAMING_VIOLATION           =   0x40,
        LDAP_OBJECT_CLASS_VIOLATION     =   0x41,
        LDAP_NOT_ALLOWED_ON_NONLEAF     =   0x42,
        LDAP_NOT_ALLOWED_ON_RDN         =   0x43,
        LDAP_ALREADY_EXISTS             =   0x44,
        LDAP_NO_OBJECT_CLASS_MODS       =   0x45,
        LDAP_RESULTS_TOO_LARGE          =   0x46,
        LDAP_AFFECTS_MULTIPLE_DSAS      =   0x47,
        LDAP_VIRTUAL_LIST_VIEW_ERROR    =   0x4c,
        LDAP_OTHER                      =   0x50,
        LDAP_SERVER_DOWN                =   0x51,
        LDAP_LOCAL_ERROR                =   0x52,
        LDAP_ENCODING_ERROR             =   0x53,
        LDAP_DECODING_ERROR             =   0x54,
        LDAP_TIMEOUT                    =   0x55,
        LDAP_AUTH_UNKNOWN               =   0x56,
        LDAP_FILTER_ERROR               =   0x57,
        LDAP_USER_CANCELLED             =   0x58,
        LDAP_PARAM_ERROR                =   0x59,
        LDAP_NO_MEMORY                  =   0x5a,
        LDAP_CONNECT_ERROR              =   0x5b,
        LDAP_NOT_SUPPORTED              =   0x5c,
        LDAP_NO_RESULTS_RETURNED        =   0x5e,
        LDAP_CONTROL_NOT_FOUND          =   0x5d,
        LDAP_MORE_RESULTS_TO_RETURN     =   0x5f,
        LDAP_CLIENT_LOOP                =   0x60,
        LDAP_REFERRAL_LIMIT_EXCEEDED    =   0x61
    } LDAP_RETCODE;


    typedef struct tagCOPYDATASTRUCT {
        ULONG_PTR dwData;
        DWORD cbData;
        PVOID lpData;
    } COPYDATASTRUCT, *PCOPYDATASTRUCT;

    typedef enum _GET_FILEEX_INFO_LEVELS {
        GetFileExInfoStandard,
        GetFileExMaxInfoLevel
    } GET_FILEEX_INFO_LEVELS;

    typedef enum _FINDEX_INFO_LEVELS { 
      FindExInfoStandard,
      FindExInfoBasic,
      FindExInfoMaxInfoLevel
    } FINDEX_INFO_LEVELS;

    typedef enum _FINDEX_SEARCH_OPS { 
      FindExSearchNameMatch,
      FindExSearchLimitToDirectories,
      FindExSearchLimitToDevices
    } FINDEX_SEARCH_OPS;

    typedef struct _OSVERSIONINFOEX {
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        CHAR  szCSDVersion[128];
        WORD  wServicePackMajor;
        WORD  wServicePackMinor;
        WORD  wSuiteMask;
        BYTE  wProductType;
        BYTE  wReserved;
    } OSVERSIONINFOEX, *LPOSVERSIONINFOEX;

    typedef struct boss_fakewin_struct_WSAData {
            WORD                    wVersion;
            WORD                    wHighVersion;
            #if BOSS_X64
                unsigned short          iMaxSockets;
                unsigned short          iMaxUdpDg;
                char *                  lpVendorInfo;
                char                    szDescription[WSADESCRIPTION_LEN+1];
                char                    szSystemStatus[WSASYS_STATUS_LEN+1];
            #else
                char                    szDescription[WSADESCRIPTION_LEN+1];
                char                    szSystemStatus[WSASYS_STATUS_LEN+1];
                unsigned short          iMaxSockets;
                unsigned short          iMaxUdpDg;
                char *                  lpVendorInfo;
            #endif
    } WSADATA, *LPWSADATA;

    #define WSAData                      boss_fakewin_struct_WSAData
    #define MSG_PEEK                     0x2             /* peek at incoming message */

    typedef struct boss_fakewin_struct_in_addr {
        union {
            struct { UCHAR s_b1,s_b2,s_b3,s_b4; } S_un_b;
            struct { USHORT s_w1,s_w2; } S_un_w;
            #if BOSS_WINDOWS
                ULONG S_addr;
            #else
                __uint32_t S_addr;
            #endif
        } S_un;
        #define s_addr  S_un.S_addr /* can be used for most tcp & ip code */
        #define s_host  S_un.S_un_b.s_b2    // host on imp
        #define s_net   S_un.S_un_b.s_b1    // network
        #define s_imp   S_un.S_un_w.s_w2    // imp
        #define s_impno S_un.S_un_b.s_b4    // imp #
        #define s_lh    S_un.S_un_b.s_b3    // logical host
    } IN_ADDR, *PIN_ADDR, *LPIN_ADDR;
    #define in_addr boss_fakewin_struct_in_addr

    typedef struct boss_fakewin_struct_in6_addr {
        union {
            UCHAR Byte[16];
            USHORT Word[8];
        } u;
        #define s6_addr u.Byte
    } IN6_ADDR, *PIN6_ADDR, *LPIN6_ADDR;
    #define in6_addr boss_fakewin_struct_in6_addr

    typedef struct {
        union {
            struct {
                ULONG Zone : 28;
                ULONG Level : 4;
            };
            ULONG Value;
        };
    } SCOPE_ID, *PSCOPE_ID;

    typedef struct boss_fakewin_struct_sockaddr {
        #if BOSS_WINDOWS
            #if (_WIN32_WINNT < 0x0600)
                u_short sa_family;
            #else
                ADDRESS_FAMILY sa_family;           // Address family.
            #endif
        #elif BOSS_LINUX | BOSS_ANDROID
            u_short sa_family;
        #else
            __uint8_t sa_len;
            __uint8_t sa_family;
        #endif
        CHAR sa_data[14];                   // Up to 14 bytes of direct address.
    } SOCKADDR, *PSOCKADDR, *LPSOCKADDR;
    #define sockaddr boss_fakewin_struct_sockaddr

    struct boss_fakewin_struct_sockaddr_in {
        #if BOSS_WINDOWS
            short sin_family;
        #elif BOSS_LINUX | BOSS_ANDROID
            u_short sin_family;
        #else
            __uint8_t sin_len;
            __uint8_t sin_family;
        #endif
        u_short sin_port;
        struct  boss_fakewin_struct_in_addr sin_addr;
        char    sin_zero[8];
    };
    #define sockaddr_in boss_fakewin_struct_sockaddr_in

    typedef struct boss_fakewin_struct_sockaddr_storage {
        ADDRESS_FAMILY ss_family;      // address family
        CHAR __ss_pad1[_SS_PAD1SIZE];  // 6 byte pad, this is to make
                                        //   implementation specific pad up to
                                        //   alignment field that follows explicit
                                        //   in the data structure
        __int64 __ss_align;            // Field to force desired structure
        CHAR __ss_pad2[_SS_PAD2SIZE];  // 112 byte pad to achieve desired size;
                                        //   _SS_MAXSIZE value minus size of
                                        //   ss_family, __ss_pad1, and
                                        //   __ss_align fields is 112
    } SOCKADDR_STORAGE_LH, *PSOCKADDR_STORAGE_LH, *LPSOCKADDR_STORAGE_LH;
    #define sockaddr_storage boss_fakewin_struct_sockaddr_storage

    typedef struct _FILETIME {
        DWORD dwLowDateTime;
        DWORD dwHighDateTime;
    } FILETIME, *LPFILETIME;

    typedef struct _SYSTEMTIME {
      WORD wYear;
      WORD wMonth;
      WORD wDayOfWeek;
      WORD wDay;
      WORD wHour;
      WORD wMinute;
      WORD wSecond;
      WORD wMilliseconds;
    } SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

    typedef struct _SECURITY_ATTRIBUTES {
        DWORD nLength;
        LPVOID lpSecurityDescriptor;
        BOOL bInheritHandle;
    } SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

    typedef union _LARGE_INTEGER {
      struct {
        DWORD LowPart;
        LONG  HighPart;
      };
      struct {
        DWORD LowPart;
        LONG  HighPart;
      } u;
      LONGLONG QuadPart;
    } LARGE_INTEGER, *PLARGE_INTEGER;

    typedef union _ULARGE_INTEGER {
      struct {
        DWORD LowPart;
        DWORD HighPart;
      };
      struct {
        DWORD LowPart;
        DWORD HighPart;
      } u;
      ULONGLONG QuadPart;
    } ULARGE_INTEGER, *PULARGE_INTEGER;

    typedef struct tagBITMAP {
      LONG   bmType;
      LONG   bmWidth;
      LONG   bmHeight;
      LONG   bmWidthBytes;
      WORD   bmPlanes;
      WORD   bmBitsPixel;
      LPVOID bmBits;
    } BITMAP, *PBITMAP;

    typedef struct tagBITMAPINFOHEADER {
      DWORD biSize;
      LONG  biWidth;
      LONG  biHeight;
      WORD  biPlanes;
      WORD  biBitCount;
      DWORD biCompression;
      DWORD biSizeImage;
      LONG  biXPelsPerMeter;
      LONG  biYPelsPerMeter;
      DWORD biClrUsed;
      DWORD biClrImportant;
    } BITMAPINFOHEADER, *PBITMAPINFOHEADER;

    typedef struct tagRGBQUAD {
      BYTE rgbBlue;
      BYTE rgbGreen;
      BYTE rgbRed;
      BYTE rgbReserved;
    } RGBQUAD;

    typedef struct tagBITMAPINFO {
      BITMAPINFOHEADER bmiHeader;
      RGBQUAD          bmiColors[1];
    } BITMAPINFO, *PBITMAPINFO, *LPBITMAPINFO;

    typedef struct tagDIBSECTION {
      BITMAP           dsBm;
      BITMAPINFOHEADER dsBmih;
      DWORD            dsBitfields[3];
      HANDLE           dshSection;
      DWORD            dsOffset;
    } DIBSECTION, *PDIBSECTION;

    typedef struct _GUID {
        DWORD Data1;
        WORD  Data2;
        WORD  Data3;
        BYTE  Data4[8];
    } GUID;

    typedef struct _STARTUPINFO {
      DWORD  cb;
      LPTSTR lpReserved;
      LPTSTR lpDesktop;
      LPTSTR lpTitle;
      DWORD  dwX;
      DWORD  dwY;
      DWORD  dwXSize;
      DWORD  dwYSize;
      DWORD  dwXCountChars;
      DWORD  dwYCountChars;
      DWORD  dwFillAttribute;
      DWORD  dwFlags;
      WORD   wShowWindow;
      WORD   cbReserved2;
      LPBYTE lpReserved2;
      HANDLE hStdInput;
      HANDLE hStdOutput;
      HANDLE hStdError;
    } STARTUPINFO, *LPSTARTUPINFO;

    typedef struct _PROCESS_INFORMATION {
      HANDLE hProcess;
      HANDLE hThread;
      DWORD  dwProcessId;
      DWORD  dwThreadId;
    } PROCESS_INFORMATION, *LPPROCESS_INFORMATION;

    typedef struct _RTL_CRITICAL_SECTION {
        UNKNOWN DebugInfo;
        LONG LockCount;
        LONG RecursionCount;
        HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
        HANDLE LockSemaphore;
        ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
    } RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;
    typedef RTL_CRITICAL_SECTION CRITICAL_SECTION, *LPCRITICAL_SECTION;

    typedef struct _SID_IDENTIFIER_AUTHORITY {
        BYTE Value[6];
    } SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;

    typedef struct _SID {
        BYTE  Revision;
        BYTE  SubAuthorityCount;
        SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
        DWORD SubAuthority[ANYSIZE_ARRAY];
    } SID, *PSID;

    typedef struct _SYSTEM_INFO {
        union {
            DWORD dwOemId;          // Obsolete field...do not use
            struct {
                WORD wProcessorArchitecture;
                WORD wReserved;
            } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
        DWORD dwPageSize;
        LPVOID lpMinimumApplicationAddress;
        LPVOID lpMaximumApplicationAddress;
        DWORD_PTR dwActiveProcessorMask;
        DWORD dwNumberOfProcessors;
        DWORD dwProcessorType;
        DWORD dwAllocationGranularity;
        WORD wProcessorLevel;
        WORD wProcessorRevision;
    } SYSTEM_INFO, *LPSYSTEM_INFO;

    typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
        DWORD dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD nFileSizeHigh;
        DWORD nFileSizeLow;
    } WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

    typedef struct _WIN32_FIND_DATAA {
        DWORD dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD nFileSizeHigh;
        DWORD nFileSizeLow;
        DWORD dwReserved0;
        DWORD dwReserved1;
        CHAR  cFileName[MAX_PATH];
        CHAR  cAlternateFileName[14];
        #if BOSS_MAC_OSX
            DWORD dwFileType;
            DWORD dwCreatorType;
            WORD  wFinderFlags;
        #endif
    } WIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
    #define WIN32_FIND_DATA WIN32_FIND_DATAA

    typedef struct _WIN32_FIND_DATAW {
        DWORD dwFileAttributes;
        FILETIME ftCreationTime;
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        DWORD nFileSizeHigh;
        DWORD nFileSizeLow;
        DWORD dwReserved0;
        DWORD dwReserved1;
        WCHAR cFileName[MAX_PATH];
        WCHAR cAlternateFileName[14];
        #if BOSS_MAC_OSX
            DWORD dwFileType;
            DWORD dwCreatorType;
            WORD  wFinderFlags;
        #endif
    } WIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

    typedef struct _WSABUF {
        ULONG len;     /* the length of the buffer */
        CHAR* buf;     /* the pointer to the buffer */
    } WSABUF, *LPWSABUF;

    typedef struct boss_fakewin_struct_sockaddr_in6 {
        ADDRESS_FAMILY sin6_family; // AF_INET6.
        USHORT sin6_port;           // Transport level port number.
        ULONG  sin6_flowinfo;       // IPv6 flow information.
        IN6_ADDR sin6_addr;         // IPv6 address.
        union {
            ULONG sin6_scope_id;     // Set of interfaces for a scope.
            SCOPE_ID sin6_scope_struct;
        };
    } SOCKADDR_IN6_LH, *PSOCKADDR_IN6_LH, *LPSOCKADDR_IN6_LH;
    #define sockaddr_in6 boss_fakewin_struct_sockaddr_in6

    typedef struct boss_fakewin_struct_hostent {
        char*         h_name;
        char**        h_aliases;
        #if BOSS_WINDOWS
            short     h_addrtype;
            short     h_length;
        #else
            int       h_addrtype;
            int       h_length;
        #endif
        char**        h_addr_list;
        #define h_addr  h_addr_list[0]
    } HOSTENT, *PHOSTENT, *LPHOSTENT;
    #define hostent boss_fakewin_struct_hostent

    typedef struct boss_fakewin_struct_servent {
        char*         s_name;
        char**        s_aliases;
        #if BOSS_WINDOWS
            short     s_port;
        #else
            int       s_port;
        #endif
        char*         s_proto;
    } SERVENT, *PSERVENT, *LPSERVENT;
    #define servent boss_fakewin_struct_servent

    typedef struct boss_fakewin_struct_addrinfo {
        struct hostent *ai_hostent;
        struct servent *ai_servent;
        struct sockaddr_in ai_addr_in;
        struct sockaddr *ai_addr;
        size_t ai_addrlen;
        int ai_family;
        int ai_socktype;
        int ai_protocol;
        long ai_port;
        struct boss_fakewin_struct_addrinfo *ai_next;
        #if BOSS_LINUX | BOSS_MAC_OSX | BOSS_IPHONE | BOSS_ANDROID
            int ai_flags;
            char* ai_canonname;
        #endif
    } ADDRINFOA, *PADDRINFOA;
    #define addrinfo boss_fakewin_struct_addrinfo

    typedef struct _WSAPROTOCOLCHAIN {
        int   ChainLen;
        DWORD ChainEntries[MAX_PROTOCOL_CHAIN];
    } WSAPROTOCOLCHAIN, *LPWSAPROTOCOLCHAIN;

    typedef struct _WSAPROTOCOL_INFO {
        DWORD            dwServiceFlags1;
        DWORD            dwServiceFlags2;
        DWORD            dwServiceFlags3;
        DWORD            dwServiceFlags4;
        DWORD            dwProviderFlags;
        GUID             ProviderId;
        DWORD            dwCatalogEntryId;
        WSAPROTOCOLCHAIN ProtocolChain;
        int              iVersion;
        int              iAddressFamily;
        int              iMaxSockAddr;
        int              iMinSockAddr;
        int              iSocketType;
        int              iProtocol;
        int              iProtocolMaxOffset;
        int              iNetworkByteOrder;
        int              iSecurityScheme;
        DWORD            dwMessageSize;
        DWORD            dwProviderReserved;
        TCHAR            szProtocol[WSAPROTOCOL_LEN+1];
    } WSAPROTOCOL_INFO, *LPWSAPROTOCOL_INFO;

    typedef struct ldap {
        struct {
            UINT_PTR sb_sd;
            UCHAR Reserved1[(10*sizeof(ULONG))+1];
            ULONG_PTR sb_naddr;
            UCHAR Reserved2[(6*sizeof(ULONG))];
        } ld_sb;
        PCHAR   ld_host;
        ULONG   ld_version;
        UCHAR   ld_lberoptions;
        ULONG   ld_deref;
        ULONG   ld_timelimit;
        ULONG   ld_sizelimit;
        ULONG   ld_errno;
        PCHAR   ld_matched;
        PCHAR   ld_error;
        ULONG   ld_msgid;
        UCHAR Reserved3[(6*sizeof(ULONG))+1];
        ULONG   ld_cldaptries;
        ULONG   ld_cldaptimeout;
        ULONG   ld_refhoplimit;
        ULONG   ld_options;
    } LDAP, * PLDAP;

    typedef struct ldapmsg {
        ULONG lm_msgid;             // message number for given connection
        ULONG lm_msgtype;           // message type of the form LDAP_RES_xxx
        PVOID lm_ber;               // ber form of message
        struct ldapmsg *lm_chain;   // pointer to next result value
        struct ldapmsg *lm_next;    // pointer to next message
        ULONG lm_time;
        PLDAP   Connection;         // connection from which we received response
        PVOID   Request;            // owning request (opaque structure)
        ULONG   lm_returncode;      // server's return code
        USHORT  lm_referral;        // index of referral within ref table
        BOOLEAN lm_chased;          // has referral been chased already?
        BOOLEAN lm_eom;             // is this the last entry for this message?
        BOOLEAN ConnectionReferenced; // is the Connection still valid?
    } LDAPMessage, *PLDAPMessage;

    typedef struct berelement {
        PCHAR   opaque;     // this is an opaque structure used just for
    } BerElement;

    #define NULLBER ((BerElement *) 0)

    typedef struct berval {
        ULONG  bv_len;
        PCHAR  bv_val;
    } LDAP_BERVAL, * PLDAP_BERVAL, BERVAL, * PBERVAL, BerValue;


    typedef struct _CONSOLE_READCONSOLE_CONTROL {
        ULONG nLength;
        ULONG nInitialChars;
        ULONG dwCtrlWakeupMask;
        ULONG dwControlKeyState;
    } CONSOLE_READCONSOLE_CONTROL, *PCONSOLE_READCONSOLE_CONTROL;

    typedef VOID (WINAPI *PFIBER_START_ROUTINE)(LPVOID lpFiberParameter);
    typedef PFIBER_START_ROUTINE LPFIBER_START_ROUTINE;

    typedef LPVOID (WINAPI *PFIBER_CALLOUT_ROUTINE)(LPVOID lpParameter);


    typedef VOID (WINAPI *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode,
            DWORD dwNumberOfBytesTransfered,
            LPOVERLAPPED lpOverlapped);


    typedef ULONG   SERVICETYPE;
    typedef unsigned int GROUP;

    typedef struct _flowspec
    {
        ULONG       TokenRate;              /* In Bytes/sec */
        ULONG       TokenBucketSize;        /* In Bytes */
        ULONG       PeakBandwidth;          /* In Bytes/sec */
        ULONG       Latency;                /* In microseconds */
        ULONG       DelayVariation;         /* In microseconds */
        SERVICETYPE ServiceType;
        ULONG       MaxSduSize;             /* In Bytes */
        ULONG       MinimumPolicedSize;     /* In Bytes */
        
    } FLOWSPEC, *PFLOWSPEC, * LPFLOWSPEC;

    typedef struct _QualityOfService
    {
        FLOWSPEC      SendingFlowspec;       /* the flow spec for data sending */
        FLOWSPEC      ReceivingFlowspec;     /* the flow spec for data receiving */
        WSABUF        ProviderSpecific;      /* additional provider specific stuff */
    } QOS, FAR * LPQOS;

    typedef int (CALLBACK * LPCONDITIONPROC)(
            IN LPWSABUF lpCallerId,
            IN LPWSABUF lpCallerData,
            IN OUT LPQOS lpSQOS,
            IN OUT LPQOS lpGQOS,
            IN LPWSABUF lpCalleeId,
            IN LPWSABUF lpCalleeData,
            OUT GROUP FAR * g,
            IN DWORD_PTR dwCallbackData);

    typedef void (CALLBACK * LPWSAOVERLAPPED_COMPLETION_ROUTINE)(
            IN DWORD dwError,
            IN DWORD cbTransferred,
            IN LPWSAOVERLAPPED lpOverlapped,
            IN DWORD dwFlags);

    #ifdef __cplusplus
        extern "C" {
    #endif
        SOCKET boss_fakewin_socket(int, int, int);
        int boss_fakewin_connect(SOCKET, const struct boss_fakewin_struct_sockaddr*, int);
        int boss_fakewin_getsockopt(SOCKET, int, int, char*, int*);
        int boss_fakewin_setsockopt(SOCKET, int, int, const char*, int);
        int boss_fakewin_select(int, boss_fd_set*, boss_fd_set*, boss_fd_set*, struct timeval*);
        int boss_fakewin_recv(SOCKET, char*, int, int);
        int boss_fakewin_send(SOCKET, const char*, int, int);
        int boss_fakewin_closesocket(SOCKET);
        int boss_fakewin_ioctlsocket(SOCKET,long,u_long*);
        struct boss_fakewin_struct_hostent* boss_fakewin_gethostbyname(const char*);
        unsigned short boss_fakewin_htons(unsigned short);
        int boss_fakewin_getaddrinfo(PCSTR,PCSTR,const ADDRINFOA*,PADDRINFOA*);
        void boss_fakewin_freeaddrinfo(struct addrinfo*);
        const char* boss_fakewin_gai_strerror(int);
        unsigned long boss_fakewin_inet_addr(const char *cp);
        int boss_fakewin_bind(SOCKET, const struct boss_fakewin_struct_sockaddr*, int);
        int boss_fakewin_getsockname(SOCKET, struct boss_fakewin_struct_sockaddr*, int*);
        int boss_fakewin_getpeername(SOCKET, struct boss_fakewin_struct_sockaddr*, int*);
        int boss_fakewin_sendto(SOCKET, const char*, int, int, const struct boss_fakewin_struct_sockaddr*, int);
        int boss_fakewin_recvfrom(SOCKET, char *, int, int, struct boss_fakewin_struct_sockaddr*, int*);
        SOCKET boss_fakewin_accept(SOCKET, struct boss_fakewin_struct_sockaddr*, int*);
        int boss_fakewin_listen(SOCKET s, int backlog);
        struct boss_fakewin_struct_servent* boss_fakewin_getservbyname(const char*, const char*);
        char* boss_fakewin_inet_ntoa(struct boss_fakewin_struct_in_addr in);
        int boss_fakewin_shutdown(SOCKET, int);
        void boss_fakewin_exit(int);
        int boss_fakewin_getpid();
        errno_t boss_fakewin_mbstowcs_s(size_t*,wchar_t*,size_t,const char*,size_t);
        errno_t boss_fakewin_wcstombs_s(size_t*,char*,size_t,const wchar_t*,size_t);

        HANDLE boss_fakewin_CreateFileW(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
        HANDLE boss_fakewin_CreateFileMapping(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCTSTR);
        BOOL boss_fakewin_CreateDirectory(LPCSTR,LPSECURITY_ATTRIBUTES);
        BOOL boss_fakewin_RemoveDirectory(LPCSTR);
        BOOL boss_fakewin_CloseHandle(HANDLE);
        LPVOID boss_fakewin_MapViewOfFile(HANDLE,DWORD,DWORD,DWORD,size_t);
        BOOL boss_fakewin_UnmapViewOfFile(LPCVOID);
        HANDLE boss_fakewin_CreateThread(LPSECURITY_ATTRIBUTES,size_t,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
        BOOL boss_fakewin_DeregisterEventSource(HANDLE);
        BOOL boss_fakewin_DeviceIoControl(HANDLE,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
        DWORD boss_fakewin_ExpandEnvironmentStringsA(LPCSTR,LPSTR,DWORD);
        DWORD boss_fakewin_ExpandEnvironmentStringsW(LPCWSTR,LPWSTR,DWORD);
        BOOL boss_fakewin_FindClose(HANDLE);
        HANDLE boss_fakewin_FindFirstFileA(LPCSTR,LPWIN32_FIND_DATAA);
        HANDLE boss_fakewin_FindFirstFileW(LPCWSTR,LPWIN32_FIND_DATAW);
        HANDLE boss_fakewin_FindFirstFileExA(LPCSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
        BOOL boss_fakewin_FindNextFileA(HANDLE,LPWIN32_FIND_DATAA);
        BOOL boss_fakewin_FindNextFileW(HANDLE,LPWIN32_FIND_DATAW);
        HWND boss_fakewin_FindWindow(LPCTSTR,LPCTSTR);
        BOOL boss_fakewin_FlushConsoleInputBuffer(HANDLE);
        BOOL boss_fakewin_FlushFileBuffers(HANDLE);
        DWORD boss_fakewin_FormatMessageA(DWORD,LPCVOID,DWORD,DWORD,LPSTR,DWORD,va_list*);
        DWORD boss_fakewin_FormatMessageW(DWORD,LPCVOID,DWORD,DWORD,LPWSTR,DWORD,va_list*);
        HANDLE boss_fakewin_GetCurrentProcess();
        HANDLE boss_fakewin_GetCurrentThread();
        DWORD boss_fakewin_GetCurrentThreadId();
        BOOL boss_fakewin_GetExitCodeThread(HANDLE,LPDWORD);
        DWORD boss_fakewin_GetFileAttributes(LPCSTR);
        DWORD boss_fakewin_GetFileAttributesW(LPCWSTR);
        BOOL boss_fakewin_GetFileAttributesExA(LPCSTR,GET_FILEEX_INFO_LEVELS,LPVOID);
        BOOL boss_fakewin_GetFileAttributesExW(LPCWSTR,GET_FILEEX_INFO_LEVELS,LPVOID);
        DWORD boss_fakewin_GetFullPathNameA(LPCSTR,DWORD,LPSTR,LPSTR*);
        DWORD boss_fakewin_GetFullPathNameW(LPCWSTR,DWORD,LPWSTR,LPWSTR*);
        DWORD boss_fakewin_GetLastError();
        void boss_fakewin_SetLastError(DWORD);
        HMODULE boss_fakewin_LoadLibraryA(LPCSTR);
        HMODULE boss_fakewin_LoadLibraryW(LPCWSTR);
        BOOL boss_fakewin_FreeLibrary(HMODULE);
        HMODULE boss_fakewin_GetModuleHandleA(LPCSTR);
        HMODULE boss_fakewin_GetModuleHandleW(LPCWSTR);
        DWORD boss_fakewin_GetModuleFileName(HMODULE,LPSTR,DWORD);
        FARPROC boss_fakewin_GetProcAddress(HMODULE,LPCSTR);
        BOOL boss_fakewin_GetProcessAffinityMask(HANDLE,PDWORD_PTR,PDWORD_PTR);
        HWINSTA boss_fakewin_GetProcessWindowStation();
        BOOL boss_fakewin_GetProcessTimes(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);
        DWORD boss_fakewin_GetShortPathNameW(LPCWSTR,LPWSTR,DWORD);
        HANDLE boss_fakewin_GetStdHandle(DWORD);
        void boss_fakewin_GetSystemInfo(LPSYSTEM_INFO);
        void boss_fakewin_GetNativeSystemInfo(LPSYSTEM_INFO);
        void boss_fakewin_GetSystemTimeAsFileTime(LPFILETIME);
        BOOL boss_fakewin_SystemTimeToFileTime(const SYSTEMTIME*,LPFILETIME);
        DWORD boss_fakewin_GetTickCount();
        BOOL boss_fakewin_GetUserObjectInformationW(HANDLE,int,PVOID,DWORD,LPDWORD);
        HLOCAL boss_fakewin_LocalFree(HLOCAL);
        int boss_fakewin_MessageBox(HWND,LPCTSTR,LPCTSTR,UINT);
        BOOL boss_fakewin_CopyFile(LPCSTR,LPCSTR,BOOL);
        BOOL boss_fakewin_DeleteFileA(LPCSTR);
        BOOL boss_fakewin_MoveFileEx(LPCSTR,LPCSTR,DWORD);
        BOOL boss_fakewin_MoveFileExW(LPCWSTR,LPCWSTR,DWORD);
        DWORD boss_fakewin_GetTempPathA(DWORD,LPSTR);
        UINT boss_fakewin_GetTempFileNameA(LPCSTR,LPCSTR,UINT,LPSTR);
        int boss_fakewin_MultiByteToWideChar(UINT,DWORD,LPCSTR,int,LPWSTR,int);
        int boss_fakewin_WideCharToMultiByte(UINT,DWORD,LPCWSTR,int,LPSTR,int,LPCSTR,LPBOOL);
        LONG boss_fakewin_RegCloseKey(HKEY);
        HANDLE boss_fakewin_RegisterEventSource(LPCTSTR,LPCTSTR);
        LONG boss_fakewin_RegOpenKeyExW(HKEY,LPCWSTR,DWORD,REGSAM,PHKEY);
        LONG boss_fakewin_RegQueryValueExW(HKEY,LPCWSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
        BOOL boss_fakewin_ReportEvent(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCTSTR*,LPVOID);
        PVOID boss_fakewin_SecureZeroMemory(PVOID,size_t);
        LRESULT boss_fakewin_SendMessage(HWND,UINT,WPARAM,LPARAM);
        BOOL boss_fakewin_SetFileAttributesW(LPCWSTR,DWORD);
        void boss_fakewin_Sleep(DWORD);
        void boss_fakewin_SleepEx(DWORD,BOOL);
        BOOL boss_fakewin_VerifyVersionInfo(LPOSVERSIONINFOEX,DWORD,DWORDLONG);
        BOOL boss_fakewin_SetEvent(HANDLE);
        BOOL boss_fakewin_ResetEvent(HANDLE);
        HANDLE boss_fakewin_CreateEvent(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCTSTR);
        HANDLE boss_fakewin_CreateEventW(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCWSTR);
        DWORD boss_fakewin_WaitForSingleObject(HANDLE,DWORD);
        DWORD boss_fakewin_WaitForMultipleObjects(DWORD, const HANDLE*, BOOL, DWORD);
        void boss_fakewin_DeleteCriticalSection(LPCRITICAL_SECTION);
        BOOL boss_fakewin_TryEnterCriticalSection(LPCRITICAL_SECTION);
        void boss_fakewin_EnterCriticalSection(LPCRITICAL_SECTION);
        void boss_fakewin_InitializeCriticalSection(LPCRITICAL_SECTION);
        void boss_fakewin_LeaveCriticalSection(LPCRITICAL_SECTION);
        int boss_fakewin_WSAGetLastError();
        void boss_fakewin_WSASetLastError(int);
        int boss_fakewin_WSACleanup();
        int boss_fakewin_WSAStartup(WORD,LPWSADATA);
        INT boss_fakewin_WSAStringToAddressA(LPSTR,INT,LPWSAPROTOCOL_INFO,LPSOCKADDR,LPINT);
        BOOL boss_fakewin_QueryPerformanceFrequency(LARGE_INTEGER*);
        BOOL boss_fakewin_QueryPerformanceCounter(LARGE_INTEGER*);
        void boss_fakewin_ZeroMemory(PVOID,ULONG_PTR);
        int boss_fakewin_GetObject(HGDIOBJ,int,LPVOID);
        BOOL boss_fakewin_DeleteObject(HGDIOBJ);
        HDC boss_fakewin_GetDC(HWND);
        int boss_fakewin_ReleaseDC(HWND,HDC);
        HBITMAP boss_fakewin_CreateDIBSection(HDC,const BITMAPINFO*,UINT,VOID**,HANDLE,DWORD);
        DWORD boss_fakewin_TlsAlloc(void);
        BOOL boss_fakewin_TlsFree(DWORD dwTlsIndex);
        LPVOID boss_fakewin_TlsGetValue(DWORD dwTlsIndex);
        BOOL boss_fakewin_TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
        HANDLE boss_fakewin_CreateMutexA(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
        BOOL boss_fakewin_ReleaseMutex(HANDLE);
        HANDLE boss_fakewin_CreateSemaphoreA(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCSTR);
        BOOL boss_fakewin_ReleaseSemaphore(HANDLE,LONG,LPLONG);
        void boss_fakewin_GetStartupInfo(LPSTARTUPINFO);
        BOOL boss_fakewin_CreateProcessA(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,
            BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFO,LPPROCESS_INFORMATION);
        LPCH boss_fakewin_GetEnvironmentStrings(void);
        LPWCH boss_fakewin_GetEnvironmentStringsW(void);
        UINT boss_fakewin_GetSystemDirectoryA(LPSTR lpBuffer, UINT uSize);
        UINT boss_fakewin_GetSystemDirectoryW(LPWSTR lpBuffer, UINT uSize);
        UINT boss_fakewin_GetEnvironmentVariableA(LPCSTR lpName, LPSTR lpBuffer, UINT uSize);
        UINT boss_fakewin_GetEnvironmentVariableW(LPCWSTR lpName, LPWSTR lpBuffer, UINT uSize);        
        BOOL boss_fakewin_ReadConsoleA(HANDLE, LPVOID, DWORD, LPDWORD, PCONSOLE_READCONSOLE_CONTROL);
        BOOL boss_fakewin_ReadConsoleW(HANDLE, LPVOID, DWORD, LPDWORD, PCONSOLE_READCONSOLE_CONTROL);
        VOID boss_fakewin_GetSystemTime(LPSYSTEMTIME);
        VOID boss_fakewin_SwitchToFiber(LPVOID);
        VOID boss_fakewin_DeleteFiber(LPVOID);
        BOOL boss_fakewin_ConvertFiberToThread(void);
        LPVOID boss_fakewin_ConvertThreadToFiber(LPVOID);
        LPVOID boss_fakewin_CreateFiberEx(SIZE_T, SIZE_T, DWORD, LPFIBER_START_ROUTINE, LPVOID);
        LPVOID boss_fakewin_ConvertThreadToFiberEx(LPVOID, DWORD);
        LPVOID boss_fakewin_CreateFiber(SIZE_T, LPFIBER_START_ROUTINE, LPVOID);
        LPVOID boss_fakewin_ConvertThreadToFiber(LPVOID);
        BOOL boss_fakewin_GetConsoleMode(HANDLE, LPDWORD);
        BOOL boss_fakewin_SetConsoleMode(HANDLE, LPDWORD);
        BOOL boss_fakewin_ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
        BOOL boss_fakewin_ReadFileEx(HANDLE, LPVOID, DWORD, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE);
        DWORD boss_fakewin_GetFileType(HANDLE);
        BOOL boss_fakewin_PeekNamedPipe(HANDLE, LPVOID, DWORD, LPDWORD, LPDWORD, LPDWORD);
        DWORD boss_fakewin_timeGetTime(void);
        PVOID boss_fakewin_GetFiberData(void);
        PVOID boss_fakewin_GetCurrentFiber(void);
        BOOL boss_fakewin_GetVersionExA(LPOSVERSIONINFOA);
        BOOL boss_fakewin_GetVersionExW(LPOSVERSIONINFOW);
        int boss_fakewin_WSAIoctl(SOCKET,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
        PWCHAR boss_fakewin_ldap_err2stringW( ULONG err );
        PCHAR boss_fakewin_ldap_err2stringA( ULONG err );
        ULONG boss_fakewin_ldap_get_option( LDAP *ld, int option, void *outvalue );
        ULONG boss_fakewin_ldap_get_optionW( LDAP *ld, int option, void *outvalue );
        ULONG boss_fakewin_ldap_set_option( LDAP *ld, int option, const void *outvalue );
        ULONG boss_fakewin_ldap_set_optionW( LDAP *ld, int option, const void *outvalue );
        LDAP* boss_fakewin_ldap_initW(const PWSTR HostName, ULONG PortNumber);
        LDAP* boss_fakewin_ldap_init(const PSTR HostName, ULONG PortNumber);
        ULONG boss_fakewin_ldap_simple_bindW( LDAP *ld, PWSTR dn, PWSTR passwd );
        ULONG boss_fakewin_ldap_simple_bindA( LDAP *ld, PSTR dn, PSTR passwd );
        ULONG boss_fakewin_ldap_simple_bind_sW( LDAP *ld, PWSTR dn, PWSTR passwd );
        ULONG boss_fakewin_ldap_simple_bind_sA( LDAP *ld, PSTR dn, PSTR passwd );
        ULONG boss_fakewin_ldap_searchW(LDAP *ld, const PWSTR base, ULONG scope, const PWSTR filter, PZPWSTR attrs, LONG attrsonly);
        ULONG boss_fakewin_ldap_searchA(LDAP *ld, const PSTR base, ULONG scope, const PSTR filter, PZPSTR attrs, LONG attrsonly);
        ULONG boss_fakewin_ldap_search_sW(LDAP *ld, const PWSTR base, ULONG scope, const PWSTR filter, PZPWSTR attrs, LONG attrsonly, LDAPMessage** res);
        ULONG boss_fakewin_ldap_search_sA(LDAP *ld, const PSTR base, ULONG scope, const PSTR filter, PZPSTR attrs, LONG attrsonly, LDAPMessage** res);
        LDAPMessage* boss_fakewin_ldap_first_entry(LDAP *ld, LDAPMessage *res);
        PWCHAR boss_fakewin_ldap_get_dnW(LDAP *ld, LDAPMessage *entry);
        PCHAR boss_fakewin_ldap_get_dn(LDAP *ld, LDAPMessage *entry);
        VOID boss_fakewin_ldap_memfreeW(PWCHAR Block);
        VOID boss_fakewin_ldap_memfree(PCHAR Block);
        PWCHAR boss_fakewin_ldap_first_attributeW(LDAP *ld, LDAPMessage* entry, BerElement** ptr);
        PCHAR boss_fakewin_ldap_first_attribute(LDAP *ld, LDAPMessage* entry, BerElement** ptr);
        PWCHAR boss_fakewin_ldap_next_attributeW(LDAP *ld, LDAPMessage* entry, BerElement* ptr);
        PCHAR boss_fakewin_ldap_next_attribute(LDAP *ld, LDAPMessage* entry, BerElement* ptr);
        struct berval** boss_fakewin_ldap_get_values_lenW(LDAP *ExternalHandle, LDAPMessage* Message, const PWSTR attr);
        struct berval** boss_fakewin_ldap_get_values_len(LDAP *ExternalHandle, LDAPMessage* Message, const PSTR attr);
        ULONG boss_fakewin_ldap_value_free_len(struct berval **vals);
        LDAPMessage* boss_fakewin_ldap_next_entry(LDAP *ld, LDAPMessage *entry);
        ULONG boss_fakewin_ldap_msgfree(LDAPMessage *res);
        ULONG boss_fakewin_ldap_unbind(LDAP *ld);
        ULONG boss_fakewin_ldap_unbind_s(LDAP *ld);
        VOID boss_fakewin_ber_free(BerElement *pBerElement, INT fbuf);

    #ifdef __cplusplus
        }
    #endif
#endif

#ifdef __cplusplus
    extern "C" {
#endif
    DWORD boss_fakewin_GetCurrentDirectoryW(DWORD,LPWSTR);
#ifdef __cplusplus
    }
#endif
