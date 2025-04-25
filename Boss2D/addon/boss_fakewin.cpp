#include <boss.h>

#include <iostream>
#include <functional>
#include <cctype>

#if BOSS_WINDOWS_MINGW
    #include <cmath>
#endif

#include <platform/boss_platform.hpp>
#include "boss_fakewin.h"

#undef GetCurrentDirectoryW
extern "C" DWORD boss_fakewin_GetCurrentDirectoryW(DWORD nBufferLength, LPWSTR lpBuffer)
{
    static const WString& Root = Platform::File::GetFullPath((wchars) L"");

    BOSS_ASSERT("########## nBufferLength가 부족합니다", Root.Length() < (sint32) nBufferLength);
    Memory::Copy(lpBuffer, (wchars) Root, sizeof(wchar_t) * (Root.Length() + 1));
    return Root.Length();
}

#ifdef BOSS_FAKEWIN_IS_ENABLED
    #undef socket
    #undef connect
    #undef getsockopt
    #undef setsockopt
    #undef select
    #undef recv
    #undef send
    #undef closesocket
    #undef ioctlsocket
    #undef gethostbyname
    #undef htons
    #undef getaddrinfo
    #undef freeaddrinfo
    #undef gai_strerror
    #undef inet_addr
    #undef bind
    #undef getsockname
    #undef getpeername
    #undef sendto
    #undef recvfrom
    #undef accept
    #undef listen
    #undef getservbyname
    #undef inet_ntoa
    #undef shutdown
    #undef _exit
    #undef getpid
    #undef _getpid
    #undef mbstowcs_s
    #undef wcstombs_s

    #undef CreateFileW
    #undef CreateFileMapping
    #undef CreateDirectory
    #undef RemoveDirectory
    #undef CloseHandle
    #undef MapViewOfFile
    #undef UnmapViewOfFile
    #undef CreateThread
    #undef DeregisterEventSource
    #undef DeviceIoControl
    #undef ExpandEnvironmentStringsA
    #undef ExpandEnvironmentStringsW
    #undef FindClose
    #undef FindFirstFileA
    #undef FindFirstFileW
    #undef FindFirstFileExA
    #undef FindNextFileA
    #undef FindNextFileW
    #undef FindWindow
    #undef FlushConsoleInputBuffer
    #undef FlushFileBuffers
    #undef FormatMessageA    
    #undef FormatMessageW
    #undef GetCurrentProcess
    #undef GetCurrentThread
    #undef GetCurrentThreadId
    #undef GetExitCodeThread
    #undef GetFileAttributes
    #undef GetFileAttributesW
    #undef GetFileAttributesExA
    #undef GetFileAttributesExW
    #undef GetFullPathNameA
    #undef GetFullPathNameW
    #undef GetLastError
    #undef SetLastError
    #undef LoadLibraryA
    #undef LoadLibraryW
    #undef FreeLibrary
    #undef GetModuleHandleA
    #undef GetModuleHandleW
    #undef GetModuleFileName
    #undef GetProcAddress
    #undef GetProcessAffinityMask
    #undef GetProcessWindowStation
    #undef GetProcessTimes
    #undef GetShortPathNameW
    #undef GetStdHandle
    #undef GetSystemInfo
    #undef GetNativeSystemInfo
    #undef GetSystemTimeAsFileTime
    #undef SystemTimeToFileTime
    #undef GetTickCount
    #undef GetUserObjectInformationW
    #undef LocalFree
    #undef MessageBox
    #undef CopyFile
    #undef DeleteFileA
    #undef MoveFileEx
    #undef MoveFileExW
    #undef GetTempPathA
    #undef GetTempFileNameA
    #undef MultiByteToWideChar
    #undef WideCharToMultiByte
    #undef RegCloseKey
    #undef RegisterEventSource
    #undef RegOpenKeyExW
    #undef RegQueryValueExW
    #undef ReportEvent
    #undef SecureZeroMemory
    #undef SendMessage
    #undef SetFileAttributesW
    #undef Sleep
    #undef SleepEx
    #undef VerifyVersionInfo
    #undef SetEvent
    #undef ResetEvent
    #undef CreateEvent
    #undef CreateEventW
    #undef WaitForSingleObject
    #undef WaitForMultipleObjects
    #undef DeleteCriticalSection
    #undef TryEnterCriticalSection
    #undef EnterCriticalSection
    #undef InitializeCriticalSection
    #undef LeaveCriticalSection
    #undef WSAGetLastError
    #undef WSASetLastError
    #undef WSACleanup
    #undef WSAStartup
    #undef WSAStringToAddressA
    #undef QueryPerformanceFrequency
    #undef QueryPerformanceCounter
    #undef ZeroMemory
    #undef GetObject
    #undef DeleteObject
    #undef GetDC
    #undef ReleaseDC
    #undef CreateDIBSection
    #undef TlsAlloc
    #undef TlsFree
    #undef TlsGetValue
    #undef TlsSetValue
    #undef CreateMutexA
    #undef ReleaseMutex
    #undef CreateSemaphoreA
    #undef ReleaseSemaphore
    #undef GetStartupInfo
    #undef CreateProcessA
    #undef GetEnvironmentStrings
    #undef GetEnvironmentStringsW
    #undef GetSystemDirectoryA
    #undef GetSystemDirectoryW
    #undef GetEnvironmentVariableA
    #undef GetEnvironmentVariableW
    #undef GetSystemTime
    #undef SwitchToFiber
    #undef DeleteFiber
    #undef ConvertFiberToThread
    #undef ConvertThreadToFiber
    #undef CreateFiberEx
    #undef ConvertThreadToFiberEx
    #undef CreateFiber
    #undef GetConsoleMode
    #undef SetConsoleMode
    #undef ReadFile
    #undef ReadFileEx
    #undef GetFileType
    #undef PeekNamedPipe
    #undef strerror_s
    #undef ntohs
    #undef timeGetTime
    #undef GetFiberData
    #undef GetCurrentFiber
    #undef GetVersionExA
    #undef GetVersionExW
    #undef WSAIoctl
    #undef err2stringW
    #undef err2stringA
    #undef ldap_get_option
    #undef ldap_get_optionW
    #undef ldap_set_option
    #undef ldap_set_optionW
    #undef ldap_initW
    #undef ldap_init
    #undef ldap_simple_bindW
    #undef ldap_simple_bindA
    #undef ldap_simple_bind_sW
    #undef ldap_simple_bind_sA
    #undef ldap_searchW
    #undef ldap_searchA
    #undef ldap_search_sW
    #undef ldap_search_sA
    #undef ldap_first_entry
    #undef ldap_get_dnW
    #undef ldap_get_dn    
    #undef ldap_first_attributeW
    #undef ldap_first_attribute
    #undef ldap_next_attributeW
    #undef ldap_next_attribute
    #undef ldap_get_values_lenW
    #undef ldap_get_values_len
    #undef ldap_value_free_len
    #undef ldap_next_entry
    #undef ldap_msgfree
    #undef ldap_unbind
    #undef ldap_unbind_s

    #undef _access
    #undef _waccess
    #undef _chmod
    #undef _wchmod
    #undef _wfopen
    #undef _wfopen_s
    #undef _fopen
    #undef _fopen_s
    #undef _fseek
    #undef _ftell
    #undef _rewind
    #undef _fread
    #undef _fwrite
    #undef _fgetc
    #undef _fgets
    #undef _ungetc
    #undef _fprintf
    #undef _fclose
    #undef _feof
    #undef _ftime_s
    #undef _localtime_s
    #undef _gmtime_s
    #undef _wopen
    #undef _open
    #undef _close
    #undef _read
    #undef _write
    #undef _get_osfhandle
    #undef _lseek
    #undef _lseeki64
    #undef _lfind
    #undef _chsize_s
    #undef _stat
    #undef _stat64
    #undef _fstat
    #undef _fstat64
    #undef _wchdir
    #undef _wmkdir
    #undef _wrmdir
    #undef _unlink
    #undef _wunlink
    #undef _mktemp_s
    #undef _wgetenv
    #undef _wgetcwd
    #undef fd_set
    #undef FD_SET
    #undef FD_ZERO
    #undef _fileno
    #undef _getch
    #undef _getdrive
    #undef _lrotl
    #undef _lrotr
    #undef _rotl
    #undef _setmode
    #undef stricmp
    #undef _stricmp
    #undef strnicmp
    #undef _strnicmp
    #undef _mbsicmp
    #undef _wcsicmp
    #undef _snprintf
    #undef _vscprintf
    #undef _vsnprintf_s
    #undef _vsntprintf
    #undef _vsnwprintf
    #undef _beginthreadex
    #undef _fullpath
    #undef _ltoa
    #undef _ultoa
    #undef _isnan
    #undef _isinf
    #undef _finite
    #undef _splitpath
    #undef _strtoi64
    #undef _strtoui64
    #undef _set_errno
    #undef _byteswap_ushort
    #undef _byteswap_ulong
    #undef _byteswap_uint64
    #undef strlen
    #undef wcslen
    #undef strerror
    #undef strerror_s    
    #undef strcpy_s
    #undef strncpy_s
    #undef wcscpy_s
    #undef wcsncpy_s
    #undef strcpy
    #undef strncpy
    #undef wcscpy
    #undef wcsncpy
    #undef _strdup
    #undef _wcsdup
    #undef strpbrk
    #undef wcspbrk
    #undef ber_free    
    
    // 명칭재정의관련
    #undef access
    #undef wfopen
    #undef wfopen_s
    #undef fopen
    #undef fopen_s
    #undef fseek
    #undef ftell
    #undef rewind
    #undef fread
    #undef fwrite
    #undef fgetc
    #undef fgets
    #undef ungetc
    #undef fprintf
    #undef fclose
    #undef feof
    #undef ftime_s
    #undef localtime_s
    #undef gmtime_s
    #undef lseek
    #undef lseeki64
    #undef chsize_s
    #undef fileno
    #undef unlink
    #undef ltoa
    #undef _snprintf
    #undef _vsntprintf
    #undef fd_set
    #undef errno

    #define BOSS_NEED_SOCKET_TRACE 0
    #if BOSS_NEED_SOCKET_TRACE
        #define BOSS_SOCKET_TRACE(...) BOSS_TRACE(__VA_ARGS__)
    #else
        #define BOSS_SOCKET_TRACE(...) do{} while(0)
    #endif

    extern "C" SOCKET boss_fakewin_socket(int af, int type, int protocol)
    {
        BOSS_SOCKET_TRACE("########## socket");
        return boss_socket(af, type, protocol);
    }

    extern "C" int boss_fakewin_connect(SOCKET s, const struct boss_fakewin_struct_sockaddr* name, int namelen)
    {
        BOSS_SOCKET_TRACE("########## connect(family:%d)", (sint32) ((boss_fakewin_struct_sockaddr_in*) name)->sin_family);
        BOSS_SOCKET_TRACE("########## connect(ip4:%d.%d.%d.%d)",
            (sint32) ((boss_fakewin_struct_sockaddr_in*) name)->sin_addr.S_un.S_un_b.s_b1,
            (sint32) ((boss_fakewin_struct_sockaddr_in*) name)->sin_addr.S_un.S_un_b.s_b2,
            (sint32) ((boss_fakewin_struct_sockaddr_in*) name)->sin_addr.S_un.S_un_b.s_b3,
            (sint32) ((boss_fakewin_struct_sockaddr_in*) name)->sin_addr.S_un.S_un_b.s_b4);
        BOSS_SOCKET_TRACE("########## connect(port:%d)", (sint32) boss_htons(((boss_fakewin_struct_sockaddr_in*) name)->sin_port));
        int Result = boss_connect(s, *((const void**) &name), namelen);
        BOSS_SOCKET_TRACE("########## connect() -> %d", Result);
        return Result;
    }

    extern "C" int boss_fakewin_getsockopt(SOCKET s, int level, int optname, char* optval, int* optlen)
    {
        BOSS_SOCKET_TRACE("########## getsockopt");
        return boss_getsockopt(s, level, optname, optval, optlen);
    }
    
    extern "C" int boss_fakewin_setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen)
    {
        BOSS_SOCKET_TRACE("########## setsockopt");
        return boss_setsockopt(s, level, optname, optval, optlen);
    }

    extern "C" int boss_fakewin_select(int nfds, boss_fd_set* readfds, boss_fd_set* writefds, boss_fd_set* exceptfds, struct timeval* timeout)
    {
        BOSS_SOCKET_TRACE("########## select");
        return boss_select(nfds, readfds, writefds, exceptfds, timeout);
    }

    extern "C" int boss_fakewin_recv(SOCKET s, char* buf, int len, int flags)
    {
        BOSS_SOCKET_TRACE("########## recv(%d) ===> %d", (sint32) s, (sint32) len);
        return boss_recv(s, buf, len, flags);
    }

    extern "C" int boss_fakewin_send(SOCKET s, const char* buf, int len, int flags)
    {
        BOSS_SOCKET_TRACE("########## send(%d) ===> %d", (sint32) s, (sint32) len);
        return boss_send(s, buf, len, flags);
    }

    extern "C" int boss_fakewin_closesocket(SOCKET s)
    {
        BOSS_SOCKET_TRACE("########## closesocket");
        return boss_closesocket(s);
    }

    extern "C" int boss_fakewin_ioctlsocket(SOCKET s, long cmd, u_long* argp)
    {
        BOSS_SOCKET_TRACE("########## ioctlsocket");
        return boss_ioctlsocket(s, cmd, argp);
    }

    extern "C" struct boss_fakewin_struct_hostent* boss_fakewin_gethostbyname(const char* name)
    {
        BOSS_SOCKET_TRACE("########## gethostbyname");
        return (struct boss_fakewin_struct_hostent*) boss_gethostbyname(name);
    }

    extern "C" unsigned short boss_fakewin_htons(unsigned short number)
    {
        BOSS_SOCKET_TRACE("########## htons");
        return boss_htons(number);
    }

    extern "C" int boss_fakewin_getaddrinfo(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA* pHints, PADDRINFOA* ppResult)
    {
        BOSS_SOCKET_TRACE("########## getaddrinfo");
        return boss_getaddrinfo(pNodeName, pServiceName, pHints, (void**) ppResult);
    }

    extern "C" void boss_fakewin_freeaddrinfo(struct boss_fakewin_struct_addrinfo* ai)
    {
        BOSS_SOCKET_TRACE("########## freeaddrinfo");
        boss_freeaddrinfo(ai);
    }

    extern "C" const char* boss_fakewin_gai_strerror(int ret)
    {
        BOSS_SOCKET_TRACE("########## gai_strerror");
        return boss_gai_strerror(ret);
    }

    extern "C" unsigned long boss_fakewin_inet_addr(const char *cp)
    {
        BOSS_SOCKET_TRACE("########## inet_addr");
        return boss_inet_addr(cp);
    }

    extern "C" int boss_fakewin_bind(SOCKET s, const struct boss_fakewin_struct_sockaddr* addr, int namelen)
    {
        BOSS_SOCKET_TRACE("########## bind");
        return boss_bind(s, addr, namelen);
    }

    extern "C" int boss_fakewin_getsockname(SOCKET s, struct boss_fakewin_struct_sockaddr* addr, int* namelen)
    {
        BOSS_SOCKET_TRACE("########## getsockname");
        return boss_getsockname(s, addr, namelen);
    }

    extern "C" int boss_fakewin_getpeername(SOCKET s, struct boss_fakewin_struct_sockaddr* addr, int* namelen)
    {
        BOSS_SOCKET_TRACE("########## getpeername");
        return boss_getpeername(s, addr, namelen);
    }

    extern "C" int boss_fakewin_sendto(SOCKET s, const char* buf, int len, int flags, const struct boss_fakewin_struct_sockaddr* to, int tolen)
    {
        BOSS_SOCKET_TRACE("########## sendto");
        return boss_sendto(s, buf, len, flags, to, tolen);
    }

    extern "C" int boss_fakewin_recvfrom(SOCKET s, char* buf, int len, int flags, struct boss_fakewin_struct_sockaddr* from, int* fromlen)
    {
        BOSS_SOCKET_TRACE("########## recvfrom");
        return boss_recvfrom(s, buf, len, flags, from, fromlen);
    }

    extern "C" SOCKET boss_fakewin_accept(SOCKET s, struct boss_fakewin_struct_sockaddr* addr, int* addrlen)
    {
        BOSS_SOCKET_TRACE("########## accept");
        return boss_accept(s, addr, addrlen);
    }

    extern "C" int boss_fakewin_listen(SOCKET s, int backlog)
    {
        BOSS_SOCKET_TRACE("########## listen");
        return boss_listen(s, backlog);
    }

    extern "C" struct boss_fakewin_struct_servent* boss_fakewin_getservbyname(const char* name, const char* proto)
    {
        BOSS_SOCKET_TRACE("########## getservbyname");
        return (struct boss_fakewin_struct_servent*) boss_getservbyname(name, proto);
    }

    extern "C" char* boss_fakewin_inet_ntoa(struct boss_fakewin_struct_in_addr in)
    {
        BOSS_SOCKET_TRACE("########## inet_ntoa");
        return boss_inet_ntoa(&in);
    }

    extern "C" int boss_fakewin_shutdown(SOCKET s, int how)
    {
        BOSS_SOCKET_TRACE("########## shutdown");
        return boss_shutdown(s, how);
    }

    extern "C" void boss_fakewin_exit(int status)
    {
        BOSS_ASSERT("########## exit준비중", false);
    }

    extern "C" int boss_fakewin_getpid()
    {
        BOSS_ASSERT("########## getpid준비중", false);
        return 0;
    }

    extern "C" errno_t boss_fakewin_mbstowcs_s(size_t* pReturnValue, wchar_t* wcstr,
        size_t sizeInWords, const char* mbstr, size_t count)
    {
        const WString& NewString = WString::FromChars(mbstr, count);
        const sint32 CopyLen = Math::Min(NewString.Length(), sizeInWords - 1);
        if(pReturnValue) *pReturnValue = CopyLen;
        if(wcstr)
        {
            Memory::Copy(wcstr, (wchars) NewString, sizeof(wchar_t) * CopyLen);
            wcstr[CopyLen] = L'\0';
        }
        return 0;
    }

    extern "C" errno_t boss_fakewin_wcstombs_s(size_t* pReturnValue, char* mbstr,
        size_t sizeInBytes, const wchar_t* wcstr, size_t count)
    {
        const String& NewString = String::FromWChars(wcstr, count);
        const sint32 CopyLen = Math::Min(NewString.Length(), sizeInBytes - 1);
        if(pReturnValue) *pReturnValue = CopyLen;
        if(mbstr)
        {
            Memory::Copy(mbstr, (chars) NewString, sizeof(char) * CopyLen);
            mbstr[CopyLen] = '\0';
        }
        return 0;
    }

    ////////////////////////////////////////////////////////////////////////////////
    class FWHandleManager
    {
    public:
        virtual sint32 GetLastID() const = 0;
        virtual void Close(sint32 id) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////
    class FWHandle
    {
    private:
        FWHandle(FWHandleManager& manager) : mManager(manager), mID(manager.GetLastID())
        {
        }
        ~FWHandle()
        {
            mManager.Close(mID);
        }
    public:
        static HANDLE Create(FWHandleManager& manager)
        {return (HANDLE) new FWHandle(manager);}
        static bool Close(HANDLE handle)
        {delete (FWHandle*) handle; return true;}
        static FWHandle* From(HANDLE handle)
        {return (FWHandle*) handle;}
    public:
        FWHandleManager& mManager;
        const sint32 mID;
    };

    ////////////////////////////////////////////////////////////////////////////////
    class MapHandleManager : public FWHandleManager
    {
    public:
        class Element
        {
        public:
            Element() {file = nullptr; readonly = false;}
            ~Element() {boss_fclose(file);}
        public:
            boss_file file;
            bool readonly;
        };
    public:
        MapHandleManager() {mLastMapID = 0;}
        ~MapHandleManager() {}
    public:
        sint32 GetLastID() const override
        {
            return mLastMapID;
        }
        void Close(sint32 id) override
        {
            mIDToMap.Remove(id);
        }
    public:
        Element& AddMap()
        {
            return mIDToMap[++mLastMapID];
        }
        Element* ToMap(HANDLE handle)
        {
            if(auto CurHandle = FWHandle::From(handle))
            {
                if(&CurHandle->mManager == this)
                    return mIDToMap.Access(CurHandle->mID);
            }
            return nullptr;
        }
    private:
        sint32 mLastMapID;
        Map<Element> mIDToMap;
    public:
        static MapHandleManager& ST() {static MapHandleManager _; return _;}
    };

    extern "C" HANDLE boss_fakewin_CreateFileW(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE)
    {
        BOSS_ASSERT("########## CreateFileW준비중", false); return 0;
    }

    extern "C" HANDLE boss_fakewin_CreateFileMapping(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect,
        DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCTSTR lpName)
    {
        BOSS_ASSERT("########## lpAttributes는 nullptr만 지원합니다", lpAttributes == nullptr);

        const sint32 CurFD = *((sint32*) &hFile);
        //Platform::File::FDOpenRetain(CurFD);

        auto& NewMap = MapHandleManager::ST().AddMap();
        //NewMap.file = CurFD;
        NewMap.readonly = !!(flProtect & PAGE_READONLY);
        return FWHandle::Create(MapHandleManager::ST());
    }

    extern "C" BOOL boss_fakewin_CreateDirectory(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
    {
        BOSS_ASSERT("########## CreateDirectory준비중", false); return 0;
    }

    extern "C" BOOL boss_fakewin_RemoveDirectory(LPCSTR lpPathName)
    {
        BOSS_ASSERT("########## RemoveDirectory준비중", false); return 0;
    }

    extern "C" BOOL boss_fakewin_CloseHandle(HANDLE hObject)
    {
        return FWHandle::Close(hObject);
    }

    extern "C" LPVOID boss_fakewin_MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh,
        DWORD dwFileOffsetLow, size_t dwNumberOfBytesToMap)
    {
        if(auto CurFileInfo = MapHandleManager::ST().ToMap(hFileMappingObject))
        {
            const sint64 Offset64 = (((sint64) dwFileOffsetHigh) << 32) | dwFileOffsetLow;
            return Platform::File::FDMap(CurFileInfo->file, Offset64, dwNumberOfBytesToMap, CurFileInfo->readonly);
        }
        return nullptr;
    }

    extern "C" BOOL boss_fakewin_UnmapViewOfFile(LPCVOID lpBaseAddress)
    {
        return Platform::File::FDUnmap(lpBaseAddress);
    }

    ////////////////////////////////////////////////////////////////////////////////
    class ThreadHandleManager : public FWHandleManager
    {
    public:
        class Element
        {
        public:
            Element() {mCB = nullptr; mPayload = nullptr; mFinished = false;}
            ~Element() {}
        public:
            void Run()
            {
                Platform::Utility::ThreadingEx(
                    [](void* data)->uint32
                    {
                        auto self = (Element*) data;
                        uint32 Result = 0;
                        if(self->mCB)
                            Result = self->mCB(self->mPayload);
                        self->mFinished = true;
                        return Result;
                    }, this, prioritytype_normal);
            }
        public:
            ThreadExCB mCB;
            payload mPayload;
            bool mFinished;
        };
    public:
        ThreadHandleManager() {mLastThreadID = 0;}
        ~ThreadHandleManager() {}
    public:
        sint32 GetLastID() const override
        {
            return mLastThreadID;
        }
        void Close(sint32 id) override
        {
            mIDToThread.Remove(id);
        }
    public:
        Element& AddThread()
        {
            return mIDToThread[++mLastThreadID];
        }
        Element* ToThread(HANDLE handle)
        {
            if(auto CurHandle = FWHandle::From(handle))
            {
                if(&CurHandle->mManager == this)
                    return mIDToThread.Access(CurHandle->mID);
            }
            return nullptr;
        }
    private:
        sint32 mLastThreadID;
        Map<Element> mIDToThread;
    public:
        static ThreadHandleManager& ST() {static ThreadHandleManager _; return _;}
    };

    extern "C" HANDLE boss_fakewin_CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, size_t dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress,
        LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
    {
        auto& NewThread = ThreadHandleManager::ST().AddThread();
        NewThread.mCB = (ThreadExCB) lpStartAddress;
        NewThread.mPayload = (payload) lpParameter;
        NewThread.Run();
        return FWHandle::Create(ThreadHandleManager::ST());
    }

    extern "C" BOOL boss_fakewin_DeregisterEventSource(HANDLE) {BOSS_ASSERT("########## DeregisterEventSource준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_DeviceIoControl(HANDLE,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPOVERLAPPED) {BOSS_ASSERT("########## DeviceIoControl준비중", false); return 0;}

    extern "C" DWORD boss_fakewin_ExpandEnvironmentStringsA(LPCSTR lpSrc, LPSTR lpDst, DWORD nSize)
    {
        BOSS_ASSERT("########## ExpandEnvironmentStringsA동작 확인해야 함", false);
        static const WString& wRoot = Platform::File::GetFullPath((wchars) L"");
        const String Root = String::FromWChars(wRoot);

        String Result;
        bool IsInnerCode = false;
        while(*lpSrc)
        {
            if(*lpSrc != '%')
            {
                Result += *lpSrc;
                lpSrc += 1;
            }
            else if(IsInnerCode)
            {
                IsInnerCode = false;
                Result += *lpSrc;
                lpSrc += 1;
            }
            else if(!Memory::Compare("%HOMEDRIVE%", lpSrc, sizeof(char) * 11))
            {
                Result += Root[0];
                Result += Root[1];
                lpSrc += 11;
            }
            else if(!Memory::Compare("%HOMEPATH%", lpSrc, sizeof(char) * 10))
            {
                Result += &((chars) Root)[2];
                lpSrc += 10;
            }
            else if(!Memory::Compare("%USERPROFILE%", lpSrc, sizeof(char) * 13))
            {
                Result += Root;
                lpSrc += 13;
            }
            else if(!Memory::Compare("%APPDATA%", lpSrc, sizeof(char) * 9))
            {
                Result += Root;
                Result += (chars) "/appdata/roaming";
                lpSrc += 9;
            }
            else if(!Memory::Compare("%LOCALAPPDATA%", lpSrc, sizeof(char) * 14))
            {
                Result += Root;
                Result += (chars) "/appdata/local";
                lpSrc += 14;
            }
            else
            {
                IsInnerCode = true;
                Result += *lpSrc;
                lpSrc += 1;
            }
        }

        BOSS_ASSERT("########## nSize가 부족합니다", Result.Length() < (sint32) nSize);
        Memory::Copy(lpDst, (chars) Result, sizeof(char) * (Result.Length() + 1));
        return Result.Length();
    }

    extern "C" DWORD boss_fakewin_ExpandEnvironmentStringsW(LPCWSTR lpSrc, LPWSTR lpDst, DWORD nSize)
    {
        static const WString& Root = Platform::File::GetFullPath((wchars) L"");

        WString Result;
        bool IsInnerCode = false;
        while(*lpSrc)
        {
            if(*lpSrc != L'%')
            {
                Result += *lpSrc;
                lpSrc += 1;
            }
            else if(IsInnerCode)
            {
                IsInnerCode = false;
                Result += *lpSrc;
                lpSrc += 1;
            }
            else if(!Memory::Compare(L"%HOMEDRIVE%", lpSrc, sizeof(wchar_t) * 11))
            {
                Result += Root[0];
                Result += Root[1];
                lpSrc += 11;
            }
            else if(!Memory::Compare(L"%HOMEPATH%", lpSrc, sizeof(wchar_t) * 10))
            {
                Result += &((wchars) Root)[2];
                lpSrc += 10;
            }
            else if(!Memory::Compare(L"%USERPROFILE%", lpSrc, sizeof(wchar_t) * 13))
            {
                Result += Root;
                lpSrc += 13;
            }
            else if(!Memory::Compare(L"%APPDATA%", lpSrc, sizeof(wchar_t) * 9))
            {
                Result += Root;
                Result += (wchars) L"/appdata/roaming";
                lpSrc += 9;
            }
            else if(!Memory::Compare(L"%LOCALAPPDATA%", lpSrc, sizeof(wchar_t) * 14))
            {
                Result += Root;
                Result += (wchars) L"/appdata/local";
                lpSrc += 14;
            }
            else
            {
                IsInnerCode = true;
                Result += *lpSrc;
                lpSrc += 1;
            }
        }

        BOSS_ASSERT("########## nSize가 부족합니다", Result.Length() < (sint32) nSize);
        Memory::Copy(lpDst, (wchars) Result, sizeof(wchar_t) * (Result.Length() + 1));
        return Result.Length();
    }

    class SearchResult
    {
    public:
        SearchResult(chars pathname)
        {
            String Checkname = pathname;
            if(Checkname[-2] == '*') Checkname.SubTail(1);
            if(Checkname[-2] != '/' && Checkname[-2] != '\\')
                Checkname.AddTail("/");
            Pathname = WString::FromChars(Checkname);
            Cursor = 0;
        }
        ~SearchResult() {}

        void GetResultAtCursor(LPWIN32_FIND_DATAW lpFindFileData)
        {
            const WString& Filename = WString::FromChars(Itemnames[Cursor]);
            WIN32_FILE_ATTRIBUTE_DATA FileData;
            BOOL Result = boss_fakewin_GetFileAttributesExW(Pathname + Filename, GetFileExInfoStandard, &FileData);

            Memory::Set(lpFindFileData, 0x00, sizeof(WIN32_FIND_DATAW));
            Memory::Copy(lpFindFileData->cFileName, (wchars) Filename, sizeof(wchar_t) * (Filename.Length() + 1));
            if(Result) // ".", ".."의 경우 실패할 수 있음(WIN32에서만 성공함)
            {
                lpFindFileData->dwFileAttributes = FileData.dwFileAttributes;
                lpFindFileData->ftCreationTime = FileData.ftCreationTime;
                lpFindFileData->ftLastAccessTime = FileData.ftLastAccessTime;
                lpFindFileData->ftLastWriteTime = FileData.ftLastWriteTime;
                lpFindFileData->nFileSizeHigh = FileData.nFileSizeHigh;
                lpFindFileData->nFileSizeLow = FileData.nFileSizeLow;
            }
        }

    public:
        WString Pathname;
        Strings Itemnames;
        sint32 Cursor;
    };

    extern "C" BOOL boss_fakewin_FindClose(HANDLE hFindFile)
    {
        return (boss_closedir((void*) hFindFile) == 0);

        /*SearchResult* CurResult = (SearchResult*) hFindFile;
        delete CurResult;
        return true;*/
    }

    extern "C" HANDLE boss_fakewin_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
    {
        BOSS_ASSERT("########## FindFirstFileA준비중", false);
        return 0;
    }

    extern "C" HANDLE boss_fakewin_FindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData)
    {
        HANDLE NewHandle = (HANDLE) boss_opendir(String::FromWChars(lpFileName));
        return (NewHandle)? NewHandle : INVALID_HANDLE_VALUE;

        /*const String FilenameUTF8 = String::FromWChars(lpFileName);
        SearchResult* NewSearchResult = new SearchResult(FilenameUTF8);
        NewSearchResult->Itemnames.AtAdding() = ".";
        NewSearchResult->Itemnames.AtAdding() = "..";

        sint32 Result = Platform::File::Search(FilenameUTF8,
            [](chars itemname, payload data) -> void
            {
                SearchResult* NewSearchResult = (SearchResult*) data;
                NewSearchResult->Itemnames.AtAdding() = itemname;
            }, NewSearchResult, false);

        if(Result == -1)
        {
            delete NewSearchResult;
            return INVALID_HANDLE_VALUE;
        }

        NewSearchResult->GetResultAtCursor(lpFindFileData);
        NewSearchResult->Cursor++;
        return (HANDLE) NewSearchResult;*/
    }

    extern "C" HANDLE boss_fakewin_FindFirstFileExA(LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId,
        LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
    {
        BOSS_ASSERT("########## FindFirstFileA준비중", false);
        return 0;
    }

    extern "C" BOOL boss_fakewin_FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
    {
        BOSS_ASSERT("########## FindNextFileA준비중", false);
        return 0;
    }

    extern "C" BOOL boss_fakewin_FindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData)
    {
        void* curdir = boss_readdir((void*) hFindFile);
        chars CurDirname = boss_getdirname(curdir);
        WString CurDirnameW = WString::FromChars(CurDirname);
        Memory::Copy(lpFindFileData->cFileName, (wchars) CurDirnameW, CurDirnameW.Length());
        lpFindFileData->cFileName[CurDirnameW.Length()] = L'\0';
        return TRUE;

        /*SearchResult* CurResult = (SearchResult*) hFindFile;
        if(CurResult->Itemnames.Count() <= CurResult->Cursor)
        {
            boss_fakewin_SetLastError(ERROR_NO_MORE_FILES);
            return false;
        }

        CurResult->GetResultAtCursor(lpFindFileData);
        CurResult->Cursor++;
        return true;*/
    }

    extern "C" HWND boss_fakewin_FindWindow(LPCTSTR,LPCTSTR) {BOSS_ASSERT("########## FindWindow준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_FlushConsoleInputBuffer(HANDLE) {BOSS_ASSERT("########## FlushConsoleInputBuffer준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_FlushFileBuffers(HANDLE) {BOSS_ASSERT("########## FlushFileBuffers준비중", false); return 0;}

    extern "C" DWORD boss_fakewin_FormatMessageA(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId,
        LPSTR lpBuffer, DWORD nSize, va_list* Arguments)
    {
        BOSS_ASSERT("########## dwFlags는 FORMAT_MESSAGE_FROM_SYSTEM만 지원됩니다", dwFlags == FORMAT_MESSAGE_FROM_SYSTEM);
        if(13 + 1 <= nSize)
        {
            Memory::Copy(lpBuffer, "Unknown Error", sizeof(char) * (13 + 1));
            return 13;
        }
        return 0;
    }
    extern "C" DWORD boss_fakewin_FormatMessageW(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId,
        LPWSTR lpBuffer, DWORD nSize, va_list* Arguments)
    {
        BOSS_ASSERT("########## dwFlags는 FORMAT_MESSAGE_FROM_SYSTEM만 지원됩니다", dwFlags == FORMAT_MESSAGE_FROM_SYSTEM);
        if(13 + 1 <= nSize)
        {
            Memory::Copy(lpBuffer, "Unknown Error", sizeof(wchar_t) * (13 + 1));
            return 13;
        }
        return 0;
    }

    extern "C" HANDLE boss_fakewin_GetCurrentProcess() {BOSS_ASSERT("########## GetCurrentProcess준비중", false); return 0;}

    extern "C" HANDLE boss_fakewin_GetCurrentThread() {BOSS_ASSERT("########## GetCurrentThread준비중", false); return 0;}

    extern "C" DWORD boss_fakewin_GetCurrentThreadId()
    {
        return (Platform::Utility::CurrentThreadID() & 0xFFFFFFFF);
    }

    extern "C" BOOL boss_fakewin_GetExitCodeThread(HANDLE,LPDWORD) {BOSS_ASSERT("########## GetExitCodeThread준비중", false); return 0;}

    extern "C" DWORD boss_fakewin_GetFileAttributes(LPCSTR lpFileName) {BOSS_ASSERT("########## GetFileAttributes준비중", false); return 0;}

    extern "C" DWORD boss_fakewin_GetFileAttributesW(LPCWSTR lpFileName)
    {
        const DWORD FileAttributes = Platform::File::GetAttributes(lpFileName);
        if(FileAttributes == -1)
            boss_fakewin_SetLastError(ERROR_FILE_NOT_FOUND);
        return FileAttributes;
    }

    extern "C" BOOL boss_fakewin_GetFileAttributesExA(LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
    {
        BOSS_ASSERT("########## GetFileAttributesExA준비중", false);
        return 0;
    }

    extern "C" BOOL boss_fakewin_GetFileAttributesExW(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
    {
        BOSS_ASSERT("########## fInfoLevelId는 GetFileExInfoStandard만 지원합니다", fInfoLevelId == GetFileExInfoStandard);

        uint64 GetSize, GetCreateTime, GetAccessTime, GetModifyTime;
        const DWORD FileAttributes = Platform::File::GetAttributes(lpFileName,
            &GetSize, &GetCreateTime, &GetAccessTime, &GetModifyTime);
        if(FileAttributes == -1)
        {
            boss_fakewin_SetLastError(ERROR_FILE_NOT_FOUND);
            return false;
        }

        LPWIN32_FILE_ATTRIBUTE_DATA Result = (LPWIN32_FILE_ATTRIBUTE_DATA) lpFileInformation;
        Result->dwFileAttributes = FileAttributes;
        Result->nFileSizeHigh = GetSize >> 32;
        Result->nFileSizeLow = GetSize & 0xFFFFFFFF;
        Result->ftCreationTime.dwHighDateTime = GetCreateTime >> 32;
        Result->ftCreationTime.dwLowDateTime = GetCreateTime & 0xFFFFFFFF;
        Result->ftLastAccessTime.dwHighDateTime = GetAccessTime >> 32;
        Result->ftLastAccessTime.dwLowDateTime = GetAccessTime & 0xFFFFFFFF;
        Result->ftLastWriteTime.dwHighDateTime = GetModifyTime >> 32;
        Result->ftLastWriteTime.dwLowDateTime = GetModifyTime & 0xFFFFFFFF;
        return true;
    }

    extern "C" DWORD boss_fakewin_GetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR* lpFilePart)
    {
        BOSS_ASSERT("########## lpFilePart는 nullptr만 지원합니다", lpFilePart == nullptr);

        const WString FileNameUTF16 = WString::FromChars(lpFileName);
        const String NewFullPath = String::FromWChars(Platform::File::GetFullPath(FileNameUTF16));
        if(!lpBuffer)
            return NewFullPath.Length() + 1;

        const sint32 CopyLength = Math::Min(NewFullPath.Length(), nBufferLength - 1);
        Memory::Copy(lpBuffer, (chars) NewFullPath, sizeof(char) * CopyLength);
        lpBuffer[CopyLength] = '\0';
        return CopyLength;
    }

    extern "C" DWORD boss_fakewin_GetFullPathNameW(LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR* lpFilePart)
    {
        BOSS_ASSERT("########## lpFilePart는 nullptr만 지원합니다", lpFilePart == nullptr);

        const WString NewFullPath = Platform::File::GetFullPath(lpFileName);
        if(!lpBuffer)
            return NewFullPath.Length() + 1;

        const sint32 CopyLength = Math::Min(NewFullPath.Length(), nBufferLength - 1);
        Memory::Copy(lpBuffer, (wchars) NewFullPath, sizeof(wchar_t) * CopyLength);
        lpBuffer[CopyLength] = L'\0';
        return CopyLength;
    }

    extern "C" DWORD boss_fakewin_GetLastError()
    {
        DWORD Result = boss_geterrno();
        BOSS_TRACE("########## GetLastError(%u)", Result);
        return Result;
    }

    extern "C" void boss_fakewin_SetLastError(DWORD dwErrCode)
    {
        BOSS_TRACE("########## SetLastError(%u)", dwErrCode);
        boss_seterrno((sint32) dwErrCode);
    }

    extern "C" HMODULE boss_fakewin_LoadLibraryA(LPCSTR lpFileName) {BOSS_ASSERT("########## LoadLibraryA준비중", false); return 0;}
    extern "C" HMODULE boss_fakewin_LoadLibraryW(LPCWSTR lpFileName) {BOSS_ASSERT("########## LoadLibraryW준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_FreeLibrary(HMODULE hModule) {BOSS_ASSERT("########## FreeLibrary준비중", false); return 0; }

    extern "C" HMODULE boss_fakewin_GetModuleHandleA(LPCSTR)
    {
        return 0;
    }

    extern "C" HMODULE boss_fakewin_GetModuleHandleW(LPCWSTR) {BOSS_ASSERT("########## GetModuleHandleW준비중", false); return 0;}
    extern "C" DWORD boss_fakewin_GetModuleFileName(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
    {
        const String AppPath = "myapp.exe";
        String AppPath2 = String::FromWChars(Platform::File::GetFullPath(WString::FromChars(AppPath)));
        AppPath2.Replace('/', '\\');
        Memory::Copy(lpFilename, (chars) AppPath2, AppPath2.Length() + 1);
        return AppPath2.Length();
    }
    extern "C" FARPROC boss_fakewin_GetProcAddress(HMODULE,LPCSTR) {BOSS_ASSERT("########## GetProcAddress준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_GetProcessAffinityMask(HANDLE,PDWORD_PTR,PDWORD_PTR) {BOSS_ASSERT("########## GetProcessAffinityMask준비중", false); return 0;}

    extern "C" HWINSTA boss_fakewin_GetProcessWindowStation()
    {
        return (HWINSTA) 0x0000005c;
    }

    extern "C" BOOL boss_fakewin_GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime)
    {
        BOSS_ASSERT("########## GetProcessTimes준비중", false); return 0;
    }

    extern "C" DWORD boss_fakewin_GetShortPathNameW(LPCWSTR lpszLongPath, LPWSTR lpszShortPath, DWORD cchBuffer)
    {
        const WString& NewString = (wchars) lpszLongPath;
        Memory::Copy(lpszShortPath, (wchars) NewString, sizeof(wchar_t) * (NewString.Length() + 1));
        return NewString.Length();
    }

    extern "C" HANDLE boss_fakewin_GetStdHandle(DWORD) {BOSS_ASSERT("########## GetStdHandle준비중", false); return 0;}

    extern "C" void boss_fakewin_GetSystemInfo(LPSYSTEM_INFO lpSystemInfo)
    {
        Memory::Set(lpSystemInfo, 0x00, sizeof(SYSTEM_INFO));
        lpSystemInfo->dwNumberOfProcessors = 1;
        lpSystemInfo->dwAllocationGranularity = 65536;
    }

    extern "C" void boss_fakewin_GetNativeSystemInfo(LPSYSTEM_INFO lpSystemInfo)
    {
        Memory::Set(lpSystemInfo, 0x00, sizeof(SYSTEM_INFO));
        lpSystemInfo->dwNumberOfProcessors = 1;
        lpSystemInfo->dwAllocationGranularity = 65536;
    }

    extern "C" void boss_fakewin_GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
    {
        const uint64 FileTime = 10 * 1000 * Platform::Utility::CurrentTimeMsec();
        lpSystemTimeAsFileTime->dwHighDateTime = FileTime >> 32;
        lpSystemTimeAsFileTime->dwLowDateTime = FileTime & 0xFFFFFFFF;
    }

    extern "C" BOOL boss_fakewin_SystemTimeToFileTime(const SYSTEMTIME* lpSystemTime, LPFILETIME lpFileTime)
    {
        auto NewClock = Platform::Clock::Create(lpSystemTime->wYear, lpSystemTime->wMonth, lpSystemTime->wDay,
            lpSystemTime->wHour, lpSystemTime->wMinute, lpSystemTime->wSecond, ((sint64) lpSystemTime->wMilliseconds) * 1000000);
        const uint64 FileTime = 10 * 1000 * Platform::Clock::GetMsec(NewClock);
        Platform::Clock::Release(NewClock);
        lpFileTime->dwHighDateTime = FileTime >> 32;
        lpFileTime->dwLowDateTime = FileTime & 0xFFFFFFFF;
        return TRUE;
    }

    extern "C" DWORD boss_fakewin_GetTickCount()
    {
        return (DWORD) Platform::Utility::CurrentTimeMsec();
    }

    extern "C" BOOL boss_fakewin_GetUserObjectInformationW(HANDLE hObj, int nIndex, PVOID pvInfo, DWORD nLength, LPDWORD lpnLengthNeeded)
    {
        BOSS_ASSERT("########## hObj는 0x0000005c만 지원합니다", hObj == (HANDLE) 0x0000005c);
        BOSS_ASSERT("########## nIndex는 UOI_NAME만 지원합니다", nIndex == UOI_NAME);

        if(lpnLengthNeeded)
            *lpnLengthNeeded = sizeof(wchar_t) * (7 + 1);

        if(pvInfo)
        {
            Memory::Copy(pvInfo, L"WinSta0", sizeof(wchar_t) * (7 + 1));
            return true;
        }

        boss_fakewin_SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return false;
    }

    extern "C" HLOCAL boss_fakewin_LocalFree(HLOCAL hMem)
    {
        Buffer::Free((buffer) hMem);
        return nullptr;
    }

    extern "C" int boss_fakewin_MessageBox(HWND,LPCTSTR,LPCTSTR,UINT) {BOSS_ASSERT("########## MessageBox준비중", false); return 0;}

    extern "C" BOOL boss_fakewin_CopyFile(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists)
    {BOSS_ASSERT("########## CopyFile준비중", false); return 0;}

    extern "C" BOOL boss_fakewin_DeleteFileA(LPCSTR lpFileName)
    {BOSS_ASSERT("########## DeleteFileA준비중", false); return 0;}

    extern "C" BOOL boss_fakewin_MoveFileEx(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, DWORD dwFlags)
    {
        BOSS_ASSERT("########## MoveFileEx준비중", false); return 0;
    }

    extern "C" BOOL boss_fakewin_MoveFileExW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags)
    {
        BOSS_ASSERT("########## dwFlags는 (MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)만 지원합니다",
            dwFlags == (MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED));

        Platform::File::Remove(lpNewFileName);
        return Platform::File::Rename(lpExistingFileName, lpNewFileName);
    }

    extern "C" DWORD boss_fakewin_GetTempPathA(DWORD nBufferLength, LPSTR lpBuffer)
    {
        BOSS_ASSERT("########## GetTempPathA준비중", false); return 0;
    }

    extern "C" UINT boss_fakewin_GetTempFileNameA(LPCSTR lpPathName, LPCSTR lpPrefixString, UINT uUnique, LPSTR lpTempFileName)
    {
        BOSS_ASSERT("########## GetTempFileNameA준비중", false); return 0;
    }

    extern "C" int boss_fakewin_MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
    {
        BOSS_ASSERT("########## CodePage는 CP_UTF8만 지원합니다", CodePage == CP_UTF8);
        BOSS_ASSERT("########## dwFlags는 MB_ERR_INVALID_CHARS만 지원합니다", dwFlags == 0 || dwFlags == MB_ERR_INVALID_CHARS);

        const WString& NewString = WString::FromChars(lpMultiByteStr, cbMultiByte);
        if(0 < cchWideChar)
        {
            BOSS_ASSERT("########## cchWideChar가 부족합니다", NewString.Length() + 1 <= cchWideChar);
            Memory::Copy(lpWideCharStr, (wchars) NewString, sizeof(wchar_t) * (NewString.Length() + 1));
        }
        return NewString.Length() + 1;
    }

    extern "C" int boss_fakewin_WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar,
        LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
    {
        BOSS_ASSERT("########## CodePage는 CP_UTF8만 지원합니다", CodePage == CP_UTF8);
        BOSS_ASSERT("########## dwFlags는 WC_ERR_INVALID_CHARS만 지원합니다", dwFlags == 0 || dwFlags == WC_ERR_INVALID_CHARS);
        BOSS_ASSERT("########## lpDefaultChar는 nullptr만 지원합니다", lpDefaultChar == nullptr);
        BOSS_ASSERT("########## lpUsedDefaultChar는 nullptr만 지원합니다", lpUsedDefaultChar == nullptr);

        const String& NewString = String::FromWChars(lpWideCharStr, cchWideChar);
        if(0 < cbMultiByte)
        {
            BOSS_ASSERT("########## cbMultiByte가 부족합니다", NewString.Length() + 1 <= cbMultiByte);
            Memory::Copy(lpMultiByteStr, (chars) NewString, sizeof(char) * (NewString.Length() + 1));
        }
        return NewString.Length() + 1;
    }

    extern "C" LONG boss_fakewin_RegCloseKey(HKEY hKey)
    {
        return 0;
    }

    extern "C" HANDLE boss_fakewin_RegisterEventSource(LPCTSTR,LPCTSTR) {BOSS_ASSERT("########## RegisterEventSource준비중", false); return 0;}

    extern "C" LONG boss_fakewin_RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
    {
        return 0;
    }

    extern "C" LONG boss_fakewin_RegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
    {
        return 0;
    }

    extern "C" BOOL boss_fakewin_ReportEvent(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCTSTR*,LPVOID) {BOSS_ASSERT("########## ReportEvent준비중", false); return 0;}

    extern "C" PVOID boss_fakewin_SecureZeroMemory(PVOID ptr, size_t cnt)
    {
        Memory::Set(ptr, 0x00, cnt);
        return ptr;
    }

    extern "C" LRESULT boss_fakewin_SendMessage(HWND,UINT,WPARAM,LPARAM) {BOSS_ASSERT("########## SendMessage준비중", false); return 0;}

    extern "C" BOOL boss_fakewin_SetFileAttributesW(LPCWSTR lpFileName, DWORD dwFileAttributes)
    {
        return true;
    }

    extern "C" void boss_fakewin_Sleep(DWORD dwMilliseconds)
    {
        Platform::Utility::Sleep(dwMilliseconds, true, true, false);
    }

    extern "C" void boss_fakewin_SleepEx(DWORD, BOOL)
    {
        BOSS_ASSERT("########## SleepEx준비중", false);
    }

    extern "C" BOOL boss_fakewin_VerifyVersionInfo(LPOSVERSIONINFOEX lpVersionInfo, DWORD dwTypeMask, DWORDLONG dwlConditionMask)
    {
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    class EventHandleManager : public FWHandleManager
    {
    public:
        class Element
        {
        public:
            Element() {on = false;}
            ~Element() {}
        public:
            bool on;
        };
    public:
        EventHandleManager() {mLastEventID = 0; mEventMutex = Mutex::Open();}
        ~EventHandleManager() {Mutex::Close(mEventMutex);}
    public:
        sint32 GetLastID() const override
        {
            return mLastEventID;
        }
        void Close(sint32 id) override
        {
            mEventMap.Remove(id);
        }
    public:
        Element& AddEvent()
        {
            return mEventMap[++mLastEventID];
        }
        Element* ToEvent(HANDLE handle)
        {
            if(auto CurHandle = FWHandle::From(handle))
            {
                if(&CurHandle->mManager == this)
                {
                    Mutex::Lock(mEventMutex);
                    return mEventMap.Access(CurHandle->mID);
                }
            }
            return nullptr;
        }
        void Release()
        {
            Mutex::Unlock(mEventMutex);
        }
    private:
        sint32 mLastEventID;
        Map<Element> mEventMap;
        id_mutex mEventMutex;
    public:
        static EventHandleManager& ST() {static EventHandleManager _; return _;}
    };

    extern "C" BOOL boss_fakewin_SetEvent(HANDLE hEvent)
    {
        if(auto CurEvent = EventHandleManager::ST().ToEvent(hEvent))
        {
            CurEvent->on = true;
            EventHandleManager::ST().Release();
            return TRUE;
        }
        return FALSE;
    }

    extern "C" BOOL boss_fakewin_ResetEvent(HANDLE hEvent)
    {
        if(auto CurEvent = EventHandleManager::ST().ToEvent(hEvent))
        {
            CurEvent->on = false;
            EventHandleManager::ST().Release();
            return TRUE;
        }
        return FALSE;
    }

    extern "C" HANDLE boss_fakewin_CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName)
    {
        return FWHandle::Create(EventHandleManager::ST());
    }

    extern "C" HANDLE boss_fakewin_CreateEventW(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCWSTR)
    {
        return FWHandle::Create(EventHandleManager::ST());
    }

    extern "C" DWORD boss_fakewin_WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
    {
        if(!hHandle) return WAIT_FAILED;
        uint64 EndTime = (dwMilliseconds == INFINITE)? 0 : Platform::Utility::CurrentTimeMsec() + dwMilliseconds;
        while(EndTime == 0 || Platform::Utility::CurrentTimeMsec() < EndTime)
        {
            if(auto CurEvent = EventHandleManager::ST().ToEvent(hHandle))
            {
                if(CurEvent->on)
                {
                    CurEvent->on = false;
                    EventHandleManager::ST().Release();
                    return WAIT_OBJECT_0;
                }
                EventHandleManager::ST().Release();
            }
            else if(auto CurThread = ThreadHandleManager::ST().ToThread(hHandle))
            {
                if(CurThread->mFinished)
                    return WAIT_OBJECT_0;
            }
            Platform::Utility::Sleep(10, true, true, false);
        }
        return WAIT_TIMEOUT;
    }

    extern "C" DWORD boss_fakewin_WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds)
    {
        uint64 EndTime = (dwMilliseconds == INFINITE)? 0 : Platform::Utility::CurrentTimeMsec() + dwMilliseconds;
        while(EndTime == 0 || Platform::Utility::CurrentTimeMsec() < EndTime)
        {
            bool Exist = false, Finded = false;
            for(sint32 i = 0; i < nCount; ++i)
            {
                if(!lpHandles[i]) continue;
                Exist = true;
                if(auto CurEvent = EventHandleManager::ST().ToEvent(lpHandles[i]))
                {
                    if(CurEvent->on)
                    {
                        CurEvent->on = false;
                        Finded = true;
                    }
                    EventHandleManager::ST().Release();
                }
                else if(auto CurThread = ThreadHandleManager::ST().ToThread(lpHandles[i]))
                {
                    if(CurThread->mFinished)
                        Finded = true;
                }
            }
            if(!Exist)
                return WAIT_FAILED;
            if(Finded)
                return WAIT_OBJECT_0;
            Platform::Utility::Sleep(10, true, true, false);
        }
        return WAIT_TIMEOUT;
    }

    extern "C" void boss_fakewin_DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
    {
        Mutex::Close((id_mutex) lpCriticalSection->LockSemaphore);
    }
    extern "C" BOOL boss_fakewin_TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
    {
        return (lpCriticalSection->LockCount == 0);
    }
    extern "C" void boss_fakewin_EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
    {
        lpCriticalSection->LockCount++;
        Mutex::Lock((id_mutex) lpCriticalSection->LockSemaphore);
    }
    extern "C" void boss_fakewin_InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
    {
        lpCriticalSection->LockCount = 0;
        lpCriticalSection->LockSemaphore = Mutex::Open();
    }
    extern "C" void boss_fakewin_LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
    {
        Mutex::Unlock((id_mutex) lpCriticalSection->LockSemaphore);
        lpCriticalSection->LockCount--;
    }

    extern "C" int boss_fakewin_WSAGetLastError()
    {
        sint32 ErrorCode = boss_geterrno();
        if(EINTR <= ErrorCode && ErrorCode <= EREMOTE)
            ErrorCode += 10000;
        else ErrorCode = 0;
        BOSS_SOCKET_TRACE("########## WSAGetLastError(%d)", ErrorCode);
        return ErrorCode;
    }

    extern "C" void boss_fakewin_WSASetLastError(int iError)
    {
        BOSS_SOCKET_TRACE("########## WSASetLastError(%d)", iError);
        if(10000 <= iError)
            boss_seterrno(iError - 10000);
        else boss_seterrno(iError);
    }

    extern "C" int boss_fakewin_WSACleanup()
    {
        return 0;
    }

    extern "C" int boss_fakewin_WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData)
    {
        Memory::Set(lpWSAData, 0x00, sizeof(WSADATA));
        lpWSAData->wVersion = wVersionRequested;
        return 0;
    }

    extern "C" INT boss_fakewin_WSAStringToAddressA(LPSTR,INT,LPWSAPROTOCOL_INFO,LPSOCKADDR,LPINT) {BOSS_ASSERT("########## WSAStringToAddressA준비중", false); return 0;}

    extern "C" BOOL boss_fakewin_QueryPerformanceFrequency(LARGE_INTEGER* lpFrequency)
    {
        //BOSS_ASSERT("########## QueryPerformanceFrequency준비중", false);
        return 0;
    }
    extern "C" BOOL boss_fakewin_QueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount)
    {
        if(lpPerformanceCount)
        {
            lpPerformanceCount->QuadPart = Platform::Utility::CurrentTimeMsec();
            return 1;
        }
        return 0;
    }

    extern "C" void boss_fakewin_ZeroMemory(PVOID Destination, ULONG_PTR Length) {BOSS_ASSERT("########## ZeroMemory준비중", false);}
    extern "C" int boss_fakewin_GetObject(HGDIOBJ hgdiobj, int cbBuffer, LPVOID lpvObject) {BOSS_ASSERT("########## GetObject준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_DeleteObject(HGDIOBJ hObject) {BOSS_ASSERT("########## DeleteObject준비중", false); return 0;}
    extern "C" HDC boss_fakewin_GetDC(HWND hWnd) {BOSS_ASSERT("########## GetDC준비중", false); return 0;}
    extern "C" int boss_fakewin_ReleaseDC(HWND hWnd, HDC hDC) {BOSS_ASSERT("########## ReleaseDC준비중", false); return 0;}
    extern "C" HBITMAP boss_fakewin_CreateDIBSection(HDC hdc, const BITMAPINFO* pbmi, UINT iUsage, VOID** ppvBits, HANDLE hSection, DWORD dwOffset) {BOSS_ASSERT("########## CreateDIBSection준비중", false); return 0;}

    extern "C" DWORD boss_fakewin_TlsAlloc(void) {BOSS_ASSERT("########## TlsAlloc준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_TlsFree(DWORD dwTlsIndex) {BOSS_ASSERT("########## TlsFree준비중", false); return 0;}
    extern "C" LPVOID boss_fakewin_TlsGetValue(DWORD dwTlsIndex) {BOSS_ASSERT("########## TlsGetValue준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue) {BOSS_ASSERT("########## TlsSetValue준비중", false); return 0;}

    extern "C" HANDLE boss_fakewin_CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName)
    {
        //BOSS_ASSERT("########## CreateMutexA준비중", false);
        return 0;
    }

    extern "C" BOOL boss_fakewin_ReleaseMutex(HANDLE hMutex)
    {
        //BOSS_ASSERT("########## ReleaseMutex준비중", false);
        return 0;
    }

    extern "C" HANDLE boss_fakewin_CreateSemaphoreA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
        LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName)
    {BOSS_ASSERT("########## CreateSemaphoreA준비중", false); return 0;}
    extern "C" BOOL boss_fakewin_ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount)
    {BOSS_ASSERT("########## ReleaseSemaphore준비중", false); return 0;}
    extern "C" void boss_fakewin_GetStartupInfo(LPSTARTUPINFO lpStartupInfo)
    {BOSS_ASSERT("########## GetStartupInfo준비중", false);}
    extern "C" BOOL boss_fakewin_CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory,
        LPSTARTUPINFO lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
    {BOSS_ASSERT("########## CreateProcessA준비중", false); return 0;}

    extern "C" int boss_fakewin_access(const char* path, int mode)
    {
        BOSS_ASSERT("########## mode는 0만 지원합니다", mode == 0);

        return (Platform::File::CanAccess(WString::FromChars(path)))? 0 : -1;
    }

    extern "C" int boss_fakewin_waccess(const wchar_t* path, int mode)
    {
        BOSS_ASSERT("########## mode는 0만 지원합니다", mode == 0);

        return (Platform::File::CanAccess(path))? 0 : -1;
    }

    extern "C" int boss_fakewin_chmod(const char* filename, int pmode)
    {
        BOSS_ASSERT("########## _chmod준비중", false);
        //if(S_IWRITE & pmode)
        //    return (Platform::File::CanWritable(filename))? 0 : -1;
        //if(S_IREAD & pmode)
        //    return (Platform::File::CanAccess(filename))? 0 : -1;
        return -1;
    }
    
    extern "C" int boss_fakewin_wchmod(const wchar_t* filename, int pmode)
    {
        if(S_IWRITE & pmode)
            return (Platform::File::CanWritable(filename))? 0 : -1;
        if(S_IREAD & pmode)
            return (Platform::File::CanAccess(filename))? 0 : -1;
        return -1;
    }

    extern "C" FILE* boss_fakewin_wfopen(const wchar_t* filename, const wchar_t* mode)
    {
        BOSS_TRACE("########## wfopen");
        return boss_fakewin_fopen(String::FromWChars(filename), String::FromWChars(mode));
    }

    extern "C" errno_t boss_fakewin_wfopen_s(FILE** pfile, const wchar_t* filename, const wchar_t* mode)
    {
        BOSS_TRACE("########## wfopen_s");
        if(pfile)
        {
            *pfile = boss_fakewin_wfopen(filename, mode);
            return (*pfile)? 0 : EINVAL;
        }
        return EINVAL;
    }

    extern "C" FILE* boss_fakewin_fopen(char const* filename, char const* mode)
    {
        auto Result = (FILE*) boss_fopen(filename, mode);
        BOSS_TRACE("########## fopen(%s:%s) => 0x%08X", filename, mode, Result);
        return Result;
    }

    extern "C" errno_t boss_fakewin_fopen_s(FILE** pfile, char const* filename, char const* mode)
    {
        BOSS_TRACE("########## fopen_s");
        if(pfile)
        {
            *pfile = boss_fakewin_fopen(filename, mode);
            return (*pfile)? 0 : EINVAL;
        }
        return EINVAL;
    }

    extern "C" int boss_fakewin_fseek(FILE* stream, long int offset, int origin)
    {
        BOSS_TRACE("########## fseek");
        return boss_fseek((boss_file) stream, offset, origin);
    }

    extern "C" long int boss_fakewin_ftell(FILE* stream)
    {
        BOSS_TRACE("########## ftell");
        return boss_ftell((boss_file) stream);
    }

    extern "C" void boss_fakewin_rewind(FILE* stream)
    {
        BOSS_TRACE("########## rewind");
        boss_rewind((boss_file) stream);
    }

    extern "C" size_t boss_fakewin_fread(void* ptr, size_t size, size_t count, FILE* stream)
    {
        BOSS_TRACE("########## fread");
        return boss_fread(ptr, size, count, (boss_file) stream);
    }

    extern "C" size_t boss_fakewin_fwrite(const void* ptr, size_t size, size_t count, FILE* stream)
    {
        BOSS_TRACE("########## fwrite");
        return boss_fwrite(ptr, size, count, (boss_file) stream);
    }

    extern "C" int boss_fakewin_fgetc(FILE* stream)
    {
        BOSS_TRACE("########## fgetc");
        return boss_fgetc((boss_file) stream);
    }

    extern "C" char* boss_fakewin_fgets(char* str, int num, FILE* stream)
    {
        BOSS_TRACE("########## fgets");
        return boss_fgets(str, num, (boss_file) stream);
    }

    extern "C" int boss_fakewin_ungetc(int character, FILE* stream)
    {
        BOSS_TRACE("########## ungetc");
        return boss_ungetc(character, (boss_file) stream);
    }

    extern "C" int boss_fakewin_fprintf(FILE* stream, const char* format, ...)
    {
        BOSS_TRACE("########## fprintf");
        va_list Args;
        va_start(Args, format);
        const sint32 Size = boss_fprintf((boss_file) stream, format, Args);
        va_end(Args);
        return Size;
    }

    extern "C" int boss_fakewin_fclose(FILE* stream)
    {
        BOSS_TRACE("########## fclose");
        return boss_fclose((boss_file) stream);
    }

    extern "C" int boss_fakewin_feof(FILE* stream)
    {
        BOSS_TRACE("########## feof");
        return boss_feof((boss_file) stream);
    }

    extern "C" errno_t boss_fakewin_ftime_s(struct boss_fakewin_struct_timeb*)
    {
        BOSS_ASSERT("########## _ftime_s준비중", false);
        return 0;
    }

    extern "C" errno_t boss_fakewin_localtime_s(struct tm*, const time_t*)
    {
        BOSS_ASSERT("########## _localtime_s준비중", false);
        return 0;
    }

    extern "C" errno_t boss_fakewin_gmtime_s(struct tm* tmp, const time_t* timer)
    {
        BOSS_ASSERT("########## __gmtime_s준비중", false);
        return 0;
    }

    extern "C" int boss_fakewin_wopen(const wchar_t* filename, int oflag, int pmode)
    {
        int TotalOFlags = O_RDONLY | O_WRONLY | O_RDWR | O_APPEND | O_CREAT |
            O_EXCL | O_TRUNC | O_UNKNOWN | O_BINARY | O_NOINHERIT | O_TEMPORARY;
        BOSS_ASSERT(String::Format("oflag(0x%08X)가 지원수준을 벗어납니다", oflag),
            !((oflag & 0x7FFFF) & ~TotalOFlags));
        const bool Writable = !!(pmode & S_IWRITE) | !!(oflag & O_CREAT);
        const bool Append = !!(oflag & O_APPEND);
        const bool Exclusive = (!!(oflag & O_WRONLY) | !!(oflag & O_RDWR)) & !!(oflag & O_EXCL);
        const bool Truncate = !!(oflag & O_TRUNC);

        return Platform::File::FDOpen(filename, Writable, Append, Exclusive, Truncate);
    }

    extern "C" int boss_fakewin_open(const char* filename, int oflag)
    {
        return boss_fakewin_wopen(WString::FromChars(filename), oflag, 0);
    }

    extern "C" int boss_fakewin_close(int fd)
    {
        return Platform::File::FDClose(fd)? 0 : -1;
    }

    extern "C" long boss_fakewin_read(int fd, void* buffer, unsigned int count)
    {
        return Platform::File::FDRead(fd, buffer, count);
    }

    extern "C" long boss_fakewin_write(int fd, const void* buffer, unsigned int count)
    {
        return Platform::File::FDWrite(fd, buffer, count);
    }

    extern "C" intptr_t boss_fakewin_get_osfhandle(int fd)
    {
        return fd;
    }

    extern "C" long boss_fakewin_lseek(int fd, long offset, int origin)
    {
        return Platform::File::FDSeek(fd, offset, origin);
    }

    extern "C" __int64 boss_fakewin_lseeki64(int fd, __int64 offset, int origin)
    {
        return Platform::File::FDSeek(fd, offset, origin);
    }

    extern "C" void* boss_fakewin_lfind(const void* key, const void* base, unsigned int* num, unsigned int width, int (*compare)(const void*, const void*))
    {
        BOSS_ASSERT("########## lfind준비중", false);
        return 0;
    }

    extern "C" errno_t boss_fakewin_chsize_s(int fd, __int64 size)
    {
        return Platform::File::FDResize(fd, size)? 0 : -1;
    }

    extern "C" int boss_fakewin_stat(const char* file_name, struct boss_fakewin_struct_stat* _Stat)
    {
        BOSS_ASSERT("########## _stat준비중", false);
        return 0;
    }

    extern "C" int boss_fakewin_stat64(const char* file_name, struct boss_fakewin_struct_stat64* _Stat)
    {
        BOSS_ASSERT("########## _stat64준비중", false);
        return 0;
    }

    extern "C" int boss_fakewin_fstat(int fd, struct boss_fakewin_struct_stat* _Stat)
    {
        uint64 GetSize, GetCreateTime, GetAccessTime, GetModifyTime;
        const DWORD FileAttributes = Platform::File::FDAttributes(fd,
            &GetSize, &GetCreateTime, &GetAccessTime, &GetModifyTime);
        if(FileAttributes == -1)
            return -1;

        Memory::Set(_Stat, 0x00, sizeof(struct boss_fakewin_struct_stat));
        _Stat->st_mode = 0;
        if(FileAttributes & FILE_ATTRIBUTE_DIRECTORY) _Stat->st_mode |= S_IFDIR;
        else
        {
            _Stat->st_mode |= S_IFREG;
            if(FileAttributes & FILE_ATTRIBUTE_READONLY) _Stat->st_mode |= S_IREAD;
            else _Stat->st_mode |= S_IREAD | S_IWRITE;
        }

        _Stat->st_dev = 0;
        _Stat->st_ino = 0;
        _Stat->st_nlink = 1; // No NTFS
        _Stat->st_uid = 0;
        _Stat->st_gid = 0;
        _Stat->st_rdev = 0;
        _Stat->st_size = GetSize;
        #if BOSS_MAC_OSX // 시간입력 확인해 봐야 함!
            _Stat->st_atimespec.tv_sec = WindowToEpoch(GetAccessTime / 10000) / 1000;
            _Stat->st_atimespec.tv_nsec = 0;
            _Stat->st_mtimespec.tv_sec = WindowToEpoch(GetModifyTime / 10000) / 1000;
            _Stat->st_mtimespec.tv_nsec = 0;
            _Stat->st_ctimespec.tv_sec = WindowToEpoch(GetCreateTime / 10000) / 1000;
            _Stat->st_ctimespec.tv_nsec = 0;
        #else
            _Stat->st_atime = WindowToEpoch(GetAccessTime / 10000) / 1000;
            _Stat->st_mtime = WindowToEpoch(GetModifyTime / 10000) / 1000;
            _Stat->st_ctime = WindowToEpoch(GetCreateTime / 10000) / 1000;
        #endif
        return 0;
    }

    extern "C" int boss_fakewin_fstat64(int fd, struct boss_fakewin_struct_stat64* _Stat)
    {
        uint64 GetSize, GetCreateTime, GetAccessTime, GetModifyTime;
        const DWORD FileAttributes = Platform::File::FDAttributes(fd,
            &GetSize, &GetCreateTime, &GetAccessTime, &GetModifyTime);
        if(FileAttributes == -1)
            return -1;

        Memory::Set(_Stat, 0x00, sizeof(struct boss_fakewin_struct_stat64));
        _Stat->st_mode = 0;
        if(FileAttributes & FILE_ATTRIBUTE_DIRECTORY) _Stat->st_mode |= S_IFDIR;
        else
        {
            _Stat->st_mode |= S_IFREG;
            if(FileAttributes & FILE_ATTRIBUTE_READONLY) _Stat->st_mode |= S_IREAD;
            else _Stat->st_mode |= S_IREAD | S_IWRITE;
        }

        _Stat->st_dev = 0;
        _Stat->st_ino = 0;
        _Stat->st_nlink = 1; // No NTFS
        _Stat->st_uid = 0;
        _Stat->st_gid = 0;
        _Stat->st_rdev = 0;
        _Stat->st_size = GetSize;
        #if BOSS_MAC_OSX // 시간입력 확인해 봐야 함!
            _Stat->st_atimespec.tv_sec = WindowToEpoch(GetAccessTime / 10000) / 1000;
            _Stat->st_atimespec.tv_nsec = 0;
            _Stat->st_mtimespec.tv_sec = WindowToEpoch(GetModifyTime / 10000) / 1000;
            _Stat->st_mtimespec.tv_nsec = 0;
            _Stat->st_ctimespec.tv_sec = WindowToEpoch(GetCreateTime / 10000) / 1000;
            _Stat->st_ctimespec.tv_nsec = 0;
        #else
            _Stat->st_atime = WindowToEpoch(GetAccessTime / 10000) / 1000;
            _Stat->st_mtime = WindowToEpoch(GetModifyTime / 10000) / 1000;
            _Stat->st_ctime = WindowToEpoch(GetCreateTime / 10000) / 1000;
        #endif
        return 0;
    }

    extern "C" int boss_fakewin_wchdir(const wchar_t* dirname)
    {
        #if BOSS_WINDOWS
            return _wchdir(dirname);
        #else
            BOSS_ASSERT("########## 준비중", false); return 0;
        #endif
    }

    extern "C" int boss_fakewin_wmkdir(const wchar_t* dirname)
    {
        return Platform::File::CreateDir(dirname)? 0 : -1;
    }

    extern "C" int boss_fakewin_wrmdir(const wchar_t* dirname)
    {
        if(!Platform::File::ExistForDir(String::FromWChars(dirname)))
        {
            boss_fakewin_SetLastError(ERROR_DIRECTORY);
            return -1;
        }

        return Platform::File::RemoveDir(dirname)? 0 : -1;
    }

    extern "C" int boss_fakewin_unlink(const char* filename)
    {
        return Platform::File::Remove(WString::FromChars(filename))? 0 : -1;
    }

    extern "C" int boss_fakewin_wunlink(const wchar_t* filename)
    {
        return Platform::File::Remove(filename)? 0 : -1;
    }

    extern "C" errno_t boss_fakewin_mktemp_s(char* _TemplateName, size_t _Size)
    {
        return Platform::File::Tempname(_TemplateName, _Size)? 0 : -1;
    }

    extern "C" wchar_t* boss_fakewin_wgetenv(const wchar_t* varname)
    {
        return (wchar_t*) L"";
    }

    extern "C" wchar_t* boss_fakewin_wgetcwd(wchar_t* buffer, int maxlen)
    {
        #if BOSS_WINDOWS
            return _wgetcwd(buffer, maxlen);
        #else
            BOSS_ASSERT("########## 준비중", false); return 0;
        #endif
    }

    extern "C" void boss_fakewin_FD_SET(int fd, boss_fd_set* fdset)
    {
        boss_FD_SET(fd, fdset);
    }

    extern "C" int boss_fakewin_FD_ISSET(int fd, boss_fd_set* fdset)
    {
        return boss_FD_ISSET(fd, fdset);
    }

    extern "C" void boss_fakewin_FD_ZERO(boss_fd_set* fdset)
    {
        boss_FD_ZERO(fdset);
    }

    extern "C" int boss_fakewin_fileno(FILE* _File)
    {
        return Platform::File::FDFromFile((boss_file) _File);
    }

    extern "C" int boss_fakewin_getch()
    {
        #if BOSS_WINDOWS
            return _getch();
        #else
            BOSS_ASSERT("########## 준비중", false); return 0;
        #endif
    }

    extern "C" int boss_fakewin_getdrive()
    {
        return Platform::File::GetDriveCode();
    }

    extern "C" unsigned long boss_fakewin_lrotl(unsigned long _Val, int _Shift)
    {
        _Shift &= 0x1F;
        const unsigned long Result = (_Val << _Shift) | (_Val >> (32 - _Shift));

        #if BOSS_WINDOWS
            BOSS_ASSERT("########## _lrotl의 연산값에 문제가 있습니다", Result == _lrotl(_Val, _Shift));
        #endif

        return Result;
    }

    extern "C" unsigned long boss_fakewin_lrotr(unsigned long _Val, int _Shift)
    {
        _Shift &= 0x1F;
        const unsigned long Result = (_Val >> _Shift) | (_Val << (32 - _Shift));

        #if BOSS_WINDOWS
            BOSS_ASSERT("########## _lrotr의 연산값에 문제가 있습니다", Result == _lrotr(_Val, _Shift));
        #endif

        return Result;
    }    

    extern "C" unsigned int boss_fakewin_rotl(unsigned int _Val, int _Shift)
    {
        _Shift &= 0x1F;
        const unsigned int Result = (_Val >> _Shift) | (_Val << (32 - _Shift));

        #if BOSS_WINDOWS
            BOSS_ASSERT("########## _rotl의 연산값에 문제가 있습니다", Result == _rotl(_Val, _Shift));
        #endif

        return Result;
    }

    extern "C" int boss_fakewin_setmode(int _FileHandle, int _Mode)
    {
        #if BOSS_WINDOWS
            return _setmode(_FileHandle, _Mode);
        #else
            BOSS_ASSERT("########## 준비중", false); return 0;
        #endif
    }

    extern "C" int boss_fakewin_stricmp(const char* str1, const char* str2)
    {
        return boss_stricmp(str1, str2);
    }

    extern "C" int boss_fakewin_strnicmp(const char* str1, const char* str2, size_t maxcount)
    {
        return boss_strnicmp(str1, str2, maxcount);
    }

    extern "C" int boss_fakewin_mbsicmp(const unsigned char* str1, const unsigned char* str2)
    {
        return boss_stricmp((const char*) str1, (const char*) str2);
    }

    extern "C" int boss_fakewin_wcsicmp(wchar_t const* str1, wchar_t const* str2)
    {
        return boss_wcsicmp(str1, str2);
    }

    extern "C" int boss_fakewin_vscprintf(const char* _Format, va_list _ArgList)
    {
        return boss_vsnprintf(nullptr, 0, _Format, _ArgList);
    }

    extern "C" int boss_fakewin_vsnprintf_s(char* _DstBuf, size_t _SizeInBytes, size_t _MaxCount, const char* _Format, va_list _ArgList)
    {
        BOSS_ASSERT("########## _MaxCount는 ((size_t) -1)만 지원합니다", _MaxCount == ((size_t) -1));

        return boss_vsnprintf(_DstBuf, _SizeInBytes, _Format, _ArgList);
    }

    extern "C" int boss_fakewin_vsnprintf(char* _DstBuf, size_t _SizeInBytes, const char* _Format, va_list _ArgList)
    {
        return boss_vsnprintf(_DstBuf, _SizeInBytes, _Format, _ArgList);
    }

    extern "C" int boss_fakewin_vsnwprintf(wchar_t* _DstBuf, size_t _SizeInBytes, const wchar_t* _Format, va_list _ArgList)
    {
        return boss_vsnwprintf(_DstBuf, _SizeInBytes, _Format, _ArgList);
    }

    extern "C" uintptr_t boss_fakewin_beginthreadex(void* security, unsigned stack_size, unsigned (*start_address)(void*), void* arglist, unsigned initflag, unsigned* thrdaddr)
    {
        return (uintptr_t) Platform::Utility::ThreadingEx(start_address, arglist, prioritytype_normal);
    }

    extern "C" char* boss_fakewin_fullpath(char* absPath, const char* relPath, size_t maxLength)
    {
        BOSS_ASSERT("########## _fullpath준비중", false);
        return 0;
    }
    extern "C" char* boss_fakewin_ltoa(long value, char* str, int radix)
    {
        BOSS_ASSERT("########## _ltoa준비중", false);
        return 0;
    }
    extern "C" char* boss_fakewin_ultoa(unsigned long value, char* str, int radix)
    {
        BOSS_ASSERT("########## _ultoa준비중", false);
        return 0;
    }
    extern "C" int boss_fakewin_isnan(double x)
    {
        #if BOSS_WINDOWS
            return _isnan(x);
        #else
            BOSS_ASSERT("########## _isnan준비중", false);
            return 0;
        #endif
    }
    extern "C" int boss_fakewin_isinf(double x)
    {
        #if BOSS_WINDOWS
            return !_finite(x);
        #else
            BOSS_ASSERT("########## _isinf준비중", false);
            return 0;
        #endif
    }
    extern "C" int boss_fakewin_finite(double x)
    {
        #if BOSS_WINDOWS
            return _finite(x);
        #else
            BOSS_ASSERT("########## _finite준비중", false);
            return 0;
        #endif
    }
    extern "C" void boss_fakewin_splitpath(const char* path, char* drive, char* dir, char* fname, char* ext)
    {
        BOSS_ASSERT("########## _splitpath준비중", false);
    }
    extern "C" __int64 boss_fakewin_strtoi64(const char* nptr, char** endptr, int base)
    {
        BOSS_ASSERT("########## 현재 10/16진수만 가능합니다", base == 10 || base == 16);
        if(base == 10) return Parser::GetInt<__int64>(nptr);
        if(base == 16) return Parser::GetHex32<__int64>(nptr);
        return 0;
    }
    extern "C" unsigned __int64 boss_fakewin_strtoui64(const char* nptr, char** endptr, int base)
    {
        BOSS_ASSERT("########## _strtoui64준비중", false);
        return 0;
    }
    extern "C" errno_t boss_fakewin_set_errno(int value)
    {
        boss_seterrno(value);
        return 0;
    }
    extern "C" LPCH boss_fakewin_GetEnvironmentStrings(void)
    {
        return (LPCH) "";
    }
    extern "C" LPWCH boss_fakewin_GetEnvironmentStringsW(void)
    {
        return (LPWCH) L"";
    }
    extern "C" UINT boss_fakewin_GetSystemDirectoryA(LPSTR lpBuffer, UINT uSize)
    {
        BOSS_ASSERT("########## GetSystemDirectoryA준비중", false);
        return 0;
    }
    extern "C" UINT boss_fakewin_GetSystemDirectoryW(LPWSTR lpBuffer, UINT uSize)
    {
        BOSS_ASSERT("########## GetSystemDirectoryW준비중", false);
        return 0;
    }
    extern "C" UINT boss_fakewin_GetEnvironmentVariableA(LPCSTR lpName, LPSTR lpBuffer, UINT uSize)
    {
        BOSS_ASSERT("########## GetEnvironmentVariableA준비중", false);
        return 0;
    }
    extern "C" UINT boss_fakewin_GetEnvironmentVariableW(LPCWSTR lpName, LPWSTR lpBuffer, UINT uSize)
    {
        if(!WString::Compare(lpName, L"USERPROFILE"))
        {
            String AssetsRem = Platform::File::RootForAssetsRem();
            AssetsRem.SubTail(1); // '/'기호제거
            const WString AssetsRemW = WString::FromChars(AssetsRem);
            if(lpBuffer)
            {
                const sint32 CopyLen = Math::Min(AssetsRemW.Length() + 1, uSize);
                Memory::Copy(lpBuffer, (wchars) AssetsRemW, sizeof(wchar_t) * CopyLen);
                return CopyLen - 1;
            }
            return AssetsRemW.Length() + 1;
        }
        return 0;
    }
    extern "C" BOOL boss_fakewin_ReadConsoleA(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead, LPDWORD lpNumberOfCharsRead, PCONSOLE_READCONSOLE_CONTROL pInputControl)
    {
        BOSS_ASSERT("########## ReadConsoleA준비중", false);
        return 0;
    }
    extern "C" BOOL boss_fakewin_ReadConsoleW(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead, LPDWORD lpNumberOfCharsRead, PCONSOLE_READCONSOLE_CONTROL pInputControl)
    {
        BOSS_ASSERT("########## ReadConsoleW준비중", false);
        return 0;
    }
    extern "C" VOID boss_fakewin_GetSystemTime(LPSYSTEMTIME lpSystemTime)
    {
        auto CurClock = Platform::Clock::CreateAsCurrent();
        sint32 Year = 0, Month = 0, Day = 0, Hour = 0, Minute = 0, Second = 0;
        sint64 Nanosecond = 0;
        Platform::Clock::GetDetail(CurClock, &Nanosecond, &Second, &Minute, &Hour, &Day, &Month, &Year);
        Platform::Clock::Release(CurClock);
        lpSystemTime->wYear = Year;
        lpSystemTime->wMonth = Month;
        lpSystemTime->wDayOfWeek = 0;///////////////////////////////////////////
        lpSystemTime->wDay = Day;
        lpSystemTime->wHour = Hour;
        lpSystemTime->wMinute = Minute;
        lpSystemTime->wSecond = Second;
        lpSystemTime->wMilliseconds = Nanosecond / 1000000;
    }
    extern "C" VOID boss_fakewin_SwitchToFiber(LPVOID lpFiber)
    {
        BOSS_ASSERT("########## SwitchToFiber준비중", false);
    }
    extern "C" VOID boss_fakewin_DeleteFiber(LPVOID lpFiber)
    {
        BOSS_ASSERT("########## DeleteFiber준비중", false);
    }
    extern "C" BOOL boss_fakewin_ConvertFiberToThread(void)
    {
        BOSS_ASSERT("########## ConvertFiberToThread준비중", false);
        return 0;
    }
    extern "C" LPVOID boss_fakewin_ConvertThreadToFiber(LPVOID lpParameter)
    {
        BOSS_ASSERT("########## ConvertThreadToFiber준비중", false);
        return 0;
    }
    extern "C" LPVOID boss_fakewin_CreateFiberEx(SIZE_T dwStackCommitSize, SIZE_T dwStackReserveSize,DWORD dwFlags, LPFIBER_START_ROUTINE lpStartAddress, LPVOID lpParameter)
    {
        BOSS_ASSERT("########## CreateFiberEx준비중", false);
        return 0;
    }
    extern "C" LPVOID boss_fakewin_ConvertThreadToFiberEx(LPVOID lpParameter, DWORD dwFlags)
    {
        BOSS_ASSERT("########## ConvertThreadToFiberEx준비중", false);
        return 0;
    }
    extern "C" LPVOID boss_fakewin_CreateFiber(SIZE_T dwStackSize, LPFIBER_START_ROUTINE lpStartAddress, LPVOID lpParameter)
    {
        BOSS_ASSERT("########## CreateFiber준비중", false);
        return 0;
    }
    extern "C" BOOL boss_fakewin_GetConsoleMode(HANDLE hConsoleHandle, LPDWORD lpMode)
    {
        BOSS_ASSERT("########## GetConsoleMode준비중", false);
        return 0;
    }
    extern "C" BOOL boss_fakewin_SetConsoleMode(HANDLE hConsoleHandle, LPDWORD dwMode)
    {
        BOSS_ASSERT("########## SetConsoleMode준비중", false);
        return 0;
    }
    extern "C" BOOL boss_fakewin_ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
    {
        BOSS_ASSERT("########## ReadFile준비중", false);
        return 0;
    }
    extern "C" BOOL boss_fakewin_ReadFileEx(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
    {
        BOSS_ASSERT("########## ReadFileEx준비중", false);
        return 0;
    }
    extern "C" DWORD boss_fakewin_GetFileType(HANDLE hFile)
    {
        BOSS_ASSERT("########## GetFileType준비중", false);
        return 0;
    }
    extern "C" BOOL boss_fakewin_PeekNamedPipe(HANDLE hNamedPipe, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesRead, LPDWORD lpTotalBytesAvail, LPDWORD lpBytesLeftThisMessage)
    {
        BOSS_ASSERT("########## PeekNamedPipe준비중", false);
        return 0;
    }
    extern "C" DWORD boss_fakewin_timeGetTime(void)
    {
        BOSS_ASSERT("########## timeGetTime준비중", false);
        return 0;
    }
    extern "C" PVOID boss_fakewin_GetFiberData(void)
    {
        //return *(PVOID *) (ULONG_PTR) __readfsdword (0x10);
        BOSS_ASSERT("########## GetFiberData준비중", false);
        return 0;
    }
    extern "C" PVOID boss_fakewin_GetCurrentFiber(void)
    {
        //return (PVOID) (ULONG_PTR) __readfsdword (0x10);
        BOSS_ASSERT("########## GetCurrentFiber준비중", false);
        return 0;
    }
    extern "C" unsigned short boss_fakewin_byteswap_ushort(unsigned short _Number)
    {
        return
            ((_Number & 0x00FF) << 8) |
            ((_Number & 0xFF00) >> 8);
    }
    extern "C" unsigned long boss_fakewin_byteswap_ulong(unsigned long _Number)
    {
        return
            ((_Number & 0x000000FF) << 24) |
            ((_Number & 0x0000FF00) << 8) |
            ((_Number & 0x00FF0000) >> 8) |
            ((_Number & 0xFF000000) >> 24);
    }
    extern "C" unsigned __int64 boss_fakewin_byteswap_uint64(unsigned __int64 _Number)
    {
        return
            ((_Number & ox00000000000000FF) << 56) |
            ((_Number & ox000000000000FF00) << 40) |
            ((_Number & ox0000000000FF0000) << 24) |
            ((_Number & ox00000000FF000000) << 8) |
            ((_Number & ox000000FF00000000) >> 8) |
            ((_Number & ox0000FF0000000000) >> 24) |
            ((_Number & ox00FF000000000000) >> 40) |
            ((_Number & oxFF00000000000000) >> 56);
    }
    extern "C" size_t boss_fakewin_strlen(const char *str)
    {
        return boss_strlen(str);
    }
    extern "C" size_t boss_fakewin_wcslen(const wchar_t *str)
    {
        return boss_wcslen(str);
    }
    extern "C" char* boss_fakewin_strerror(int errnum)
    {
        static String Result;
        switch(errnum)
        {
        case EPERM           : Result = "Operation not permitted"; break;
        case ENOENT          : Result = "No such file or directory"; break;
        case ESRCH           : Result = "No such process"; break;
        case EINTR           : Result = "Interrupted system call"; break;
        case EIO             : Result = "Input/output error"; break;
        case ENXIO           : Result = "Device not configured"; break;
        case E2BIG           : Result = "Argument list too long"; break;
        case ENOEXEC         : Result = "Exec format error"; break;
        case EBADF           : Result = "Bad file descriptor"; break;
        case ECHILD          : Result = "No child processes"; break;
        case EDEADLK         : Result = "Resource deadlock avoided"; break;
        case ENOMEM          : Result = "Cannot allocate memory"; break;
        case EACCES          : Result = "Permission denied"; break;
        case EFAULT          : Result = "Bad address"; break;
        case ENOTBLK         : Result = "Block device required"; break;
        case EBUSY           : Result = "Device busy"; break;
        case EEXIST          : Result = "File exists"; break;
        case EXDEV           : Result = "Cross-device link"; break;
        case ENODEV          : Result = "Operation not supported by device"; break;
        case ENOTDIR         : Result = "Not a directory"; break;
        case EISDIR          : Result = "Is a directory"; break;
        case EINVAL          : Result = "Invalid argument"; break;
        case ENFILE          : Result = "Too many open files in system"; break;
        case EMFILE          : Result = "Too many open files"; break;
        case ENOTTY          : Result = "Inappropriate ioctl for device"; break;
        case ETXTBSY         : Result = "Text file busy"; break;
        case EFBIG           : Result = "File too large"; break;
        case ENOSPC          : Result = "No space left on device"; break;
        case ESPIPE          : Result = "Illegal seek"; break;
        case EROFS           : Result = "Read-only file system"; break;
        case EMLINK          : Result = "Too many links"; break;
        case EPIPE           : Result = "Broken pipe"; break;
        case EDOM            : Result = "Numerical argument out of domain"; break;
        case ERANGE          : Result = "Result too large"; break;
        case EWOULDBLOCK     : Result = "Operation would block"; break;
        case EINPROGRESS     : Result = "Operation now in progress"; break;
        case EALREADY        : Result = "Operation already in progress"; break;
        case ENOTSOCK        : Result = "Socket operation on non-socket"; break;
        case EDESTADDRREQ    : Result = "Destination address required"; break;
        case EMSGSIZE        : Result = "Message too long"; break;
        case EPROTOTYPE      : Result = "Protocol wrong type for socket"; break;
        case ENOPROTOOPT     : Result = "Protocol not available"; break;
        case EPROTONOSUPPORT : Result = "Protocol not supported"; break;
        case ESOCKTNOSUPPORT : Result = "Socket type not supported"; break;
        case EOPNOTSUPP      : Result = "Operation not supported on socket"; break;
        case EPFNOSUPPORT    : Result = "Protocol family not supported"; break;
        case EAFNOSUPPORT    : Result = "Address family not supported by protocol family"; break;
        case EADDRINUSE      : Result = "Address already in use"; break;
        case EADDRNOTAVAIL   : Result = "Can't assign requested address"; break;
        case ENETDOWN        : Result = "Network is down"; break;
        case ENETUNREACH     : Result = "Network is unreachable"; break;
        case ENETRESET       : Result = "Network dropped connection on reset"; break;
        case ECONNABORTED    : Result = "Software caused connection abort"; break;
        case ECONNRESET      : Result = "Connection reset by peer"; break;
        case ENOBUFS         : Result = "No buffer space available"; break;
        case EISCONN         : Result = "Socket is already connected"; break;
        case ENOTCONN        : Result = "Socket is not connected"; break;
        case ESHUTDOWN       : Result = "Can't send after socket shutdown"; break;
        case ETOOMANYREFS    : Result = "Too many references: can't splice"; break;
        case ETIMEDOUT       : Result = "Connection timed out"; break;
        case ECONNREFUSED    : Result = "Connection refused"; break;
        case ELOOP           : Result = "Too many levels of symbolic links"; break;
        case ENAMETOOLONG    : Result = "File name too long"; break;
        case EHOSTDOWN       : Result = "Host is down"; break;
        case EHOSTUNREACH    : Result = "No route to host"; break;
        case ENOTEMPTY       : Result = "Directory not empty"; break;
        case EPROCLIM        : Result = "Too many processes"; break;
        case EUSERS          : Result = "Too many users"; break;
        case EDQUOT          : Result = "Disc quota exceeded"; break;
        case ESTALE          : Result = "Stale NFS file handle"; break;
        case EREMOTE         : Result = "Too many levels of remote in path"; break;
        case EBADRPC         : Result = "RPC struct is bad"; break;
        case ERPCMISMATCH    : Result = "RPC version wrong"; break;
        case EPROGUNAVAIL    : Result = "RPC prog. not avail"; break;
        case EPROGMISMATCH   : Result = "Program version wrong"; break;
        case EPROCUNAVAIL    : Result = "Bad procedure for program"; break;
        case ENOLCK          : Result = "No locks available"; break;
        case ENOSYS          : Result = "Function not implemented"; break;
        case EFTYPE          : Result = "Inappropriate file type or format"; break;
        default: Result = ""; break;
        }
        return (char*)(chars) Result;
    }
    extern "C" errno_t boss_fakewin_strerror_s(char *buf, rsize_t bufsz, errno_t errnum)
    {
        chars ErrText = boss_fakewin_strerror(errnum);
        sint32 ErrTextLen = boss_strlen(ErrText);
        Memory::Copy(buf, ErrText, Math::Min(ErrTextLen + 1, bufsz));
        buf[bufsz - 1] = '\0';
        return 0;
    }
    extern "C" errno_t boss_fakewin_strcpy_s(char *strDestination, size_t numberOfElements, const char *strSource)
    {
        BOSS_ASSERT("########## strcpy_s준비중", false);
        return 0;
    }
    extern "C" errno_t boss_fakewin_strncpy_s(char* strDestination, size_t numberOfElements, const char* strSource, size_t count)
    {
        boss_strncpy(strDestination, strSource, (numberOfElements < count)? numberOfElements : count);
        return 0;
    }
    extern "C" errno_t boss_fakewin_wcscpy_s(wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource)
    {
        BOSS_ASSERT("########## wcscpy_s준비중", false);
        return 0;
    }
    extern "C" errno_t boss_fakewin_wcsncpy_s(wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource, size_t count)
    {
        BOSS_ASSERT("########## wcsncpy_s준비중", false);
        return 0;
    }
    extern "C" char* boss_fakewin_strcpy(char *strDestination, const char *strSource)
    {
        return boss_strcpy(strDestination, strSource);
    }
    extern "C" char* boss_fakewin_strncpy(char* strDestination, const char* strSource, size_t count)
    {
        return boss_strncpy(strDestination, strSource, count);
    }
    extern "C" wchar_t* boss_fakewin_wcscpy(wchar_t *strDestination, const wchar_t *strSource)
    {
        return boss_wcscpy(strDestination, strSource);
    }
    extern "C" wchar_t* boss_fakewin_wcsncpy(wchar_t *strDestination, const wchar_t *strSource, size_t count)
    {
        return boss_wcsncpy(strDestination, strSource, count);
    }
    extern "C" char* boss_fakewin_strdup(const char *strSource)
    {
        auto Length = boss_strlen(strSource);
        auto NewSource = malloc(Length + 1);
        memcpy(NewSource, strSource, Length + 1);
        return (char*) NewSource;
    }
    extern "C" wchar_t* boss_fakewin_wcsdup(const wchar_t *strSource)
    {
        BOSS_ASSERT("########## _wcsdup준비중", false);
        return 0;
    }
    extern "C" char* boss_fakewin_strpbrk(const char *str, const char *strCharSet)
    {
        return boss_strpbrk(str, strCharSet);
    }
    extern "C" wchar_t* boss_fakewin_wcspbrk(const wchar_t *str, const wchar_t *strCharSet)
    {
        return boss_wcspbrk(str, strCharSet);
    }
    extern "C" VOID boss_fakewin_ber_free(BerElement *pBerElement, INT fbuf)
    {
        BOSS_ASSERT("########## ber_free준비중", false);
    }            
    extern "C" BOOL boss_fakewin_GetVersionExA(LPOSVERSIONINFOA lpVersionInfo)
    {
        lpVersionInfo->dwMinorVersion = VER_MINORVERSION;
        lpVersionInfo->dwMajorVersion = VER_MAJORVERSION;
        lpVersionInfo->dwBuildNumber = VER_BUILDNUMBER;
        lpVersionInfo->dwPlatformId = VER_PLATFORMID;
        return 0;
    }
    extern "C" BOOL boss_fakewin_GetVersionExW(LPOSVERSIONINFOW lpVersionInfo)
    {
        lpVersionInfo->dwMinorVersion = VER_MINORVERSION;
        lpVersionInfo->dwMajorVersion = VER_MAJORVERSION;
        lpVersionInfo->dwBuildNumber = VER_BUILDNUMBER;
        lpVersionInfo->dwPlatformId = VER_PLATFORMID;
        return 0;
    }
    extern "C" int boss_fakewin_WSAIoctl(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
    {
        BOSS_ASSERT("########## WSAIoctl준비중", false);
        return 0;
    }
    extern "C" PWCHAR boss_fakewin_ldap_err2stringW(ULONG err)
    {
        BOSS_ASSERT("########## ldap_err2stringW준비중", false);
        return 0;
    }
    extern "C" PCHAR boss_fakewin_ldap_err2stringA(ULONG err)
    {
        BOSS_ASSERT("########## ldap_err2stringA준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_get_option( LDAP *ld, int option, void *outvalue )
    {
        BOSS_ASSERT("########## ldap_get_option준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_get_optionW( LDAP *ld, int option, void *outvalue )
    {
        BOSS_ASSERT("########## ldap_get_optionW준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_set_option( LDAP *ld, int option, const void *outvalue )
    {
        BOSS_ASSERT("########## ldap_set_option준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_set_optionW( LDAP *ld, int option, const void *outvalue )
    {
        BOSS_ASSERT("########## ldap_set_optionW준비중", false);
        return 0;
    }
    extern "C" LDAP* boss_fakewin_ldap_initW(const PWSTR HostName, ULONG PortNumber)
    {
        BOSS_ASSERT("########## ldap_initW준비중", false);
        return 0;
    }
    extern "C" LDAP* boss_fakewin_ldap_init(const PSTR HostName, ULONG PortNumber)
    {
        BOSS_ASSERT("########## ldap_init준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_simple_bindW( LDAP *ld, PWSTR dn, PWSTR passwd )
    {
        BOSS_ASSERT("########## ldap_simple_bindW준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_simple_bindA( LDAP *ld, PSTR dn, PSTR passwd )
    {
        BOSS_ASSERT("########## ldap_simple_bindA준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_simple_bind_sW( LDAP *ld, PWSTR dn, PWSTR passwd )
    {
        BOSS_ASSERT("########## ldap_simple_bind_sW준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_simple_bind_sA( LDAP *ld, PSTR dn, PSTR passwd )
    {
        BOSS_ASSERT("########## ldap_simple_bind_sA준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_searchW(LDAP *ld, const PWSTR base, ULONG scope, const PWSTR filter, PZPWSTR attrs, LONG attrsonly)
    {
        BOSS_ASSERT("########## ldap_searchW준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_searchA(LDAP *ld, const PSTR base, ULONG scope, const PSTR filter, PZPSTR attrs, LONG attrsonly)
    {
        BOSS_ASSERT("########## ldap_searchA준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_search_sW(LDAP *ld, const PWSTR base, ULONG scope, const PWSTR filter, PZPWSTR attrs, LONG attrsonly, LDAPMessage** res)
    {
        BOSS_ASSERT("########## ldap_search_sW준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_search_sA(LDAP *ld, const PSTR base, ULONG scope, const PSTR filter, PZPSTR attrs, LONG attrsonly, LDAPMessage** res)
    {
        BOSS_ASSERT("########## ldap_search_sA준비중", false);
        return 0;
    }
    extern "C" LDAPMessage* boss_fakewin_ldap_first_entry(LDAP *ld, LDAPMessage *res)
    {
        BOSS_ASSERT("########## ldap_first_entry준비중", false);
        return 0;
    }
    extern "C" PWCHAR boss_fakewin_ldap_get_dnW(LDAP *ld, LDAPMessage *entry)
    {
        BOSS_ASSERT("########## ldap_first_entry준비중", false);
        return 0;
    }
    extern "C" PCHAR boss_fakewin_ldap_get_dn(LDAP *ld, LDAPMessage *entry)
    {
        BOSS_ASSERT("########## ldap_first_entry준비중", false);
        return 0;
    }    
    extern "C" VOID boss_fakewin_ldap_memfreeW(PWCHAR Block)
    {
        BOSS_ASSERT("########## ldap_first_entry준비중", false);
    }
    extern "C" VOID boss_fakewin_ldap_memfree(PCHAR Block)
    {
        BOSS_ASSERT("########## ldap_first_entry준비중", false);
    }
    extern "C" PWCHAR boss_fakewin_ldap_first_attributeW(LDAP *ld, LDAPMessage* entry, BerElement** ptr)
    {
        BOSS_ASSERT("########## ldap_first_attributeW준비중", false);
        return 0;
    }
    extern "C" PCHAR boss_fakewin_ldap_first_attribute(LDAP *ld, LDAPMessage* entry, BerElement** ptr)
    {
        BOSS_ASSERT("########## ldap_first_attribute준비중", false);
        return 0;
    }
    extern "C" PWCHAR boss_fakewin_ldap_next_attributeW(LDAP *ld, LDAPMessage* entry, BerElement* ptr)
    {
        BOSS_ASSERT("########## ldap_next_attributeW준비중", false);
        return 0;
    }
    extern "C" PCHAR boss_fakewin_ldap_next_attribute(LDAP *ld, LDAPMessage* entry, BerElement* ptr)
    {
        BOSS_ASSERT("########## ldap_next_attribute준비중", false);
        return 0;
    }    
    extern "C" struct berval** boss_fakewin_ldap_get_values_lenW(LDAP *ExternalHandle, LDAPMessage* Message, const PWSTR attr)
    {
        BOSS_ASSERT("########## ldap_get_values_lenW준비중", false);
        return 0;
    }
    extern "C" struct berval** boss_fakewin_ldap_get_values_len(LDAP *ExternalHandle, LDAPMessage* Message, const PSTR attr)
    {
        BOSS_ASSERT("########## ldap_get_values_len준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_value_free_len(struct berval **vals)
    {
        BOSS_ASSERT("########## ldap_next_attribute준비중", false);
        return 0;
    }
    extern "C" LDAPMessage* boss_fakewin_ldap_next_entry(LDAP *ld, LDAPMessage *entry)
    {
        BOSS_ASSERT("########## ldap_next_entry준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_msgfree(LDAPMessage *res)
    {
        BOSS_ASSERT("########## ldap_msgfree준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_unbind(LDAP *ld)
    {
        BOSS_ASSERT("########## ldap_unbind준비중", false);
        return 0;
    }
    extern "C" ULONG boss_fakewin_ldap_unbind_s(LDAP *ld)
    {
        BOSS_ASSERT("########## ldap_unbind_s준비중", false);
        return 0;
    }

#endif
