#include <boss.hpp>
#include "boss_platform_impl.hpp"

#if BOSS_WINDOWS
    #include <shlobj.h>
    #pragma comment(lib, "user32.lib")
    #pragma comment(lib, "comdlg32.lib")
    #pragma comment(lib, "ole32.lib")
    #pragma comment(lib, "shell32.lib")
    #pragma comment(lib, "advapi32.lib")
    #include <pdh.h>
    #pragma comment(lib, "pdh.lib")
#elif BOSS_LINUX
    #ifndef BOSS_SILENT_NIGHT_IS_ENABLED
        #include <gtk/gtk.h>
    #endif
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
#elif BOSS_IPHONE
    #include "ios/src/BossWebView.h"
#elif BOSS_ANDROID
    #include <sys/sysinfo.h>
    #include <jni.h>
#endif

#if BOSS_IPHONE
    extern void* GetIOSApplicationUIView();
#elif BOSS_ANDROID
    extern JNIEnv* GetAndroidJNIEnv();
    extern jobject GetAndroidApplicationActivity();
#endif

namespace BOSS
{
    namespace PlatformImpl
    {
        ////////////////////////////////////////////////////////////////////////////////
        // 일반함수와 전역인스턴스
        ////////////////////////////////////////////////////////////////////////////////
        namespace Core
        {
            class ProcedureLockClass
            {
            public:
                ProcedureLockClass() {mMutex = Mutex::Open();}
                ~ProcedureLockClass() {Mutex::Close(mMutex);}
            public:
                id_mutex mMutex;
            };
            ProcedureLockClass g_ProcedureLock;

            void LockProcedure()
            {
                Mutex::Lock(g_ProcedureLock.mMutex);
            }

            void UnlockProcedure()
            {
                Mutex::Unlock(g_ProcedureLock.mMutex);
            }

            class ProcedureClass
            {
            public:
                ProcedureClass() {mID = CreateID(); mCb = nullptr; mData = nullptr; mMsec = 1;}
                ~ProcedureClass() {}
                ProcedureClass(const ProcedureClass& rhs) {operator=(rhs);}
                ProcedureClass& operator=(const ProcedureClass& rhs)
                {mID = rhs.mID; mCb = rhs.mCb; mData = rhs.mData; mMsec = rhs.mMsec; return *this;}
            private:
                sint32 CreateID() {static sint32 _ = 0; return ++_;}
            public:
                sint32 mID;
                ProcedureEvent mEvent;
                ProcedureCB mCb;
                payload mData;
                sint32 mMsec;
            };
            Map<ProcedureClass> g_AllProcedures;
            Queue<ProcedureClass> g_NewProcedures;

            sint32 GetProcedureCount()
            {
                return g_AllProcedures.Count();
            }

            void FlushProcedure()
            {
                LockProcedure();
                {
                    // 추가
                    while(0 < g_NewProcedures.Count())
                    {
                        auto SrcProcedure = g_NewProcedures.Dequeue();
                        auto& DestProcedure = g_AllProcedures[SrcProcedure.mID];
                        DestProcedure.mID = SrcProcedure.mID;
                        DestProcedure.mCb = SrcProcedure.mCb;
                        DestProcedure.mData = SrcProcedure.mData;
                        DestProcedure.mMsec = SrcProcedure.mMsec;
                    }

                    // 삭제
                    for(sint32 i = g_AllProcedures.Count() - 1; 0 <= i; --i)
                    {
                        chararray GetID;
                        auto CurProcedure = g_AllProcedures.AccessByOrder(i, &GetID);
                        if(!CurProcedure->mCb)
                        {
                            const sint32 OldProcedureID = MapPath::ToInt(GetID);
                            g_AllProcedures.Remove(OldProcedureID);
                        }
                    }
                }
                UnlockProcedure();
            }

            ProcedureCB GetProcedureCB(sint32 i, uint64 oldmsec, uint64 newmsec)
            {
                auto CurProcedure = g_AllProcedures.AccessByOrder(i);
                if(oldmsec / CurProcedure->mMsec != newmsec / CurProcedure->mMsec)
                    return CurProcedure->mCb;
                return nullptr;
            }

            payload GetProcedureData(sint32 i)
            {
                auto CurProcedure = g_AllProcedures.AccessByOrder(i);
                return CurProcedure->mData;
            }

            String NormalPath(chars itemname, bool QCodeTest)
            {
                chars NormalName = boss_normalpath(itemname, nullptr);
                if(QCodeTest && NormalName[0] == 'Q' && NormalName[1] == ':')
                    NormalName += 2;
                return String(NormalName);
            }

            WString NormalPathW(wchars itemname, bool QCodeTest)
            {
                String ItemName = String::FromWChars(itemname);
                chars NormalName = boss_normalpath(ItemName, nullptr);
                if(QCodeTest && NormalName[0] == 'Q' && NormalName[1] == ':')
                    NormalName += 2;
                return WString::FromChars(NormalName);
            }

            class RootClass
            {
            public:
                RootClass() {mMutex = Mutex::Open();}
                ~RootClass() {Mutex::Close(mMutex);}
            public:
                id_mutex mMutex;
                String mPath[3];
            };
            RootClass g_root;

            void SetRoot(sint32 i, chars dirname)
            {
                Mutex::Lock(g_root.mMutex);
                g_root.mPath[i] = dirname;
                Mutex::Unlock(g_root.mMutex);
            }

            String GetCopiedRoot(sint32 i)
            {
                Mutex::Lock(g_root.mMutex);
                String Result;
                Result = (chars) g_root.mPath[i];
                Mutex::Unlock(g_root.mMutex);
                return Result;
            }

            View::CreatorCB g_Creator = View::Creator;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // 래핑함수
        ////////////////////////////////////////////////////////////////////////////////
        namespace Wrap
        {
            sint32 AddProcedure(ProcedureCB cb, payload data, sint32 msec)
            {
                if(auto NewProcedure = Core::g_NewProcedures.Create())
                {
                    NewProcedure->mCb = cb;
                    NewProcedure->mData = data;
                    NewProcedure->mMsec = msec;
                    return NewProcedure->mID;
                }
                return -1;
            }

            bool SubProcedure(sint32 id)
            {
                // mCb를 제거해 놓으면 다음 FlushProcedure호출시 제거
                if(auto CurProcedure = Core::g_AllProcedures.Access(id))
                {
                    CurProcedure->mCb = nullptr;
                    return true;
                }
                return false;
            }

            chars Utility_GetOSName()
            {
                #if BOSS_WINDOWS
                    return "WINDOWS";
                #elif BOSS_LINUX
                    return "UBUNTU";
                #elif BOSS_MAC_OSX
                    return "OSX";
                #elif BOSS_IPHONE
                    return "IOS";
                #elif BOSS_ANDROID
                    return "ANDROID";
                #else
                    return "UNKNOWN";
                #endif
            }

            chars Utility_GetDeviceID()
            {
                static String DeviceID;
                if(DeviceID.Length())
                    return DeviceID;

                // 하드웨어정보
                sint32 OSCode16 = 0x1F; // 32가지
                Map<String> StringCollector;
                #if BOSS_WINDOWS
                    OSCode16 &= 0x01;
                    wchar_t UserName[256 + 1];
                    DWORD UserNameLen = 256 + 1;
                    if(GetUserNameW(UserName, &UserNameLen))
                        StringCollector("UserName") = String::FromWChars(UserName);
                    DWORD VolSerial = 0;
                    if(GetVolumeInformationA("C:/", NULL, NULL, &VolSerial, NULL, NULL, NULL, NULL))
                        StringCollector("VolSerial") = String::Format("%X", VolSerial);
                #elif BOSS_LINUX
                    OSCode16 &= 0x02;
                    const int Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
                    if(Sock != -1)
                    {
                        struct ifreq Ifr;
                        struct ifconf Ifc;
                        char Buf[1024];
                        Ifc.ifc_len = sizeof(Buf);
                        Ifc.ifc_buf = Buf;
                        if(ioctl(Sock, SIOCGIFCONF, &Ifc) == -1) {}
                        struct ifreq* It = Ifc.ifc_req;
                        const struct ifreq* const End = It + (Ifc.ifc_len / sizeof(struct ifreq));
                        while(It != End)
                        {
                            strcpy(Ifr.ifr_name, It->ifr_name);
                            if(ioctl(Sock, SIOCGIFFLAGS, &Ifr) == 0)
                            if(!(Ifr.ifr_flags & IFF_LOOPBACK))
                            if(ioctl(Sock, SIOCGIFHWADDR, &Ifr) == 0)
                            {
                                String Ip6Text;
                                bytes Ip6Code = (bytes) Ifr.ifr_hwaddr.sa_data;
                                for(sint32 i = 0; i < 6; ++i)
                                {
                                    if(0 < i) Ip6Text += '.';
                                    Ip6Text += String::FromInteger(Ip6Code[i] & 0xFF);
                                }
                                StringCollector("Ip6") = Ip6Text;
                                break;
                            }
                            It++;
                        }
                    }
                #else
                    OSCode16 &= 0x00;
                    StringCollector.AtAdding() = "null";
                #endif

                // 핑거프린트화
                String FingerPrint;
                for(sint32 i = 0, iend = StringCollector.Count(); i < iend; ++i)
                {
                    if(0 < i) FingerPrint += '_';
                    FingerPrint += *StringCollector.AccessByOrder(i);
                }

                // CRC64처리
                uint64 CrcTable[256];
                for(sint32 i = 0; i < 256; ++i)
                {
                    uint64 crc = i;
                    for(sint32 j = 0; j < 8; ++j)
                    {
                        if(crc & 1)
                            crc = (crc >> 1) ^ 0xC96C5795D7870F42;
                        else crc >>= 1;
                    }
                    CrcTable[i] = crc;
                }
                uint64 CrcCode = 0;
                chars CrcFocus = FingerPrint;
                while(*CrcFocus)
                    CrcCode = CrcTable[(CrcCode ^ *(CrcFocus++)) & 0xFF] ^ (CrcCode >> 8);

                // 6자리 장치식별코드
                sint32 DeviceCode[10];
                DeviceCode[0] = ((OSCode16 & 0x1F) << 1) | (CrcCode & 0x1);
                for(sint32 i = 0; i < 9; ++i)
                    DeviceCode[i + 1] = (CrcCode >> (1 + i * 7)) & 0x3F;

                // 장치식별ID화
                for(sint32 i = 0; i < 10; ++i)
                {
                    const sint32 CurCode = DeviceCode[i];
                    if(CurCode < 26)
                        DeviceID += 'A' + CurCode;
                    else if(CurCode < 26 + 26)
                        DeviceID += 'a' + (CurCode - 26);
                    else if(CurCode < 26 + 26 + 10)
                        DeviceID += '0' + (CurCode - 26 - 26);
                    // 어차피 CRC64과정에서 손실된 정보라 미관상 조금 더 손실시킴
                    else if(CurCode == 10 + 26 + 26)
                        DeviceID += 'x';
                    else DeviceID += 'X';
                }
                return DeviceID;
            }

            sint64 Utility_CurrentAvailableMemory(sint64* totalbytes)
            {
                #if BOSS_WINDOWS
                    MEMORYSTATUSEX MemoryStatus;
                    Memory::Set(&MemoryStatus, 0, sizeof(MemoryStatus));
                    MemoryStatus.dwLength = sizeof(MemoryStatus);
                    if(GlobalMemoryStatusEx(&MemoryStatus))
                    {
                        if(totalbytes) *totalbytes = (sint64) MemoryStatus.ullTotalPhys;
                        return (sint64) MemoryStatus.ullAvailPhys;
                    }
                #elif BOSS_ANDROID
                    struct sysinfo MemoryStatus;
                    if(!sysinfo(&MemoryStatus))
                    {
                        if(totalbytes) *totalbytes = (sint64) MemoryStatus.totalram;
                        return (sint64) MemoryStatus.freeram;
                    }
                #endif
                return -1;
            }

            double Utility_CurrentTrafficCPU()
            {
                #if BOSS_WINDOWS && !BOSS_WINDOWS_MINGW
                    static struct CPUStateClass
                    {
                        CPUStateClass()
                        {
                            PdhOpenQuery(NULL, NULL, &mCpuQuery);
                            PdhAddCounterW(mCpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &mCpuTotal);
                            PdhCollectQueryData(mCpuQuery);
                        }
                        ~CPUStateClass() {}
                        PDH_HQUERY mCpuQuery;
                        PDH_HCOUNTER mCpuTotal;
                    } CPUState;

                    PDH_FMT_COUNTERVALUE CounterVal;
                    PdhCollectQueryData(CPUState.mCpuQuery);
                    PdhGetFormattedCounterValue(CPUState.mCpuTotal, PDH_FMT_DOUBLE, NULL, &CounterVal);
                    return CounterVal.doubleValue * 0.01;
                #endif
                return 0;
            }

            class StaticalMutexClass
            {
            public:
                StaticalMutexClass()
                {
                    // static 인스턴스끼리의 호출순서로 생성자보다 Lock()이 먼저 호출될 수 있기에
                    // 본 생성자에선 mMutex를 초기화하지 않는다. (static 인스턴스는 자동으로 nullptr초기화됨)
                }
                ~StaticalMutexClass()
                {
                    if(mMutex)
                    {
                        Mutex::Close(mMutex);
                        mMutex = nullptr;
                    }
                }
            public:
                inline void Lock()
                {
                    if(!mMutex) // static 인스턴스끼리의 호출로 Lock()이 생성자보다 먼저 호출될 수 있음.
                        mMutex = Mutex::Open();
                    Mutex::Lock(mMutex);
                }
                inline void Unlock()
                {
                    if(mMutex)
                        Mutex::Unlock(mMutex);
                }
            private:
                id_mutex mMutex;
            };

            static Map<bool> g_OptionFlagMap;
            static StaticalMutexClass g_OptionFlagMutex;
            void Option_SetOptionFlag(chars name, bool flag)
            {
                g_OptionFlagMutex.Lock();
                g_OptionFlagMap(name) = flag;
                g_OptionFlagMutex.Unlock();
            }

            bool Option_GetOptionFlag(chars name)
            {
                g_OptionFlagMutex.Lock();
                bool* Result = g_OptionFlagMap.Access(name);
                g_OptionFlagMutex.Unlock();
                return (Result)? *Result : false;
            }

            Strings Option_GetOptionFlagNames()
            {
                Strings Result;
                payload Param = (payload) &Result;
                g_OptionFlagMutex.Lock();
                g_OptionFlagMap.AccessByCallback(
                    [](const MapPath* path, bool* data, payload param)->void
                    {
                        Strings& Result = *((Strings*) param);
                        Result.AtAdding() = &path->GetPath()[0];
                    }, Param);
                g_OptionFlagMutex.Unlock();
                return Result;
            }

            static Map<String> g_OptionTextMap;
            static StaticalMutexClass g_OptionTextMutex;
            void Option_SetOptionText(chars name, chars text)
            {
                g_OptionTextMutex.Lock();
                g_OptionTextMap(name) = text;
                g_OptionTextMutex.Unlock();
            }

            chars Option_GetOptionText(chars name)
            {
                g_OptionTextMutex.Lock();
                String* Result = g_OptionTextMap.Access(name);
                g_OptionTextMutex.Unlock();
                return (Result)? *Result : "";
            }

            Strings Option_GetOptionTextNames()
            {
                Strings Result;
                payload Param = (payload) &Result;
                g_OptionTextMutex.Lock();
                g_OptionTextMap.AccessByCallback(
                    [](const MapPath* path, String* data, payload param)->void
                    {
                        Strings& Result = *((Strings*) param);
                        Result.AtAdding() = &path->GetPath()[0];
                    }, Param);
                g_OptionTextMutex.Unlock();
                return Result;
            }

            static Map<payload> g_OptionPayloadMap;
            static StaticalMutexClass g_OptionPayloadMutex;
            void Option_SetOptionPayload(chars name, payload data)
            {
                g_OptionPayloadMutex.Lock();
                g_OptionPayloadMap(name) = data;
                g_OptionPayloadMutex.Unlock();
            }

            payload Option_GetOptionPayload(chars name)
            {
                g_OptionPayloadMutex.Lock();
                payload* Result = g_OptionPayloadMap.Access(name);
                g_OptionPayloadMutex.Unlock();
                return (Result)? *Result : nullptr;
            }

            Strings Option_GetOptionPayloadNames()
            {
                Strings Result;
                payload Param = (payload) &Result;
                g_OptionPayloadMutex.Lock();
                g_OptionPayloadMap.AccessByCallback(
                    [](const MapPath* path, payload* data, payload param)->void
                    {
                        Strings& Result = *((Strings*) param);
                        Result.AtAdding() = &path->GetPath()[0];
                    }, Param);
                g_OptionPayloadMutex.Unlock();
                return Result;
            }

            #if BOSS_WINDOWS
                static int __stdcall _lpfn(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
                {
                    switch(uMsg)
                    {
                    case BFFM_INITIALIZED:
                        if(lpData) SendMessageW(hwnd, BFFM_SETSELECTION, TRUE, lpData);
                        break;
                    }
                    return 0;
                }
            #endif

            bool Popup_FileDialog(DialogShellType type, String& path, String* shortpath, chars title, wchars filters, sint32* filterresult)
            {
                bool Result = false;
                #if BOSS_WINDOWS
                    CoInitialize(NULL);
                    wchar_t ResultPath[_MAX_PATH] = {L'\0'};
                    WString TitleName = WString::FromChars(title);
                    WString InitDir = Platform::File::GetDirName(WString::FromChars(path), L'/', L'\\');

                    if(type == DST_Dir)
                    {
                        BROWSEINFOW bi = {0};
                        bi.lpszTitle = (wchars) TitleName;
                        bi.lParam = (LPARAM)(wchars) InitDir;
                        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
                        bi.lpfn = _lpfn;

                        LPITEMIDLIST pidlFolder = SHBrowseForFolderW(&bi);
                        if(pidlFolder && SHGetPathFromIDListW(pidlFolder, ResultPath))
                        {
                            CoTaskMemFree(pidlFolder);
                            Result = true;
                        }
                    }
                    else
                    {
                        OPENFILENAMEW ofn = {0};
                        ofn.lStructSize = sizeof(OPENFILENAMEW);
                        ofn.lpstrTitle = (wchars) TitleName;
                        ofn.lpstrInitialDir = (wchars) InitDir;
                        ofn.lpstrFilter = filters;
                        ofn.lpstrFile = ResultPath;
                        ofn.nMaxFile = _MAX_PATH - 4;

                        if(type == DST_FileSave)
                        {
                            if(GetSaveFileNameW(&ofn))
                            {
                                if(filterresult)
                                    *filterresult = sint32(ofn.nFilterIndex) - 1;
                                Result = true;
                            }
                        }
                        else if(GetOpenFileNameW(&ofn))
                        {
                            if(filterresult)
                                *filterresult = sint32(ofn.nFilterIndex) - 1;
                            Result = true;
                        }
                    }

                    if(Result)
                    {
                        path = String::FromWChars(ResultPath);
                        if(shortpath) *shortpath = String::FromWChars(Platform::File::GetShortName(ResultPath));
                    }
                    CoUninitialize();
                #elif BOSS_LINUX & !defined(BOSS_SILENT_NIGHT_IS_ENABLED)
                    auto NewDialog = gtk_file_chooser_dialog_new(title, nullptr,
                        (type == DST_Dir)? GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER :
                        ((type == DST_FileSave)? GTK_FILE_CHOOSER_ACTION_SAVE : GTK_FILE_CHOOSER_ACTION_OPEN),
                        "_Cancel", GTK_RESPONSE_CANCEL, "_OK", GTK_RESPONSE_ACCEPT, nullptr);
                    if(gtk_dialog_run(GTK_DIALOG(NewDialog)) == GTK_RESPONSE_ACCEPT)
                        Result = true;

                    if(Result)
                    {
                        auto CurChooser = GTK_FILE_CHOOSER(NewDialog);
                        auto NewFilename = gtk_file_chooser_get_filename(CurChooser);
                        if(NewFilename[0] == '/') path = String("Q:") + NewFilename;
                        else path = NewFilename;
                        if(shortpath) *shortpath = String::FromWChars(
                            Platform::File::GetShortName(WString::FromChars(NewFilename)));
                        g_free(NewFilename);
                    }
                    gtk_widget_destroy(NewDialog);
                #endif
                return Result;
            }

            sint32 Popup_MessageDialog(chars title, chars text, DialogButtonType type)
            {
                #if BOSS_WINDOWS & !defined(BOSS_SILENT_NIGHT_IS_ENABLED)
                    UINT DialogType = MB_ICONINFORMATION | MB_OK;
                    switch(type)
                    {
                    case DBT_YesNo:
                        DialogType = MB_ICONQUESTION | MB_YESNO;
                        break;
                    case DBT_OKCancel:
                        DialogType = MB_ICONINFORMATION | MB_OKCANCEL;
                        break;
                    case DBT_OkCancelIgnore:
                        DialogType = MB_ICONWARNING | MB_ABORTRETRYIGNORE;
                        break;
                    }

                    WString Caption = WString::FromChars(title);
                    WString Text = WString::FromChars(text);
                    int Result = MessageBoxW(NULL, Text, Caption, DialogType);
                    switch(type)
                    {
                    case DBT_YesNo:
                        switch(Result)
                        {
                        case IDYES:
                            return 0;
                        case IDNO:
                            return 1;
                        }
                        break;
                    case DBT_Ok:
                    case DBT_OKCancel:
                        switch(Result)
                        {
                        case IDOK:
                            return 0;
                        case IDCANCEL:
                            return 1;
                        }
                        break;
                    case DBT_OkCancelIgnore:
                        switch(Result)
                        {
                        case IDOK:
                            return 0;
                        case IDCANCEL:
                            return 1;
                        case IDIGNORE:
                            return 2;
                        }
                        break;
                    }
                #elif BOSS_LINUX & !defined(BOSS_SILENT_NIGHT_IS_ENABLED)
                    GtkMessageType MessageType = GTK_MESSAGE_INFO; // DBT_Ok
                    GtkButtonsType ButtonsType = GTK_BUTTONS_OK;
                    switch(type)
                    {
                    case DBT_YesNo:
                        MessageType = GTK_MESSAGE_QUESTION;
                        ButtonsType = GTK_BUTTONS_YES_NO;
                        break;
                    case DBT_OKCancel:
                        MessageType = GTK_MESSAGE_INFO;
                        ButtonsType = GTK_BUTTONS_OK_CANCEL;
                        break;
                    case DBT_OkCancelIgnore:
                        MessageType = GTK_MESSAGE_WARNING;
                        ButtonsType = GTK_BUTTONS_OK_CANCEL;
                        break;
                    }

                    auto NewDialog = gtk_message_dialog_new(nullptr,
                        GTK_DIALOG_DESTROY_WITH_PARENT, MessageType, ButtonsType, text);
                    gtk_window_set_title(GTK_WINDOW(NewDialog), title);
                    gint Result = gtk_dialog_run(GTK_DIALOG(NewDialog));
                    gtk_widget_destroy(GTK_WIDGET(NewDialog));
                    switch(type)
                    {
                    case DBT_YesNo:
                        switch(Result)
                        {
                        case GTK_RESPONSE_YES:
                            return 0;
                        case GTK_RESPONSE_NO:
                            return 1;
                        }
                        break;
                    case DBT_Ok:
                    case DBT_OKCancel:
                        switch(Result)
                        {
                        case GTK_RESPONSE_OK:
                            return 0;
                        case GTK_RESPONSE_CANCEL:
                            return 1;
                        }
                        break;
                    case DBT_OkCancelIgnore:
                        switch(Result)
                        {
                        case GTK_RESPONSE_OK:
                            return 0;
                        case GTK_RESPONSE_CANCEL:
                            return 1;
                        case GTK_RESPONSE_DELETE_EVENT:
                            return 2;
                        }
                        break;
                    }
                #endif
                return -1;
            }

            void Popup_WebBrowserDialog(String url)
            {
                #if BOSS_WINDOWS
                    #ifdef UNICODE
                        ShellExecute(NULL, L"open", (wchars) WString::FromChars(url),
                            NULL, NULL, SW_SHOWNORMAL);
                    #else
                        ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
                    #endif
                #endif
            }

            void Popup_ProgramDialog(chars exepath, chars args, bool admin, bool hide, chars dirpath, ublock* getpid)
            {
                #if BOSS_WINDOWS
                    WString ExePathW = WString::FromChars(exepath);
                    WString ArgsW = WString::FromChars(args);
                    WString DirPathW = WString::FromChars(dirpath);

                    SHELLEXECUTEINFOW ExecuteInfo;
                    Memory::Set(&ExecuteInfo, 0, sizeof(ExecuteInfo));
                    ExecuteInfo.cbSize = sizeof(ExecuteInfo);
                    ExecuteInfo.fMask = (getpid)? SEE_MASK_NOCLOSEPROCESS : 0;
                    ExecuteInfo.hwnd = NULL;
                    ExecuteInfo.lpVerb = (!admin)? NULL : L"runas";
                    ExecuteInfo.lpFile = (wchars) ExePathW;
                    ExecuteInfo.lpParameters = (wchars) ArgsW;
                    ExecuteInfo.lpDirectory = (dirpath)? (wchars) DirPathW : NULL;
                    ExecuteInfo.nShow = (hide)? SW_HIDE : SW_SHOWNORMAL;
                    ShellExecuteExW(&ExecuteInfo);

                    if(getpid)
                        *getpid = (ublock) ExecuteInfo.hProcess;
                #endif
            }

            void Kill_ProgramDialog(ublock pid)
            {
                #if BOSS_WINDOWS
                    TerminateProcess((HANDLE) pid, 1);
                #endif
            }

            WString File_GetDirName(wchars itemname, wchar_t badslash, wchar_t goodslash)
            {
                if(itemname && itemname[0] != L'\0')
                {
                    String ItemnameUTF8 = String::FromWChars(itemname);
                    ItemnameUTF8.Replace(badslash, goodslash);
                    if(ItemnameUTF8[-2] == goodslash)
                        ItemnameUTF8.SubTail(1);

                    if(Platform::File::ExistForDir(ItemnameUTF8))
                        return WString::FromChars(ItemnameUTF8);

                    for(sint32 i = ItemnameUTF8.Length() - 1; 0 <= i; --i)
                        if(ItemnameUTF8[i] == goodslash)
                            return WString::FromChars(ItemnameUTF8, i);
                }
                return L"";
            }

            WString File_GetShortName(wchars itemname)
            {
                if(itemname && itemname[0] != L'\0')
                {
                    String ItemnameUTF8 = String::FromWChars(itemname);
                    if(ItemnameUTF8[-2] == L'/' || ItemnameUTF8[-2] == L'\\')
                        ItemnameUTF8.SubTail(1);

                    for(sint32 i = ItemnameUTF8.Length() - 1; 0 <= i; --i)
                        if(ItemnameUTF8[i] == L'/' || ItemnameUTF8[i] == L'\\')
                            return WString::FromChars(&((chars) ItemnameUTF8)[i + 1]);
                }
                return L"";
            }

            sint32 File_GetDriveCode()
            {
                static const sint32 DriveCode = Platform::File::GetFullPath((wchars) L"")[0] - L'A' + 1;
                return DriveCode;
            }

            bool File_Tempname(char* format, sint32 length)
            {
                static const char TempChars[36] = {
                    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                    'u', 'v', 'w', 'x', 'y', 'z'};

                String NewName;
                do
                {
                    NewName.Empty();
                    for(sint32 i = 0; i < length; ++i)
                    {
                        if(format[i] == 'X')
                            NewName += TempChars[Platform::Utility::Random() % 36];
                        else NewName += format[i];
                    }
                }
                while(Platform::File::CanAccess(WString::FromChars(NewName)));

                Memory::Copy(format, (chars) NewName, length);
                return true;
            }

            class WebNativePrivate
            {
            public:
                WebNativePrivate(sint32 id = -1) {mID = id;}
                ~WebNativePrivate()
                {
                    if(mID != -1)
                    {
                        #if BOSS_IPHONE
                            BossWebView::Release(mID);
                        #elif BOSS_ANDROID
                            JNIEnv* env = GetAndroidJNIEnv();
                            jclass BossWebViewClass = env->FindClass("com/boss2d/BossWebView");
                            jmethodID ReleaseMethodID = env->GetStaticMethodID(BossWebViewClass, "Release", "(I)V");
                            env->CallStaticVoidMethod(BossWebViewClass, ReleaseMethodID, mID);
                            env->DeleteLocalRef(BossWebViewClass);
                        #endif
                    }
                }
            public:
                void Reload(chars url)
                {
                }
            private:
                sint32 mID;
            };

            #if BOSS_ANDROID
                class WebPayloadForAndroid
                {
                public:
                    WebPayloadForAndroid() {mCB = nullptr; mData = nullptr;}
                    ~WebPayloadForAndroid() {}
                public:
                    static WebPayloadForAndroid& ST(sint32 id)
                    {static Map<WebPayloadForAndroid> _; return _[id];}
                public:
                    Platform::Web::EventCB mCB;
                    payload mData;
                public:
                    static void OnEvent(JNIEnv* env, jobject obj, jint id, jstring type, jstring text)
                    {
                        chars PrmType = (chars) ((JNIEnv*) env)->GetStringUTFChars(type, nullptr);
                        chars PrmText = (chars) ((JNIEnv*) env)->GetStringUTFChars(text, nullptr);
                        auto& CurPayload = ST(id);
                        CurPayload.mCB(CurPayload.mData, PrmType, PrmText);
                        ((JNIEnv*) env)->ReleaseStringUTFChars(type, PrmType);
                        ((JNIEnv*) env)->ReleaseStringUTFChars(text, PrmText);
                    }
                };
            #endif

            h_web_native Web_CreateNative(chars url, bool clearcookies, Platform::Web::EventCB cb, payload data)
            {
                sint32 ID = -1;
                #if BOSS_WINDOWS
                    Platform::Popup::WebBrowserDialog(url);
                #elif BOSS_IPHONE
                    ID = BossWebView::Create(GetIOSApplicationUIView(), url, clearcookies, cb, data);
                #elif BOSS_ANDROID
                    JNIEnv* env = GetAndroidJNIEnv();
                    jclass BossWebViewClass = env->FindClass("com/boss2d/BossWebView");
                    static bool JustOnce = true;
                    if(JustOnce)
                    {
                        JustOnce = false;
                        JNINativeMethod methods[] {
                            {"OnEvent", "(ILjava/lang/String;Ljava/lang/String;)V", reinterpret_cast<void*>(WebPayloadForAndroid::OnEvent)}};
                        env->RegisterNatives(BossWebViewClass, methods, sizeof(methods) / sizeof(methods[0]));
                    }

                    jmethodID CreateMethodID = env->GetStaticMethodID(BossWebViewClass,
                        "Create", "(Landroid/app/Activity;Ljava/lang/String;Z)I");
                    jobject activity = GetAndroidApplicationActivity();
                    jstring string = env->NewStringUTF(url);
                    ID = env->CallStaticIntMethod(BossWebViewClass, CreateMethodID, activity, string, (jboolean) clearcookies);
                    env->DeleteLocalRef(BossWebViewClass);

                    auto& NewPayload = WebPayloadForAndroid::ST(ID);
                    NewPayload.mCB = cb;
                    NewPayload.mData = data;
                #endif

                auto NewWeb = (WebNativePrivate*) Buffer::AllocNoConstructorOnce<WebNativePrivate>(BOSS_DBG 1);
                BOSS_CONSTRUCTOR(NewWeb, 0, WebNativePrivate, ID);
                auto NewWebHandle = h_web_native::create_by_buf(BOSS_DBG (buffer) NewWeb);
                return NewWebHandle;
            }

            void Web_ReleaseNative(h_web_native web_native)
            {
                web_native.set_buf(nullptr);
            }

            void Web_ReloadNative(h_web_native web_native, chars url)
            {
                if(auto CurWeb = (WebNativePrivate*) web_native.get())
                    CurWeb->Reload(url);
            }
        }
    }
}
