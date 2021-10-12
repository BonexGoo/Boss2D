#include <boss.hpp>

#include <wchar.h>
#include <stdio.h>
#include <stdarg.h>

#if BOSS_WINDOWS
    #if BOSS_WINDOWS_MINGW
        #include <ws2tcpip.h>
    #else
        #include <windows.h>
        #pragma comment(lib, "ws2_32.lib")
    #endif
#elif BOSS_LINUX
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <strings.h>
    #include <errno.h>
#elif BOSS_MAC_OSX || BOSS_IPHONE
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <CoreFoundation/CoreFoundation.h>
#elif BOSS_ANDROID
    #include <dirent.h>
    #include <android/asset_manager_jni.h>
#elif BOSS_WASM
    #include <sys/stat.h>
    #include <stropts.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <strings.h>
    #include <errno.h>
#endif

#if !BOSS_WINDOWS
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <fcntl.h>
#endif

extern "C"
{
    #if BOSS_LINUX
        #define THROW __THROW
    #else
        #define THROW
    #endif

    #if BOSS_WINDOWS || BOSS_LINUX || BOSS_ANDROID || BOSS_WASM
        extern int isalpha(int);
        extern int isdigit(int);
        extern int isalnum(int);
    #endif

    #if BOSS_ANDROID
        extern double strtod(const char*, char**);
    #else
        extern double atof(const char*);
    #endif

    extern char* strcpy(char*, const char*);
    extern char* strncpy(char*, const char*, size_t);
    extern wchar_t* wcscpy(wchar_t*, const wchar_t*);
    extern wchar_t* wcsncpy(wchar_t*, const wchar_t*, size_t);

    extern size_t strlen(const char*);
    extern size_t wcslen(const wchar_t*);

    #if BOSS_MAC_OSX || BOSS_IPHONE || BOSS_WASM
        extern int vasprintf(char**, const char*, va_list);
    #else
        extern int vsnprintf(char*, size_t, const char*, va_list);
    #endif

    #if BOSS_MAC_OSX || BOSS_IPHONE || BOSS_ANDROID || BOSS_WASM
        extern int vswprintf(wchar_t*, size_t, const wchar_t*, va_list);
    #else
        extern int _vsnwprintf(wchar_t*, size_t, const wchar_t*, va_list);
    #endif

    extern int strcmp(const char*, const char*);
    extern int strncmp(const char*, const char*, size_t);
    #if BOSS_MAC_OSX || BOSS_IPHONE || BOSS_ANDROID || BOSS_WASM
        extern int strcasecmp(const char *, const char *);
        extern int strncasecmp(const char *, const char *, size_t);
    #else
        extern int stricmp(const char*, const char*);
        extern int strnicmp(const char*, const char*, size_t);
    #endif
    #if BOSS_WINDOWS & !BOSS_WINDOWS_MINGW
        extern const char* strpbrk(const char*, const char*);
    #else
        extern char* strpbrk(const char*, const char*);
    #endif

    extern int wcscmp(const wchar_t*, const wchar_t*);
    extern int wcsncmp(const wchar_t*, const wchar_t*, size_t);
    #if BOSS_MAC_OSX || BOSS_IPHONE || BOSS_ANDROID
        extern int wcscasecmp(const wchar_t*, const wchar_t*);
        extern int wcsncasecmp(const wchar_t*, const wchar_t*, size_t);
    #else
        extern int _wcsicmp(const wchar_t*, const wchar_t*);
        extern int _wcsnicmp(const wchar_t*, const wchar_t*, size_t);
    #endif
    #if BOSS_WINDOWS & !BOSS_WINDOWS_MINGW
        extern const wchar_t* wcspbrk(const wchar_t*, const wchar_t*);
    #elif BOSS_LINUX
        extern "C++" const wchar_t* wcspbrk(const wchar_t*, const wchar_t*) THROW;
    #else
        extern wchar_t* wcspbrk(const wchar_t*, const wchar_t*);
    #endif

    // MFC프로젝트시의 문제해결
    extern FILE* fopen(char const*, char const*);
    void _except_handler4_common() {}

    #if BOSS_WINDOWS
        #if BOSS_X64
            #define BOSS_WINAPI
        #else
            #define BOSS_WINAPI __stdcall
        #endif
        extern SOCKET BOSS_WINAPI socket(int domain, int type, int protocol);
        extern int BOSS_WINAPI connect(SOCKET s, const struct sockaddr* name, int namelen);
        extern int BOSS_WINAPI bind(SOCKET s, const struct sockaddr* name, int namelen);
        extern SOCKET BOSS_WINAPI accept(SOCKET s, struct sockaddr* addr, int* addrlen);
        extern int BOSS_WINAPI listen(SOCKET s, int backlog);
        extern int BOSS_WINAPI shutdown(SOCKET s, int how);
        extern int BOSS_WINAPI getsockopt(SOCKET s, int level, int optname, char* optval, int* optlen);
        extern int BOSS_WINAPI setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen);
        extern int BOSS_WINAPI getsockname(SOCKET s, struct sockaddr* name, int* namelen);
        extern int BOSS_WINAPI getpeername(SOCKET s, struct sockaddr* name, int* namelen);
        extern int BOSS_WINAPI recv(SOCKET s, char* buf, int len, int flags);
        extern int BOSS_WINAPI recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen);
        extern int BOSS_WINAPI send(SOCKET s, const char* buf, int len, int flags);
        extern int BOSS_WINAPI sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);
        extern int BOSS_WINAPI ioctlsocket(SOCKET s, long cmd, unsigned long* argp);
        #if BOSS_WINDOWS_MINGW
            extern int BOSS_WINAPI select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const PTIMEVAL timeout);
        #else
            extern int BOSS_WINAPI select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout);
        #endif
        extern int BOSS_WINAPI closesocket(SOCKET s);
        extern struct hostent* BOSS_WINAPI gethostbyaddr(const char* addr, int len, int type);
        extern struct hostent* BOSS_WINAPI gethostbyname(const char* name);
        extern int BOSS_WINAPI getaddrinfo(const char* node, const char* service, const struct addrinfo* hints, struct addrinfo** result);
        extern void BOSS_WINAPI freeaddrinfo(struct addrinfo* ai);
        extern struct servent* BOSS_WINAPI getservbyname(const char* name, const char* proto);
        extern struct servent* BOSS_WINAPI getservbyport(int port, const char* proto);
        extern char* BOSS_WINAPI inet_ntoa(struct in_addr in);
        extern unsigned long BOSS_WINAPI inet_addr(const char* cp);
    #else
        extern int socket(int domain, int type, int protocol) THROW;
        extern int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
        extern int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) THROW;
        extern int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
        extern int listen(int sockfd, int backlog) THROW;
        extern int shutdown(int sockfd, int how) THROW;
        extern int getsockopt(int sockfd, int level, int optname, void* optval, socklen_t* optlen) THROW;
        extern int setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen) THROW;
        extern int getsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen) THROW;
        extern int getpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen) THROW;
        extern ssize_t recv(int sockfd, void* buf, size_t len, int flags);
        extern ssize_t recvfrom(int sockfd, void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen);
        extern ssize_t send(int sockfd, const void* buf, size_t len, int flags);
        extern ssize_t sendto(int sockfd, const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);
        #if BOSS_LINUX | BOSS_ANDROID
            extern int ioctl(int fd, int cmd, ...);
        #else
            extern int fcntl(int fd, int cmd, ...);
        #endif
        extern int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
        extern int close(int fd);
        extern struct hostent* gethostbyaddr(const void* addr, socklen_t len, int type);
        extern struct hostent* gethostbyname(const char* name);
        extern int getaddrinfo(const char* node, const char* service, const struct addrinfo* hints, struct addrinfo** res);
        extern void freeaddrinfo(struct addrinfo* res) THROW;
        extern const char* gai_strerror(int errcode) THROW;
        extern struct servent* getservbyname(const char* name, const char* proto);
        extern struct servent* getservbyport(int port, const char* proto);
        extern char* inet_ntoa(struct in_addr in) THROW;
        extern in_addr_t inet_addr(const char* cp) THROW;
    #endif
}

////////////////////////////////////////////////////////////////////////////////
// String
////////////////////////////////////////////////////////////////////////////////

extern "C" int boss_isalpha(int c)
{
    return isalpha(c);
}

extern "C" int boss_isdigit(int c)
{
    return isdigit(c);
}

extern "C" int boss_isalnum(int c)
{
    return isalnum(c);
}

extern "C" double boss_atof(const char* str)
{
    #if BOSS_ANDROID
        return strtod(str, nullptr);
    #else
        return atof(str);
    #endif
}

extern "C" char* boss_strcpy(char* dest, const char* src)
{
    return strcpy(dest, src);
}

extern "C" char* boss_strncpy(char* dest, const char* src, boss_size_t n)
{
    return strncpy(dest, src, n);
}

extern "C" wchar_t* boss_wcscpy(wchar_t* dest, const wchar_t* src)
{
    return wcscpy(dest, src);
}

extern "C" wchar_t* boss_wcsncpy(wchar_t* dest, const wchar_t* src, boss_size_t n)
{
    return wcsncpy(dest, src, n);
}

extern "C" boss_size_t boss_strlen(const char* str)
{
    return strlen(str);
}

extern "C" boss_size_t boss_wcslen(const wchar_t* str)
{
    return wcslen(str);
}

extern "C" int boss_snprintf(char* str, boss_size_t n, const char* format, ...)
{
    va_list Args;
    va_start(Args, format);
    const sint32 Size = boss_vsnprintf(str, n, format, Args);
    va_end(Args);
    return Size;
}

extern "C" int boss_vsnprintf(char* str, boss_size_t n, const char* format, boss_va_list args)
{
    #if BOSS_LINUX || BOSS_MAC_OSX || BOSS_IPHONE || BOSS_WASM
        char* NewString = nullptr;
        const sint32 Size = vasprintf(&NewString, format, args);

        sint32 CopiedSize = Size;
        if(str)
        {
            if(n - 1 < CopiedSize)
                CopiedSize = sint32(n - 1);
            Memory::Copy(str, NewString, CopiedSize);
            str[CopiedSize] = '\0';
        }

        Memory::Free(NewString);
        return CopiedSize;
    #else
        return vsnprintf(str, n, format, args);
    #endif
}

extern "C" int boss_snwprintf(wchar_t* str, boss_size_t n, const wchar_t* format, ...)
{
    va_list Args;
    va_start(Args, format);
    const sint32 Size = boss_vsnwprintf(str, n, format, Args);
    va_end(Args);
    return Size;
}

extern "C" int boss_vsnwprintf(wchar_t* str, boss_size_t n, const wchar_t* format, boss_va_list args)
{
    #if BOSS_LINUX || BOSS_IPHONE || BOSS_ANDROID || BOSS_WASM
        return vswprintf(str, n, format, args);
    #elif BOSS_MAC_OSX
        if(str == nullptr) // vswprintf의 사용방식 변화에 임시대처
        {
            wchar_t Temp[4096];
            return vswprintf(Temp, 4096, format, args);
        }
        return vswprintf(str, n, format, args);
    #else
        return _vsnwprintf(str, n, format, args);
    #endif
}

extern "C" int boss_strcmp(const char* str1, const char* str2)
{
    return strcmp(str1, str2);
}

extern "C" int boss_strncmp(const char* str1, const char* str2, boss_size_t maxcount)
{
    return strncmp(str1, str2, maxcount);
}

extern "C" int boss_stricmp(const char* str1, const char* str2)
{
    #if BOSS_LINUX || BOSS_MAC_OSX || BOSS_IPHONE || BOSS_ANDROID || BOSS_WASM
        return strcasecmp(str1, str2);
    #else
        return stricmp(str1, str2);
    #endif
}

extern "C" int boss_strnicmp(const char* str1, const char* str2, boss_size_t maxcount)
{
    #if BOSS_LINUX || BOSS_MAC_OSX || BOSS_IPHONE || BOSS_ANDROID || BOSS_WASM
        return strncasecmp(str1, str2, maxcount);
    #else
        return strnicmp(str1, str2, maxcount);
    #endif
}

extern "C" char* boss_strpbrk(const char* str1, const char* str2)
{
    return (char*) strpbrk(str1, str2);
}

extern "C" int boss_wcscmp(const wchar_t* str1, const wchar_t* str2)
{
    return wcscmp(str1, str2);
}

extern "C" int boss_wcsncmp(const wchar_t* str1, const wchar_t* str2, boss_size_t maxcount)
{
    return wcsncmp(str1, str2, maxcount);
}

extern "C" int boss_wcsicmp(const wchar_t* str1, const wchar_t* str2)
{
    #if BOSS_LINUX || BOSS_MAC_OSX || BOSS_IPHONE || BOSS_ANDROID
        return wcscasecmp(str1, str2);
    #else
        return _wcsicmp(str1, str2);
    #endif
}

extern "C" int boss_wcsnicmp(const wchar_t* str1, const wchar_t* str2, boss_size_t maxcount)
{
    #if BOSS_LINUX || BOSS_MAC_OSX || BOSS_IPHONE || BOSS_ANDROID
        return wcsncasecmp(str1, str2, maxcount);
    #else
        return _wcsnicmp(str1, str2, maxcount);
    #endif
}

extern "C" wchar_t* boss_wcspbrk(const wchar_t* str1, const wchar_t* str2)
{
    return (wchar_t*) wcspbrk(str1, str2);
}

////////////////////////////////////////////////////////////////////////////////
// File
////////////////////////////////////////////////////////////////////////////////

class FileClass
{
public:
    FileClass()
    {
        mFileID = -1;
        mFileShare = 1;
        mTypeAssets = false;
        mFilePointer = nullptr;
        mNeedSave = false;
        mContent = nullptr;
        mFileSize = -1;
        mFileOffset = 0;
    }

    ~FileClass()
    {
        if(mTypeAssets)
        {
            #if BOSS_WINDOWS
                fclose((FILE*) mFilePointer);
            #elif BOSS_LINUX || BOSS_WASM
                fclose((FILE*) mFilePointer);
            #elif BOSS_ANDROID
                AAsset_close((AAsset*) mFilePointer);
            #elif BOSS_MAC_OSX || BOSS_IPHONE
                fclose((FILE*) mFilePointer);
            #else
                #error 준비되지 않은 플랫폼입니다
            #endif
        }
        else
        {
            if(mNeedSave && mContent)
                fwrite(mContent->AtDumping(0, mFileSize), 1, mFileSize, (FILE*) mFilePointer);
            fclose((FILE*) mFilePointer);
        }
        delete mContent;
    }

public:
    void ValidSize()
    {
        if(mFileSize == -1)
        {
            if(mTypeAssets)
            {
                #if BOSS_WINDOWS
                    fseek((FILE*) mFilePointer, 0, SEEK_END);
                    #if BOSS_X64
                        mFileSize = _ftelli64((FILE*) mFilePointer);
                    #else
                        mFileSize = ftell((FILE*) mFilePointer);
                    #endif
                    fseek((FILE*) mFilePointer, 0, SEEK_SET);
                #elif BOSS_LINUX || BOSS_WASM
                    fseek((FILE*) mFilePointer, 0, SEEK_END);
                    mFileSize = ftell((FILE*) mFilePointer);
                    fseek((FILE*) mFilePointer, 0, SEEK_SET);
                #elif BOSS_ANDROID
                    mFileSize = AAsset_getLength((AAsset*) mFilePointer);
                #elif BOSS_MAC_OSX || BOSS_IPHONE
                    fseek((FILE*) mFilePointer, 0, SEEK_END);
                    mFileSize = ftell((FILE*) mFilePointer);
                    fseek((FILE*) mFilePointer, 0, SEEK_SET);
                #else
                    #error 준비되지 않은 플랫폼입니다
                #endif
            }
            else
            {
                fseek((FILE*) mFilePointer, 0, SEEK_END);
                #if BOSS_X64
                    mFileSize = _ftelli64((FILE*) mFilePointer);
                #else
                    mFileSize = ftell((FILE*) mFilePointer);
                #endif
                fseek((FILE*) mFilePointer, 0, SEEK_SET);
            }
        }
    }

    void ValidContent()
    {
        if(mContent == nullptr)
        {
            ValidSize();
            mContent = new uint08s();
            if(mTypeAssets)
            {
                #if BOSS_WINDOWS
                    fread(mContent->AtDumping(0, mFileSize), 1, mFileSize, (FILE*) mFilePointer);
                    fseek((FILE*) mFilePointer, 0, SEEK_SET);
                #elif BOSS_LINUX || BOSS_WASM
                    fread(mContent->AtDumping(0, mFileSize), 1, mFileSize, (FILE*) mFilePointer);
                    fseek((FILE*) mFilePointer, 0, SEEK_SET);
                #elif BOSS_ANDROID
                    const void* AssetBuffer = AAsset_getBuffer((AAsset*) mFilePointer);
                    Memory::Copy(mContent->AtDumping(0, mFileSize), AssetBuffer, mFileSize);
                #elif BOSS_MAC_OSX || BOSS_IPHONE
                    fread(mContent->AtDumping(0, mFileSize), 1, mFileSize, (FILE*) mFilePointer);
                    fseek((FILE*) mFilePointer, 0, SEEK_SET);
                #else
                    #error 준비되지 않은 플랫폼입니다
                #endif
            }
            else
            {
                fread(mContent->AtDumping(0, mFileSize), 1, mFileSize, (FILE*) mFilePointer);
                fseek((FILE*) mFilePointer, 0, SEEK_SET);
            }
        }
    }

public:
    sint32 mFileID;
    String mFileName;
    int mFileShare;
    bool mTypeAssets;
    void* mFilePointer;
    bool mNeedSave;
    uint08s* mContent;
    boss_ssize_t mFileSize;
    boss_ssize_t mFileOffset;
};
static Map<FileClass> gAllFiles;
static sint32 gLastFileID = -1;

namespace BOSS
{
    chars _private_GetFileName(boss_file file)
    {
        FileClass* CurFile = (FileClass*) file;
        if(CurFile)
            return CurFile->mFileName;
        return "";
    }

    sint64 _private_GetFileOffset(boss_file file)
    {
        FileClass* CurFile = (FileClass*) file;
        if(CurFile)
            return CurFile->mFileOffset;
        return -1;
    }

    void _private_SetFileRetain(boss_file file)
    {
        FileClass* CurFile = (FileClass*) file;
        if(CurFile)
            CurFile->mFileShare++;
    }
};

#if BOSS_ANDROID
    extern JNIEnv* GetAndroidJNIEnv();
    extern jobject GetAndroidApplicationContext();

    static AAssetManager* GetAAssetManager()
    {
        static AAssetManager* pAssetManager = nullptr;
        if(!pAssetManager)
        {
            jobject context = GetAndroidApplicationContext();
            BOSS_TRACE("context=0x%08X", context);

            JNIEnv* env = GetAndroidJNIEnv();
            jclass contextClass = env->GetObjectClass(context);
            BOSS_TRACE("contextClass=0x%08X", contextClass);
            jmethodID methodGetAssets = env->GetMethodID(contextClass,
                "getAssets", "()Landroid/content/res/AssetManager;");
            BOSS_TRACE("methodGetAssets=0x%08X", methodGetAssets);
            jobject localAssetManager = env->CallObjectMethod(context, methodGetAssets);
            BOSS_TRACE("localAssetManager=0x%08X", localAssetManager);
            jobject globalAssetManager = env->NewGlobalRef(localAssetManager);
            BOSS_TRACE("globalAssetManager=0x%08X", globalAssetManager);
            pAssetManager = AAssetManager_fromJava(env, globalAssetManager);
            BOSS_TRACE("pAssetManager=0x%08X", pAssetManager);
        }
        return pAssetManager;
    }
#endif

extern "C" const char* boss_normalpath(const char* itemname, boss_drive* result)
{
    boss_drive Drive = drive_error;
    String& NormalPath = *BOSS_STORAGE_SYS(String);

    // 리눅스계열 패스식별자 스킵
    if(itemname[0] == '\\' && itemname[1] == '\\' && itemname[2] == '?' && itemname[3] == '\\')
        itemname += 4;

    // 드라이브식별자 판단(itemname위치도 재조정, 중간에 등장할 경우도 고려)
    Drive = drive_relative;
    char DriveCode;
    chars DirFocus = itemname;
    for(chars item = itemname; *item != '\0'; ++item)
    {
        if(*item == '/' || *item == '\\')
            DirFocus = item + 1;
        else if(item[0] == ':' && (item[1] == '/' || item[1] == '\\'))
        {
            itemname = item + 2;
            Drive = drive_error;
            if(item - DirFocus == 1)
            {
                if('A' <= *DirFocus && *DirFocus <= 'Z')
                {
                    Drive = drive_absolute;
                    DriveCode = *DirFocus;
                }
                else if('a' <= *DirFocus && *DirFocus <= 'z')
                {
                    Drive = drive_absolute;
                    DriveCode = 'A' + (*DirFocus - 'a');
                }
            }
            else if(item - DirFocus == 8)
            {
                if(!boss_strnicmp(DirFocus, "relative", 8)) Drive = drive_relative;
            }
            else if(item - DirFocus == 6)
            {
                if(!boss_strnicmp(DirFocus, "assets", 6)) Drive = drive_assets;
                else if(!boss_strnicmp(DirFocus, "memory", 6)) Drive = drive_memory;
            }
        }
    }

    // 노멀패스에 드라이브식별자를 붙임
    branch;
    jump(Drive == drive_absolute)
    {
        NormalPath = DriveCode;
        NormalPath += ":/";
    }
    jump(Drive == drive_assets) NormalPath = "assets:/";
    jump(Drive == drive_memory) NormalPath = "memory:/";
    else NormalPath.Empty();

    // 노멀패스에 하위 항목을 붙임
    DirFocus = itemname;
    for(chars item = itemname; *item != '\0'; ++item)
    {
        if(*item == '/' || *item == '\\')
        {
            branch;
            jump(item - DirFocus == 0) nothing;
            jump(item - DirFocus == 2 && DirFocus[0] == '.' && DirFocus[1] == '.')
            {
                if(0 < NormalPath.Length()) NormalPath.SubTail(1); // 끝 슬래시 제거후
                while(0 < NormalPath.Length() && NormalPath[-2] != '/')
                    NormalPath.SubTail(1); // 가능한한 슬래시를 만나기 전까지 제거
            }
            else
            {
                NormalPath += String(DirFocus, item - DirFocus); // 디렉토리명을 붙임
                NormalPath += '/';
            }
            DirFocus = item + 1;
        }
    }
    NormalPath += DirFocus; // 파일명을 붙임

    if(result) *result = Drive;
    return NormalPath;
}

extern "C" boss_file boss_fopen(const char* filename, const char* mode)
{
    boss_drive Drive = drive_error;
    const String NormalFilename = boss_normalpath(filename, &Drive);
    if(Drive == drive_error || Drive == drive_memory) return nullptr;
    if(Drive == drive_absolute && NormalFilename[0] == 'Q')
        filename = ((chars) NormalFilename) + 3; // "Q:/"
    else if(Drive == drive_assets) filename = ((chars) NormalFilename) + 8; // "assets:/"
    else filename = NormalFilename;

    bool ReadFlag = false, SaveFlag = false;
    while(*mode != '\0')
    {
        if(*mode == 'r') ReadFlag = true;
        else if(*mode == '+') SaveFlag = true;
        else if(*mode == 'a') BOSS_ASSERT("a모드는 준비중입니다", false);
        mode++;
    }

    if(ReadFlag)
    {
        if(Drive != drive_assets)
        {
            FILE* NewFilePointer = fopen(filename, (SaveFlag)? "r+b" : "rb");
            if(!NewFilePointer)
                return nullptr;
            FileClass* NewFile = &gAllFiles[++gLastFileID];
            NewFile->mFileID = gLastFileID;
            NewFile->mFileName = filename;
            NewFile->mTypeAssets = false;
            NewFile->mFilePointer = NewFilePointer;
            NewFile->mNeedSave = SaveFlag;
            return (boss_file) NewFile;
        }
        else if(!SaveFlag)
        {
            #if BOSS_WINDOWS
                const String NewFileName = String("../assets/") + filename;
                FILE* NewAssetsPointer = fopen(NewFileName, "rb");
                if(!NewAssetsPointer) return nullptr;
            #elif BOSS_LINUX || BOSS_WASM
                const String NewFileName = String("../assets/") + filename;
                FILE* NewAssetsPointer = fopen(NewFileName, "rb");
                if(!NewAssetsPointer) return nullptr;
            #elif BOSS_MAC_OSX
                BOSS_ASSERT("개발이 필요합니다!", false);
                const String NewFileName = String("assets:/") + filename;
                FILE* NewAssetsPointer = fopen(NewFileName, "rb");
                if(!NewAssetsPointer) return nullptr;
            #elif BOSS_IPHONE
                BOSS_ASSERT("개발이 필요합니다!", false);
                const String NewFileName = String("assets:/") + filename;
                FILE* NewAssetsPointer = fopen(NewFileName, "rb");
                if(!NewAssetsPointer) return nullptr;
            #elif BOSS_ANDROID
                const String NewFileName = String("assets:/") + filename;
                AAsset* NewAssetsPointer = AAssetManager_open(GetAAssetManager(), filename, AASSET_MODE_BUFFER);
                if(!NewAssetsPointer) return nullptr;
            #else
                #error 준비되지 않은 플랫폼입니다
            #endif

            FileClass* NewFile = &gAllFiles[++gLastFileID];
            NewFile->mFileID = gLastFileID;
            NewFile->mFileName = NewFileName;
            NewFile->mTypeAssets = true;
            NewFile->mFilePointer = NewAssetsPointer;
            NewFile->mNeedSave = false;
            return (boss_file) NewFile;
        }
    }
    else if(Drive != drive_assets)
    {
        FILE* NewFilePointer = fopen(filename, "wb");
        if(!NewFilePointer) return nullptr;
        FileClass* NewFile = &gAllFiles[++gLastFileID];
        NewFile->mFileID = gLastFileID;
        NewFile->mFileName = filename;
        NewFile->mTypeAssets = false;
        NewFile->mFilePointer = NewFilePointer;
        NewFile->mNeedSave = true;
        NewFile->mContent = new uint08s();
        NewFile->mFileSize = 0;
        return (boss_file) NewFile;
    }
    return nullptr;
}

extern "C" int boss_fclose(boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        if(0 < CurFile->mFileShare && --CurFile->mFileShare == 0)
            gAllFiles.Remove(CurFile->mFileID);
        return 0;
    }
    return EOF;
}

extern "C" int boss_feof(boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        CurFile->ValidSize();
        if(CurFile->mFileOffset < CurFile->mFileSize)
            return 0;
    }
    return EOF;
}

extern "C" int boss_fseek(boss_file file, boss_ssize_t offset, int origin)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        CurFile->ValidSize();
        switch(origin)
        {
        case SEEK_SET:
            CurFile->mFileOffset = offset;
            break;
        case SEEK_CUR:
            CurFile->mFileOffset += offset;
            break;
        case SEEK_END:
            CurFile->mFileOffset = CurFile->mFileSize + offset;
            break;
        }
        if(CurFile->mFileOffset < 0)
            CurFile->mFileOffset = 0;
        else if(CurFile->mFileSize < CurFile->mFileOffset)
        {
            uint08* CurDst = CurFile->mContent->AtDumping(CurFile->mFileSize, CurFile->mFileOffset - CurFile->mFileSize);
            Memory::Set(CurDst, 0, CurFile->mFileOffset - CurFile->mFileSize);
            CurFile->mFileSize = CurFile->mFileOffset;
        }
        return 0;
    }
    return EOF;
}

extern "C" boss_ssize_t boss_ftell(boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
        return CurFile->mFileOffset;
    return EOF;
}

extern "C" boss_size_t boss_fread(void* ptr, boss_size_t size, boss_size_t count, boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        CurFile->ValidContent();
        if(CurFile->mFileOffset < CurFile->mFileSize)
        {
            const sint32 CopyLen = Math::Min(size * count,
                (CurFile->mFileSize - CurFile->mFileOffset) / size * size);
            Memory::Copy(ptr, CurFile->mContent->AtDumping(CurFile->mFileOffset, CopyLen), CopyLen);
            CurFile->mFileOffset += CopyLen;
            return CopyLen / size;
        }
    }
    return 0;
}

extern "C" boss_size_t boss_fwrite(const void* ptr, boss_size_t size, boss_size_t count, boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        CurFile->ValidContent();
        const sint32 CopyLen = size * count;
        Memory::Copy(CurFile->mContent->AtDumping(CurFile->mFileOffset, CopyLen), ptr, CopyLen);
        CurFile->mFileSize = CurFile->mContent->Count();
        CurFile->mFileOffset += CopyLen;
        return CopyLen / size;
    }
    return 0;
}

extern "C" int boss_fgetc(boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        CurFile->ValidContent();
        if(CurFile->mFileOffset < CurFile->mFileSize)
            return (*CurFile->mContent)[(sint32) CurFile->mFileOffset++];
        CurFile->mFileOffset++;
    }
    return EOF;
}

extern "C" int boss_ungetc(int character, boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        CurFile->ValidContent();
        if(0 < CurFile->mFileOffset)
        {
            CurFile->mFileOffset--;
            if(CurFile->mFileOffset < CurFile->mFileSize)
                CurFile->mContent->At(CurFile->mFileOffset) = (uint08) character;
            return character;
        }
    }
    return EOF;
}

extern "C" int boss_fprintf(boss_file file, const char* format, boss_va_list args)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        const sint32 TextLen = boss_vsnprintf(nullptr, 0, format, args);
        char* Buf = new char[TextLen + 1];
        boss_vsnprintf(Buf, TextLen + 1, format, args);

        if(file != stdin && file != stdout && file != stderr)
        {
            CurFile->ValidContent();
            Memory::Copy(CurFile->mContent->AtDumping(CurFile->mFileOffset, TextLen), Buf, TextLen);
            CurFile->mFileSize = CurFile->mContent->Count();
            CurFile->mFileOffset += TextLen;
        }

        delete[] Buf;
        return TextLen;
    }
    return EOF;
}

extern "C" void boss_rewind(boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
        CurFile->mFileOffset = 0;
}

extern "C" char* boss_fgets(char* str, int num, boss_file file)
{
    FileClass* CurFile = (FileClass*) file;
    if(CurFile)
    {
        CurFile->ValidContent();
        bytes CurContent = CurFile->mContent->AtDumping(0, CurFile->mFileSize);
        if(CurFile->mFileOffset < CurFile->mFileSize)
        {
            for(sint32 i = 0, iend = num - 1; i < iend; ++i)
            {
                const char NewChar = (const char) CurContent[CurFile->mFileOffset++];
                str[i] = NewChar;
                if(NewChar == '\n' || CurFile->mFileOffset == CurFile->mFileSize)
                {
                    str[i + 1] = '\0';
                    return str;
                }
            }
            str[num - 1] = '\0';
            return str;
        }
    }
    return nullptr;
}

class DirClass
{
public:
    DirClass()
    {
        mDirID = -1;
        mTypeAssets = false;
        mDirHandle = nullptr;
        mDirPointer = nullptr;
        mNextFlag = false;
    }

    ~DirClass()
    {
        #if BOSS_WINDOWS
            FindClose((HANDLE) mDirHandle);
            delete (WIN32_FIND_DATAW*) mDirPointer;
        #elif BOSS_LINUX || BOSS_WASM
            closedir((DIR*) mDirHandle);
        #elif BOSS_MAC_OSX || BOSS_IPHONE
            closedir((DIR*) mDirHandle);
        #elif BOSS_ANDROID
            if(mTypeAssets)
                AAssetDir_close((AAssetDir*) mDirHandle);
            else closedir((DIR*) mDirHandle);
        #else
            #error 준비되지 않은 플랫폼입니다
        #endif
    }

public:
    sint32 mDirID;
    bool mTypeAssets;
    void* mDirHandle;
    void* mDirPointer;
    bool mNextFlag;
    String mLastFilePath;
};
static Map<DirClass> gAllDirs;
static sint32 gLastDirID = -1;

extern "C" boss_dir boss_opendir(const char* dirname)
{
    boss_drive Drive = drive_error;
    const String NormalFilename = boss_normalpath(dirname, &Drive);
    if(Drive == drive_error || Drive == drive_memory) return nullptr;
    if(Drive == drive_absolute && NormalFilename[0] == 'Q')
        dirname = ((chars) NormalFilename) + 3; // "Q:/"
    else if(Drive == drive_assets) dirname = ((chars) NormalFilename) + 8; // "assets:/"
    else dirname = NormalFilename;

    if(Drive == drive_assets)
    {
        #if BOSS_WINDOWS
            WIN32_FIND_DATAW* NewFindFileData = new WIN32_FIND_DATAW();
            HANDLE NewDirHandle = FindFirstFileW((wchars) WString::FromChars(String("../assets/") + dirname), NewFindFileData);
            if(NewDirHandle == INVALID_HANDLE_VALUE)
            {
                delete NewFindFileData;
                return nullptr;
            }
        #else
            String DirnameAtAssets = dirname;
            if(2 < DirnameAtAssets.Length() && DirnameAtAssets[-3] == '/' && DirnameAtAssets[-2] == '*')
                DirnameAtAssets.SubTail(2);
            dirname = DirnameAtAssets;

            #if BOSS_LINUX || BOSS_WASM
                const String ParentPath = ".";
                DIR* NewDirHandle = opendir((chars) (ParentPath + '/' + dirname));
                if(!NewDirHandle) return nullptr;
                void* NewFindFileData = nullptr;
            #elif BOSS_MAC_OSX || BOSS_IPHONE
                CFURLRef ResourceURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
                #if BOSS_MAC_OSX
                    char TempString[1024];
                    if(!CFURLGetFileSystemRepresentation(ResourceURL, TRUE, (UInt8*) TempString, 1024))
                        return nullptr;
                    const String ParentPath = TempString;
                #else // 아이폰 및 시뮬레이터
                    const String ParentPath = CFStringGetCStringPtr(CFURLGetString(ResourceURL), kCFStringEncodingUTF8) + 7; // 7은 "file://"
                #endif
                CFRelease(ResourceURL);
                DIR* NewDirHandle = opendir((chars) (ParentPath + '/' + dirname));
                if(!NewDirHandle) return nullptr;
                void* NewFindFileData = nullptr;
            #elif BOSS_ANDROID
                AAssetDir* NewDirHandle = AAssetManager_openDir(GetAAssetManager(), dirname);
                if(!NewDirHandle) return nullptr;
                chars NewFindFileData = AAssetDir_getNextFileName(NewDirHandle);
                if(!NewFindFileData)
                {
                    AAssetDir_close(NewDirHandle);
                    return nullptr;
                }
            #else
                #error 준비되지 않은 플랫폼입니다
            #endif
        #endif

        DirClass* NewDir = &gAllDirs[++gLastDirID];
        NewDir->mDirID = gLastDirID;
        NewDir->mTypeAssets = true;
        NewDir->mDirHandle = (void*) NewDirHandle;
        NewDir->mDirPointer = (void*) NewFindFileData;
        NewDir->mNextFlag = true;
        return (boss_dir) NewDir;
    }
    else
    {
        #if BOSS_WINDOWS
            WIN32_FIND_DATAW* NewFindFileData = new WIN32_FIND_DATAW();
            HANDLE NewDirHandle = FindFirstFileW((wchars) WString::FromChars(dirname), NewFindFileData);
            if(NewDirHandle == INVALID_HANDLE_VALUE)
            {
                delete NewFindFileData;
                return nullptr;
            }
        #elif BOSS_LINUX || BOSS_MAC_OSX || BOSS_IPHONE || BOSS_ANDROID || BOSS_WASM
            String DirnameAtAssets = dirname;
            if(2 < DirnameAtAssets.Length() && DirnameAtAssets[-3] == '/' && DirnameAtAssets[-2] == '*')
                DirnameAtAssets.SubTail(2);
            dirname = DirnameAtAssets;
            DIR* NewDirHandle = opendir(dirname);
            if(!NewDirHandle) return nullptr;
            void* NewFindFileData = nullptr;
        #else
            #error 준비되지 않은 플랫폼입니다
        #endif

        DirClass* NewDir = &gAllDirs[++gLastDirID];
        NewDir->mDirID = gLastDirID;
        NewDir->mTypeAssets = false;
        NewDir->mDirHandle = (void*) NewDirHandle;
        NewDir->mDirPointer = (void*) NewFindFileData;
        NewDir->mNextFlag = true;
        return (boss_dir) NewDir;
    }
    return nullptr;
}

extern "C" boss_dirent boss_readdir(boss_dir dir)
{
    DirClass* CurDir = (DirClass*) dir;
    if(CurDir)
    {
        #if BOSS_WINDOWS
            if(CurDir->mNextFlag)
            {
                CurDir->mLastFilePath = String::FromWChars(((WIN32_FIND_DATAW*) CurDir->mDirPointer)->cFileName);
                CurDir->mNextFlag = FindNextFileW((HANDLE) CurDir->mDirHandle, (WIN32_FIND_DATAW*) CurDir->mDirPointer);
                return (boss_dirent) (chars) CurDir->mLastFilePath;
            }
        #elif BOSS_LINUX || BOSS_WASM
            struct dirent* CurDirEnt = readdir((DIR*) CurDir->mDirHandle);
            if(CurDirEnt)
            {
                CurDir->mLastFilePath = CurDirEnt->d_name;
                return (boss_dirent) (chars) CurDir->mLastFilePath;
            }
        #elif BOSS_MAC_OSX || BOSS_IPHONE
            struct dirent* CurDirEnt = readdir((DIR*) CurDir->mDirHandle);
            if(CurDirEnt)
            {
                CurDir->mLastFilePath = CurDirEnt->d_name;
                return (boss_dirent) (chars) CurDir->mLastFilePath;
            }
        #elif BOSS_ANDROID
            if(CurDir->mTypeAssets)
            {
                if(CurDir->mNextFlag)
                {
                    CurDir->mLastFilePath = (chars) CurDir->mDirPointer;
                    CurDir->mDirPointer = (void*) AAssetDir_getNextFileName((AAssetDir*) CurDir->mDirHandle);
                    CurDir->mNextFlag = (CurDir->mDirPointer != nullptr);
                    return (boss_dirent) (chars) CurDir->mLastFilePath;
                }
            }
            else
            {
                struct dirent* CurDirEnt = readdir((DIR*) CurDir->mDirHandle);
                if(CurDirEnt)
                {
                    CurDir->mLastFilePath = CurDirEnt->d_name;
                    return (boss_dirent) (chars) CurDir->mLastFilePath;
                }
            }
        #else
            #error 준비되지 않은 플랫폼입니다
        #endif
    }
    return nullptr;
}

const char* boss_getdirname(boss_dirent dirent)
{
    return (chars) dirent;
}

extern "C" int boss_closedir(boss_dir dir)
{
    DirClass* CurDir = (DirClass*) dir;
    if(CurDir)
    {
        gAllDirs.Remove(CurDir->mDirID);
        return 0;
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Socket
////////////////////////////////////////////////////////////////////////////////

extern "C" int boss_socket(int domain, int type, int protocol)
{
    return socket(domain, type, protocol);
}

extern "C" int boss_connect(int sockfd, const void* addr, int addrlen)
{
    #if BOSS_WINDOWS | BOSS_LINUX | BOSS_ANDROID | BOSS_WASM
        return connect(sockfd, (const struct sockaddr*) addr, addrlen);
    #else
        struct sockaddr_in AddrIn;
        Memory::Copy(&AddrIn, addr, addrlen);
        AddrIn.sin_len = addrlen;
        return connect(sockfd, (const struct sockaddr*) &AddrIn, (socklen_t) addrlen);
    #endif
}

extern "C" int boss_bind(int sockfd, const void* addr, int addrlen)
{
    #if BOSS_WINDOWS | BOSS_LINUX | BOSS_ANDROID | BOSS_WASM
        return bind(sockfd, (const struct sockaddr*) addr, addrlen);
    #else
        struct sockaddr_in AddrIn;
        Memory::Copy(&AddrIn, addr, addrlen);
        AddrIn.sin_len = addrlen;
        return bind(sockfd, (const struct sockaddr*) &AddrIn, (socklen_t) addrlen);
    #endif
}

extern "C" int boss_accept(int sockfd, void* addr, int* addrlen)
{
    #if BOSS_WINDOWS | BOSS_ANDROID
        return accept(sockfd, (struct sockaddr*) addr, addrlen);
    #else
        return accept(sockfd, (struct sockaddr*) addr, (socklen_t*) addrlen);
    #endif
}

extern "C" int boss_listen(int sockfd, int backlog)
{
    return listen(sockfd, backlog);
}

extern "C" int boss_shutdown(int sockfd, int how)
{
    return shutdown(sockfd, how);
}

extern "C" int boss_getsockopt(int sockfd, int level, int optname, void* optval, int* optlen)
{
    #if BOSS_WINDOWS | BOSS_ANDROID
        return getsockopt(sockfd, level, optname, (char*) optval, optlen);
    #else
        return getsockopt(sockfd, level, optname, optval, (socklen_t*) optlen);
    #endif
}

extern "C" int boss_setsockopt(int sockfd, int level, int optname, const void* optval, int optlen)
{
    #if BOSS_WINDOWS | BOSS_ANDROID
        return setsockopt(sockfd, level, optname, (const char*) optval, optlen);
    #else
        return setsockopt(sockfd, level, optname, optval, (socklen_t) optlen);
    #endif
}

extern "C" int boss_getsockname(int sockfd, void* addr, int* addrlen)
{
    #if BOSS_WINDOWS | BOSS_ANDROID
        return getsockname(sockfd, (struct sockaddr*) addr, addrlen);
    #else
        return getsockname(sockfd, (struct sockaddr*) addr, (socklen_t*) addrlen);
    #endif
}

extern "C" int boss_getpeername(int sockfd, void* addr, int* addrlen)
{
    #if BOSS_WINDOWS | BOSS_ANDROID
        return getpeername(sockfd, (struct sockaddr*) addr, addrlen);
    #else
        return getpeername(sockfd, (struct sockaddr*) addr, (socklen_t*) addrlen);
    #endif
}

extern "C" boss_ssize_t boss_recv(int sockfd, void* buf, boss_size_t len, int flags)
{
    #if BOSS_WINDOWS | BOSS_ANDROID
        return recv(sockfd, (char*) buf, len, flags);
    #else
        return recv(sockfd, buf, len, flags);
    #endif
}

extern "C" boss_ssize_t boss_recvfrom(int sockfd, void* buf, boss_size_t len, int flags, void* src_addr, int* addrlen)
{
    #if BOSS_WINDOWS | BOSS_ANDROID
        return recvfrom(sockfd, (char*) buf, len, flags, (struct sockaddr*) src_addr, addrlen);
    #else
        return recvfrom(sockfd, buf, len, flags, (struct sockaddr*) src_addr, (socklen_t*) addrlen);
    #endif
}

extern "C" boss_ssize_t boss_send(int sockfd, const void* buf, boss_size_t len, int flags)
{
    #if BOSS_WINDOWS | BOSS_ANDROID
        return send(sockfd, (const char*) buf, len, flags);
    #else
        return send(sockfd, buf, len, flags);
    #endif
}

extern "C" boss_ssize_t boss_sendto(int sockfd, const void* buf, boss_size_t len, int flags, const void* dest_addr, int addrlen)
{
    #if BOSS_WINDOWS | BOSS_LINUX | BOSS_ANDROID
        return sendto(sockfd, (const char*) buf, len, flags, (const struct sockaddr*) dest_addr, addrlen);
    #elif BOSS_WASM
        return sendto(sockfd, buf, len, flags, (const struct sockaddr*) dest_addr, addrlen);
    #else
        struct sockaddr_in DestAddrIn;
        Memory::Copy(&DestAddrIn, dest_addr, addrlen);
        DestAddrIn.sin_len = addrlen;
        return sendto(sockfd, buf, len, flags, (const struct sockaddr*) &DestAddrIn, (socklen_t) addrlen);
    #endif
}

extern "C" int boss_ioctlsocket(int sockfd, long cmd, unsigned long* argp)
{
    #if BOSS_WINDOWS
        return ioctlsocket(sockfd, cmd, argp);
    #elif BOSS_LINUX | BOSS_ANDROID | BOSS_WASM
        return ioctl(sockfd, cmd, argp);
    #else
        #define IOC_IN       0x80000000
        #define IOCPARM_MASK 0x7f
        #define _IOW(x,y,t)  (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
        #define FIONBIO      _IOW('f', 126, u_long)

        const sint32 OldFlags = fcntl(sockfd, F_GETFL, 0);
        sint32 NewFlag = 0;
        switch(cmd)
        {
        case FIONBIO: NewFlag = O_NONBLOCK;
        }
        return (0 <= fcntl(sockfd, F_SETFL, OldFlags | NewFlag));
    #endif
}

extern "C" int boss_select(int nfds, boss_fd_set* readfds, boss_fd_set* writefds, boss_fd_set* exceptfds, void* timeout)
{
    #if BOSS_WINDOWS
        return select(nfds, (fd_set*) readfds, (fd_set*) writefds, (fd_set*) exceptfds, (struct timeval*) timeout);
    #else
        return select(nfds,
            (fd_set*) ((readfds)? &readfds->fds_bits : nullptr),
            (fd_set*) ((writefds)? &writefds->fds_bits : nullptr),
            (fd_set*) ((exceptfds)? &exceptfds->fds_bits : nullptr),
            (struct timeval*) timeout);
    #endif
}

extern "C" int boss_closesocket(int sockfd)
{
    #if BOSS_WINDOWS
        return closesocket(sockfd);
    #else
        return close(sockfd);
    #endif
}

extern "C" void* boss_gethostbyaddr(const void* addr, int len, int type)
{
    #if BOSS_WINDOWS
        return (void*) gethostbyaddr((const char*) addr, len, type);
    #else
        return (void*) gethostbyaddr(addr, (socklen_t) len, type);
    #endif
}

extern "C" void* boss_gethostbyname(const char* name)
{
    return gethostbyname(name);
}

extern "C" int boss_getaddrinfo(const char* node, const char* service, const void* hints, void** res)
{
    return getaddrinfo(node, service, (const struct addrinfo*) hints, (struct addrinfo**) res);
}

extern "C" void boss_freeaddrinfo(void* res)
{
    return freeaddrinfo((struct addrinfo*) res);
}

extern "C" const char* boss_gai_strerror(int errcode)
{
    #if BOSS_WINDOWS
        String& Result = *BOSS_STORAGE_SYS(String);
        Result = String::Format("boss_gai_strerror: %d", errcode);
        return Result;
    #else
        return gai_strerror(errcode);
    #endif
}

extern "C" void* boss_getservbyname(const char* name, const char* proto)
{
    return (void*) getservbyname(name, proto);
}

extern "C" void* boss_getservbyport(int port, const char* proto)
{
    return (void*) getservbyport(port, proto);
}

extern "C" char* boss_inet_ntoa(void* in)
{
    return inet_ntoa(*((struct in_addr*) in));
}

extern "C" unsigned int boss_inet_addr(const char* cp)
{
    return (unsigned int) inet_addr(cp);
}

extern "C" unsigned int boss_htonl(unsigned int hostlong)
{
    #if BOSS_WINDOWS
        return (unsigned int) htonl(hostlong);
    #else
        return (unsigned int) htonl((uint32_t) hostlong);
    #endif
}

extern "C" unsigned short boss_htons(unsigned short hostshort)
{
    #if BOSS_WINDOWS
        return (unsigned short) htons(hostshort);
    #else
        return (unsigned short) htons((uint16_t) hostshort);
    #endif
}

extern "C" unsigned int boss_ntohl(unsigned int netlong)
{
    #if BOSS_WINDOWS
        return (unsigned int) ntohl(netlong);
    #else
        return (unsigned int) ntohl((uint32_t) netlong);
    #endif
}

extern "C" unsigned short boss_ntohs(unsigned short netshort)
{
    #if BOSS_WINDOWS
        return (unsigned short) ntohs(netshort);
    #else
        return (unsigned short) ntohs((uint16_t) netshort);
    #endif
}

extern "C" void boss_FD_ZERO(boss_fd_set* fdset)
{
    #if BOSS_WINDOWS
        FD_ZERO(fdset);
    #else
        FD_ZERO((fd_set*) &fdset->fds_bits);
        fdset->fd_count = 0;
    #endif
}

extern "C" void boss_FD_SET(int fd, boss_fd_set* fdset)
{
    #if BOSS_WINDOWS
        FD_SET(fd, fdset);
    #else
        FD_SET(fd, (fd_set*) &fdset->fds_bits);
        fdset->fd_count++;
    #endif
}

extern "C" void boss_FD_CLR(int fd, boss_fd_set* fdset)
{
    #if BOSS_WINDOWS
        FD_CLR(fd, fdset);
    #else
        FD_CLR(fd, (fd_set*) &fdset->fds_bits);
    #endif
}

extern "C" int boss_FD_ISSET(int fd, boss_fd_set* fdset)
{
    #if BOSS_WINDOWS
        return FD_ISSET(fd, fdset);
    #else
        return FD_ISSET(fd, (fd_set*) &fdset->fds_bits);
    #endif
}

extern "C" int boss_geterrno()
{
    #if BOSS_WINDOWS
        return WSAGetLastError() - 10000;
    #else
        return errno;
    #endif
}

extern "C" void boss_seterrno(int err)
{
    #if BOSS_WINDOWS
        if(err == 0) WSASetLastError(0);
        else WSASetLastError(err + 10000);
    #else
        errno = err;
    #endif
}

#if !defined(BOSS_NEED_FORCED_FAKEWIN) || (BOSS_NEED_FORCED_FAKEWIN != 0 && BOSS_NEED_FORCED_FAKEWIN != 1)
    #error BOSS_NEED_FORCED_FAKEWIN macro is invalid use
#endif
#if !defined(BOSS_NEED_FULLSCREEN) || (BOSS_NEED_FULLSCREEN != 0 && BOSS_NEED_FULLSCREEN != 1)
    #error BOSS_NEED_FULLSCREEN macro is invalid use
#endif
#if !defined(BOSS_NEED_MAIN) || (BOSS_NEED_MAIN != 0 && BOSS_NEED_MAIN != 1)
    #error BOSS_NEED_MAIN macro is invalid use
#endif
#if !defined(BOSS_NEED_PROFILE) || (BOSS_NEED_PROFILE != 0 && BOSS_NEED_PROFILE != 1)
    #error BOSS_NEED_PROFILE macro is invalid use
#endif
#if !defined(BOSS_NEED_RELEASE_TRACE) || (BOSS_NEED_RELEASE_TRACE != 0 && BOSS_NEED_RELEASE_TRACE != 1)
    #error BOSS_NEED_RELEASE_TRACE macro is invalid use
#endif
#if !defined(BOSS_NEED_NATIVE_OPENGL) || (BOSS_NEED_NATIVE_OPENGL != 0 && BOSS_NEED_NATIVE_OPENGL != 1)
    #error BOSS_NEED_NATIVE_OPENGL macro is invalid use
#endif
#if !defined(BOSS_NEED_SILENT_NIGHT) || (BOSS_NEED_SILENT_NIGHT != 0 && BOSS_NEED_SILENT_NIGHT != 1)
    #error BOSS_NEED_SILENT_NIGHT macro is invalid use
#endif
#if !defined(BOSS_NEED_EMBEDDED_ASSET) || (BOSS_NEED_EMBEDDED_ASSET != 0 && BOSS_NEED_EMBEDDED_ASSET != 1)
    #error BOSS_NEED_EMBEDDED_ASSET macro is invalid use
#endif
#if !defined(BOSS_NEED_EMBEDDED_CACHE) || (BOSS_NEED_EMBEDDED_CACHE != 0 && BOSS_NEED_EMBEDDED_CACHE != 1)
    #error BOSS_NEED_EMBEDDED_CACHE macro is invalid use
#endif
