#include <boss.h>
#if BOSS_MAC_OSX | BOSS_IPHONE
    #include <CoreFoundation/CoreFoundation.h>
    #include <syslog.h>
#endif

#include <boss.hpp>
#include "boss_platform_native.hpp"

#ifdef BOSS_PLATFORM_NATIVE

    #if BOSS_WINDOWS
        #include <windows.h>
    #elif _POSIX_C_SOURCE >= 199309L
        #include <time.h>
    #else
        #include <unistd.h>
    #endif
    #if !BOSS_WINDOWS
        #include <sys/stat.h>
        #include <dirent.h>
    #endif
    #include <stdio.h>
    #include <stdarg.h>
    #if BOSS_ANDROID
        #include <android/asset_manager_jni.h>
    #endif

    dependency* g_data = nullptr;
    dependency* g_window = nullptr;
    ViewAPI* g_view = nullptr;
    static bool g_isPopupAssert = false;

    #if BOSS_ANDROID
        static JavaVM* g_jvm = nullptr;
        jint JNI_OnLoad(JavaVM* vm, void*)
        {
            g_jvm = vm;
            BOSS_TRACE("g_jvm=0x%08X", g_jvm);
            return JNI_VERSION_1_6;
        }

        JNIEnv* GetAndroidJNIEnv()
        {
            JNIEnv* result = nullptr;
            if(g_jvm) g_jvm->AttachCurrentThread(&result, 0);
            return result;
        }
        static jobject g_activity = nullptr;
        jobject GetAndroidApplicationActivity()
        {
            BOSS_ASSERT("안드로이드OS의 경우에 한하여 Java소스에서 SetAndroidApplicationActivity의 선행호출이 필요합니다", g_activity);
            JNIEnv* env = GetAndroidJNIEnv();
            return env->NewGlobalRef(g_activity);
        }
        void SetAndroidApplicationActivity(jobject activity)
        {
            BOSS_ASSERT("g_jvm가 nullptr입니다", g_jvm);
            JNIEnv* env = GetAndroidJNIEnv();
            g_activity = env->NewGlobalRef(activity);
        }
        static jobject g_context = nullptr;
        jobject GetAndroidApplicationContext()
        {
            BOSS_ASSERT("안드로이드OS의 경우에 한하여 Java소스에서 SetAndroidApplicationContext의 선행호출이 필요합니다", g_context);
            JNIEnv* env = GetAndroidJNIEnv();
            return env->NewGlobalRef(g_context);
        }
        void SetAndroidApplicationContext(jobject context)
        {
            BOSS_ASSERT("g_jvm가 nullptr입니다", g_jvm);
            JNIEnv* env = GetAndroidJNIEnv();
            g_context = env->NewGlobalRef(context);
        }
    #elif BOSS_IPHONE
        void* GetIOSApplicationUIView()
        {
            return nullptr;
        }
    #endif

    #if BOSS_NEED_MAIN
        extern bool PlatformInit();
        extern void PlatformQuit();
        extern void PlatformFree();

        int main(int argc, char* argv[])
        {
            BOSS_ASSERT("This application will operate for library only.", false);
            return 0;
        }
    #endif

    #if BOSS_ANDROID
        extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt, ...);
    #endif

    extern "C" int boss_platform_assert(BOSS_DBG_PRM const char* name, const char* query)
    {
        String AssertInfo[4] = {
            String::Format("[QUERY] %s", query),
            String::Format("[METHOD] %s()", BOSS_DBG_FUNC),
            String::Format("[FILE/LINE] %s(%dLn)", BOSS_DBG_FILE, BOSS_DBG_LINE),
            String::Format("[THREAD_ID] -unknown-")};
        #if BOSS_ANDROID
            __android_log_print(7, "*******", "************************************************************");
            __android_log_print(7, "*******", name);
            __android_log_print(7, "*******", "------------------------------------------------------------");
            __android_log_print(7, "*******", (chars) AssertInfo[0]);
            __android_log_print(7, "*******", (chars) AssertInfo[1]);
            __android_log_print(7, "*******", (chars) AssertInfo[2]);
            __android_log_print(7, "*******", (chars) AssertInfo[3]);
            __android_log_print(7, "*******", "************************************************************");
        #elif BOSS_WINDOWS
            OutputDebugStringW(L"************************************************************\n");
            OutputDebugStringW((wchars) WString::FromChars(name));
            OutputDebugStringW(L"\n------------------------------------------------------------\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[0])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[1])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[2])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[3])); OutputDebugStringW(L"\n");
            OutputDebugStringW(L"************************************************************\n");
        #elif BOSS_IPHONE
            syslog(LOG_INFO, "************************************************************\n");
            syslog(LOG_INFO, name);
            syslog(LOG_INFO, "\n------------------------------------------------------------\n");
            syslog(LOG_INFO, (chars) AssertInfo[0]); syslog(LOG_INFO, "\n");
            syslog(LOG_INFO, (chars) AssertInfo[1]); syslog(LOG_INFO, "\n");
            syslog(LOG_INFO, (chars) AssertInfo[2]); syslog(LOG_INFO, "\n");
            syslog(LOG_INFO, (chars) AssertInfo[3]); syslog(LOG_INFO, "\n");
            syslog(LOG_INFO, "************************************************************\n");
        #endif

        if(g_isPopupAssert)
        {
            #if BOSS_WINDOWS
                WString AssertMessage = WString::Format(
                    L"%s\n\n%s\t\t\n%s\t\t\n%s\t\t\n%s\t\t\n\n"
                    L"(YES is Break, NO is Ignore)\t\t",
                    (wchars) WString::FromChars(name),
                    (wchars) WString::FromChars(AssertInfo[0]),
                    (wchars) WString::FromChars(AssertInfo[1]),
                    (wchars) WString::FromChars(AssertInfo[2]),
                    (wchars) WString::FromChars(AssertInfo[3]));
                switch(MessageBoxW(NULL, AssertMessage, L"ASSERT BREAK", MB_ICONWARNING | MB_ABORTRETRYIGNORE))
                {
                case IDABORT: return 0;
                case IDIGNORE: return 1;
                }
            #elif BOSS_IPHONE
                /*SInt32 nRes = 0;
                const void* keys[] = {
                    kCFUserNotificationAlertHeaderKey,
                    kCFUserNotificationAlertMessageKey};
                const void* vals[] = {
                    CFSTR("Test Foundation Message Box"),
                    CFSTR("Hello, World!")};
                CFDictionaryRef dict = CFDictionaryCreate(0, keys, vals,
                    sizeof(keys) / sizeof(*keys),
                    &kCFTypeDictionaryKeyCallBacks,
                    &kCFTypeDictionaryValueCallBacks);
                CFUserNotificationCreate(kCFAllocatorDefault, 0,
                    kCFUserNotificationPlainAlertLevel, &nRes, dict);*/
            #endif
        }
        return 2;
    }

    extern "C" void boss_platform_trace(const char* text, ...)
    {
        char TraceMessage[10240];
        va_list Args;
        va_start(Args, text);
        boss_vsnprintf(TraceMessage, 10240, text, Args);
        va_end(Args);

        #if BOSS_ANDROID
            __android_log_print(7, "#######", TraceMessage);
        #elif BOSS_WINDOWS
            OutputDebugStringW((wchars) WString::FromChars(TraceMessage));
            OutputDebugStringW(L"\n");
        #elif BOSS_IPHONE
            syslog(LOG_INFO, TraceMessage);
        #endif
    }

    namespace BOSS
    {
        ////////////////////////////////////////////////////////////////////////////////
        // PLATFORM
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::InitForGL(bool frameless, bool topmost)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::InitForMDI(bool frameless, bool topmost)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SetViewCreator(View::CreatorCB creator)
        {
            PlatformImpl::Core::g_Creator = creator;
        }

        void Platform::SetWindowName(chars name)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        h_view Platform::SetWindowView(chars viewclass)
        {
            if(g_view)
            {
                Buffer::Free((buffer) g_view);
                g_view = nullptr;
            }

            auto NewViewManager = PlatformImpl::Core::g_Creator(viewclass);
            buffer NewViewAPI = Buffer::AllocNoConstructorOnce<ViewAPI>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewViewAPI, 0, ViewAPI, NewViewManager);

            g_view = (ViewAPI*) NewViewAPI;
            h_view NewViewHandle = h_view::create_by_ptr(BOSS_DBG g_view);
            NewViewManager->SetView(NewViewHandle);
            g_view->sendCreate();
            return NewViewHandle;
        }

        void Platform::SetWindowPos(sint32 x, sint32 y)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SetWindowSize(sint32 width, sint32 height)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        rect128 Platform::GetWindowRect(bool normally)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return {0, 0, 0, 0};
        }

        void Platform::SetWindowVisible(bool visible)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SetWindowFlash()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::SetWindowMask(id_image_read image)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::SetWindowOpacity(float value)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        sint32 Platform::AddProcedure(ProcedureEvent event, ProcedureCB cb, payload data)
        {
            return PlatformImpl::Wrap::AddProcedure(cb, data, 1000);
        }

        void Platform::SubProcedure(sint32 id)
        {
            PlatformImpl::Wrap::SubProcedure(id);
        }

        void Platform::SetStatusText(chars text, UIStack stack)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        h_icon Platform::CreateIcon(chars filepath)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return h_icon::null();
        }

        h_action Platform::CreateAction(chars name, chars tip, h_view view, h_icon icon)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return h_action::null();
        }

        h_policy Platform::CreatePolicy(sint32 minwidth, sint32 minheight, sint32 maxwidth, sint32 maxheight)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return h_policy::null();
        }

        h_view Platform::CreateView(chars name, sint32 width, sint32 height, h_policy policy, chars viewclass, bool gl)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return h_view::null();
        }

        void* Platform::SetNextViewClass(h_view view, chars viewclass)
        {
            auto NewViewManager = PlatformImpl::Core::g_Creator(viewclass);
            ((ViewAPI*) view.get())->setNextViewManager(NewViewManager);
            return NewViewManager->GetClass();
        }

        bool Platform::SetNextViewManager(h_view view, View* viewmanager)
        {
            ((ViewAPI*) view.get())->setNextViewManager(viewmanager);
            return true;
        }

        h_dock Platform::CreateDock(h_view view, UIDirection direction, UIDirection directionbase)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return h_dock::null();
        }

        void Platform::AddAction(chars group, h_action action, UIRole role)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::AddSeparator(chars group, UIRole role)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::AddToggle(chars group, h_dock dock, UIRole role)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        h_window Platform::OpenChildWindow(h_view view, h_icon icon)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return h_window::null();
        }

        h_window Platform::OpenPopupWindow(h_view view, h_icon icon)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return h_window::null();
        }

        h_window Platform::OpenTrayWindow(h_view view, h_icon icon)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return h_window::null();
        }

        void Platform::CloseWindow(h_window window)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        id_cloned_share Platform::SendNotify(h_view view, chars topic, id_share in, bool needout)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::BroadcastNotify(chars topic, id_share in, NotifyType type, chars viewclass)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::PassAllViews(PassCB cb, payload data)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::UpdateAllViews()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // POPUP
        ////////////////////////////////////////////////////////////////////////////////
        bool Platform::Popup::TextDialog(String& text, chars title, chars topic, bool ispassword)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Popup::ColorDialog(uint08& r, uint08& g, uint08& b, uint08& a, chars title)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Popup::FileDialog(DialogShellType type, String& path, String* shortpath, chars title, wchars filters, sint32* filterresult)
        {
            return PlatformImpl::Wrap::Popup_FileDialog(type, path, shortpath, title, filters, filterresult);
        }

        void Platform::Popup::FileContentDialog(wchars filters)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        sint32 Platform::Popup::MessageDialog(chars title, chars text, DialogButtonType type)
        {
            return PlatformImpl::Wrap::Popup_MessageDialog(title, text, type);
        }

        void Platform::Popup::WebBrowserDialog(chars url)
        {
            return PlatformImpl::Wrap::Popup_WebBrowserDialog(url);
        }

        void Platform::Popup::OpenProgramDialog(chars exepath, chars args, bool admin, chars dirpath, ublock* getpid)
        {
            return PlatformImpl::Wrap::Popup_ProgramDialog(exepath, args, admin, dirpath, getpid);
        }

        void Platform::Popup::CloseProgramDialog(ublock pid)
        {
            PlatformImpl::Wrap::Kill_ProgramDialog(pid);
        }

        bool Platform::Popup::OpenEditTracker(String& text, UIEditType type, sint32 l, sint32 t, sint32 r, sint32 b, bool* enter)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);
            if(r <= l || b <= t) return false;

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Popup::OpenListTracker(Strings textes, sint32 l, sint32 t, sint32 r, sint32 b)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);
            if(r <= l || b <= t) return -1;

            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        void Platform::Popup::CloseAllTracker()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);

            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Popup::HasOpenedTracker()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Popup::ShowToolTip(String text)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Popup::HideToolTip()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Popup::ShowSplash(chars filepath)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Popup::HideSplash()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // UTILITY
        ////////////////////////////////////////////////////////////////////////////////
        static bool NeedSetRandom = true;
        uint32 Platform::Utility::Random()
        {
            if(NeedSetRandom)
            {
                NeedSetRandom = false;
                BOSS_ASSERT("Further development is needed.", false);
            }

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Utility::Sleep(sint32 ms, bool process_input, bool process_socket)
        {
            #if BOSS_WINDOWS
                ::Sleep(ms);
            #elif _POSIX_C_SOURCE >= 199309L
                struct timespec ts;
                ts.tv_sec = ms / 1000;
                ts.tv_nsec = (ms % 1000) * 1000000;
                nanosleep(&ts, NULL);
            #else
                usleep(ms * 1000);
            #endif
        }

        void Platform::Utility::SetMinimize()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::SetFullScreen(bool available_only)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Utility::IsFullScreen()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Utility::SetNormalWindow()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::ExitProgram()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        String Platform::Utility::GetProgramPath(bool dironly)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return String();
        }

        chars Platform::Utility::GetArgument(sint32 i, sint32* getcount)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::Utility::TestUrlSchema(chars schema, chars comparepath)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Utility::BindUrlSchema(chars schema, chars exepath, bool forcewrite)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        chars Platform::Utility::GetArgumentForUrlSchema(chars schema)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        sint32 Platform::Utility::GetScreenID(const point64& pos)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint32 Platform::Utility::GetScreenCount()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0
        }

        void Platform::Utility::GetScreenRect(rect128& rect, sint32 screenid, bool available_only)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Utility::IsScreenConnected()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return true;
        }

        id_image_read Platform::Utility::GetScreenshotImage(const rect128& rect)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap Platform::Utility::ImageToBitmap(id_image_read image, orientationtype ori)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Utility::SendToClipboard(chars text)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::SetCursor(CursorRole role)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::GetCursorPos(point64& pos)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Utility::GetCursorPosInWindow(point64& pos)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        float Platform::Utility::GetPixelRatio()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 1;
        }

        sint32 Platform::Utility::GetPhysicalDpi()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 70;
        }

        chars Platform::Utility::GetOSName()
        {
            return PlatformImpl::Wrap::Utility_GetOSName();
        }

        chars Platform::Utility::GetDeviceID()
        {
            return PlatformImpl::Wrap::Utility_GetDeviceID();
        }

        void Platform::Utility::Threading(ThreadCB cb, payload data, prioritytype priority)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void* Platform::Utility::ThreadingEx(ThreadExCB cb, payload data, prioritytype priority)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        uint64 Platform::Utility::CurrentThreadID()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        uint32 Platform::Utility::IdealThreadCount()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 3;
        }

        uint64 Platform::Utility::CurrentTimeMsec()
        {
            #if BOSS_WINDOWS
                FILETIME FileTime;
                GetSystemTimeAsFileTime(&FileTime);
                return *((uint64*) &FileTime) / 10000;
            #elif BOSS_MAC_OSX || BOSS_IPHONE
                return (uint64) (CFAbsoluteTimeGetCurrent() * 1000);
            #elif BOSS_ANDROID
                BOSS_ASSERT("Further development is needed.", false);
                return 0;
            #endif
        }

        sint64 Platform::Utility::CurrentAvailableMemory(sint64* totalbytes)
        {
            return PlatformImpl::Wrap::Utility_CurrentAvailableMemory(totalbytes);
        }

        sint64 Platform::Utility::CurrentAvailableDisk(sint32 drivecode, sint64* totalbytes)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        float Platform::Utility::CurrentTrafficCPU()
        {
            return (float) PlatformImpl::Wrap::Utility_CurrentTrafficCPU();
        }

        sint32 Platform::Utility::LastHotKey()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // CLOCK
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Clock::SetBaseTime(chars timestring)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_clock Platform::Clock::Create(sint32 year, sint32 month, sint32 day,
            sint32 hour, sint32 min, sint32 sec, sint64 nsec)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_clock Platform::Clock::CreateAsWindowTime(uint64 msec)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_clock Platform::Clock::CreateAsCurrent()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_clock Platform::Clock::CreateAsClone(id_clock_read clock)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Clock::Release(id_clock clock)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        sint64 Platform::Clock::GetPeriodNsec(id_clock_read from, id_clock_read to)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", from && to);

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Clock::AddNsec(id_clock dest, sint64 nsec)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", dest);

            BOSS_ASSERT("Further development is needed.", false);
        }

        uint64 Platform::Clock::GetMsec(id_clock clock)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", clock);

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint64 Platform::Clock::GetLocalMsecFromUTC()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Clock::GetDetail(id_clock clock, sint64* nsec,
            sint32* sec, sint32* min, sint32* hour, sint32* day, sint32* month, sint32* year)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", clock);

            BOSS_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // OPTION
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Option::SetFlag(chars name, bool flag)
        {
            PlatformImpl::Wrap::Option_SetOptionFlag(name, flag);
        }

        bool Platform::Option::GetFlag(chars name)
        {
            return PlatformImpl::Wrap::Option_GetOptionFlag(name);
        }

        Strings Platform::Option::GetFlagNames()
        {
            return PlatformImpl::Wrap::Option_GetOptionFlagNames();
        }

        void Platform::Option::SetText(chars name, chars text)
        {
            PlatformImpl::Wrap::Option_SetOptionText(name, text);
        }

        chars Platform::Option::GetText(chars name)
        {
            return PlatformImpl::Wrap::Option_GetOptionText(name);
        }

        Strings Platform::Option::GetTextNames()
        {
            return PlatformImpl::Wrap::Option_GetOptionTextNames();
        }

        void Platform::Option::SetPayload(chars name, payload data)
        {
            PlatformImpl::Wrap::Option_SetOptionPayload(name, data);
        }

        payload Platform::Option::GetPayload(chars name)
        {
            return PlatformImpl::Wrap::Option_GetOptionPayload(name);
        }

        Strings Platform::Option::GetPayloadNames()
        {
            return PlatformImpl::Wrap::Option_GetOptionPayloadNames();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // GRAPHICS
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Graphics::SetScissor(float x, float y, float w, float h)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetColor(uint08 r, uint08 g, uint08 b, uint08 a)
        {
            BOSS_ASSERT("Qt5플랫폼처럼 CurCanvas()로의 통합이 필요!!!", false);

            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            ViewAPI::CurColor() = Color(r, g, b, a);
        }

        void Platform::Graphics::SetMask(MaskRole role)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetFont(chars name, float size)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetFontForFreeType(chars nickname, sint32 height)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetZoom(float zoom)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            ViewAPI::CurPainter()->setZoom(zoom);
        }

        void Platform::Graphics::EraseRect(float x, float y, float w, float h)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::FillRect(float x, float y, float w, float h)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            ViewAPI::CurPainter()->fillRect(Rect(x, y, x + w, y + h), ViewAPI::CurColor());
        }

        void Platform::Graphics::FillCircle(float x, float y, float w, float h)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::FillPolygon(float x, float y, Points p)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawRect(float x, float y, float w, float h, float thick)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawLine(const Point& begin, const Point& end, float thick)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawCircle(float x, float y, float w, float h, float thick)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawBezier(const Vector& begin, const Vector& end, float thick)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawPolyLine(float x, float y, Points p, float thick, bool ring)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            const sint32 Count = p.Count();
            if(Count < 2) return;

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawPolyBezier(float x, float y, Points p, float thick, bool showfirst, bool showlast)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            const sint32 Count = p.Count();
            if(Count < 4) return;

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawRingBezier(float x, float y, Points p, float thick, float curve)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            const sint32 Count = p.Count();
            if(Count < 3) return;

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawTextureToFBO(id_texture_read texture, float tx, float ty, float tw, float th,
            orientationtype ori, bool antialiasing, float x, float y, float w, float h, Color color, uint32 fbo)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        id_image Platform::Graphics::CreateImage(id_bitmap_read bitmap, bool mirrored)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        sint32 Platform::Graphics::GetImageWidth(id_image_read image)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetImageHeight(id_image_read image)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::RemoveImage(id_image image)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_image_routine Platform::Graphics::CreateImageRoutine(id_bitmap_read bitmap, sint32 resizing_width, sint32 resizing_height, const Color coloring)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Graphics::UpdateImageRoutineTimeout(uint64 msec)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_image_read Platform::Graphics::BuildImageRoutineOnce(id_image_routine routine, bool use_timeout)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Graphics::RemoveImageRoutine(id_image_routine routine)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawImage(id_image_read image, float ix, float iy, float iw, float ih, float x, float y, float w, float h)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawPolyImageToFBO(id_image_read image, const Point (&ips)[3], float x, float y, const Point (&ps)[3], const Color (&colors)[3], uint32 fbo)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Graphics::DrawString(float x, float y, float w, float h, chars string, sint32 count, UIFontAlign align, UIFontElide elide)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Graphics::DrawStringW(float x, float y, float w, float h, wchars string, sint32 count, UIFontAlign align, UIFontElide elide)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Graphics::GetLengthOfString(bool byword, sint32 clipping_width, chars string, sint32 count)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetLengthOfStringW(bool byword, sint32 clipping_width, wchars string, sint32 count)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringWidth(chars string, sint32 count)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringWidthW(wchars string, sint32 count)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetFreeTypeStringWidth(chars nickname, sint32 height, chars string, sint32 count)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringHeight()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringAscent()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        bool Platform::Graphics::BeginGL()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Graphics::EndGL()
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        id_texture Platform::Graphics::CreateTexture(bool nv21, bool bitmapcache, sint32 width, sint32 height, const void* bits)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_texture Platform::Graphics::CloneTexture(id_texture texture)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::Graphics::IsTextureNV21(id_texture_read texture)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        uint32 Platform::Graphics::GetTextureID(id_texture_read texture, sint32 i)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetTextureWidth(id_texture_read texture)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetTextureHeight(id_texture_read texture)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::RemoveTexture(id_texture texture)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        id_bitmap Platform::Graphics::CreateBitmapFromTextureGL(id_texture_read texture)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap Platform::Graphics::CreateBitmapFromTextureFast(id_texture texture)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_surface Platform::Graphics::CreateSurface(sint32 width, sint32 height)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        uint32 Platform::Graphics::GetSurfaceFBO(id_surface_read surface)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetSurfaceWidth(id_surface_read surface)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetSurfaceHeight(id_surface_read surface)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::RemoveSurface(id_surface surface)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::BindSurface(id_surface surface)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::UnbindSurface(id_surface surface)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawSurface(id_surface_read surface, float sx, float sy, float sw, float sh, float x, float y, float w, float h)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawSurfaceToFBO(id_surface_read surface, float sx, float sy, float sw, float sh,
            orientationtype ori, bool antialiasing, float x, float y, float w, float h, Color color, uint32 fbo)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("This is blank platform. You can use BOSS_PLATFORM_XXX.", false);
        }

        id_image_read Platform::Graphics::GetImageFromSurface(id_surface_read surface)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Graphics::GetBitmapFromSurface(id_surface_read surface, orientationtype ori)
        {
            BOSS_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // ANIMATE
        ////////////////////////////////////////////////////////////////////////////////
        id_animate Platform::Animate::OpenForLottie(chars filename)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Animate::Close(id_animate animate)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        float Platform::Animate::Seek(id_animate animate, float delta, bool rewind)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Animate::Draw(id_animate animate, float ox, float oy, float scale, float rotate)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // FILE
        ////////////////////////////////////////////////////////////////////////////////
        bool Platform::File::Exist(chars filename)
        {
            if(FILE* CheckFile = fopen(filename, "rb"))
            {
                fclose(CheckFile);
                return true;
            }
            return false;
        }

        bool Platform::File::ExistForDir(chars dirname)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        id_file_read Platform::File::OpenForRead(chars filename)
        {
            return (id_file_read) fopen(filename, "rb");
        }

        id_file Platform::File::OpenForWrite(chars filename, bool autocreatedir)
        {
            FILE* WriteFile = fopen(filename, "wb");
            if(autocreatedir && !WriteFile)
            {
                String DirPath = "";
                for(chars iChar = filename; *iChar; ++iChar)
                {
                    const char OneChar = *iChar;
                    if(OneChar == '/' || OneChar == '\\')
                        #if BOSS_WINDOWS
                            CreateDirectoryA(DirPath, nullptr);
                        #else
                            mkdir(DirPath, 0777);
                        #endif
                    DirPath += OneChar;
                }
                WriteFile = fopen(filename, "wb");
            }
            return (id_file) WriteFile;
        }

        void Platform::File::Close(id_file_read file)
        {
            fclose((FILE*) file);
        }

        const sint32 Platform::File::Size(id_file_read file)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            long CurrentPos = ftell((FILE*) file);
            fseek((FILE*) file, 0, SEEK_END);
            long Result = ftell((FILE*) file);
            fseek((FILE*) file, CurrentPos, SEEK_SET);
            return Result;
        }

        const sint32 Platform::File::Read(id_file_read file, uint08* data, const sint32 size)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            return (sint32) fread(data, 1, size, (FILE*) file);
        }

        const sint32 Platform::File::ReadLine(id_file_read file, char* text, const sint32 size)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            if(fgets(text, size, (FILE*) file))
                return boss_strlen(text);
            return 0;
        }

        const sint32 Platform::File::Write(id_file file, bytes data, const sint32 size)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            return (sint32) fwrite(data, 1, size, (FILE*) file);
        }

        void Platform::File::Seek(id_file_read file, const sint32 focus)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            fseek((FILE*) file, focus, SEEK_SET);
        }

        const sint32 Platform::File::Focus(id_file_read file)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            return (sint32) ftell((FILE*) file);
        }

        sint32 Platform::File::Search(chars dirname, SearchCB cb, payload data, bool needfullpath)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint32 Platform::File::GetAttributes(wchars itemname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            #if BOSS_WINDOWS
                BOSS_ASSERT("Further development is needed.", false);
                return -1;
            #else
                struct stat CurInfo;
                if(lstat(ItemnameUTF8, &CurInfo) == -1)
                    return -1; // INVALID_FILE_ATTRIBUTES

                sint32 Result = 0;
                if(!(CurInfo.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH))) Result |= 0x1; // FILE_ATTRIBUTE_READONLY
                #if !BOSS_ANDROID
                    if(CurInfo.st_flags & UF_HIDDEN) Result |= 0x2; // FILE_ATTRIBUTE_HIDDEN
                #endif
                if(S_ISDIR(CurInfo.st_mode)) Result |= 0x10; // FILE_ATTRIBUTE_DIRECTORY
                if(S_ISLNK(CurInfo.st_mode)) Result |= 0x400; // FILE_ATTRIBUTE_REPARSE_POINT

                if(size) *size = (uint64) CurInfo.st_size;
                #if !BOSS_ANDROID && (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
                    if(ctime) *ctime = ((uint64) CurInfo.st_ctimespec.tv_sec) * 1000000000 + CurInfo.st_ctimespec.tv_nsec;
                    if(atime) *atime = ((uint64) CurInfo.st_atimespec.tv_sec) * 1000000000 + CurInfo.st_atimespec.tv_nsec;
                    if(mtime) *mtime = ((uint64) CurInfo.st_mtimespec.tv_sec) * 1000000000 + CurInfo.st_mtimespec.tv_nsec;
                #else
                    if(ctime) *ctime = ((uint64) CurInfo.st_ctime) * 1000000000 + CurInfo.st_ctimensec;
                    if(atime) *atime = ((uint64) CurInfo.st_atime) * 1000000000 + CurInfo.st_atimensec;
                    if(mtime) *mtime = ((uint64) CurInfo.st_mtime) * 1000000000 + CurInfo.st_mtimensec;
                #endif
                return Result;
            #endif
        }

        WString Platform::File::GetFullPath(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname, false));
            if((('A' <= ItemnameUTF8[0] && ItemnameUTF8[0] <= 'Z') ||
                ('a' <= ItemnameUTF8[0] && ItemnameUTF8[0] <= 'z')) && ItemnameUTF8[1] == ':')
                return WString::FromChars(ItemnameUTF8);

            String AbsoluteName = BOSS::Platform::File::RootForAssetsRem() + String::FromWChars(itemname);
            if(AbsoluteName[-2] == '/' || AbsoluteName[-2] == '\\')
                AbsoluteName.SubTail(1);

            #if BOSS_WINDOWS
                return WString::FromChars(AbsoluteName);
            #elif BOSS_MAC_OSX || BOSS_IPHONE
                return WString::FromChars("Q:" + AbsoluteName);
            #elif BOSS_ANDROID
                return WString::FromChars("Q:" + AbsoluteName);
            #endif
        }

        WString Platform::File::GetDirName(wchars itemname, wchar_t badslash, wchar_t goodslash)
        {
            return PlatformImpl::Wrap::File_GetDirName(itemname, badslash, goodslash);
        }

        WString Platform::File::GetShortName(wchars itemname)
        {
            return PlatformImpl::Wrap::File_GetShortName(itemname);
        }

        sint32 Platform::File::GetDriveCode()
        {
            return PlatformImpl::Wrap::File_GetDriveCode();
        }

        bool Platform::File::CanAccess(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::CanWritable(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::Remove(wchars itemname, bool autoremovedir)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::Rename(wchars existing_itemname, wchars new_itemname)
        {
            const String ExistingItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(existing_itemname));
            const String NewItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(new_itemname));

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::Tempname(char* format, sint32 length)
        {
            return PlatformImpl::Wrap::File_Tempname(format, length);
        }

        bool Platform::File::CreateDir(wchars dirname, bool autocreatedir)
        {
            const String DirnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(dirname));

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::RemoveDir(wchars dirname, bool autoremovedir)
        {
            const String DirnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(dirname));

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::File::FDOpen(wchars filename, bool writable, bool append, bool exclusive, bool truncate)
        {
            const String FilenameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(filename));

            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        bool Platform::File::FDClose(sint32 fd)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::File::FDFromFile(boss_file file)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        boss_file Platform::File::FDToFile(sint32 fd)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        sint64 Platform::File::FDRead(sint32 fd, void* data, sint64 size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint64 Platform::File::FDWrite(sint32 fd, const void* data, sint64 size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint64 Platform::File::FDSeek(sint32 fd, sint64 offset, sint32 origin)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        bool Platform::File::FDResize(sint32 fd, sint64 size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void* Platform::File::FDMap(boss_file file, sint64 offset, sint64 size, bool readonly)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::File::FDUnmap(const void* map)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        uint32 Platform::File::FDAttributes(sint32 fd, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1; // INVALID_FILE_ATTRIBUTES
        }

        void Platform::File::ResetAssetsRoot(chars dirname)
        {
            PlatformImpl::Core::SetRoot(0, PlatformImpl::Core::NormalPath(dirname, false));
            BOSS_TRACE("Platform::File::ResetAssetsRoot() ==> \"%s\"", (chars) PlatformImpl::Core::GetCopiedRoot(0));
        }

        void Platform::File::ResetAssetsRemRoot(chars dirname)
        {
            PlatformImpl::Core::SetRoot(1, PlatformImpl::Core::NormalPath(dirname, false));
            BOSS_TRACE("Platform::File::ResetAssetsRemRoot() ==> \"%s\"", (chars) PlatformImpl::Core::GetCopiedRoot(1));
        }

        const String Platform::File::RootForAssets()
        {
            const String Result = PlatformImpl::Core::GetCopiedRoot(0);
            if(0 < Result.Length())
                return Result;

            String NewPath;
            #if BOSS_WINDOWS
                NewPath = "../assets/";
            #elif BOSS_MAC_OSX
                BOSS_ASSERT("Further development is needed.", false);
                NewPath = "../assets/";
            #elif BOSS_IPHONE
                NewPath = CFStringGetCStringPtr(CFURLGetString(
                    CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle())), kCFStringEncodingUTF8) + 7; // 7은 file://
            #elif BOSS_ANDROID
                NewPath = "assets:/";
            #else
                NewPath = "../assets/";
            #endif

            PlatformImpl::Core::SetRoot(0, PlatformImpl::Core::NormalPath(NewPath, false));
            BOSS_TRACE("Platform::File::RootForAssets() ==> \"%s\"", (chars) PlatformImpl::Core::GetCopiedRoot(0));
            return PlatformImpl::Core::GetCopiedRoot(0);
        }

        const String Platform::File::RootForAssetsRem()
        {
            const String Result = PlatformImpl::Core::GetCopiedRoot(1);
            if(0 < Result.Length())
                return Result;

            String NewPath;
            #if BOSS_WINDOWS
                NewPath = "../assets-rem";
                CreateDirectoryA(NewPath, nullptr);
                NewPath += "/";
            #elif BOSS_MAC_OSX
                BOSS_ASSERT("Further development is needed.", false);
                NewPath = "../assets-rem/";
            #elif BOSS_IPHONE
                NewPath = CFStringGetCStringPtr(
                    CFURLGetString(CFCopyHomeDirectoryURL()), kCFStringEncodingUTF8) + 7; // 7은 file://
                NewPath += "Library";
                if(access(NewPath, 0777)) mkdir(NewPath, 0777);
                NewPath += "/Caches";
                if(access(NewPath, 0777)) mkdir(NewPath, 0777);
                NewPath += "/assets-rem";
                if(access(NewPath, 0777)) mkdir(NewPath, 0777);
                NewPath += "/";
            #elif BOSS_ANDROID
                BOSS_ASSERT("Further development is needed.", false);
                NewPath = "../assets-rem/";
            #else
                NewPath = "../assets-rem/";
            #endif

            PlatformImpl::Core::SetRoot(1, PlatformImpl::Core::NormalPath(NewPath, false));
            BOSS_TRACE("Platform::File::RootForAssetsRem() ==> \"%s\"", (chars) PlatformImpl::Core::GetCopiedRoot(1));
            return PlatformImpl::Core::GetCopiedRoot(1);
        }

        const String Platform::File::RootForData()
        {
            String Result = "";
            BOSS_ASSERT("Further development is needed.", false);
            return Result;
        }

        const String Platform::File::RootForDesktop()
        {
            String Result = "";
            BOSS_ASSERT("Further development is needed.", false);
            return Result;
        }

        const String Platform::File::RootForStartup()
        {
            String Result = "";
            BOSS_ASSERT("Further development is needed.", false);
            return Result;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SOUND
        ////////////////////////////////////////////////////////////////////////////////
        id_sound Platform::Sound::OpenForFile(chars filename, bool loop, sint32 fade_msec)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_sound Platform::Sound::OpenForStream(sint32 channel, sint32 sample_rate, sint32 sample_size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Sound::Close(id_sound sound)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Sound::SetVolume(float volume, sint32 fade_msec)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Sound::Play(id_sound sound, float volume_rate)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Sound::Stop(id_sound sound)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Sound::NowPlaying(id_sound sound)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Sound::AddStreamForPlay(id_sound sound, bytes raw, sint32 size, sint32 timeout)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        void Platform::Sound::StopAll()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Sound::PauseAll()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Sound::ResumeAll()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SOCKET
        ////////////////////////////////////////////////////////////////////////////////
        id_socket Platform::Socket::OpenForTCP()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_socket Platform::Socket::OpenForUDP()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_socket Platform::Socket::OpenForWS()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::Socket::Close(id_socket socket, sint32 timeout)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Socket::Connect(id_socket socket, chars domain, uint16 port, sint32 timeout)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Socket::Disconnect(id_socket socket, sint32 timeout)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Socket::BindForUdp(id_socket socket, uint16 port, sint32 timeout)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Socket::RecvAvailable(id_socket socket)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint32 Platform::Socket::Recv(id_socket socket, uint08* data, sint32 size, sint32 timeout, ip4address* ip_udp, uint16* port_udp)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint32 Platform::Socket::Send(id_socket socket, bytes data, sint32 size, sint32 timeout, bool utf8)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        class Hostent
        {
        public:
            Hostent() :
                h_addrtype(2), // AF_INET
                h_length(4) // IPv4
            {
                h_name = nullptr;
                h_aliases = nullptr;
                h_addr_list = nullptr;
            }

            ~Hostent()
            {
                delete[] h_name;
                if(h_aliases)
                for(chars* ptr_aliases = h_aliases; *ptr_aliases; ++ptr_aliases)
                    delete[] *ptr_aliases;
                delete[] h_aliases;
                if(h_addr_list)
                for(bytes* ptr_addr_list = h_addr_list; *ptr_addr_list; ++ptr_addr_list)
                    delete[] *ptr_addr_list;
                delete[] h_addr_list;
            }

        public:
            chars h_name;
            chars* h_aliases;
            const sint16 h_addrtype;
            const sint16 h_length;
            bytes* h_addr_list;
        };

        void* Platform::Socket::GetHostByName(chars name)
        {
            static Map<Hostent> HostentMap;
            if(HostentMap.Access(name))
                HostentMap.Remove(name);

            Hostent& CurHostent = HostentMap(name);

            BOSS_ASSERT("Further development is needed.", false);
            return &CurHostent;
        }

        class Servent
        {
        public:
            Servent()
            {
                s_name = nullptr;
                s_aliases = nullptr;
                s_port = 0;
                s_proto = nullptr;
            }

            ~Servent()
            {
                delete[] s_name;
                for(chars* ptr_aliases = s_aliases; ptr_aliases; ++ptr_aliases)
                    delete[] *ptr_aliases;
                delete[] s_aliases;
                delete[] s_proto;
            }

        public:
            chars s_name;
            chars* s_aliases;
            sint16 s_port;
            chars s_proto;
        };

        void* Platform::Socket::GetServByName(chars name, chars proto)
        {
            static Map<Servent> ServentMap;
            Servent& CurServent = ServentMap(name);

            BOSS_ASSERT("Further development is needed.", false);
            return &CurServent;
        }

        ip4address Platform::Socket::GetLocalAddress(ip6address* ip6)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return {};
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SERVER
        ////////////////////////////////////////////////////////////////////////////////
        id_server Platform::Server::CreateTCP(bool sizefield)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_server Platform::Server::CreateWS(chars name)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Server::Release(id_server server)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Server::Listen(id_server server, uint16 port)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Server::TryNextPacket(id_server server)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        packettype Platform::Server::GetPacketType(id_server server)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return packettype_null;
        }

        sint32 Platform::Server::GetPacketPeerID(id_server server)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        bytes Platform::Server::GetPacketBuffer(id_server server, sint32* getsize)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::Server::SendToPeer(id_server server, sint32 peerid, const void* buffer, sint32 buffersize, bool utf8)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Server::KickPeer(id_server server, sint32 peerid)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Server::GetPeerInfo(id_server server, sint32 peerid, ip4address* ip4, ip6address* ip6, uint16* port)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // PIPE
        ////////////////////////////////////////////////////////////////////////////////
        id_pipe Platform::Pipe::Open(chars name)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Pipe::Close(id_pipe pipe)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Pipe::IsServer(id_pipe pipe)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        ConnectStatus Platform::Pipe::Status(id_pipe pipe)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return CS_Disconnected;
        }

        sint32 Platform::Pipe::RecvAvailable(id_pipe pipe)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Pipe::Recv(id_pipe pipe, uint08* data, sint32 size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        const Context* Platform::Pipe::RecvJson(id_pipe pipe)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::Pipe::Send(id_pipe pipe, bytes data, sint32 size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Pipe::SendJson(id_pipe pipe, const String& json)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // WEB
        ////////////////////////////////////////////////////////////////////////////////
        h_web Platform::Web::Create(chars url, sint32 width, sint32 height, bool clearcookies, EventCB cb, payload data)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_web::null();
        }

        void Platform::Web::Release(h_web web)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Web::Reload(h_web web, chars url)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Web::NowLoading(h_web web, float* rate)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Web::Resize(h_web web, sint32 width, sint32 height)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Web::SendTouchEvent(h_web web, TouchType type, sint32 x, sint32 y)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Web::SendKeyEvent(h_web web, sint32 code, chars text, bool pressed)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Web::CallJSFunction(h_web web, chars script, sint32 matchid)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_texture_read Platform::Web::GetPageTexture(h_web web)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_image_read Platform::Web::GetPageImage(h_web web)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Web::GetPageBitmap(h_web web, orientationtype ori)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        h_web_native Platform::Web::CreateNative(chars url, bool clearcookies, EventCB cb, payload data)
        {
            return PlatformImpl::Wrap::Web_CreateNative(url, clearcookies, cb, data);
        }

        void Platform::Web::ReleaseNative(h_web_native web_native)
        {
            PlatformImpl::Wrap::Web_ReleaseNative(web_native);
        }

        void Platform::Web::ReloadNative(h_web_native web_native, chars url)
        {
            PlatformImpl::Wrap::Web_ReloadNative(web_native, url);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // PURCHASE
        ////////////////////////////////////////////////////////////////////////////////
        id_purchase Platform::Purchase::Open(chars name, PurchaseType type)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Purchase::Close(id_purchase purchase)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Purchase::IsPurchased(id_purchase purchase)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Purchase::Purchasing(id_purchase purchase, PurchaseCB cb)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // BLUETOOTH
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Bluetooth::SearchingDeviceBegin()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Bluetooth::SearchingDeviceEnd()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Bluetooth::SearchingServiceBegin(chars deviceaddress, Strings uuidfilters)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Bluetooth::SearchingServiceEnd()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_bluetooth Platform::Bluetooth::CreateServer(chars service, chars uuid, bool ble)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bluetooth Platform::Bluetooth::CreateClient(chars uuid)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Bluetooth::Release(id_bluetooth bluetooth)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Bluetooth::Connected(id_bluetooth bluetooth)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Bluetooth::ReadAvailable(id_bluetooth bluetooth)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Bluetooth::Read(id_bluetooth bluetooth, uint08* data, const sint32 size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        bool Platform::Bluetooth::Write(id_bluetooth bluetooth, bytes data, const sint32 size, chars uuid_for_ble)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SERIAL
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Serial::GetAllNames(String* spec)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return Strings();
        }

        id_serial Platform::Serial::Open(chars name, sint32 baudrate, SerialDecodeCB dec, SerialEncodeCB enc)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Serial::Close(id_serial serial)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Serial::Connected(id_serial serial)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Serial::ReadReady(id_serial serial, sint32* gettype)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Serial::ReadAvailable(id_serial serial)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Serial::Read(id_serial serial, chars format, ...)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Serial::Write(id_serial serial, chars format, ...)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Serial::WriteFlush(id_serial serial, sint32 type)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // CAMERA
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Camera::GetAllNames(String* spec)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return Strings();
        }

        id_camera Platform::Camera::Open(chars name, sint32 width, sint32 height)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Camera::Close(id_camera camera)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_camera Platform::Camera::Clone(id_camera camera)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Camera::Capture(id_camera camera, bool preview, bool needstop)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_texture Platform::Camera::CloneCapturedTexture(id_camera camera, bool bitmapcache)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_image_read Platform::Camera::LastCapturedImage(id_camera camera, sint32 maxwidth, sint32 maxheight, sint32 rotate)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Camera::LastCapturedBitmap(id_camera camera, orientationtype ori)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        size64 Platform::Camera::LastCapturedSize(id_camera camera)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return {0, 0};
        }

        uint64 Platform::Camera::LastCapturedTimeMsec(id_camera camera, sint32* avgmsec)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Camera::TotalPictureShotCount(id_camera camera)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Camera::TotalPreviewShotCount(id_camera camera)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // MICROPHONE
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Microphone::GetAllNames(String* spec)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return Strings();
        }

        id_microphone Platform::Microphone::Open(chars name, sint32 maxcount)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Microphone::Close(id_microphone microphone)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        sint32 Platform::Microphone::GetBitRate(id_microphone microphone)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Microphone::GetChannel(id_microphone microphone)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Microphone::GetSampleRate(id_microphone microphone)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        bool Platform::Microphone::TryNextSound(id_microphone microphone)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bytes Platform::Microphone::GetSoundData(id_microphone microphone, sint32* length, uint64* timems)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }
    }

    NativePainter::NativePainter()
    {
        switch(__version())
        {
        case 0x20: initShaderES20(); break;
        case 0x30: initShaderES30(); break;
        default:
            BOSS_ASSERT("지원되는 OpenGL버전이 아닙니다", false);
            m_program = 0;
            m_vshader = 0;
            m_fshader = 0;
            break;
        }
        m_width = 0;
        m_height = 0;
    }

    NativePainter::~NativePainter()
    {
        termShaderES();
    }

    void NativePainter::reset(const sint32 width, const sint32 height)
    {
        m_width = width * 2; // 현 SPINE좌표계랑 맞추기 위한 방편
        m_height = height * 2; // 차후 수정요망
        setZoom(1);
    }

    void NativePainter::setZoom(const float zoom)
    {
        __loadIdentity();
        // 4평면만 사용하기에 zoom이 2배가 되어야 함
        const float w = zoom * +2 / m_width;
        const float h = zoom * -2 / m_height;
        __multiply(
            w, 0, 0, 0,
            0, h, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);
    }

    void NativePainter::fillRect(const BOSS::Rect& rect, const BOSS::Color& color)
    {
        #if BOSS_NEED_NATIVE_OPENGL
            glBindFramebuffer(GL_FRAMEBUFFER, 1); __testGL(BOSS_DBG 0);
            // 편리한 좌표계를 위해 4평면만 사용
            glViewport(-m_width, 0, m_width * 2, m_height * 2); __testGL(BOSS_DBG 0);

            glUseProgram(m_program); __testGL(BOSS_DBG 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0); __testGL(BOSS_DBG 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); __testGL(BOSS_DBG 0);
            glEnableVertexAttribArray(VerticeID); __testGL(BOSS_DBG 0);
            glEnableVertexAttribArray(ColorID); __testGL(BOSS_DBG 0);
            glVertexAttribPointer(VerticeID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &m_attrib[0].vertices[0]); __testGL(BOSS_DBG 0);
            glVertexAttribPointer(ColorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &m_attrib[0].colors[0]); __testGL(BOSS_DBG 0);
            glUniformMatrix4fv(m_matrix, 1, GL_FALSE, (const GLfloat*) &m_m[0][0]); __testGL(BOSS_DBG 0);

            glDisable(GL_CULL_FACE); __testGL(BOSS_DBG 0);
            glDisable(GL_DEPTH_TEST); __testGL(BOSS_DBG 0);
            glDisable(GL_SCISSOR_TEST); __testGL(BOSS_DBG 0);
            glEnable(GL_BLEND); __testGL(BOSS_DBG 0);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); __testGL(BOSS_DBG 0);

            m_attrib[0].vertices[0] = rect.l;
            m_attrib[0].vertices[1] = rect.t;
            m_attrib[0].color32 = color.ToABGR();
            m_attrib[1].vertices[0] = rect.r;
            m_attrib[1].vertices[1] = rect.t;
            m_attrib[1].color32 = color.ToABGR();
            m_attrib[2].vertices[0] = rect.l;
            m_attrib[2].vertices[1] = rect.b;
            m_attrib[2].color32 = color.ToABGR();
            m_attrib[3].vertices[0] = rect.r;
            m_attrib[3].vertices[1] = rect.b;
            m_attrib[3].color32 = color.ToABGR();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); __testGL(BOSS_DBG 0);
        #endif
    }

    void NativePainter::initShaderES(chars vsource, chars fsource)
    {
        #if BOSS_NEED_NATIVE_OPENGL
            static chars VertexShaderSourceCodePtr = vsource;
            m_vshader = glCreateShader(GL_VERTEX_SHADER); __testGL(BOSS_DBG 0);
            glShaderSource(m_vshader, 1, &VertexShaderSourceCodePtr, NULL); __testGL(BOSS_DBG 0);
            glCompileShader(m_vshader); __testShader(BOSS_DBG m_vshader);

            static chars FragmentShaderSourceCodePtr = fsource;
            m_fshader = glCreateShader(GL_FRAGMENT_SHADER); __testGL(BOSS_DBG 0);
            glShaderSource(m_fshader, 1, &FragmentShaderSourceCodePtr, NULL); __testGL(BOSS_DBG 0);
            glCompileShader(m_fshader); __testShader(BOSS_DBG m_fshader);

            m_program = glCreateProgram(); __testGL(BOSS_DBG 0);
            glAttachShader(m_program, m_vshader); __testShader(BOSS_DBG m_vshader);
            glAttachShader(m_program, m_fshader); __testShader(BOSS_DBG m_fshader);

            glEnableVertexAttribArray(VerticeID); __testGL(BOSS_DBG 0);
            glEnableVertexAttribArray(ColorID); __testGL(BOSS_DBG 0);
            glBindAttribLocation(m_program, VerticeID, "a_position"); __testGL(BOSS_DBG 0);
            glBindAttribLocation(m_program, ColorID, "a_color"); __testGL(BOSS_DBG 0);
            glVertexAttribPointer(VerticeID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &m_attrib[0].vertices[0]); __testGL(BOSS_DBG 0);
            glVertexAttribPointer(ColorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &m_attrib[0].colors[0]); __testGL(BOSS_DBG 0);

            glLinkProgram(m_program); __testProgram(BOSS_DBG m_program);
            glValidateProgram(m_program); __testProgram(BOSS_DBG m_program);

            __loadIdentity();
            glUseProgram(m_program); __testProgram(BOSS_DBG m_program);
            m_matrix = glGetUniformLocation(m_program, "u_matrix"); __testGL(BOSS_DBG 0);
            glUniformMatrix4fv(m_matrix, 1, GL_FALSE, (const GLfloat*) &m_m[0][0]); __testGL(BOSS_DBG 0);
        #endif
    }

    void NativePainter::initShaderES20()
    {
        initShaderES(
            "attribute highp vec2 a_position;\n"
            "attribute highp vec4 a_color;\n"
            "uniform highp mat4 u_matrix;\n"
            "varying mediump vec4 v_fragmentColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_matrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
            "    v_fragmentColor = a_color;\n"
            "}",
            "uniform highp mat4 u_matrix;\n"
            "varying mediump vec4 v_fragmentColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_FragColor = v_fragmentColor;\n"
            "}");
    }

    void NativePainter::initShaderES30()
    {
        initShaderES(String::Format(
            "#version 300 es\n"
            "layout (location = %d) in highp vec2 a_position;\n"
            "layout (location = %d) in highp vec4 a_color;\n"
            "uniform highp mat4 u_matrix;\n"
            "out mediump vec4 v_fragmentColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_matrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
            "    v_fragmentColor = a_color;\n"
            "}", VerticeID, ColorID),
            "#version 300 es\n"
            "layout (location = 0) out highp vec4 oColour;\n"
            "uniform highp mat4 u_matrix;\n"
            "in mediump vec4 v_fragmentColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    oColour = v_fragmentColor;\n"
            "}");
    }

    void NativePainter::termShaderES()
    {
        #if BOSS_NEED_NATIVE_OPENGL
            glDeleteProgram(m_program); __testGL(BOSS_DBG 0);
            m_program = 0;
            glDeleteShader(m_vshader); __testGL(BOSS_DBG 0);
            m_vshader = 0;
            glDeleteShader(m_fshader); __testGL(BOSS_DBG 0);
            m_fshader = 0;
        #endif
    }

    void NativePainter::__testGL(BOSS_DBG_PRM sint32 nouse)
    {
        #if BOSS_NEED_NATIVE_OPENGL
            if(auto errorCode = glGetError())
                BOSS_ASSERT_PRM(String::Format("__testGL(error:%d) is failed", errorCode), false);
        #endif
    }

    uint08 NativePainter::__version()
    {
        #if BOSS_NEED_NATIVE_OPENGL
            static uint08 Version = 0x00;
            if(Version != 0x00) return Version;

            chars VersionString = (chars) glGetString(GL_VERSION);
            // 예시1: OpenGL ES 2.0 IMGSGX543-124.1
            // 예시2: OpenGL ES 3.0 APPLE-12.0.38
            BOSS_ASSERT("이해할 수 없는 버전정보입니다",
                VersionString[9] == ' ' && VersionString[11] == '.' && VersionString[13] == ' ');

            const uint32 VersionMajor = VersionString[10] - '0';
            const uint32 VersionMinor = VersionString[12] - '0';
            Version = ((VersionMajor & 0xF) << 4) | (VersionMinor & 0xF);
            return Version;
        #else
            return 0x00;
        #endif
    }

    void NativePainter::__testShader(BOSS_DBG_PRM GLuint shader)
    {
        #if BOSS_NEED_NATIVE_OPENGL
            GLint status;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            if(status == GL_FALSE)
            {
                GLchar log[4096];
                GLsizei s;
                glGetShaderInfoLog(shader, 4096, &s, log);
                BOSS_ASSERT_PRM(String::Format("__testShader(%s) is failed", log), false);
            }
            else if(auto errorCode = glGetError())
                BOSS_ASSERT_PRM(String::Format("__testShader(error:%d) is failed", errorCode), false);
        #endif
    }

    void NativePainter::__testProgram(BOSS_DBG_PRM GLuint program)
    {
        #if BOSS_NEED_NATIVE_OPENGL
            GLint linked;
            glGetProgramiv(program, GL_LINK_STATUS, &linked);
            if(!linked)
            {
                int i32InfoLogLength, i32CharsWritten;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
                char* pszInfoLog = new char[i32InfoLogLength];
                glGetProgramInfoLog(program, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
                BOSS_ASSERT_PRM(String::Format("__testProgram(%s) is failed", pszInfoLog), false);
                delete [] pszInfoLog;
            }
            else if(auto errorCode = glGetError())
                BOSS_ASSERT_PRM(String::Format("__testProgram(error:%d) is failed", errorCode), false);
        #endif
    }

    void NativePainter::__loadIdentity()
    {
        m_m[0][0] = 1; m_m[0][1] = 0; m_m[0][2] = 0; m_m[0][3] = 0;
        m_m[1][0] = 0; m_m[1][1] = 1; m_m[1][2] = 0; m_m[1][3] = 0;
        m_m[2][0] = 0; m_m[2][1] = 0; m_m[2][2] = 1; m_m[2][3] = 0;
        m_m[3][0] = 0; m_m[3][1] = 0; m_m[3][2] = 0; m_m[3][3] = 1;
    }

    void NativePainter::__multiply(
        const float m00, const float m01, const float m02, const float m03,
        const float m10, const float m11, const float m12, const float m13,
        const float m20, const float m21, const float m22, const float m23,
        const float m30, const float m31, const float m32, const float m33)
    {
        GLfloat m[4][4];
        Memory::Copy(&m[0][0], &m_m[0][0], sizeof(GLfloat) * 16);
        m_m[0][0] = m[0][0] * m00 + m[0][1] * m10 + m[0][2] * m20 + m[0][3] * m30;
        m_m[0][1] = m[0][0] * m01 + m[0][1] * m11 + m[0][2] * m21 + m[0][3] * m31;
        m_m[0][2] = m[0][0] * m02 + m[0][1] * m12 + m[0][2] * m22 + m[0][3] * m32;
        m_m[0][3] = m[0][0] * m03 + m[0][1] * m13 + m[0][2] * m23 + m[0][3] * m33;
        m_m[1][0] = m[1][0] * m00 + m[1][1] * m10 + m[1][2] * m20 + m[1][3] * m30;
        m_m[1][1] = m[1][0] * m01 + m[1][1] * m11 + m[1][2] * m21 + m[1][3] * m31;
        m_m[1][2] = m[1][0] * m02 + m[1][1] * m12 + m[1][2] * m22 + m[1][3] * m32;
        m_m[1][3] = m[1][0] * m03 + m[1][1] * m13 + m[1][2] * m23 + m[1][3] * m33;
        m_m[2][0] = m[2][0] * m00 + m[2][1] * m10 + m[2][2] * m20 + m[2][3] * m30;
        m_m[2][1] = m[2][0] * m01 + m[2][1] * m11 + m[2][2] * m21 + m[2][3] * m31;
        m_m[2][2] = m[2][0] * m02 + m[2][1] * m12 + m[2][2] * m22 + m[2][3] * m32;
        m_m[2][3] = m[2][0] * m03 + m[2][1] * m13 + m[2][2] * m23 + m[2][3] * m33;
        m_m[3][0] = m[3][0] * m00 + m[3][1] * m10 + m[3][2] * m20 + m[3][3] * m30;
        m_m[3][1] = m[3][0] * m01 + m[3][1] * m11 + m[3][2] * m21 + m[3][3] * m31;
        m_m[3][2] = m[3][0] * m02 + m[3][1] * m12 + m[3][2] * m22 + m[3][3] * m32;
        m_m[3][3] = m[3][0] * m03 + m[3][1] * m13 + m[3][2] * m23 + m[3][3] * m33;
    }

    ViewAPI::ViewAPI(View* manager)
    {
        m_view_manager = manager;
        m_next_manager = nullptr;
    }

    ViewAPI::ViewAPI(const ViewAPI& rhs)
    {BOSS_ASSERT("호출불가", false);}

    ViewAPI& ViewAPI::operator=(const ViewAPI& rhs)
    {BOSS_ASSERT("호출불가", false); return *this;}

    ViewAPI::~ViewAPI()
    {
        delete m_view_manager;
        delete m_next_manager;
    }

    NativePainter*& ViewAPI::CurPainter()
    {static NativePainter* _ = nullptr; return _;}

    BOSS::Color& ViewAPI::CurColor()
    {static BOSS::Color _; return _;}

    h_view ViewAPI::changeViewManager(View* manager)
    {
        h_view OldViewHandle;
        if(m_view_manager)
        {
            OldViewHandle = m_view_manager->SetView(h_view::null());
            delete m_view_manager;
        }

        m_view_manager = manager;
        return OldViewHandle;
    }

    void ViewAPI::setNextViewManager(View* manager)
    {
        delete m_next_manager;
        m_next_manager = manager;
    }

    void ViewAPI::render(sint32 width, sint32 height)
    {
        static NativePainter Painter;
        // for assert dialog
        if(CurPainter()) return;

        Painter.reset(width, height);
        CurPainter() = &Painter;
        m_view_manager->OnRender(width, height, 0, 0, width, height);
        CurPainter() = nullptr;

        if(m_next_manager)
        {
            h_view OldViewHandle = changeViewManager(m_next_manager);
            m_next_manager->SetView(OldViewHandle);
            m_next_manager = nullptr;
            sendCreate();
        }
    }

    void ViewAPI::touch(TouchType type, sint32 id, sint32 x, sint32 y)
    {m_view_manager->OnTouch(type, id, x, y);}

    void ViewAPI::sendCreate()
    {m_view_manager->OnCreate();}

#endif
