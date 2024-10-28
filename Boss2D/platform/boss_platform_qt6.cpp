#include <boss.hpp>
#include "boss_platform_qt6.hpp"

#ifdef BOSS_PLATFORM_QT6

    #if BOSS_WINDOWS
        #include <windows.h>
    #elif _POSIX_C_SOURCE >= 199309L
        #include <time.h>
    #else
        #include <unistd.h>
    #endif
    #include <stdio.h>
    #include <stdarg.h>

    MainWindow* g_window = nullptr;
    sint32 g_argc = 0;
    char** g_argv = nullptr;

	#if BOSS_NEED_MAIN
        extern bool PlatformInit();
        extern void PlatformQuit();
        extern void PlatformFree();

        int main(int argc, char* argv[])
        {
            int result = 0;
            {
                QApplication app(argc, argv);

                MainWindow mainWindow;
                g_window = &mainWindow;
                g_argc = argc;
                g_argv = argv;

                if(PlatformInit())
                {
                    result = app.exec();
                    PlatformQuit();
                }
                g_window = nullptr;
                Platform::Popup::HideSplash();
            }

            PlatformFree();
            // 스토리지(TLS) 영구제거
            Storage::ClearAll(SCL_SystemAndUser);
            return result;
        }
    #endif

    extern "C" int boss_platform_assert(BOSS_DBG_PRM const char* name, const char* query)
    {
        String AssertInfo[4] = {
            String::Format("[QUERY] %s", query),
            String::Format("[METHOD] %s()", BOSS_DBG_FUNC),
            String::Format("[FILE/LINE] %s(%dLn)", BOSS_DBG_FILE, BOSS_DBG_LINE),
            String::Format("[THREAD_ID] -unknown-")};
        #if BOSS_WINDOWS
            OutputDebugStringW(L"************************************************************\n");
            OutputDebugStringW((wchars) WString::FromChars(name));
            OutputDebugStringW(L"\n------------------------------------------------------------\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[0])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[1])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[2])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[3])); OutputDebugStringW(L"\n");
            OutputDebugStringW(L"************************************************************\n");
        #endif

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
        #endif
        return 2;
    }

    extern "C" void boss_platform_trace(const char* text, ...)
    {
        char TraceMessage[10240];
        va_list Args;
        va_start(Args, text);
        boss_vsnprintf(TraceMessage, 10240, text, Args);
        va_end(Args);

        #if BOSS_WINDOWS
            OutputDebugStringW((wchars) WString::FromChars(TraceMessage));
            OutputDebugStringW(L"\n");
        #endif
    }

    namespace BOSS
    {
        ////////////////////////////////////////////////////////////////////////////////
        // PLATFORM
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::InitForGL(bool frameless, bool topmost, chars url)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::InitForMDI(bool frameless, bool topmost)
        {
            g_window->InitForWidget(frameless, topmost);
        }

        void Platform::SetViewCreator(View::CreatorCB creator)
        {
            PlatformImpl::Core::g_Creator = creator;
        }

        void Platform::SetWindowName(chars name)
        {
            g_window->setWindowTitle(QString::fromUtf8(name, -1));
        }

        h_view Platform::SetWindowView(chars viewclass)
        {
            auto NewViewManager = PlatformImpl::Core::g_Creator(viewclass);
            return g_window->View()->ChangeViewManager(NewViewManager);
        }

        void Platform::SetWindowPos(sint32 x, sint32 y)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SetWindowSize(sint32 width, sint32 height)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SetWindowRect(sint32 x, sint32 y, sint32 width, sint32 height)
        {
            g_window->setGeometry(x, y, width, height);
        }

        rect128 Platform::GetWindowRect(bool normally)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return {0, 0, 0, 0};
        }

        void Platform::SetWindowVisible(bool visible)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SetWindowTopMost(bool enable)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        sint64 Platform::GetWindowHandle()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        void Platform::SetWindowWebUrl(chars url)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SendWindowWebTouchEvent(TouchType type, sint32 x, sint32 y)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SendWindowWebKeyEvent(sint32 code, chars text, bool pressed)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::CallWindowWebJSFunction(chars script, sint32 matchid)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SetWindowFlash()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::SetWindowMask(id_image_read image)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::SetWindowOpacity(float value)
        {
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
            BOSS_ASSERT("Further development is needed.", false);
        }

        h_icon Platform::CreateIcon(chars filepath)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_icon::null();
        }

        h_action Platform::CreateAction(chars name, chars tip, h_view view, h_icon icon)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_action::null();
        }

        h_policy Platform::CreatePolicy(sint32 minwidth, sint32 minheight, sint32 maxwidth, sint32 maxheight)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_policy::null();
        }

        h_view Platform::CreateView(chars name, sint32 width, sint32 height, h_policy policy, chars viewclass, bool gl)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_view::null();
        }

        void* Platform::SetNextViewClass(h_view view, chars viewclass)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::SetNextViewManager(h_view view, View* viewmanager)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        h_dock Platform::CreateDock(h_view view, UIDirection direction, UIDirection directionbase)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_dock::null();
        }

        void Platform::AddAction(chars group, h_action action, UIRole role)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::AddSeparator(chars group, UIRole role)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::AddToggle(chars group, h_dock dock, UIRole role)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        h_window Platform::OpenChildWindow(h_view view, h_icon icon)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_window::null();
        }

        h_window Platform::OpenPopupWindow(h_view view, h_icon icon)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_window::null();
        }

        h_window Platform::OpenTrayWindow(h_view view, h_icon icon)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return h_window::null();
        }

        void Platform::CloseWindow(h_window window)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_cloned_share Platform::SendNotify(h_view view, chars topic, id_share in, bool needout)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::SendDirectNotify(h_view view, chars topic, id_share in)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::BroadcastNotify(chars topic, id_share in, NotifyType type, chars viewclass, bool directly)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SendKeyEvent(h_view view, sint32 code, chars text, bool pressed)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::PassAllViews(PassCB cb, payload data)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::UpdateAllViews()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // POPUP
        ////////////////////////////////////////////////////////////////////////////////
        bool Platform::Popup::TextDialog(String& text, chars title, chars topic, bool ispassword)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Popup::ColorDialog(uint08& r, uint08& g, uint08& b, uint08& a, chars title)
        {
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

        void Platform::Popup::OpenProgramDialog(chars exepath, chars args, bool admin, bool hide, chars dirpath, ublock* getpid)
        {
            return PlatformImpl::Wrap::Popup_ProgramDialog(exepath, args, admin, hide, dirpath, getpid);
        }

        void Platform::Popup::CloseProgramDialog(ublock pid)
        {
            PlatformImpl::Wrap::Popup_KillProgramDialog(pid);
        }

        sint64 Platform::Popup::FindWindowHandle(chars titlename)
        {
            return PlatformImpl::Wrap::Popup_FindWindowHandle(titlename);
        }

        bool Platform::Popup::MoveWindow(sint64 hwnd, sint32 left, sint32 top, sint32 right, sint32 bottom, bool repaint)
        {
            return PlatformImpl::Wrap::Popup_MoveWindow(hwnd, left, top, right, bottom, repaint);
        }

        bool Platform::Popup::MoveWindowGroup(sint64s windowparams)
        {
            return PlatformImpl::Wrap::Popup_MoveWindowGroup(windowparams);
        }

        bool Platform::Popup::MoveWindowGroupCaptured(sint64s windowparams, bool release)
        {
            return PlatformImpl::Wrap::Popup_MoveWindowGroupCaptured(windowparams, release);
        }

        void Platform::Popup::SetTrackerFont(chars family, sint32 pointsize)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Popup::OpenEditTracker(String& text, UIEditType type, sint32 l, sint32 t, sint32 r, sint32 b, bool* enter)
        {
            if(r <= l || b <= t) return false;

            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Popup::OpenListTracker(Strings textes, sint32 l, sint32 t, sint32 r, sint32 b)
        {
            if(r <= l || b <= t) return -1;

            BOSS_ASSERT("Further development is needed.", false);
            return -1;
        }

        void Platform::Popup::CloseAllTracker()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Popup::HasOpenedTracker()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Popup::ShowToolTip(String text)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Popup::HideToolTip()
        {
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
        uint32 Platform::Utility::Random(bool forcereset)
        {
            if(NeedSetRandom || forcereset)
            {
                NeedSetRandom = false;
                BOSS_ASSERT("Further development is needed.", false);
            }

            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Utility::Sleep(sint32 ms, bool process_input, bool process_socket, bool block_event)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::SetMinimize()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::SetFullScreen(bool available_only, sint32 addheight)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Utility::IsFullScreen()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Utility::SetNormalWindow()
        {
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

        bool Platform::Utility::BindExtProgram(chars extname, chars programid, chars exepath)
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
            QPoint GeometryPoint(pos.x, pos.y);
            sint32 Index = 0;
            auto AllScreens = QGuiApplication::screens();
            for(auto Screen : AllScreens)
            {
                if(Screen->geometry().contains(GeometryPoint))
                    return Index;
                Index++;
            }
            return -1;
        }

        sint32 Platform::Utility::GetScreenCount()
        {
            return QGuiApplication::screens().count();
        }

        void Platform::Utility::GetScreenRect(rect128& rect, sint32 screenid, bool available_only)
        {
            const sint32 NumScreens = GetScreenCount();
            if(NumScreens == 0)
                rect.l = rect.t = rect.r = rect.b = 0;
            else if(screenid < NumScreens)
            {
                if(screenid == -1)
                {
                    rect128 TotalRect = {100000, 100000, -100000, -100000};
                    for(sint32 i = 0; i < NumScreens; ++i)
                    {
                        QRect GeometryRect = (available_only)?
                            QGuiApplication::screens()[i]->availableGeometry() :
                            QGuiApplication::screens()[i]->geometry();
                        TotalRect.l = Math::Min(TotalRect.l, GeometryRect.left());
                        TotalRect.t = Math::Min(TotalRect.t, GeometryRect.top());
                        TotalRect.r = Math::Max(TotalRect.r, GeometryRect.right() + 1);
                        TotalRect.b = Math::Max(TotalRect.b, GeometryRect.bottom() + 1);
                    }
                    rect.l = TotalRect.l;
                    rect.t = TotalRect.t;
                    rect.r = TotalRect.r;
                    rect.b = TotalRect.b;
                }
                else
                {
                    QRect GeometryRect = (available_only)?
                        QGuiApplication::screens()[screenid]->availableGeometry() :
                        QGuiApplication::screens()[screenid]->geometry();
                    rect.l = GeometryRect.left();
                    rect.t = GeometryRect.top();
                    rect.r = GeometryRect.right() + 1;
                    rect.b = GeometryRect.bottom() + 1;
                }
            }
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

        void Platform::Utility::SendToTextClipboard(chars text)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        String Platform::Utility::RecvFromTextClipboard()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return String();
        }

        String Platform::Utility::CreateSystemFont(bytes data, const sint32 size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return String();
        }

        void Platform::Utility::SetCursor(CursorRole role)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::GetCursorPos(point64& pos)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::SetCursorPos(point64 pos)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Utility::GetCursorPosInWindow(point64& pos)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Utility::GetLogicalDpi()
        {
            return (g_window)? g_window->logicalDpiX() : 96;
        }

        sint32 Platform::Utility::GetPhysicalDpi()
        {
            return (g_window)? g_window->physicalDpiX() : 92;
        }

        float Platform::Utility::GetReversedGuiRatio()
        {
            return (g_window)? (96.0 / g_window->logicalDpiX()) * (g_window->physicalDpiX() / 92.0) : 1;
        }

        chars Platform::Utility::GetOSName()
        {
            return PlatformImpl::Wrap::Utility_GetOSName();
        }

        sint32 Platform::Utility::EnumPrograms(Context& programs, bool visible_only)
        {
            return PlatformImpl::Wrap::Utility_EnumPrograms(programs, visible_only);
        }

        chars Platform::Utility::GetDeviceID()
        {
            return PlatformImpl::Wrap::Utility_GetDeviceID();
        }

        sint64 Platform::Utility::GetProcessID()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        chars Platform::Utility::GetLocaleBCP47()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return "ko";
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
            return EpochToWindow(QDateTime::currentMSecsSinceEpoch());
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

        bool Platform::Utility::IsWordableKey(sint32 virtualkey)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
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
            sint32* sec, sint32* min, sint32* hour, sint32* day, sint32* month, sint32* year, sint32* weekday)
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
        void Platform::Graphics::SetScissor(double x, double y, double w, double h)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetColor(uint08 r, uint08 g, uint08 b, uint08 a)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetMask(MaskRole role)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetShader(ShaderRole role)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetFont(chars name, float size)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetFontForFreeType(chars nickname, sint32 height)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetZoom(double zoom, OrientationRole orientation)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::EraseRect(float x, float y, float w, float h)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::FillRect(float x, float y, float w, float h)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::FillRectToFBO(float x, float y, float w, float h, Color color, uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::FillCircle(float x, float y, float w, float h)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::FillPolygon(float x, float y, Points p)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawRect(float x, float y, float w, float h, float thick)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawLine(const Point& begin, const Point& end, float thick)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawCircle(float x, float y, float w, float h, float thick)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawBezier(const Vector& begin, const Vector& end, float thick)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawPolyLine(float x, float y, Points p, float thick, bool ring)
        {
            const sint32 Count = p.Count();
            if(Count < 2) return;

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawPolyBezier(float x, float y, Points p, float thick, bool showfirst, bool showlast)
        {
            const sint32 Count = p.Count();
            if(Count < 4) return;

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawRingBezier(float x, float y, Points p, float thick, float curve)
        {
            const sint32 Count = p.Count();
            if(Count < 3) return;

            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawTextureToFBO(id_texture_read texture, float tx, float ty, float tw, float th,
            orientationtype ori, bool antialiasing, float x, float y, float w, float h, Color color, uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_image Platform::Graphics::CreateImage(id_bitmap_read bitmap, bool mirrored)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        sint32 Platform::Graphics::GetImageWidth(id_image_read image)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetImageHeight(id_image_read image)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::RemoveImage(id_image image)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_image_routine Platform::Graphics::CreateImageRoutine(id_bitmap_read bitmap, sint32 resizing_width, sint32 resizing_height, const Color coloring)
        {
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
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawPolyImageToFBO(id_image_read image, const Point (&ips)[3], float x, float y, const Point (&ps)[3], const Color (&colors)[3], uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawRotatedImageToFBO(id_image_read image, float ox, float oy, float degree, float x, float y, Color color, uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Graphics::DrawString(float x, float y, float w, float h, chars string, sint32 count, UIFontAlign align, UIFontElide elide)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Graphics::DrawStringW(float x, float y, float w, float h, wchars string, sint32 count, UIFontAlign align, UIFontElide elide)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Graphics::GetLengthOfString(bool byword, sint32 clipping_width, chars string, sint32 count)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetLengthOfStringW(bool byword, sint32 clipping_width, wchars string, sint32 count)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringWidth(chars string, sint32 count)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringWidthW(wchars string, sint32 count)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetFreeTypeStringWidth(chars nickname, sint32 height, chars string, sint32 count)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringHeight()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringAscent()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        bool Platform::Graphics::BeginGL()
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Graphics::EndGL()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_texture Platform::Graphics::CreateTexture(bool nv21, bool bitmapcache, sint32 width, sint32 height, const void* bits)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_texture Platform::Graphics::CloneTexture(id_texture texture)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::Graphics::IsTextureNV21(id_texture_read texture)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        uint32 Platform::Graphics::GetTextureID(id_texture_read texture, sint32 i)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetTextureWidth(id_texture_read texture)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetTextureHeight(id_texture_read texture)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::RemoveTexture(id_texture texture)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_bitmap Platform::Graphics::CreateBitmapFromTextureGL(id_texture_read texture)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap Platform::Graphics::CreateBitmapFromTextureFast(id_texture texture)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_surface Platform::Graphics::CreateSurface(sint32 width, sint32 height)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        uint32 Platform::Graphics::GetSurfaceFBO(id_surface_read surface)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetSurfaceWidth(id_surface_read surface)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetSurfaceHeight(id_surface_read surface)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::RemoveSurface(id_surface surface)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::BindSurface(id_surface surface)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::UnbindSurface(id_surface surface)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawSurface(id_surface_read surface, float sx, float sy, float sw, float sh, float x, float y, float w, float h)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawSurfaceToFBO(id_surface_read surface, float sx, float sy, float sw, float sh,
            orientationtype ori, bool antialiasing, float x, float y, float w, float h, Color color, uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_image_read Platform::Graphics::GetImageFromSurface(id_surface_read surface)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Graphics::GetBitmapFromSurface(id_surface_read surface, orientationtype ori)
        {
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
            const String FilenameUTF8 = PlatformImpl::Core::NormalPath(filename);
            QFileInfo CurInfo(QString::fromUtf8(FilenameUTF8, -1));
            return CurInfo.exists() && CurInfo.isFile();
        }

        bool Platform::File::ExistForDir(chars dirname)
        {
            const String DirnameUTF8 = PlatformImpl::Core::NormalPath(dirname);
            QFileInfo CurInfo(QString::fromUtf8(DirnameUTF8, -1));
            return CurInfo.exists() && CurInfo.isDir();
        }

        id_file_read Platform::File::OpenForRead(chars filename)
        {
            const String FilenameUTF8 = PlatformImpl::Core::NormalPath(filename);
            QFile* NewFile = new QFile(QString::fromUtf8(FilenameUTF8, -1));
            if(!NewFile->open(QFileDevice::ReadOnly))
            {
                BOSS_TRACE("OpenForRead(%s) - The file is nonexistent", filename);
                delete NewFile;
                return nullptr;
            }
            BOSS_TRACE("OpenForRead(%s)", filename);
            return (id_file_read) NewFile;
        }

        static void _CreateMiddleDir(chars itemname)
        {
            String Dirname;
            for(chars iChar = itemname; *iChar; ++iChar)
            {
                const char OneChar = *iChar;
                if(OneChar == '/' || OneChar == '\\')
                {
                    if(QDir().mkdir(QString::fromUtf8(Dirname, -1)))
                        BOSS_TRACE("_CreateMiddleDir(%s)", (chars) Dirname);
                    Dirname += '/';
                }
                else Dirname += OneChar;
            }
        }

        id_file Platform::File::OpenForWrite(chars filename, bool autocreatedir)
        {
            const String FilenameUTF8 = PlatformImpl::Core::NormalPath(filename);
            QFile* NewFile = new QFile(QString::fromUtf8(FilenameUTF8, -1));
            if(!NewFile->open(QFileDevice::WriteOnly))
            {
                delete NewFile;
                if(autocreatedir)
                {
                    _CreateMiddleDir(FilenameUTF8);
                    return OpenForWrite(filename, false);
                }
                else
                {
                    BOSS_TRACE("OpenForWrite(%s) - The folder is nonexistent", filename);
                    return nullptr;
                }
            }
            BOSS_TRACE("OpenForWrite(%s)", filename);
            return (id_file) NewFile;
        }

        void Platform::File::Close(id_file_read file)
        {
            delete (QFile*) file;
        }

        const sint32 Platform::File::Size(id_file_read file)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            return ((QFile*) file)->size();
        }

        const sint32 Platform::File::Read(id_file_read file, uint08* data, const sint32 size)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            return ((QFile*) file)->read((char*) data, size);
        }

        const sint32 Platform::File::ReadLine(id_file_read file, char* text, const sint32 size)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            return ((QFile*) file)->readLine(text, size);
        }

        const sint32 Platform::File::Write(id_file file, bytes data, const sint32 size)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            return ((QFile*) file)->write((chars) data, size);
        }

        void Platform::File::Seek(id_file_read file, const sint32 focus)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            ((QFile*) file)->seek(focus);
        }

        const sint32 Platform::File::Focus(id_file_read file)
        {
            BOSS_ASSERT("해당 파일이 없습니다", file);
            return (sint32) ((QFile*) file)->pos();
        }

        sint32 Platform::File::Search(chars dirname, SearchCB cb, payload data, bool needfullpath)
        {
            String ParsedDirname = dirname;
            if(!ParsedDirname.Right(2).Compare("/*") || !ParsedDirname.Right(2).Compare("\\*"))
                ParsedDirname.SubTail(2);

            const String PathUTF8 = PlatformImpl::Core::NormalPath(ParsedDirname);
            QString PathQ = QString::fromUtf8(PathUTF8, -1);
            sint32 FindPos = PathQ.lastIndexOf("assets:");
            if(0 <= FindPos) PathQ = PathQ.mid(FindPos);

            QDir TargetDir;
            bool Exists = false;
            if(FindPos < 0)
            {
                TargetDir = PathQ;
                Exists = TargetDir.exists();
            }
            else
            {
                TargetDir = QString((chars) RootForAssets().Left(RootForAssets().Length() - 1)) + PathQ.mid(7);
                Exists = TargetDir.exists();
                if(!Exists)
                {
                    TargetDir = QString((chars) RootForAssetsRem().Left(RootForAssets().Length() - 1)) + PathQ.mid(7);
                    Exists = TargetDir.exists();
                }
            }
            if(!Exists)
            {
                BOSS_TRACE("Search(%s) - The TargetDir is nonexistent", (chars) PathUTF8);
                return -1;
            }

            const QStringList& List = TargetDir.entryList(QDir::Files | QDir::Dirs | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
            BOSS_TRACE("Search(%s) - %d", (chars) PathUTF8, List.size());

            if(cb) for(sint32 i = 0, iend = List.size(); i < iend; ++i)
            {
                if(needfullpath)
                    cb(PlatformImpl::Core::NormalPath(
                        String::Format("%s/%s", (chars) ParsedDirname, List.at(i).toUtf8().constData()), false), data);
                else cb(List.at(i).toUtf8().constData(), data);
                BOSS_TRACE("Search() - Result: %s", List.at(i).toUtf8().constData());
            }
            return List.size();
        }

        sint32 Platform::File::GetAttributes(wchars itemname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            QFileInfo CurInfo(QString::fromUtf8(ItemnameUTF8, -1));
            if(!CurInfo.exists()) return -1; // INVALID_FILE_ATTRIBUTES

            sint32 Result = 0;
            if(!CurInfo.isWritable()) Result |= 0x1; // FILE_ATTRIBUTE_READONLY
            if(CurInfo.isHidden()) Result |= 0x2; // FILE_ATTRIBUTE_HIDDEN
            if(CurInfo.isDir()) Result |= 0x10; // FILE_ATTRIBUTE_DIRECTORY
            if(CurInfo.isSymLink()) Result |= 0x400; // FILE_ATTRIBUTE_REPARSE_POINT

            if(size) *size = (uint64) CurInfo.size();
            if(ctime) *ctime = 10 * 1000 * EpochToWindow(CurInfo.birthTime().toMSecsSinceEpoch());
            if(atime) *atime = 10 * 1000 * EpochToWindow(CurInfo.lastRead().toMSecsSinceEpoch());
            if(mtime) *mtime = 10 * 1000 * EpochToWindow(CurInfo.lastModified().toMSecsSinceEpoch());
            return Result;
        }

        void Platform::File::SetAttributeHidden(wchars itemname)
        {
            PlatformImpl::Wrap::File_SetAttributeHidden(itemname);
        }

        WString Platform::File::GetFullPath(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname, false));

            if((('A' <= ItemnameUTF8[0] && ItemnameUTF8[0] <= 'Z') ||
                ('a' <= ItemnameUTF8[0] && ItemnameUTF8[0] <= 'z')) && ItemnameUTF8[1] == ':')
                return WString::FromChars(ItemnameUTF8);

            QFileInfo CurInfo(QString::fromUtf8(BOSS::Platform::File::RootForAssetsRem() + ItemnameUTF8, -1));
            String AbsoluteName = CurInfo.absoluteFilePath().toUtf8().constData();
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
            QFileInfo CurInfo(QString::fromUtf8(ItemnameUTF8, -1));

            const bool Result = CurInfo.exists();
            BOSS_TRACE("CanAccess(%s)%s", (chars) ItemnameUTF8, Result? "" : " - Failed");
            return Result;
        }

        bool Platform::File::CanWritable(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));
            QFileInfo CurInfo(QString::fromUtf8(ItemnameUTF8, -1));

            const bool Result = (CurInfo.exists() && CurInfo.isWritable());
            BOSS_TRACE("CanWritable(%s)%s", (chars) ItemnameUTF8, Result? "" : " - Failed");
            return Result;
        }

        static void _RemoveMiddleDir(chars itemname)
        {
            String Itemname, LastDirname;
            for(chars iChar = itemname; *iChar; ++iChar)
            {
                const char OneChar = *iChar;
                if(OneChar == '/' || OneChar == '\\')
                {
                    LastDirname = Itemname;
                    Itemname += '/';
                }
                else Itemname += OneChar;
            }
            if(0 < LastDirname.Length())
            {
                const bool Result = QDir().rmdir(QString::fromUtf8(LastDirname, -1));
                BOSS_TRACE("_RemoveMiddleDir(%s)%s", (chars) LastDirname, Result? "" : " - Failed");
                if(Result) _RemoveMiddleDir(LastDirname);
            }
        }

        bool Platform::File::Remove(wchars itemname, bool autoremovedir)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            const bool Result = QFile::remove(QString::fromUtf8(ItemnameUTF8, -1));
            if(Result && autoremovedir)
                _RemoveMiddleDir(ItemnameUTF8);
            BOSS_TRACE("Remove(%s)%s", (chars) ItemnameUTF8, Result? "" : " - Failed");
            return Result;
        }

        bool Platform::File::Rename(wchars existing_itemname, wchars new_itemname)
        {
            const String ExistingItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(existing_itemname));
            const String NewItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(new_itemname));

            const bool Result = QFile::rename(QString::fromUtf8(ExistingItemnameUTF8, -1), QString::fromUtf8(NewItemnameUTF8, -1));
            BOSS_TRACE("Rename(%s -> %s)%s", (chars) ExistingItemnameUTF8, (chars) NewItemnameUTF8, Result? "" : " - Failed");
            return Result;
        }

        bool Platform::File::Tempname(char* format, sint32 length)
        {
            return PlatformImpl::Wrap::File_Tempname(format, length);
        }

        bool Platform::File::CreateDir(wchars dirname, bool autocreatedir)
        {
            const String DirnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(dirname));

            if(QDir(QString::fromUtf8(DirnameUTF8, -1)).exists())
            {
                BOSS_TRACE("CreateDir(%s) - Skipped", (chars) DirnameUTF8);
                return true;
            }

            if(!QDir().mkdir(QString::fromUtf8(DirnameUTF8, -1)))
            {
                if(autocreatedir)
                {
                    _CreateMiddleDir(DirnameUTF8);
                    return CreateDir(dirname, false);
                }
                else
                {
                    BOSS_TRACE("CreateDir(%s) - Failed", (chars) DirnameUTF8);
                    return false;
                }
            }
            BOSS_TRACE("CreateDir(%s)", (chars) DirnameUTF8);
            return true;
        }

        bool Platform::File::RemoveDir(wchars dirname, bool autoremovedir)
        {
            const String DirnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(dirname));

            if(!QDir(QString::fromUtf8(DirnameUTF8, -1)).exists())
            {
                if(autoremovedir)
                    _RemoveMiddleDir(DirnameUTF8);
                BOSS_TRACE("RemoveDir(%s) - Skipped", (chars) DirnameUTF8);
                return true;
            }

            const bool Result = QDir().rmdir(QString::fromUtf8(DirnameUTF8, -1));
            if(Result && autoremovedir)
                _RemoveMiddleDir(DirnameUTF8);
            BOSS_TRACE("RemoveDir(%s)%s", (chars) DirnameUTF8, Result? "" : " - Failed");
            return Result;
        }

        extern chars _private_GetFileName(boss_file file);
        extern sint64 _private_GetFileOffset(boss_file file);
        extern void _private_SetFileRetain(boss_file file);

        class FDFile
        {
            BOSS_DECLARE_NONCOPYABLE_CLASS(FDFile)
        public:
            FDFile()
            {
                mID = -1;
                mFile = nullptr;
            }
            ~FDFile()
            {
                boss_fclose(mFile);
            }
        public:
            void SetID(sint32 id)
            {
                mID = id;
            }
            sint32 SetFile(boss_file file, bool retain)
            {
                mFile = file;
                if(retain)
                    _private_SetFileRetain(mFile);
                return mID;
            }
            boss_file GetRetainedFile()
            {
                _private_SetFileRetain(mFile);
                return mFile;
            }
        public:
            inline boss_file file() {return mFile;}
        private:
            sint32 mID;
            boss_file mFile;
        };

        class FDFilePool
        {
        public:
            FDFilePool() {mMutex = Mutex::Open();}
            ~FDFilePool() {Mutex::Close(mMutex);}
        private:
            static sint32 MakeID() {static sint32 _ = -1; return ++_;}
        public:
            FDFile& New()
            {
                Mutex::Lock(mMutex);
                const sint32 NewID = MakeID();
                FDFile& NewFile = mMap[NewID];
                NewFile.SetID(NewID);
                Mutex::Unlock(mMutex);
                return NewFile;
            }
            FDFile* Get(sint32 fd)
            {
                Mutex::Lock(mMutex);
                FDFile* CurFile = mMap.Access(fd);
                Mutex::Unlock(mMutex);
                return CurFile;
            }
            void Delete(sint32 fd)
            {
                Mutex::Lock(mMutex);
                mMap.Remove(fd);
                Mutex::Unlock(mMutex);
            }
        private:
            id_mutex mMutex;
            Map<FDFile> mMap;
        } gFilePool;

        sint32 Platform::File::FDOpen(wchars filename, bool writable, bool append, bool exclusive, bool truncate)
        {
            const String FileName = String::FromWChars(filename);
            if(exclusive && File::Exist(FileName))
                return -1;

            chars Mode = nullptr;
            if(writable)
            {
                if(append && !truncate)
                    Mode = "ab";
                else if(!truncate)
                    Mode = "r+b";
                else Mode = "wb";
            }
            else
            {
                if(truncate)
                    Mode = "w+b";
                else Mode = "rb";
            }

            FDFile& NewFile = gFilePool.New();
            return NewFile.SetFile(boss_fopen(FileName, Mode), false);
        }

        bool Platform::File::FDClose(sint32 fd)
        {
            FDFile* OldFile = gFilePool.Get(fd);
            if(OldFile)
            {
                gFilePool.Delete(fd);
                return true;
            }
            return false;
        }

        sint32 Platform::File::FDFromFile(boss_file file)
        {
            FDFile& NewFile = gFilePool.New();
            return NewFile.SetFile(file, true);
        }

        boss_file Platform::File::FDToFile(sint32 fd)
        {
            FDFile* CurFile = gFilePool.Get(fd);
            if(CurFile)
                return CurFile->GetRetainedFile();
            return nullptr;
        }

        sint64 Platform::File::FDRead(sint32 fd, void* data, sint64 size)
        {
            FDFile* CurFile = gFilePool.Get(fd);
            if(CurFile)
                return boss_fread(data, 1, size, CurFile->file());
            return -1;
        }

        sint64 Platform::File::FDWrite(sint32 fd, const void* data, sint64 size)
        {
            FDFile* CurFile = gFilePool.Get(fd);
            if(CurFile)
                return boss_fwrite(data, 1, size, CurFile->file());
            return -1;
        }

        sint64 Platform::File::FDSeek(sint32 fd, sint64 offset, sint32 origin)
        {
            FDFile* CurFile = gFilePool.Get(fd);
            if(CurFile)
            {
                const sint64 Result = boss_fseek(CurFile->file(), offset, origin);
                if(Result == 0)
                    return _private_GetFileOffset(CurFile->file());
            }
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
            FDFile* CurFile = gFilePool.Get(fd);
            if(CurFile)
            {
                QFileInfo CurInfo(_private_GetFileName(CurFile->file()));

                uint32 Result = 0;
                if(!CurInfo.isWritable()) Result |= 0x1; // FILE_ATTRIBUTE_READONLY
                if(CurInfo.isHidden()) Result |= 0x2; // FILE_ATTRIBUTE_HIDDEN
                if(CurInfo.isDir()) Result |= 0x10; // FILE_ATTRIBUTE_DIRECTORY
                if(CurInfo.isSymLink()) Result |= 0x400; // FILE_ATTRIBUTE_REPARSE_POINT

                if(size) *size = (uint64) CurInfo.size();
                if(ctime) *ctime = 10 * 1000 * EpochToWindow(CurInfo.birthTime().toMSecsSinceEpoch());
                if(atime) *atime = 10 * 1000 * EpochToWindow(CurInfo.lastRead().toMSecsSinceEpoch());
                if(mtime) *mtime = 10 * 1000 * EpochToWindow(CurInfo.lastModified().toMSecsSinceEpoch());
                return Result;
            }
            return -1;
        }

        void Platform::File::ResetAssetsRoot(chars dirname)
        {
            PlatformImpl::Core::SetRoot(0, PlatformImpl::Core::NormalPath(dirname, false));
            BOSS_TRACE("Platform::File::ResetAssetsRoot() ==> [%s]", (chars) PlatformImpl::Core::GetCopiedRoot(0));
        }

        void Platform::File::ResetAssetsRemRoot(chars dirname)
        {
            PlatformImpl::Core::SetRoot(1, PlatformImpl::Core::NormalPath(dirname, false));
            BOSS_TRACE("Platform::File::ResetAssetsRemRoot() ==> [%s]", (chars) PlatformImpl::Core::GetCopiedRoot(1));
        }

        const String Platform::File::RootForAssets()
        {
            const String Result = PlatformImpl::Core::GetCopiedRoot(0);
            if(0 < Result.Length())
                return Result;

            #if BOSS_WINDOWS
                String NewPath = String::Format("%s/..", QCoreApplication::applicationDirPath().toUtf8().constData());
                if(!Platform::File::ExistForDir(NewPath + "/assets"))
                {
                    // Qt크레이터에서 바로 실행
                    const String AssetsPath1 = String::Format("%s/../..", QCoreApplication::applicationDirPath().toUtf8().constData());
                    if(Platform::File::ExistForDir(AssetsPath1 + "/assets"))
                        NewPath = AssetsPath1 + "/assets/";
                    else
                    {
                        // 비주얼스튜디오에서 바로 실행
                        const String AssetsPath2 = String::Format("%s/../../..", QCoreApplication::applicationDirPath().toUtf8().constData());
                        NewPath = AssetsPath2 + "/assets/";
                    }
                }
                else NewPath += "/assets/";
            #elif BOSS_LINUX
                String NewPath = String::Format("Q:%s/../assets/", QCoreApplication::applicationDirPath().toUtf8().constData());
                const String AssetsPath = String::Format("Q:%s/../..", QCoreApplication::applicationDirPath().toUtf8().constData());
                if(Platform::File::ExistForDir(AssetsPath + "/assets"))
                    NewPath = AssetsPath + "/assets/";
            #elif BOSS_MAC_OSX
                String NewPath = String::Format("Q:%s/../../assets/", QCoreApplication::applicationDirPath().toUtf8().constData());
            #elif BOSS_IPHONE
                String NewPath = String::Format("Q:%s/assets/", QCoreApplication::applicationDirPath().toUtf8().constData());
            #elif BOSS_ANDROID
                String NewPath = "assets:/";
            #else
                String NewPath = "../assets/";
            #endif
            
            PlatformImpl::Core::SetRoot(0, PlatformImpl::Core::NormalPath(NewPath, false));
            BOSS_TRACE("Platform::File::RootForAssets() ==> [%s]", (chars) PlatformImpl::Core::GetCopiedRoot(0));
            return PlatformImpl::Core::GetCopiedRoot(0);
        }

        const String Platform::File::RootForAssetsRem()
        {
            const String Result = PlatformImpl::Core::GetCopiedRoot(1);
            if(0 < Result.Length())
                return Result;

            #if BOSS_WINDOWS
                String NewPath = String::Format("%s/..", QCoreApplication::applicationDirPath().toUtf8().constData());
                if(!Platform::File::ExistForDir(NewPath + "/assets"))
                {
                    // Qt크레이터에서 바로 실행
                    const String AssetsPath1 = String::Format("%s/../..", QCoreApplication::applicationDirPath().toUtf8().constData());
                    if(Platform::File::ExistForDir(AssetsPath1 + "/assets"))
                        NewPath = AssetsPath1;
                    else
                    {
                        // 비주얼스튜디오에서 바로 실행
                        const String AssetsPath2 = String::Format("%s/../../..", QCoreApplication::applicationDirPath().toUtf8().constData());
                        if(Platform::File::ExistForDir(AssetsPath2 + "/assets"))
                            NewPath = AssetsPath2;
                    }
                }
            #elif BOSS_LINUX
                String NewPath = String::Format("Q:%s/..", QCoreApplication::applicationDirPath().toUtf8().constData());
                const String AssetsPath = String::Format("Q:%s/../..", QCoreApplication::applicationDirPath().toUtf8().constData());
                if(Platform::File::ExistForDir(AssetsPath + "/assets"))
                    NewPath = AssetsPath;
            #elif BOSS_MAC_OSX
                String NewPath = String::Format("Q:%s/../../../..", QCoreApplication::applicationDirPath().toUtf8().constData());
            #else
                String NewPath = String::Format("Q:%s", QStandardPaths::standardLocations(QStandardPaths::DataLocation).value(0).toUtf8().constData());
            #endif
            NewPath += "/assets-rem/";

            PlatformImpl::Core::SetRoot(1, PlatformImpl::Core::NormalPath(NewPath, false));
            _CreateMiddleDir(PlatformImpl::Core::NormalPath(PlatformImpl::Core::GetCopiedRoot(1)));
            BOSS_TRACE("Platform::File::RootForAssetsRem() ==> [%s]", (chars) PlatformImpl::Core::GetCopiedRoot(1));
            return PlatformImpl::Core::GetCopiedRoot(1);
        }

        const String Platform::File::RootForData()
        {
            const String Result = PlatformImpl::Core::GetCopiedRoot(2);
            if(0 < Result.Length())
                return Result;

            String NewPath;
            #if BOSS_LINUX | BOSS_MAC_OSX | BOSS_IPHONE
                NewPath = String::Format("Q:%s", QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).value(0).toUtf8().constData());
            #elif BOSS_ANDROID
                String StoragePathes = getenv("SECONDARY_STORAGE");
                StoragePathes.Replace(':', '\0');
                if(File::ExistForDir((chars) StoragePathes))
                    NewPath = "Q:" + StoragePathes;
                else NewPath = String::Format("Q:%s", QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).value(0).toUtf8().constData());
            #else
                NewPath = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).value(0).toUtf8().constData();
            #endif
            NewPath += '/';

            PlatformImpl::Core::SetRoot(2, PlatformImpl::Core::NormalPath(NewPath, false));
            _CreateMiddleDir(PlatformImpl::Core::NormalPath(PlatformImpl::Core::GetCopiedRoot(2)));
            BOSS_TRACE("Platform::File::RootForData() ==> [%s]", (chars) PlatformImpl::Core::GetCopiedRoot(2));
            return PlatformImpl::Core::GetCopiedRoot(2);
        }

        const String Platform::File::RootForDesktop()
        {
            #if BOSS_LINUX | BOSS_MAC_OSX
                String NewPath = String::Format("Q:%s", QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).value(0).toUtf8().constData());
            #elif BOSS_IPHONE
                String NewPath = String::Format("Q:%s", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toUtf8().constData());
            #else
                String NewPath = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).value(0).toUtf8().constData();
            #endif

            NewPath = PlatformImpl::Core::NormalPath(NewPath + '/', false);
            _CreateMiddleDir(NewPath);
            BOSS_TRACE("Platform::File::RootForDesktop() ==> [%s]", (chars) NewPath);
            return NewPath;
        }

        const String Platform::File::RootForStartup()
        {
            String NewPath = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).value(0).toUtf8().constData();
            NewPath = PlatformImpl::Core::NormalPath(NewPath + '/', false);
            _CreateMiddleDir(NewPath);
            return NewPath;
        }

        const String Platform::File::RootForDocuments()
        {
            String NewPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).value(0).toUtf8().constData();
            NewPath = PlatformImpl::Core::NormalPath(NewPath + '/', false);
            _CreateMiddleDir(NewPath);
            return NewPath;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SOUND
        ////////////////////////////////////////////////////////////////////////////////
        id_sound Platform::Sound::OpenForFile(chars filename, bool loop)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_sound Platform::Sound::OpenForUrl(chars url, bool loop)
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

        void Platform::Sound::SetVolume(id_sound sound, float volume, sint32 apply_msec)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Sound::ApplyVolumeOnce(id_sound sound)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Sound::Play(id_sound sound)
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

        id_socket Platform::Socket::OpenForWS(bool use_wss)
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

        void Platform::Socket::ConnectAsync(id_socket socket, chars domain, uint16 port)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        bool Platform::Socket::IsConnected(id_socket socket)
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

        id_server Platform::Server::CreateWS(chars name, bool use_wss)
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
        id_pipe Platform::Pipe::Open(chars name, EventCB cb, payload data)
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

        bool Platform::Bluetooth::Write(id_bluetooth bluetooth, bytes data, const sint32 size)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Bluetooth::Content::MusicPlayWithSelector()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Bluetooth::Content::MusicStop()
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Bluetooth::Content::SetVolume(float value)
        {
            BOSS_ASSERT("Further development is needed.", false);
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

        id_microphone Platform::Microphone::Open(chars name, sint32 samplerate, sint32 channelcount, sint32 maxqueue)
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

        const float* Platform::Microphone::GetSoundData(id_microphone microphone, sint32* count, sint32* channel, uint64* timems)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return nullptr;
        }
    }

#endif
