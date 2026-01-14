#include <boss.hpp>
#include "boss_platform_qt6.hpp"

#ifdef BOSS_PLATFORM_QT6

    #if BOSS_WINDOWS
        #include <windows.h>
    #elif BOSS_WASM
        #include <emscripten.h>
    #else
        #if !BOSS_LINUX && _POSIX_C_SOURCE >= 199309L
            #include <time.h>
        #endif
        #include <unistd.h>
    #endif
    #include <format/boss_bmp.hpp>

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
            static QAndroidJniEnvironment g_environment;
            return g_environment;
        }
        jobject GetAndroidApplicationActivity()
        {
            QAndroidJniObject Activity = QtAndroid::androidActivity();
            return GetAndroidJNIEnv()->NewGlobalRef(Activity.object());
        }
        void SetAndroidApplicationActivity(jobject activity)
        {
        }
        jobject GetAndroidApplicationContext()
        {
            QAndroidJniObject Context = QtAndroid::androidContext();
            return GetAndroidJNIEnv()->NewGlobalRef(Context.object());
        }
        void SetAndroidApplicationContext(jobject context)
        {
        }
    #elif BOSS_IPHONE
        void* GetIOSApplicationUIView()
        {
            return QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", QGuiApplication::focusWindow());
        }
    #elif BOSS_WASM
        // Ready
        static bool g_WasmReady = false;
        extern "C" void EMSCRIPTEN_KEEPALIVE OnWasmReady()
        {
            g_WasmReady = true;
        }
        void WaitForWasmReady()
        {
            //while(!g_WasmReady)
            //    emscripten_sleep(10);
        }
        // Flush
        static bool g_WasmFlush = false;
        extern "C" void EMSCRIPTEN_KEEPALIVE OnWasmFlush()
        {
            g_WasmFlush = true;
        }
        void WaitForWasmFlush()
        {
            g_WasmFlush = false;
            EM_ASM
            (
                FS.syncfs(false, function(err)
                    {
                        if(!err)
                            Module._OnWasmFlush();
                    });
            );
            //while(!g_WasmFlush)
            //    emscripten_sleep(10);
        }
    #endif

    h_view g_view;
    MainWindow* g_window = nullptr;
    sint32 g_argc = 0;
    char** g_argv = nullptr;
    static bool g_canPopupAssert = false;

	#if BOSS_NEED_MAIN
        extern bool PlatformInit();
        extern void PlatformQuit();
        extern void PlatformFree();

        int main(int argc, char* argv[])
        {
            //#ifdef __EMSCRIPTEN__
            //    emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
            //#endif
            //#if BOSS_WASM
            //    EM_ASM
            //    (
            //        FS.mkdir('/boss2d');
            //        FS.mount(IDBFS, {}, '/boss2d');
            //        FS.syncfs(true, function(err)
            //            {
            //                if(!err)
            //                    Module._OnWasmReady();
            //            });
            //    );
            //#endif

            g_canPopupAssert = true;
            int result = 0;
            {
                QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
                QApplication app(argc, argv);

                MainWindow mainWindow(&app);
                g_window = &mainWindow;
                g_argc = argc;
                g_argv = argv;

                if(PlatformInit())
                {
                    mainWindow.SetInitedPlatform();
                    if(mainWindow.FirstVisible())
                    {
                        #if BOSS_NEED_FULLSCREEN
                            mainWindow.showFullScreen();
                        #else
                            mainWindow.show();
                        #endif
                    }

                    #if BOSS_WASM
                        result = app.exec();
                    #else
                        try {result = app.exec();}
                        catch(QException& e)
                        {BOSS_ASSERT(String::Format("QException: %s", e.what()), false);}
                    #endif
                    PlatformQuit();
                }
                g_window = nullptr;
                Platform::Popup::HideSplash();
            }
            g_canPopupAssert = false;

            PlatformFree();
            // 스토리지(TLS) 영구제거
            Storage::ClearAll(SCL_SystemAndUser);
            return result;
        }
    #endif

    #if BOSS_ANDROID
        extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt, ...);
    #endif

    extern "C" int boss_platform_assert(BOSS_DBG_PRM const char* name, const char* query)
    {
        char AssertInfo[4][1024];
        boss_snprintf(AssertInfo[0], 1024, "[QUERY] %s", query);
        boss_snprintf(AssertInfo[1], 1024, "[METHOD] %s()", BOSS_DBG_FUNC);
        boss_snprintf(AssertInfo[2], 1024, "[FILE/LINE] %s(%dLn)", BOSS_DBG_FILE, BOSS_DBG_LINE);
        boss_snprintf(AssertInfo[3], 1024, "[THREAD_ID] %u", QThread::currentThreadId());
        #if BOSS_ANDROID
            __android_log_print(7, "*******", "\n************************************************************");
            __android_log_print(7, "*******", name);
            __android_log_print(7, "*******", "------------------------------------------------------------");
            __android_log_print(7, "*******", AssertInfo[0]);
            __android_log_print(7, "*******", AssertInfo[1]);
            __android_log_print(7, "*******", AssertInfo[2]);
            __android_log_print(7, "*******", AssertInfo[3]);
            __android_log_print(7, "*******", "************************************************************");
        #else
            qDebug().noquote() << "\n************************************************************";
            qDebug().noquote() << name;
            qDebug().noquote() << "------------------------------------------------------------";
            qDebug().noquote() << AssertInfo[0];
            qDebug().noquote() << AssertInfo[1];
            qDebug().noquote() << AssertInfo[2];
            qDebug().noquote() << AssertInfo[3];
            qDebug().noquote() << "************************************************************";
        #endif

        static bool IsRunning = false;
        if(!IsRunning && g_canPopupAssert)
        {
            IsRunning = true;
            #if BOSS_WINDOWS
                WString AssertMessage = WString::Format(
                    L"%s\n\n%s\t\t\n%s\t\t\n%s\t\t\n%s\t\t\n"
                    L"(YES is Break, NO is Ignore)\t\t",
                    (wchars) WString::FromChars(name),
                    (wchars) WString::FromChars(AssertInfo[0]),
                    (wchars) WString::FromChars(AssertInfo[1]),
                    (wchars) WString::FromChars(AssertInfo[2]),
                    (wchars) WString::FromChars(AssertInfo[3]));
                switch(MessageBoxW(NULL, AssertMessage, L"ASSERT BREAK", MB_ICONWARNING | MB_ABORTRETRYIGNORE))
                {
                case IDABORT: IsRunning = false; return 0;
                case IDIGNORE: IsRunning = false; return 1;
                }
            #elif BOSS_LINUX
                String AssertMessage = String::Format(
                    "%s\n\n%s\t\t\n%s\t\t\n%s\t\t\n%s\t\t\n"
                    "(Ok: Break, Cancel: Ignore, ⓧ: Ignore all)\t\t", name,
                    (chars) AssertInfo[0],
                    (chars) AssertInfo[1],
                    (chars) AssertInfo[2],
                    (chars) AssertInfo[3]);
                switch(Platform::Popup::MessageDialog("ASSERT BREAK", AssertMessage, DBT_OkCancelIgnore))
                {
                case 0: IsRunning = false; return 0; // IDOK
                case 1: IsRunning = false; return 2; // IDCANCEL
                case 2: IsRunning = false; return 1; // IDIGNORE
                }
            #else
                char AssertMessage[4096];
                boss_snprintf(AssertMessage, 4096,
                    "%s\t\t\n%s\t\t\n%s\t\t\n%s\t\t\n"
                    "(YES is Break, NO is Ignore)\t\t",
                    AssertInfo[0], AssertInfo[1],
                    AssertInfo[2], AssertInfo[3]);
                QMessageBox AssertBox(QMessageBox::Warning, "ASSERT BREAK", QString::fromUtf8(name),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll);
                AssertBox.setInformativeText(AssertMessage);
                AssertBox.setDefaultButton(QMessageBox::Yes);
                const int Result = AssertBox.exec();
                switch(Result)
                {
                case QMessageBox::Yes: IsRunning = false; return 0;
                case QMessageBox::NoToAll: IsRunning = false; return 1;
                }
            #endif
            IsRunning = false;
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
            OutputDebugStringA(TraceMessage);
            OutputDebugStringA("\n");
        #else
            qDebug().noquote() << TraceMessage;
        #endif
    }

    CanvasClass::CanvasClass() : mIsTypeSurface(true)
    {
        mIsSurfaceBinded = false;
        mSavedCanvas = nullptr;
        mSavedZoom.scale = 1;
        mSavedZoom.orientation = OR_Normal;
        mSavedMask = QPainter::CompositionMode_SourceOver;
        mPainterWidth = 0;
        mPainterHeight = 0;
        mShader = SR_Normal;
    }
    CanvasClass::CanvasClass(QPaintDevice* device) : mIsTypeSurface(false)
    {
        mIsSurfaceBinded = false;
        mSavedCanvas = nullptr;
        mSavedZoom.scale = 1;
        mSavedZoom.orientation = OR_Normal;
        mSavedMask = QPainter::CompositionMode_SourceOver;
        mPainterWidth = 0;
        mPainterHeight = 0;
        mShader = SR_Normal;
        BindCore(device);
    }
    CanvasClass::~CanvasClass()
    {
        if(!mIsTypeSurface)
            UnbindCore();
    }
    void CanvasClass::Bind(QPaintDevice* device)
    {
        BOSS_ASSERT("본 함수의 사용은 Surface타입에서만 허용합니다", mIsTypeSurface);
        if(!mIsSurfaceBinded)
        {
            mIsSurfaceBinded = true;
            BindCore(device);
        }
    }
    void CanvasClass::Unbind()
    {
        BOSS_ASSERT("본 함수의 사용은 Surface타입에서만 허용합니다", mIsTypeSurface);
        if(mIsSurfaceBinded)
        {
            mIsSurfaceBinded = false;
            UnbindCore();
        }
    }
    void CanvasClass::BindCore(QPaintDevice* device)
    {
        BOSS_ASSERT("mSavedCanvas는 nullptr이어야 합니다", !mSavedCanvas);
        if((mSavedCanvas = ST()) != ST_FIRST())
        {
            const bool M11Flag = (mSavedCanvas->mPainter.transform().m11() < 0);
            const bool M12Flag = (mSavedCanvas->mPainter.transform().m12() < 0);
            const bool M21Flag = (mSavedCanvas->mPainter.transform().m21() < 0);
            mSavedCanvas->mSavedZoom.scale = mSavedCanvas->mPainter.transform().m11() * ((M11Flag)? -1 : 1);
            if(M12Flag) mSavedCanvas->mSavedZoom.orientation = OR_CW270;
            else if(M11Flag) mSavedCanvas->mSavedZoom.orientation = OR_CW180;
            else if(M21Flag) mSavedCanvas->mSavedZoom.orientation = OR_CW90;
            else mSavedCanvas->mSavedZoom.orientation = OR_Normal;

            mSavedCanvas->mSavedFont = mSavedCanvas->mPainter.font();
            mSavedCanvas->mSavedMask = mSavedCanvas->mPainter.compositionMode();
            mSavedCanvas->mPainter.end();
            mSavedCanvas->mPainterWidth = mPainterWidth;
            mSavedCanvas->mPainterHeight = mPainterHeight;
        }
        mPainter.begin(device);
        mPainter.setRenderHint(QPainter::SmoothPixmapTransform, smooth());
        mPainterWidth = device->width();
        mPainterHeight = device->height();
        ST() = this;
    }
    void CanvasClass::UnbindCore()
    {
        BOSS_ASSERT("CanvasClass는 스택식으로 해제해야 합니다", ST() == this);
        mPainter.end();
        if((ST() = mSavedCanvas) != ST_FIRST())
        {
            mPainterWidth = mSavedCanvas->mPainterWidth;
            mPainterHeight = mSavedCanvas->mPainterHeight;
            mSavedCanvas->mPainter.begin(mSavedCanvas->mPainter.device());
            mSavedCanvas->mPainter.setRenderHint(QPainter::SmoothPixmapTransform, smooth());
            Platform::Graphics::SetZoom(mSavedCanvas->mSavedZoom.scale, mSavedCanvas->mSavedZoom.orientation);
            mSavedCanvas->mPainter.setFont(mSavedCanvas->mSavedFont);
            mSavedCanvas->mPainter.setClipRect(mSavedCanvas->mScissor);
            mSavedCanvas->mPainter.setCompositionMode(mSavedCanvas->mSavedMask);
            mSavedCanvas = nullptr;
        }
    }

    namespace BOSS
    {
        ////////////////////////////////////////////////////////////////////////////////
        // PLATFORM
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::InitForGL(bool frameless, bool topmost, chars bgweb)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::InitForMDI(bool frameless, bool topmost, void* bgwidget, chars bgweb)
        {
            g_window->InitForWidget(frameless, topmost, (QWidget*) bgwidget, bgweb);
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
            const QRect LastGeometry = g_window->GetWindowRect();
            g_window->SetWindowRect(x, y, LastGeometry.width(), LastGeometry.height());
        }

        void Platform::SetWindowSize(sint32 width, sint32 height)
        {
            const QRect LastGeometry = g_window->GetWindowRect();
            g_window->SetWindowRect(LastGeometry.x(), LastGeometry.y(), width, height);
        }

        void Platform::SetWindowRect(sint32 x, sint32 y, sint32 width, sint32 height)
        {
            g_window->SetWindowRect(x, y, width, height);
        }

        rect128 Platform::GetWindowRect(bool normally)
        {
            const QRect LastGeometry = g_window->GetWindowRect();
            return {LastGeometry.x(), LastGeometry.y(),
                (LastGeometry.x() + LastGeometry.width()),
                (LastGeometry.y() + LastGeometry.height())};
        }

        void Platform::SetWindowVisible(bool visible)
        {
            if(!g_window->InitedPlatform())
                g_window->SetFirstVisible(visible);
            else if(visible)
                g_window->show();
            else g_window->hide();
        }

        void Platform::SetWindowRaise()
        {
            g_window->SetGroupGeometry(false, false);
        }

        void Platform::SetWindowTopMost(bool enable)
        {
            g_window->setWindowFlag(Qt::WindowStaysOnTopHint, enable);
            g_window->show();
        }

        sint64 Platform::GetWindowHandle()
        {
            return (sint64) g_window->winId();
        }

        void Platform::SetWindowWebUrl(chars url)
        {
            g_window->SetWindowWebUrl(url);
        }

        void Platform::ReloadWindowWeb()
        {
            g_window->ReloadWindowWeb();
        }

        void Platform::SendWindowWebTouchEvent(TouchType type, sint32 x, sint32 y)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SendWindowWebKeyEvent(sint32 code, chars text, bool pressed)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SendWindowWebPythonStart(chars pid, chars filename, chars args)
        {
            g_window->SendWindowWebPythonStart(pid, filename, args);
        }

        void Platform::SendWindowWebPythonStop(chars pid)
        {
            g_window->SendWindowWebPythonStop(pid);
        }

        void Platform::SendWindowWebPythonStopAll()
        {
            g_window->SendWindowWebPythonStopAll();
        }

        void Platform::SendWindowWebPythonCall(chars pid, chars func, chars args)
        {
            g_window->SendWindowWebPythonCall(pid, func, args);
        }

        void Platform::CallWindowWebJSFunction(chars script, sint32 matchid)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::SetWindowFlash()
        {
            #if BOSS_WINDOWS
                FLASHWINFO FlashInfo;
                FlashInfo.cbSize = sizeof(FlashInfo);
                FlashInfo.hwnd = (HWND) g_window->winId();
                FlashInfo.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
                FlashInfo.uCount = 20;
                FlashInfo.dwTimeout = 400;
                FlashWindowEx(&FlashInfo);
            #endif
        }

        bool Platform::SetWindowMask(id_image_read image)
        {
            bool IsFramelessStyle = !!(g_window->windowFlags() & Qt::FramelessWindowHint);
            BOSS_ASSERT("SetWindowMask는 Frameless스타일에서만 동작합니다", IsFramelessStyle);
            if(IsFramelessStyle)
            {
                if(image) // 윈도우가 보여지기 전에 setMask를 호출하면 그 이후에 계속 프레임이
                    g_window->setMask(((QPixmap*) image)->mask()); // 현저히 떨어지는(33f/s → 10f/s) 이유를 알아낼 것
                else g_window->clearMask();
            }
            return IsFramelessStyle;
        }

        void Platform::SetWindowOpacity(float value)
        {
            g_window->setWindowOpacity(value);
        }

        sint32 Platform::AddProcedure(ProcedureEvent event, ProcedureCB cb, payload data)
        {
            switch(event)
            {
            case PE_1SEC: return PlatformImpl::Wrap::AddProcedure(cb, data, 1000);
            case PE_100MSEC: return PlatformImpl::Wrap::AddProcedure(cb, data, 100);
            case PE_FRAME: return PlatformImpl::Wrap::AddProcedure(cb, data, 1000 / USER_FRAMECOUNT);
            }
            BOSS_ASSERT("알 수 없는 ProcedureEvent입니다", false);
            return -1;
        }

        void Platform::SubProcedure(sint32 id)
        {
            PlatformImpl::Wrap::SubProcedure(id);
        }

        void Platform::SetUserEventListener(chars event, UserEventCB cb, payload data)
        {
            PlatformImpl::Wrap::SetUserEventListener(event, cb, data);
        }

        void Platform::ClearUserEventListener(chars event)
        {
            PlatformImpl::Wrap::ClearUserEventListener(event);
        }

        void Platform::SendUserEvent(chars event, chars args)
        {
            PlatformImpl::Wrap::SendUserEvent(event, args);
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
            id_cloned_share Result = nullptr;
            ((View*) view.get())->SendNotify(NT_Normal, topic, in, (needout)? &Result : nullptr, false);
            return Result;
        }

        void Platform::SendDirectNotify(h_view view, chars topic, id_share in)
        {
            ((View*) view.get())->SendNotify(NT_Normal, topic, in, nullptr, true);
        }

        void Platform::BroadcastNotify(chars topic, id_share in, NotifyType type, chars viewclass, bool directly)
        {
            if(auto Views = View::SearchBegin(viewclass))
            {
                struct Payload
                {
                    chars topic;
                    id_share in;
                    NotifyType type;
                    bool directly;
                } Param = {topic, in, type, directly};

                Views->AccessByCallback([](const MapPath*, h_view* view, payload param)->void
                {
                    if(g_window)
                    {
                        const Payload* Param = (const Payload*) param;
                        ((View*) view->get())->SendNotify(Param->type, Param->topic, Param->in, nullptr, Param->directly);
                    }
                }, &Param);
                View::SearchEnd();
            }
        }

        void Platform::SendKeyEvent(h_view view, sint32 code, chars text, bool pressed)
        {
            BOSS_TRACE("SendKeyEvent(%d, %s)", code, (pressed)? "pressed" : "released");
            ((View*) view.get())->OnKey(code, text, pressed);
        }

        void Platform::PassAllViews(PassCB cb, payload data)
        {
            BOSS_ASSERT("콜백함수가 nullptr입니다", cb);
            if(auto Views = View::SearchBegin(nullptr))
            {
                struct Payload {PassCB cb; payload data; bool canceled;} Param = {cb, data, false};
                Views->AccessByCallback([](const MapPath*, h_view* view, payload param)->void
                {
                    Payload* Param = (Payload*) param;
                    if(Param->canceled) return;
                    Param->canceled = Param->cb(view->get(), Param->data);
                }, &Param);
                View::SearchEnd();
            }
        }

        void Platform::UpdateAllViews()
        {
            if(auto Views = View::SearchBegin(nullptr))
            {
                Views->AccessByCallback([](const MapPath*, h_view* view, payload param)->void
                {
                    ((View*) view->get())->DirtyAllSurfaces();
                    g_window->View()->update();
                }, nullptr);
                View::SearchEnd();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // POPUP
        ////////////////////////////////////////////////////////////////////////////////
        bool Platform::Popup::TextDialog(String& text, chars title, chars topic, bool ispassword)
        {
            bool IsOk = false;
            QString NewText = QInputDialog::getText(g_window,
                QString::fromUtf8(title, -1),
                QString::fromUtf8(topic, -1),
                (ispassword)? QLineEdit::Password : QLineEdit::Normal,
                QString::fromUtf8(text, -1), &IsOk);
            if(IsOk) text = NewText.toUtf8().constData();
            return IsOk;
        }

        bool Platform::Popup::ColorDialog(uint08& r, uint08& g, uint08& b, uint08& a, chars title)
        {
            QColor NewColor = QColorDialog::getColor(QColor(r, g, b, a), g_window,
                QString::fromUtf8(title, -1), QColorDialog::ShowAlphaChannel);
            const bool IsOk = NewColor.isValid();
            if(IsOk)
            {
                r = NewColor.red();
                g = NewColor.green();
                b = NewColor.blue();
                a = NewColor.alpha();
            }
            return IsOk;
        }

        bool Platform::Popup::FileDialog(DialogShellType type, String& path, String* shortpath, chars title, wchars filters, sint32* filterresult)
        {
            return PlatformImpl::Wrap::Popup_FileDialog(type, path, shortpath, title, filters, filterresult);
        }

        void Platform::Popup::FileContentDialog(wchars filters)
        {
            #if BOSS_WASM // getOpenFileContent의 호출은 일단 Windows에서 크래시발생하여 WASM에서만 사용키로 함
                const QString NameFilters = QString::fromUtf16((const char16_t*) filters);
                QFileDialog::getOpenFileContent(NameFilters,
                    [](const QString& filename, const QByteArray& filebuffer)->void
                    {
                        uint08s NewContent;
                        Memory::Copy(NewContent.AtDumpingAdded(filebuffer.size()),
                            filebuffer.constData(), filebuffer.size());
                        // 결과전달
                        Platform::BroadcastNotify(filename.toUtf8().constData(), NewContent, NT_FileContent);
                    });
            #else
                String Path;
                String ShortPath;
                if(Platform::Popup::FileDialog(DST_FileOpen, Path, &ShortPath, "Open", filters))
                {
                    if(auto CurFile = Platform::File::OpenForRead(Path))
                    {
                        auto FileSize = Platform::File::Size(CurFile);
                        uint08s NewContent;
                        Platform::File::Read(CurFile, NewContent.AtDumpingAdded(FileSize), FileSize);
                        Platform::File::Close(CurFile);
                        // 결과전달
                        Platform::BroadcastNotify(ShortPath, NewContent, NT_FileContent);
                    }
                }
            #endif
        }

        sint32 Platform::Popup::MessageDialog(chars title, chars text, DialogButtonType type)
        {
            return PlatformImpl::Wrap::Popup_MessageDialog(title, text, type);
        }

        void Platform::Popup::WebBrowserDialog(chars url)
        {
            PlatformImpl::Wrap::Popup_WebBrowserDialog(url);
        }

        void Platform::Popup::OpenProgramDialog(chars exepath, chars args, bool admin, bool hide, chars dirpath, ublock* getpid)
        {
            PlatformImpl::Wrap::Popup_ProgramDialog(exepath, args, admin, hide, dirpath, getpid);
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

        static QString g_tracker_fontfamily;
        static sint32 g_tracker_fontsize = 0;
        void Platform::Popup::SetTrackerFont(chars family, sint32 pointsize)
        {
            g_tracker_fontfamily = QString::fromUtf8(family);
            g_tracker_fontsize = (sint32) Math::Round(pointsize * Platform::Utility::GetReversedGuiRatio());
        }

        bool Platform::Popup::OpenEditTracker(String& text, UIEditType type, sint32 l, sint32 t, sint32 r, sint32 b, bool* enter)
        {
            if(r <= l || b <= t) return false;

            EditTracker* NewTracker = new EditTracker(type, QString::fromUtf8(text, -1), g_window->View());
            if(0 < g_tracker_fontsize)
                NewTracker->setFont(QFont(g_tracker_fontfamily, g_tracker_fontsize));
            switch(NewTracker->Popup(l, t, r - l, b - t))
            {
            case EditTracker::TCT_Enter:
                if(enter) *enter = true;
                break;
            case EditTracker::TCT_Escape:
            case EditTracker::TCT_ForcedExit:
                delete NewTracker;
                return false;
            case EditTracker::TCT_FocusOut:
                break;
            }

            text = NewTracker->text().toUtf8().constData();
            delete NewTracker;
            return true;
        }

        sint32 Platform::Popup::OpenListTracker(Strings textes, sint32 l, sint32 t, sint32 r, sint32 b)
        {
            if(r <= l || b <= t) return -1;

            ListTracker* NewTracker = new ListTracker(textes, g_window->View());
            if(0 < g_tracker_fontsize)
                NewTracker->setFont(QFont(g_tracker_fontfamily, g_tracker_fontsize));
            const sint32 Result = NewTracker->Popup(l, t, r - l, b - t);
            delete NewTracker;
            return Result;
        }

        void Platform::Popup::CloseAllTracker()
        {
            Tracker::CloseAll(g_window->View());
        }

        bool Platform::Popup::HasOpenedTracker()
        {
            return Tracker::HasAnyone(g_window->View());
        }

        void Platform::Popup::ShowToolTip(String text)
        {
            QToolTip::showText(QCursor::pos(), QString::fromUtf8(text, -1));
        }

        void Platform::Popup::HideToolTip()
        {
            QToolTip::hideText();
        }

        static QSplashScreen* g_splash = nullptr;
        void Platform::Popup::ShowSplash(chars filepath)
        {
            HideSplash();
            g_splash = new QSplashScreen(QPixmap(filepath));
            g_splash->show();
        }

        void Platform::Popup::HideSplash()
        {
            delete g_splash;
            g_splash = nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // UTILITY
        ////////////////////////////////////////////////////////////////////////////////
        static QRandomGenerator* g_RandomGenerator = nullptr;
        uint32 Platform::Utility::Random(bool forcereset)
        {
            if(!g_RandomGenerator || forcereset)
            {
                delete g_RandomGenerator;
                g_RandomGenerator = new QRandomGenerator((uint32) (CurrentTimeMsec() & 0xFFFFFFFF));
            }
            return g_RandomGenerator->generate();
        }

        void Platform::Utility::Sleep(sint32 ms, bool process_input, bool process_socket, bool block_event)
        {
            QTime StartTime = QTime::currentTime();
            if(process_input || process_socket)
            {
                QEventLoop EventLoop(QThread::currentThread());
                sint32 Flag = QEventLoop::AllEvents;
                if(!process_input) Flag |= QEventLoop::ExcludeUserInputEvents;
                if(!process_socket) Flag |= QEventLoop::ExcludeSocketNotifiers;
                EventLoop.processEvents((QEventLoop::ProcessEventsFlag) Flag, ms);
            }
            sint32 SleepTime = ms - StartTime.msecsTo(QTime::currentTime());
            if(0 < SleepTime) QThread::msleep(SleepTime);
        }

        void Platform::Utility::SetMinimize()
        {
            g_window->showMinimized();
        }

        void Platform::Utility::SetFullScreen(bool available_only, sint32 addheight)
        {
            g_window->showFullScreen();
        }

        bool Platform::Utility::IsFullScreen()
        {
            return ((g_window->windowState() & Qt::WindowFullScreen) != 0);
        }

        void Platform::Utility::SetNormalWindow()
        {
            g_window->showNormal();
        }

        void Platform::Utility::ExitProgram()
        {
            QApplication::quit();
        }

        String Platform::Utility::GetProgramPath(bool dironly)
        {
            String& FilePath = *BOSS_STORAGE_SYS(String);
            String& DirPath = *BOSS_STORAGE_SYS(String);
            if(dironly)
            {
                if(0 < DirPath.Length())
                    return DirPath;
            }
            else if(0 < FilePath.Length())
                return FilePath;

            FilePath = QCoreApplication::applicationFilePath().toUtf8().constData();
            FilePath = boss_normalpath(FilePath, nullptr);
            #if !BOSS_WINDOWS
                FilePath = "Q:/" + FilePath;
            #endif

            for(sint32 i = FilePath.Length() - 1; 0 <= i; --i)
            {
                if(FilePath[i] == '/')
                {
                    DirPath = FilePath.Left(i + 1); // '/'기호 포함
                    break;
                }
            }

            if(dironly)
                return DirPath;
            return FilePath;
        }

        chars Platform::Utility::GetArgument(sint32 i, sint32* getcount)
        {
            if(getcount) *getcount = g_argc;
            if(i < g_argc)
                return g_argv[i];
            return "";
        }

        bool Platform::Utility::TestUrlSchema(chars schema, chars comparepath)
        {
            QSettings Settings((chars) String::Format("HKEY_CLASSES_ROOT\\%s", schema), QSettings::NativeFormat);
            String ComparePath = comparepath;
            ComparePath.Replace('/', '\\');

            String OldValue = Settings.value("shell/open/command/Default").toString().toUtf8().constData();
            if(!String::CompareNoCase(((chars) OldValue) + 1, ComparePath, ComparePath.Length()))
                return true;
            return false;
        }

        bool Platform::Utility::BindUrlSchema(chars schema, chars exepath, bool forcewrite)
        {
            QSettings Settings((chars) String::Format("HKEY_CLASSES_ROOT\\%s", schema), QSettings::NativeFormat);
            String ExePath = exepath;
            ExePath.Replace('/', '\\');

            if(!forcewrite)
            {
                String OldValue = Settings.value("shell/open/command/Default").toString().toUtf8().constData();
                if(ExePath.Length() == 0)
                    return (0 < OldValue.Length());
                else if(!String::CompareNoCase(((chars) OldValue) + 1, ExePath, ExePath.Length()))
                    return true;
            }

            // 관리자권한이 없으면 실패
            if(!Settings.isWritable())
                return false;

            if(0 < ExePath.Length())
            {
                Settings.setValue("Default", (chars) String::Format("URL:%s", schema));
                Settings.setValue("URL Protocol", "");
                Settings.beginGroup("DefaultIcon");
                    Settings.setValue("Default", (chars) ("\"" + ExePath + ",1\""));
                Settings.endGroup();
                Settings.beginGroup("shell");
                    Settings.setValue("Default", "open");
                    Settings.beginGroup("open");
                        Settings.beginGroup("command");
                            Settings.setValue("Default", (chars) ("\"" + ExePath + "\" {urlschema:" + schema + "} \"%1\""));
                        Settings.endGroup();
                    Settings.endGroup();
                Settings.endGroup();
            }
            return true;
        }

        bool Platform::Utility::BindExtProgram(chars extname, chars programid, chars exepath)
        {
            QSettings Settings1((chars) String::Format("HKEY_CLASSES_ROOT\\%s", extname), QSettings::NativeFormat);
            QSettings Settings2((chars) String::Format("HKEY_CLASSES_ROOT\\%s", programid), QSettings::NativeFormat);
            String ExePath = exepath;
            ExePath.Replace('/', '\\');

            // 관리자권한이 없으면 실패
            if(!Settings1.isWritable() || !Settings2.isWritable())
                return false;

            if(0 < ExePath.Length())
            {
                Settings1.setValue("Default", programid);
                Settings2.setValue("Default", (chars) String::Format("%s(%s)", programid, extname));
                Settings2.beginGroup("shell");
                    Settings2.beginGroup("open");
                        Settings2.beginGroup("command");
                            Settings2.setValue("Default", (chars) ("\"" + ExePath + "\" \"%1\""));
                        Settings2.endGroup();
                    Settings2.endGroup();
                Settings2.endGroup();
            }
            return true;
        }

        chars Platform::Utility::GetArgumentForUrlSchema(chars schema)
        {
            const String SchemaToken = String::Format("{urlschema:%s}", schema);
            if(g_argc == 3 && !String::Compare(g_argv[1], SchemaToken))
                return g_argv[2] + boss_strlen(schema) + 3; // schema + "://"
            return "";
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
                        TotalRect.r = Math::Max(TotalRect.r, GeometryRect.right());
                        TotalRect.b = Math::Max(TotalRect.b, GeometryRect.bottom());
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
                    rect.r = GeometryRect.right();
                    rect.b = GeometryRect.bottom();
                }
            }
        }

        bool Platform::Utility::IsScreenConnected()
        {
            #if BOSS_ANDROID
                id_file_read Hdmi = Platform::File::OpenForRead("/sys/devices/virtual/switch/hdmi/state");
                if(!Hdmi) Hdmi = Platform::File::OpenForRead("/sys/class/switch/hdmi/state");
                if(Hdmi)
                {
                    sint32 Value = 0;
                    Platform::File::Read(Hdmi, (uint08*) &Value, sizeof(sint32));
                    const bool HasPhygicalMonitor = ((Value & 1) == 1);
                    Platform::File::Close(Hdmi);
                    return HasPhygicalMonitor;
                }
            #endif
            return true;
        }

        id_image_read Platform::Utility::GetScreenshotImage(const rect128& rect)
        {
            QPixmap& ScreenshotPixmap = *BOSS_STORAGE_SYS(QPixmap);
            ScreenshotPixmap = QGuiApplication::primaryScreen()->grabWindow(
                0, rect.l, rect.t, rect.r - rect.l, rect.b - rect.t);
            return (id_image_read) &ScreenshotPixmap;
        }

        id_image_read Platform::Utility::GetWindowImage(const rect128& rect, float blur)
        {
            QPixmap& WindowPixmap = *BOSS_STORAGE_SYS(QPixmap);
            const sint32 BlurBorder = blur;
            QPixmap GrabPixmap = g_window->grab(QRect(
                rect.l - BlurBorder, rect.t - BlurBorder, rect.r - rect.l + BlurBorder * 2, rect.b - rect.t + BlurBorder * 2));
            const sint32 GrabWidth = GrabPixmap.width();
            const sint32 GrabHeight = GrabPixmap.height();
            if(0.0f < blur)
            {
                QGraphicsScene Scene;
                QGraphicsPixmapItem Item(GrabPixmap);
                QGraphicsBlurEffect Blur;
                Blur.setBlurRadius(blur);
                Blur.setBlurHints(QGraphicsBlurEffect::QualityHint);
                Item.setGraphicsEffect(&Blur);
                Scene.addItem(&Item);
                QImage NewImage(GrabWidth, GrabHeight, QImage::Format_ARGB32_Premultiplied);
                NewImage.fill(Qt::transparent);
                QPainter Painter(&NewImage);
                Scene.render(&Painter, QRectF(0, 0, GrabWidth, GrabHeight), QRectF(0, 0, GrabWidth, GrabHeight));
                WindowPixmap.convertFromImage(NewImage.copy(BlurBorder, BlurBorder, GrabWidth - BlurBorder * 2, GrabHeight - BlurBorder * 2));
            }
            else WindowPixmap = GrabPixmap;
            return (id_image_read) &WindowPixmap;
        }

        id_bitmap Platform::Utility::ImageToBitmap(id_image_read image, orientationtype ori)
        {
            if(!image) return nullptr;
            QImage CurImage = ((QPixmap*) image)->toImage();
            if(!CurImage.constBits()) return nullptr;
            CurImage = CurImage.convertToFormat(QImage::Format_ARGB32);
            id_bitmap Result = Bmp::CloneFromBits(CurImage.constBits(),
                CurImage.width(), CurImage.height(), CurImage.bitPlaneCount(), ori);
            return Result;
        }

        void Platform::Utility::SendToTextClipboard(chars text)
        {
            QClipboard* Clipboard = QApplication::clipboard();
            Clipboard->setText(text, QClipboard::Clipboard);
            if(Clipboard->supportsSelection())
                Clipboard->setText(text, QClipboard::Selection);
        }

        String Platform::Utility::RecvFromTextClipboard()
        {
            QClipboard* Clipboard = QApplication::clipboard();
            return Clipboard->text(QClipboard::Clipboard).toUtf8().constData();
        }

        void Platform::Utility::SendRequest(chars url, RequestEventCB cb, payload data)
        {
            static QNetworkAccessManager Manager;
            QNetworkRequest NewRequest(QUrl(QString::fromUtf8(url)));
            QNetworkReply* Reply = Manager.get(NewRequest);
            QObject::connect(Reply, &QNetworkReply::finished,
                [Reply, cb, data]()->void
                {
                    QByteArray NewBytes = Reply->readAll();
                    if(cb) cb(data, NewBytes.constData());
                    Reply->deleteLater();
                });
        }

        Strings Platform::Utility::CreateSystemFont(bytes data, const sint32 size)
        {
            const sint32 NewFontID = QFontDatabase::addApplicationFontFromData(QByteArray((chars) data, size));
            Strings FontFamilies;
            for(auto& CurFontFamily : QFontDatabase::applicationFontFamilies(NewFontID))
                FontFamilies.AtAdding() = String(CurFontFamily.toUtf8().constData());
            return FontFamilies;
        }

        Strings Platform::Utility::EnumSystemFontStyles(chars fontfamily)
        {
            Strings FontStyles;
            for(auto& CurFontStyle : QFontDatabase::styles(fontfamily))
                FontStyles.AtAdding() = String(CurFontStyle.toUtf8().constData());
            return FontStyles;
        }

        void Platform::Utility::RemoveSystemFontAll()
        {
            QFontDatabase::removeAllApplicationFonts();
        }

        void Platform::Utility::SetCursor(CursorRole role)
        {
            switch(role)
            {
            case CR_Arrow: g_window->setCursor(Qt::ArrowCursor); break;
            case CR_UpArrow: g_window->setCursor(Qt::UpArrowCursor); break;
            case CR_Cross: g_window->setCursor(Qt::CrossCursor); break;
            case CR_Wait: g_window->setCursor(Qt::WaitCursor); break;
            case CR_IBeam: g_window->setCursor(Qt::IBeamCursor); break;
            case CR_Blank: g_window->setCursor(Qt::BlankCursor); break;
            case CR_SizeVer: g_window->setCursor(Qt::SizeVerCursor); break;
            case CR_SizeHor: g_window->setCursor(Qt::SizeHorCursor); break;
            case CR_SizeBDiag: g_window->setCursor(Qt::SizeBDiagCursor); break;
            case CR_SizeFDiag: g_window->setCursor(Qt::SizeFDiagCursor); break;
            case CR_SizeAll: g_window->setCursor(Qt::SizeAllCursor); break;
            case CR_PointingHand: g_window->setCursor(Qt::PointingHandCursor); break;
            case CR_OpenHand: g_window->setCursor(Qt::OpenHandCursor); break;
            case CR_ClosedHand: g_window->setCursor(Qt::ClosedHandCursor); break;
            case CR_Forbidden: g_window->setCursor(Qt::ForbiddenCursor); break;
            case CR_Busy: g_window->setCursor(Qt::BusyCursor); break;
            case CR_WhatsThis: g_window->setCursor(Qt::WhatsThisCursor); break;
            }
        }

        void Platform::Utility::GetCursorPos(point64& pos)
        {
            const QPoint CursorPos = QCursor::pos();
            pos.x = CursorPos.x();
            pos.y = CursorPos.y();
        }

        void Platform::Utility::SetCursorPos(point64 pos)
        {
            QCursor::setPos(pos.x, pos.y);
        }

        bool Platform::Utility::GetCursorPosInWindow(point64& pos)
        {
            if(!g_window) return false;
            const QPoint CursorPos = QCursor::pos();
            const QRect ClientRect = g_window->GetWindowRect();
            pos.x = CursorPos.x() - ClientRect.left();
            pos.y = CursorPos.y() - ClientRect.top();
            return !(CursorPos.x() < ClientRect.left() || CursorPos.y() < ClientRect.top()
                || ClientRect.right() <= CursorPos.x() || ClientRect.bottom() <= CursorPos.y());
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
            return QCoreApplication::applicationPid();
        }

        chars Platform::Utility::GetLocaleBCP47()
        {
            static char BCP47Code[8] = {'\0'};
            if(BCP47Code[0] == '\0')
            {
                const QString BCP47Text = QLocale::system().bcp47Name();
                boss_strcpy(BCP47Code, BCP47Text.toUtf8().constData());
            }
            return BCP47Code;
        }

        void Platform::Utility::Threading(ThreadCB cb, payload data, prioritytype priority)
        {
            ThreadClass::Begin(cb, data, priority);
        }

        void* Platform::Utility::ThreadingEx(ThreadExCB cb, payload data, prioritytype priority)
        {
            return ThreadClass::BeginEx(cb, data, priority);
        }

        uint64 Platform::Utility::CurrentThreadID()
        {
            return (uint64) QThread::currentThreadId();
        }

        uint32 Platform::Utility::IdealThreadCount()
        {
            return (uint32) QThread::idealThreadCount();
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

        chars Platform::Utility::GetTimeRTC(chars devicepath,
            sint32* sec, sint32* min, sint32* hour, sint32* day, sint32* month, sint32* year, sint32* weekday)
        {
            return PlatformImpl::Wrap::Utility_GetTimeRTC(devicepath, sec, min, hour, day, month, year, weekday);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // CLOCK
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Clock::SetBaseTime(chars timestring)
        {
            ClockClass::SetBaseTime(timestring);
        }

        id_clock Platform::Clock::Create(sint32 year, sint32 month, sint32 day,
            sint32 hour, sint32 min, sint32 sec, sint64 nsec)
        {
            buffer NewClock = Buffer::Alloc<ClockClass>(BOSS_DBG 1);
            ((ClockClass*) NewClock)->SetClock(year, month, day, hour, min, sec, nsec);
            return (id_clock) NewClock;
        }

        id_clock Platform::Clock::CreateAsWindowTime(uint64 msec)
        {
            buffer NewClock = Buffer::Alloc<ClockClass>(BOSS_DBG 1);
            ((ClockClass*) NewClock)->SetClock(msec);
            return (id_clock) NewClock;
        }

        id_clock Platform::Clock::CreateAsCurrent()
        {
            buffer NewClock = Buffer::Alloc<ClockClass>(BOSS_DBG 1);
            return (id_clock) NewClock;
        }

        id_clock Platform::Clock::CreateAsClone(id_clock_read clock)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", clock);
            buffer NewClock = Buffer::AllocNoConstructorOnce<ClockClass>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewClock, 0, ClockClass, *((const ClockClass*) clock));
            return (id_clock) NewClock;
        }

        void Platform::Clock::Release(id_clock clock)
        {
            Buffer::Free((buffer) clock);
        }

        sint64 Platform::Clock::GetPeriodNsec(id_clock_read from, id_clock_read to)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", from && to);
            return ((const ClockClass*) to)->GetLap() - ((const ClockClass*) from)->GetLap();
        }

        void Platform::Clock::AddNsec(id_clock dest, sint64 nsec)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", dest);
            ((ClockClass*) dest)->AddLap(nsec);
        }

        uint64 Platform::Clock::GetMsec(id_clock clock)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", clock);
            return ((const ClockClass*) clock)->GetTotalSec() * 1000 +
                (uint64) (((const ClockClass*) clock)->GetNSecInSec() / 1000000);
        }

        sint64 Platform::Clock::GetLocalMsecFromUTC()
        {
            return ClockClass::GetLocalTimeMSecFromUtc();
        }

        void Platform::Clock::GetDetail(id_clock clock, sint64* nsec,
            sint32* sec, sint32* min, sint32* hour, sint32* day, sint32* month, sint32* year, sint32* weekday)
        {
            BOSS_ASSERT("파라미터가 nullptr입니다", clock);
            const sint64 CurTotalSec = ((const ClockClass*) clock)->GetTotalSec();
            // JulianDay는 BC-4300년을 기점으로 계산된 일수
            const QDate CurDate = QDate::fromJulianDay(CurTotalSec / (60 * 60 * 24));
            if(nsec) *nsec = ((const ClockClass*) clock)->GetNSecInSec();
            if(sec) *sec = CurTotalSec % 60;
            if(min) *min = (CurTotalSec / 60) % 60;
            if(hour) *hour = (CurTotalSec / (60 * 60)) % 24;
            if(day) *day = CurDate.day();
            if(month) *month = CurDate.month();
            if(year) *year = CurDate.year();
            if(weekday) *weekday = CurDate.dayOfWeek();
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
        void Platform::Graphics::SetRenderHint(bool smooth, bool antialiasing)
        {
            CanvasClass::get()->SetRenderHint(smooth, antialiasing);
        }

        void Platform::Graphics::SetScissor(double x, double y, double w, double h)
        {
            const double LastZoom = CanvasClass::get()->zoom();
            CanvasClass::get()->SetScissor(
                Math::Floor(x * LastZoom) / LastZoom,
                Math::Floor(y * LastZoom) / LastZoom,
                Math::Ceil((x + w) * LastZoom) / LastZoom,
                Math::Ceil((y + h) * LastZoom) / LastZoom);
        }

        void Platform::Graphics::SetColor(uint08 r, uint08 g, uint08 b, uint08 a)
        {
            CanvasClass::get()->SetColor(r, g, b, a);
        }

        void Platform::Graphics::SetMask(MaskRole role)
        {
            switch(role)
            {
            case MR_SrcOver: CanvasClass::get()->SetMask(QPainter::CompositionMode_SourceOver); break;
            case MR_DstOver: CanvasClass::get()->SetMask(QPainter::CompositionMode_DestinationOver); break;
            case MR_Clear: CanvasClass::get()->SetMask(QPainter::CompositionMode_Clear); break;
            case MR_Src: CanvasClass::get()->SetMask(QPainter::CompositionMode_Source); break;
            case MR_Dst: CanvasClass::get()->SetMask(QPainter::CompositionMode_Destination); break;
            case MR_SrcIn: CanvasClass::get()->SetMask(QPainter::CompositionMode_SourceIn); break;
            case MR_DstIn: CanvasClass::get()->SetMask(QPainter::CompositionMode_DestinationIn); break;
            case MR_SrcOut: CanvasClass::get()->SetMask(QPainter::CompositionMode_SourceOut); break;
            case MR_DstOut: CanvasClass::get()->SetMask(QPainter::CompositionMode_DestinationOut); break;
            case MR_SrcAtop: CanvasClass::get()->SetMask(QPainter::CompositionMode_SourceAtop); break;
            case MR_DstAtop: CanvasClass::get()->SetMask(QPainter::CompositionMode_DestinationAtop); break;
            case MR_Xor: CanvasClass::get()->SetMask(QPainter::CompositionMode_Xor); break;
            case MR_Plus: CanvasClass::get()->SetMask(QPainter::CompositionMode_Plus); break;
            case MR_Multiply: CanvasClass::get()->SetMask(QPainter::CompositionMode_Multiply); break;
            case MR_Screen: CanvasClass::get()->SetMask(QPainter::CompositionMode_Screen); break;
            case MR_Overlay: CanvasClass::get()->SetMask(QPainter::CompositionMode_Overlay); break;
            case MR_Darken: CanvasClass::get()->SetMask(QPainter::CompositionMode_Darken); break;
            case MR_Lighten: CanvasClass::get()->SetMask(QPainter::CompositionMode_Lighten); break;
            case MR_ColorDodge: CanvasClass::get()->SetMask(QPainter::CompositionMode_ColorDodge); break;
            case MR_ColorBurn: CanvasClass::get()->SetMask(QPainter::CompositionMode_ColorBurn); break;
            case MR_HardLight: CanvasClass::get()->SetMask(QPainter::CompositionMode_HardLight); break;
            case MR_SoftLight: CanvasClass::get()->SetMask(QPainter::CompositionMode_SoftLight); break;
            case MR_Difference: CanvasClass::get()->SetMask(QPainter::CompositionMode_Difference); break;
            case MR_Exclusion: CanvasClass::get()->SetMask(QPainter::CompositionMode_Exclusion); break;
            }
        }

        void Platform::Graphics::SetShader(ShaderRole role)
        {
            CanvasClass::get()->SetShader(role);
        }

        void Platform::Graphics::SetFont(chars name, float size)
        {
            #if BOSS_MAC_OSX
                const float DeviceRatio = 1.25 * Platform::Utility::GetReversedGuiRatio();
            #else
                const float DeviceRatio = Platform::Utility::GetReversedGuiRatio();
            #endif
            CanvasClass::get()->SetFont(name, Math::Max(1, size * DeviceRatio));
        }

        void Platform::Graphics::SetFontForFreeType(chars nickname, sint32 height)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetZoom(double zoom, OrientationRole orientation)
        {
            const sint32 Width = CanvasClass::get()->painter_width();
            const sint32 Height = CanvasClass::get()->painter_height();
            BOSS_ASSERT("호출시점이 적절하지 않습니다", CanvasClass::get());
            QTransform NewTransform(zoom, 0, 0, zoom, 0, 0);
            switch(orientation)
            {
            case OR_CW90:
                NewTransform.translate(Width / zoom, 0);
                NewTransform.rotate(89.999999); // QT버그대응
                break;
            case OR_CW180:
                NewTransform.translate(Width / zoom, Height / zoom);
                NewTransform.rotate(180);
                break;
            case OR_CW270:
                NewTransform.translate(0, Height / zoom);
                NewTransform.rotate(-89.999999); // QT버그대응
                break;
            }
            hook(CanvasClass::get()->painter())
                fish.setTransform(NewTransform);
        }

        void Platform::Graphics::EraseRect(float x, float y, float w, float h)
        {
            auto OldCompositionMode = CanvasClass::get()->painter().compositionMode();
            CanvasClass::get()->painter().setCompositionMode(QPainter::CompositionMode_Clear);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, false);
            CanvasClass::get()->painter().eraseRect(QRectF(x, y, w, h));
            CanvasClass::get()->painter().setCompositionMode(OldCompositionMode);
        }

        void Platform::Graphics::EraseRoundRect(float x, float y, float w, float h, sint32 r)
        {
            auto OldCompositionMode = CanvasClass::get()->painter().compositionMode();
            r = Math::Min(Math::MinF(w, h) * 0.5 + 0.5, r);
            CanvasClass::get()->painter().setCompositionMode(QPainter::CompositionMode_Clear);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            for(sint32 i = 0, ir = r, rr = r * r; i < r; ++i, --ir)
            {
                const sint32 CurR = Math::Max(0, r - Math::Sqrt(rr - ir * ir));
                CanvasClass::get()->painter().eraseRect(QRectF(x + CurR, y + i, w - CurR * 2, 1.5));
                CanvasClass::get()->painter().eraseRect(QRectF(x + CurR, y + h - i - 1.5, w - CurR * 2, 1.5));
            }
            CanvasClass::get()->painter().eraseRect(QRectF(x, y + r, w, h - r * 2));
            CanvasClass::get()->painter().setCompositionMode(OldCompositionMode);
        }

        void Platform::Graphics::FillRect(float x, float y, float w, float h)
        {
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, false);
            CanvasClass::get()->painter().fillRect(QRectF(x, y, w, h), CanvasClass::get()->color());
        }

        void Platform::Graphics::FillRectToFBO(float x, float y, float w, float h, Color color, uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::FillCircle(float x, float y, float w, float h)
        {
            CanvasClass::get()->painter().setPen(Qt::NoPen);
            CanvasClass::get()->painter().setBrush(QBrush(CanvasClass::get()->color()));
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawEllipse(QRectF(x, y, w, h));
        }

        void Platform::Graphics::FillPolygon(float x, float y, Points p)
        {
            const sint32 Count = p.Count();
            if(Count < 3) return;

            QPointF* NewPoint = new QPointF[Count];
            for(sint32 i = 0; i < Count; ++i)
            {
                NewPoint[i].setX(x + p[i].x);
                NewPoint[i].setY(y + p[i].y);
            }

            CanvasClass::get()->painter().setPen(Qt::NoPen);
            CanvasClass::get()->painter().setBrush(QBrush(CanvasClass::get()->color()));
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawPolygon(NewPoint, Count);
            delete[] NewPoint;
        }

        void Platform::Graphics::FillBezier(float x, float y, Points p, float curve)
        {
            const sint32 Count = p.Count();
            if(Count < 3) return;

            QPainterPath NewPath;
            NewPath.moveTo(x + p[0].x, y + p[0].y);
            for(sint32 i = 0; i < Count; ++i)
            {
                const sint32 A = (i + Count - 1) % Count, B = i, C = (i + 1) % Count, D = (i + 2) % Count;
                const float Ctrl1X = x + p[B].x + (p[C].x - p[A].x) * curve;
                const float Ctrl1Y = y + p[B].y + (p[C].y - p[A].y) * curve;
                const float Ctrl2X = x + p[C].x + (p[B].x - p[D].x) * curve;
                const float Ctrl2Y = y + p[C].y + (p[B].y - p[D].y) * curve;
                NewPath.cubicTo(Ctrl1X, Ctrl1Y, Ctrl2X, Ctrl2Y, x + p[C].x, y + p[C].y);
            }

            CanvasClass::get()->painter().setPen(Qt::NoPen);
            CanvasClass::get()->painter().setBrush(QBrush(CanvasClass::get()->color()));
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawPath(NewPath);
        }

        void Platform::Graphics::DrawRect(float x, float y, float w, float h, float thick)
        {
            QPen NewPen(QBrush(CanvasClass::get()->color()), thick);
            NewPen.setCapStyle(Qt::FlatCap);
            NewPen.setJoinStyle(Qt::MiterJoin);
            CanvasClass::get()->painter().setPen(NewPen);
            CanvasClass::get()->painter().setBrush(Qt::NoBrush);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, false);
            CanvasClass::get()->painter().drawRect(QRectF(x - thick / 2, y - thick / 2, w + thick, h + thick));
        }

        void Platform::Graphics::DrawLine(const Point& begin, const Point& end, float thick)
        {
            CanvasClass::get()->painter().setPen(QPen(QBrush(CanvasClass::get()->color()), thick));
            CanvasClass::get()->painter().setBrush(Qt::NoBrush);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawLine(QPointF(begin.x, begin.y), QPointF(end.x, end.y));
        }

        void Platform::Graphics::DrawCircle(float x, float y, float w, float h, float thick)
        {
            CanvasClass::get()->painter().setPen(QPen(QBrush(CanvasClass::get()->color()), thick));
            CanvasClass::get()->painter().setBrush(Qt::NoBrush);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawEllipse(QRectF(x - thick / 2, y - thick / 2, w + thick, h + thick));
        }

        void Platform::Graphics::DrawBezier(const Vector& begin, const Vector& end, float thick)
        {
            QPainterPath NewPath;
            NewPath.moveTo(begin.x, begin.y);
            NewPath.cubicTo(begin.x + begin.vx, begin.y + begin.vy,
                end.x + end.vx, end.y + end.vy, end.x, end.y);

            CanvasClass::get()->painter().setPen(QPen(QBrush(CanvasClass::get()->color()), thick));
            CanvasClass::get()->painter().setBrush(Qt::NoBrush);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawPath(NewPath);
        }

        void Platform::Graphics::DrawPolyLine(float x, float y, Points p, float thick, bool ring)
        {
            const sint32 Count = p.Count();
            if(Count < 2) return;

            const sint32 RealCount = (ring)? Count + 1 : Count;
            QPointF* NewPoint = new QPointF[RealCount];
            for(sint32 i = 0; i < RealCount; ++i)
            {
                NewPoint[i].setX(x + p[i % Count].x);
                NewPoint[i].setY(y + p[i % Count].y);
            }

            CanvasClass::get()->painter().setPen(QPen(QBrush(CanvasClass::get()->color()), thick));
            CanvasClass::get()->painter().setBrush(Qt::NoBrush);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawPolyline(NewPoint, RealCount);
            delete[] NewPoint;
        }

        void Platform::Graphics::DrawPolyBezier(float x, float y, Points p, float thick, bool showfirst, bool showlast)
        {
            const sint32 Count = p.Count();
            if(Count < 4) return;

            QPainterPath NewPath;
            if(showfirst)
            {
                NewPath.moveTo(x + p[0].x, y + p[0].y);
                NewPath.lineTo(x + p[1].x, y + p[1].y);
            }
            else NewPath.moveTo(x + p[1].x, y + p[1].y);

            for(sint32 i = 2; i < Count - 1; ++i)
            {
                const sint32 A = i - 2, B = i - 1, C = i, D = i + 1;
                const float Ctrl1X = x + p[B].x + ((A == 0)? (p[B].x - p[A].x) * 6 : (p[C].x - p[A].x) / 6);
                const float Ctrl1Y = y + p[B].y + ((A == 0)? (p[B].y - p[A].y) * 6 : (p[C].y - p[A].y) / 6);
                const float Ctrl2X = x + p[C].x + ((D == Count - 1)? (p[C].x - p[D].x) * 6 : (p[B].x - p[D].x) / 6);
                const float Ctrl2Y = y + p[C].y + ((D == Count - 1)? (p[C].y - p[D].y) * 6 : (p[B].y - p[D].y) / 6);
                NewPath.cubicTo(Ctrl1X, Ctrl1Y, Ctrl2X, Ctrl2Y, x + p[C].x, y + p[C].y);
            }

            if(showlast)
                NewPath.lineTo(x + p[-1].x, y + p[-1].y);

            CanvasClass::get()->painter().setPen(QPen(QBrush(CanvasClass::get()->color()), thick));
            CanvasClass::get()->painter().setBrush(Qt::NoBrush);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawPath(NewPath);
        }

        void Platform::Graphics::DrawRingBezier(float x, float y, Points p, float thick, float curve)
        {
            const sint32 Count = p.Count();
            if(Count < 3) return;

            QPainterPath NewPath;
            NewPath.moveTo(x + p[0].x, y + p[0].y);
            for(sint32 i = 0; i < Count; ++i)
            {
                const sint32 A = (i + Count - 1) % Count, B = i, C = (i + 1) % Count, D = (i + 2) % Count;
                const float Ctrl1X = x + p[B].x + (p[C].x - p[A].x) * curve;
                const float Ctrl1Y = y + p[B].y + (p[C].y - p[A].y) * curve;
                const float Ctrl2X = x + p[C].x + (p[B].x - p[D].x) * curve;
                const float Ctrl2Y = y + p[C].y + (p[B].y - p[D].y) * curve;
                NewPath.cubicTo(Ctrl1X, Ctrl1Y, Ctrl2X, Ctrl2Y, x + p[C].x, y + p[C].y);
            }

            CanvasClass::get()->painter().setPen(QPen(QBrush(CanvasClass::get()->color()), thick));
            CanvasClass::get()->painter().setBrush(Qt::NoBrush);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawPath(NewPath);
        }

        void Platform::Graphics::DrawTextureToFBO(id_texture_read texture, float tx, float ty, float tw, float th,
            orientationtype ori, bool antialiasing, float x, float y, float w, float h, Color color, uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        id_image Platform::Graphics::CreateImage(id_bitmap_read bitmap, bool mirrored)
        {
            const sint32 SrcWidth = Bmp::GetWidth(bitmap);
            const sint32 SrcHeight = Bmp::GetHeight(bitmap);
            QImage NewImage(SrcWidth, SrcHeight, QImage::Format_ARGB32);

            Bmp::bitmappixel* DstBits = (Bmp::bitmappixel*) NewImage.bits();
            const Bmp::bitmappixel* SrcBits = (const Bmp::bitmappixel*) Bmp::GetBits(bitmap);
            if(mirrored)
                Memory::Copy(DstBits, SrcBits, sizeof(Bmp::bitmappixel) * SrcWidth * SrcHeight);
            else for(sint32 y = 0; y < SrcHeight; ++y)
                Memory::Copy(&DstBits[y * SrcWidth], &SrcBits[(SrcHeight - 1 - y) * SrcWidth], sizeof(Bmp::bitmappixel) * SrcWidth);

            buffer NewPixmap = Buffer::Alloc<QPixmap>(BOSS_DBG 1);
            ((QPixmap*) NewPixmap)->convertFromImage(NewImage);
            return (id_image) NewPixmap;
        }

        sint32 Platform::Graphics::GetImageWidth(id_image_read image)
        {
            if(const QPixmap* CurPixmap = (const QPixmap*) image)
                return CurPixmap->width();
            return 0;
        }

        sint32 Platform::Graphics::GetImageHeight(id_image_read image)
        {
            if(const QPixmap* CurPixmap = (const QPixmap*) image)
                return CurPixmap->height();
            return 0;
        }

        void Platform::Graphics::RemoveImage(id_image image)
        {
            Buffer::Free((buffer) image);
        }

        static uint08 g_alphaTable[256 * 256];
        static uint08 g_colorTable[256 * 256];
        class ImageRoutine
        {
        public:
            ImageRoutine() {}
            ~ImageRoutine() {delete mPlatformPixmap;}

        public:
            void Build(id_bitmap_read bitmap, const Color coloring)
            {
                const sint32 Width = Bmp::GetWidth(bitmap);
                const sint32 Height = Bmp::GetHeight(bitmap);
                QImage NewImage(Width, Height, QImage::Format_ARGB32);
                Bmp::bitmappixel* DstBits = (Bmp::bitmappixel*) NewImage.bits();
                const Bmp::bitmappixel* SrcBits = (const Bmp::bitmappixel*) Bmp::GetBits(bitmap);
                if(coloring.argb == Color::ColoringDefault)
                {
                    for(sint32 dy = 0; dy < Height; ++dy)
                    {
                        Bmp::bitmappixel* CurDstBits = &DstBits[dy * Width];
                        const Bmp::bitmappixel* CurSrcBits = &SrcBits[(Height - 1 - dy) * Width];
                        Memory::Copy(CurDstBits, CurSrcBits, Width * sizeof(Bmp::bitmappixel));
                    }
                }
                else
                {
                    ValidColorTabling();
                    const uint08* CurTableA = &g_alphaTable[coloring.a * 256];
                    const uint08* CurTableR = &g_colorTable[coloring.r * 256];
                    const uint08* CurTableG = &g_colorTable[coloring.g * 256];
                    const uint08* CurTableB = &g_colorTable[coloring.b * 256];
                    for(sint32 dy = 0; dy < Height; ++dy)
                    {
                        Bmp::bitmappixel* CurDstBits = &DstBits[dy * Width];
                        const Bmp::bitmappixel* CurSrcBits = &SrcBits[(Height - 1 - dy) * Width];
                        for(sint32 x = 0; x < Width; ++x)
                        {
                            CurDstBits->a = CurTableA[CurSrcBits->a];
                            CurDstBits->r = CurTableR[CurSrcBits->r];
                            CurDstBits->g = CurTableG[CurSrcBits->g];
                            CurDstBits->b = CurTableB[CurSrcBits->b];
                            CurDstBits++;
                            CurSrcBits++;
                        }
                    }
                }
                delete mPlatformPixmap;
                mPlatformPixmap = new QPixmap();
                *mPlatformPixmap = QPixmap::fromImage(ToReference(NewImage));
            }

        public:
            QPixmap* mPlatformPixmap {nullptr};

        private:
            void ValidColorTabling()
            {
                static bool NeedTabling = true;
                if(NeedTabling)
                {
                    NeedTabling = false;
                    for(sint32 src = 0; src < 256; ++src)
                    for(sint32 key = 0; key < 256; ++key)
                    {
                        const sint32 i = key * 256 + src;
                        if(src < 0x80)
                        {
                            if(key < 0x80)
                            {
                                const sint32 key2 = 0x80 - key;
                                g_alphaTable[i] = (uint08) (src * key / 0x80);
                                g_colorTable[i] = (uint08) (src - key2 + key2 * (0x80 - src) / 0x80);
                            }
                            else
                            {
                                const sint32 src2 = src * 2;
                                g_alphaTable[i] = (uint08) (src2 - (src2 - src) * (0xFF - key) / 0x7F);
                                g_colorTable[i] = (uint08) (0xFF - (0xFF - src) * (0xFF - key) / 0x7F);
                            }
                        }
                        else
                        {
                            if(key < 0x80)
                            {
                                const uint08 value = (uint08) (src * key / 0x80);
                                g_alphaTable[i] = value;
                                g_colorTable[i] = value;
                            }
                            else
                            {
                                const sint32 key2 = key - 0x80;
                                const uint08 value = (uint08) (src + key2 - key2 * (src - 0x80) / 0x7F);
                                g_alphaTable[i] = value;
                                g_colorTable[i] = value;
                            }
                        }
                    }
                }
            }
        };

        id_image_routine Platform::Graphics::CreateImageRoutine(id_bitmap_read bitmap, sint32 resizing_width, sint32 resizing_height, const Color coloring)
        {
            auto NewImageRoutine = new ImageRoutine();
            NewImageRoutine->Build(bitmap, coloring);
            return (id_image_routine) NewImageRoutine;
        }

        void Platform::Graphics::UpdateImageRoutineTimeout(uint64 msec)
        {
        }

        id_image_read Platform::Graphics::BuildImageRoutineOnce(id_image_routine routine, bool use_timeout)
        {
            auto CurImageRoutine = (ImageRoutine*) routine;
            return (id_image_read) CurImageRoutine->mPlatformPixmap;
        }

        void Platform::Graphics::RemoveImageRoutine(id_image_routine routine)
        {
            delete (ImageRoutine*) routine;
        }

        void Platform::Graphics::DrawImage(id_image_read image, float ix, float iy, float iw, float ih, float x, float y, float w, float h, float degree, float opacity)
        {
            BOSS_ASSERT("image파라미터가 nullptr입니다", image);
            if(image == nullptr) return;

            if(degree != 0.0f)
            {
                QTransform NewTransform;
                NewTransform.translate(x + w / 2, y + h / 2);
                NewTransform.rotate(degree);
                NewTransform.translate(-x - w / 2, -y - h / 2);
                CanvasClass::get()->painter().setTransform(NewTransform);
            }

            CanvasClass::get()->painter().setOpacity(opacity);
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, false);
            CanvasClass::get()->painter().drawPixmap(QRectF(x, y, w, h), *((const QPixmap*) image), QRectF(ix, iy, iw, ih));
            CanvasClass::get()->painter().setOpacity(1.0f);

            if(degree != 0.0f)
                CanvasClass::get()->painter().resetTransform();
        }

        void Platform::Graphics::DrawPolyImageToFBO(id_image_read image, const Point (&ips)[3], float x, float y, const Point (&ps)[3], const Color (&colors)[3], uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawRotatedImageToFBO(id_image_read image, float ox, float oy, float degree, float x, float y, Color color, uint32 fbo)
        {
            BOSS_ASSERT("Further development is needed.", false);
        }

        static Qt::Alignment _ExchangeAlignment(UIFontAlign align)
        {
            Qt::Alignment Result = Qt::AlignCenter;
            switch(align)
            {
            case UIFA_LeftTop: Result = (Qt::Alignment) (Qt::AlignLeft | Qt::AlignTop); break;
            case UIFA_CenterTop: Result = (Qt::Alignment) (Qt::AlignHCenter | Qt::AlignTop); break;
            case UIFA_RightTop: Result = (Qt::Alignment) (Qt::AlignRight | Qt::AlignTop); break;
            case UIFA_JustifyTop: Result = (Qt::Alignment) (Qt::AlignJustify | Qt::AlignTop); break;
            case UIFA_LeftMiddle: Result = (Qt::Alignment) (Qt::AlignLeft | Qt::AlignVCenter); break;
            case UIFA_CenterMiddle: Result = (Qt::Alignment) (Qt::AlignHCenter | Qt::AlignVCenter); break;
            case UIFA_RightMiddle: Result = (Qt::Alignment) (Qt::AlignRight | Qt::AlignVCenter); break;
            case UIFA_JustifyMiddle: Result = (Qt::Alignment) (Qt::AlignJustify | Qt::AlignVCenter); break;
            case UIFA_LeftAscent: Result = (Qt::Alignment) (Qt::AlignLeft | Qt::AlignBaseline); break;
            case UIFA_CenterAscent: Result = (Qt::Alignment) (Qt::AlignHCenter | Qt::AlignBaseline); break;
            case UIFA_RightAscent: Result = (Qt::Alignment) (Qt::AlignRight | Qt::AlignBaseline); break;
            case UIFA_JustifyAscent: Result = (Qt::Alignment) (Qt::AlignJustify | Qt::AlignBaseline); break;
            case UIFA_LeftBottom: Result = (Qt::Alignment) (Qt::AlignLeft | Qt::AlignBottom); break;
            case UIFA_CenterBottom: Result = (Qt::Alignment) (Qt::AlignHCenter | Qt::AlignBottom); break;
            case UIFA_RightBottom: Result = (Qt::Alignment) (Qt::AlignRight | Qt::AlignBottom); break;
            case UIFA_JustifyBottom: Result = (Qt::Alignment) (Qt::AlignJustify | Qt::AlignBottom); break;
            }
            return Result;
        }

        static Qt::TextElideMode _ExchangeTextElideMode(UIFontElide elide)
        {
            Qt::TextElideMode Result = Qt::ElideNone;
            switch(elide)
            {
            case UIFE_None: Result = Qt::ElideNone; break;
            case UIFE_Left: Result = Qt::ElideLeft; break;
            case UIFE_Center: Result = Qt::ElideMiddle; break;
            case UIFE_Right: Result = Qt::ElideRight; break;
            }
            return Result;
        }

        static inline sint32 _GetXFontAlignCode(UIFontAlign align)
        {
            sint32 Code = 0;
            switch(align)
            {
            case UIFA_LeftTop:    Code = 0; break; case UIFA_CenterTop:    Code = 1; break; case UIFA_RightTop:    Code = 2; break; case UIFA_JustifyTop:    Code = 3; break;
            case UIFA_LeftMiddle: Code = 0; break; case UIFA_CenterMiddle: Code = 1; break; case UIFA_RightMiddle: Code = 2; break; case UIFA_JustifyMiddle: Code = 3; break;
            case UIFA_LeftAscent: Code = 0; break; case UIFA_CenterAscent: Code = 1; break; case UIFA_RightAscent: Code = 2; break; case UIFA_JustifyAscent: Code = 3; break;
            case UIFA_LeftBottom: Code = 0; break; case UIFA_CenterBottom: Code = 1; break; case UIFA_RightBottom: Code = 2; break; case UIFA_JustifyBottom: Code = 3; break;
            }
            return Code;
        }

        static inline sint32 _GetYFontAlignCode(UIFontAlign align)
        {
            sint32 Code = 0;
            switch(align)
            {
            case UIFA_LeftTop:    Code = 0; break; case UIFA_CenterTop:    Code = 0; break; case UIFA_RightTop:    Code = 0; break; case UIFA_JustifyTop:    Code = 0; break;
            case UIFA_LeftMiddle: Code = 1; break; case UIFA_CenterMiddle: Code = 1; break; case UIFA_RightMiddle: Code = 1; break; case UIFA_JustifyMiddle: Code = 1; break;
            case UIFA_LeftAscent: Code = 2; break; case UIFA_CenterAscent: Code = 2; break; case UIFA_RightAscent: Code = 2; break; case UIFA_JustifyAscent: Code = 2; break;
            case UIFA_LeftBottom: Code = 3; break; case UIFA_CenterBottom: Code = 3; break; case UIFA_RightBottom: Code = 3; break; case UIFA_JustifyBottom: Code = 3; break;
            }
            return Code;
        }

        template<typename TYPE>
        static bool _DrawString(float x, float y, float w, float h, const TYPE* string, sint32 count, UIFontAlign align, UIFontElide elide)
        {
            CanvasClass::get()->painter().setPen(CanvasClass::get()->color());
            CanvasClass::get()->painter().setBrush(Qt::NoBrush);

            const QString Text = (sizeof(TYPE) == 1)?
                QString::fromUtf8((chars) string, count) : QString::fromWCharArray((wchars) string, count);
            if(elide != UIFE_None)
            {
                const QString ElidedText = CanvasClass::get()->painter().fontMetrics().elidedText(Text, _ExchangeTextElideMode(elide), w);
                if(ElidedText != Text)
                {
                    CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
                    CanvasClass::get()->painter().drawText(QRectF(x, y, w, h), ElidedText, QTextOption(_ExchangeAlignment(align)));
                    return true;
                }
            }
            CanvasClass::get()->painter().setRenderHint(QPainter::Antialiasing, CanvasClass::get()->antialiasing());
            CanvasClass::get()->painter().drawText(QRectF(x, y, w, h), Text, QTextOption(_ExchangeAlignment(align)));
            return false;
        }

        bool Platform::Graphics::DrawString(float x, float y, float w, float h, chars string, sint32 count, UIFontAlign align, UIFontElide elide)
        {
            return _DrawString(x, y, w, h, string, count, align, elide);
        }

        bool Platform::Graphics::DrawStringW(float x, float y, float w, float h, wchars string, sint32 count, UIFontAlign align, UIFontElide elide)
        {
            return _DrawString(x, y, w, h, string, count, align, elide);
        }

        template<typename TYPE>
        static sint32 _GetLengthOfString(bool byword, sint32 clipping_width, const TYPE* string, sint32 count)
        {
            const TYPE* StringFocus = string;
            if(count != -1)
            {
                const TYPE* StringFocusEnd = string + count;
                while(StringFocus < StringFocusEnd)
                {
                    const sint32 CurLetterLength = (sizeof(TYPE) == 1)?
                        String::GetLengthOfFirstLetter((chars) StringFocus) : WString::GetLengthOfFirstLetter((wchars) StringFocus);
                    const QString CurLetter = (sizeof(TYPE) == 1)?
                        QString::fromUtf8((chars) StringFocus, CurLetterLength) : QString::fromWCharArray((wchars) StringFocus, CurLetterLength);
                    clipping_width -= CanvasClass::get()->painter().fontMetrics().horizontalAdvance(CurLetter);
                    if(clipping_width < 0) break;
                    StringFocus += CurLetterLength;
                }
            }
            else while(*StringFocus)
            {
                const sint32 CurLetterLength = (sizeof(TYPE) == 1)?
                    String::GetLengthOfFirstLetter((chars) StringFocus) : WString::GetLengthOfFirstLetter((wchars) StringFocus);
                const QString CurLetter = (sizeof(TYPE) == 1)?
                    QString::fromUtf8((chars) StringFocus, CurLetterLength) : QString::fromWCharArray((wchars) StringFocus, CurLetterLength);
                clipping_width -= CanvasClass::get()->painter().fontMetrics().horizontalAdvance(CurLetter);
                if(clipping_width < 0) break;
                StringFocus += CurLetterLength;
            }
            return StringFocus - string;
        }

        sint32 Platform::Graphics::GetLengthOfString(bool byword, sint32 clipping_width, chars string, sint32 count)
        {
            return _GetLengthOfString(byword, clipping_width, string, count);
        }

        sint32 Platform::Graphics::GetLengthOfStringW(bool byword, sint32 clipping_width, wchars string, sint32 count)
        {
            return _GetLengthOfString(byword, clipping_width, string, count);
        }

        sint32 Platform::Graphics::GetStringWidth(chars string, sint32 count)
        {
            return CanvasClass::get()->painter().fontMetrics().horizontalAdvance(QString::fromUtf8(string, count));
        }

        sint32 Platform::Graphics::GetStringWidthW(wchars string, sint32 count)
        {
            return CanvasClass::get()->painter().fontMetrics().horizontalAdvance(QString::fromWCharArray(string, count));
        }

        sint32 Platform::Graphics::GetFreeTypeStringWidth(chars nickname, sint32 height, chars string, sint32 count)
        {
            BOSS_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringHeight()
        {
            return CanvasClass::get()->painter().fontMetrics().height();
        }

        sint32 Platform::Graphics::GetStringAscent()
        {
            return CanvasClass::get()->painter().fontMetrics().ascent();
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

        sint32 Platform::File::Search(chars dirname, SearchCB cb, payload data, bool needfullpath, bool fileonly)
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
                //BOSS_TRACE("Search(%s) - The TargetDir is nonexistent", (chars) PathUTF8);
                return -1;
            }

            const QStringList& List = TargetDir.entryList(((fileonly)? QDir::Files : QDir::Files | QDir::Dirs)
                | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
            //BOSS_TRACE("Search(%s) - %d", (chars) PathUTF8, List.size());

            if(cb)
            for(sint32 i = 0, iend = List.size(); i < iend; ++i)
            {
                if(needfullpath)
                    cb(PlatformImpl::Core::NormalPath(
                        String::Format("%s/%s", (chars) ParsedDirname, List.at(i).toUtf8().constData()), false), data);
                else cb(List.at(i).toUtf8().constData(), data);
                //BOSS_TRACE("Search() - Result: %s", List.at(i).toUtf8().constData());
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

        WString Platform::File::GetExtensionName(wchars itemname)
        {
            return PlatformImpl::Wrap::File_ExtensionName(itemname);
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
                String NewPath = String::Format("%s/../assets/", QCoreApplication::applicationDirPath().toUtf8().constData());
                for(sint32 i = 1; i < 5; ++i)
                {
                    String AssetsPath = QCoreApplication::applicationDirPath().toUtf8().constData();
                    for(sint32 j = 0; j < i; ++j) AssetsPath += "/..";
                    if(Platform::File::ExistForDir(AssetsPath + "/assets"))
                    {
                        NewPath = AssetsPath + "/assets/";
                        break;
                    }
                }
            #elif BOSS_LINUX
                String NewPath = String::Format("Q:%s/../assets/", QCoreApplication::applicationDirPath().toUtf8().constData());
                for(sint32 i = 1; i < 5; ++i)
                {
                    String AssetsPath = String::Format("Q:%s", QCoreApplication::applicationDirPath().toUtf8().constData());
                    for(sint32 j = 0; j < i; ++j) AssetsPath += "/..";
                    if(Platform::File::ExistForDir(AssetsPath + "/assets"))
                    {
                        NewPath = AssetsPath + "/assets/";
                        break;
                    }
                }
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
                for(sint32 i = 1; i < 5; ++i)
                {
                    String AssetsPath = QCoreApplication::applicationDirPath().toUtf8().constData();
                    for(sint32 j = 0; j < i; ++j) AssetsPath += "/..";
                    if(Platform::File::ExistForDir(AssetsPath + "/assets"))
                    {
                        NewPath = AssetsPath;
                        break;
                    }
                }
            #elif BOSS_LINUX
                String NewPath = String::Format("Q:%s/..", QCoreApplication::applicationDirPath().toUtf8().constData());
                for(sint32 i = 1; i < 5; ++i)
                {
                    String AssetsPath = String::Format("Q:%s", QCoreApplication::applicationDirPath().toUtf8().constData());
                    for(sint32 j = 0; j < i; ++j) AssetsPath += "/..";
                    if(Platform::File::ExistForDir(AssetsPath + "/assets"))
                    {
                        NewPath = AssetsPath;
                        break;
                    }
                }
            #elif BOSS_MAC_OSX
                String NewPath = String::Format("Q:%s/../../../..", QCoreApplication::applicationDirPath().toUtf8().constData());
            #else
                String NewPath = String::Format("Q:%s", QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).value(0).toUtf8().constData());
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
            id_socket Result = nullptr;
            SocketBox::Create(Result, "TCP");
            return Result;
        }

        id_socket Platform::Socket::OpenForUDP()
        {
            id_socket Result = nullptr;
            SocketBox::Create(Result, "UDP");
            return Result;
        }

        id_socket Platform::Socket::OpenForWS(bool use_wss)
        {
            id_socket Result = nullptr;
            SocketBox::Create(Result, (use_wss)? "WSS" : "WS");
            return Result;
        }

        bool Platform::Socket::Close(id_socket socket, sint32 timeout)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(!CurSocketBox) return false;

            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::TCP:
            case SocketBox::Type::UDP:
                {
                    bool Result = (CurSocketBox->m_socket->error() == QAbstractSocket::UnknownSocketError);
                    if(CurSocketBox->m_socket->state() == QAbstractSocket::ConnectedState)
                    {
                        CurSocketBox->m_socket->disconnectFromHost();
                        Result &= (CurSocketBox->m_socket->state() == QAbstractSocket::UnconnectedState ||
                            CurSocketBox->m_socket->waitForDisconnected(timeout));
                    }
                    SocketBox::Remove(socket);
                    return Result;
                }
                break;
            case SocketBox::Type::WS:
            case SocketBox::Type::WSS:
                {
                    bool Result = (CurSocketBox->m_wsocket->error() == QAbstractSocket::UnknownSocketError);
                    if(CurSocketBox->m_wsocket->state() == QAbstractSocket::ConnectedState)
                        CurSocketBox->m_wsocket->disconnect();
                    SocketBox::Remove(socket);
                    return Result;
                }
                break;
            }
            return false;
        }

        bool Platform::Socket::Connect(id_socket socket, chars domain, uint16 port, sint32 timeout)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(!CurSocketBox) return false;

            ConnectAsync(socket, domain, port);
            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::TCP:
                {
                    bool Result = CurSocketBox->m_socket->waitForConnected(timeout);
                    BOSS_TRACE("Connect-TCP(%s:%d)%s", domain, (sint32) port, Result? "" : " - Failed");
                    return Result;
                }
                break;
            case SocketBox::Type::UDP:
                {
                    BOSS_TRACE("Connect-UDP(%s:%d)", domain, (sint32) port);
                    return true;
                }
                break;
            case SocketBox::Type::WS:
            case SocketBox::Type::WSS:
                {
                    bool Result = true;
                    const uint64 WaitForMsec = Platform::Utility::CurrentTimeMsec() + timeout;
                    while(CurSocketBox->m_wsocket->state() != QAbstractSocket::ConnectedState)
                    {
                        Platform::Utility::Sleep(1, false, true, false);
                        if(WaitForMsec < Platform::Utility::CurrentTimeMsec())
                        {
                            Result = false;
                            break;
                        }
                    }
                    BOSS_TRACE("Connect-WS(%s:%d)%s", domain, (sint32) port, Result? "" : " - Failed");
                    return Result;
                }
                break;
            }
            return false;
        }

        void Platform::Socket::ConnectAsync(id_socket socket, chars domain, uint16 port)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(CurSocketBox)
            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::TCP:
                {
                    CurSocketBox->m_socket->connectToHost(QString::fromUtf8(domain, -1), port);
                }
                break;
            case SocketBox::Type::UDP:
                {
                    // GetHostByName를 사용하지 않고 빠르게 IP구하기
                    sint32 FastIP[4] = {0, 0, 0, 0};
                    sint32 FastIPFocus = 0;
                    for(chars DomainFocus = domain; *DomainFocus != '\0'; ++DomainFocus)
                    {
                        if('0' <= *DomainFocus && *DomainFocus <= '9')
                        {
                            FastIP[FastIPFocus] = FastIP[FastIPFocus] * 10;
                            FastIP[FastIPFocus] += *DomainFocus - '0';
                            if(FastIP[FastIPFocus] <= 255)
                                continue;
                        }
                        else if(*DomainFocus == '.' && ++FastIPFocus < 4)
                            continue;
                        FastIPFocus = -1;
                        break;
                    }

                    if(FastIPFocus == 3)
                    {
                        quint32 ip4Address =
                            ((FastIP[0] & 0xFF) << 24) |
                            ((FastIP[1] & 0xFF) << 16) |
                            ((FastIP[2] & 0xFF) <<  8) |
                            ((FastIP[3] & 0xFF) <<  0);
                        CurSocketBox->m_udpip.setAddress(ip4Address);
                    }
                    else
                    {
                        Hostent* CurHostent = (Hostent*) GetHostByName(domain);
                        quint32 ip4Address = *((quint32*) CurHostent->h_addr_list[0]);
                        CurSocketBox->m_udpip.setAddress(ip4Address);
                    }
                    CurSocketBox->m_udpport = port;
                }
                break;
            case SocketBox::Type::WS:
                {
                    const String UrlText = String::Format("ws://%s:%d", domain, (sint32) port);
                    CurSocketBox->m_wsocket->open(QUrl((chars) UrlText));
                }
                break;
            case SocketBox::Type::WSS:
                {
                    const String UrlText = String::Format("wss://%s:%d", domain, (sint32) port);
                    CurSocketBox->m_wsocket->open(QUrl((chars) UrlText));
                    #ifndef QT_NO_SSL
                        CurSocketBox->m_wsocket->ignoreSslErrors();
                    #endif
                }
                break;
            }
        }

        bool Platform::Socket::IsConnected(id_socket socket)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(CurSocketBox)
            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::TCP:
                return (CurSocketBox->m_socket->state() == QAbstractSocket::ConnectedState);
            case SocketBox::Type::UDP:
                return true;
            case SocketBox::Type::WS:
            case SocketBox::Type::WSS:
                return (CurSocketBox->m_wsocket->state() == QAbstractSocket::ConnectedState);
            }
            return false;
        }

        bool Platform::Socket::Disconnect(id_socket socket, sint32 timeout)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(!CurSocketBox) return false;

            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::TCP:
                {
                    CurSocketBox->m_socket->abort();
                    bool Result = (CurSocketBox->m_socket->state() == QAbstractSocket::UnconnectedState ||
                        CurSocketBox->m_socket->waitForDisconnected(timeout));
                    BOSS_TRACE("Disconnect-TCP()%s", Result? "" : " - Failed");
                    return Result;
                }
                break;
            case SocketBox::Type::UDP:
                {
                    CurSocketBox->m_udpip.clear();
                    CurSocketBox->m_udpport = 0;
                    BOSS_TRACE("Disconnect-UDP()");
                    return true;
                }
                break;
            case SocketBox::Type::WS:
            case SocketBox::Type::WSS:
                {
                    CurSocketBox->m_wsocket->abort();
                    BOSS_TRACE("Disconnect-WS()");
                    return true;
                }
                break;
            }
            return false;
        }

        bool Platform::Socket::BindForUdp(id_socket socket, uint16 port, sint32 timeout)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(!CurSocketBox) return false;

            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::UDP:
                {
                    auto UdpSocket = (QUdpSocket*) CurSocketBox->m_socket;
                    bool Result = UdpSocket->bind(port);
                    BOSS_TRACE("BindForUdp-UDP()%s", Result? "" : " - Failed");
                    return Result;
                }
                break;
            }
            return false;
        }

        sint32 Platform::Socket::RecvAvailable(id_socket socket)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(!CurSocketBox || !CurSocketBox->CheckState("RecvAvailable"))
                return -1;

            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::TCP:
                {
                    return CurSocketBox->m_socket->bytesAvailable();
                }
                break;
            case SocketBox::Type::UDP:
                {
                    auto UdpSocket = (QUdpSocket*) CurSocketBox->m_socket;
                    return (UdpSocket->hasPendingDatagrams())? UdpSocket->pendingDatagramSize() : 0;
                }
                break;
            case SocketBox::Type::WS:
            case SocketBox::Type::WSS:
                {
                    return CurSocketBox->m_wbytes.length();
                }
                break;
            }
            return -1;
        }

        sint32 Platform::Socket::Recv(id_socket socket, uint08* data, sint32 size, sint32 timeout, ip4address* ip_udp, uint16* port_udp)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(!CurSocketBox || !CurSocketBox->CheckState("Recv"))
                return -1;

            sint32 Result = -1;
            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::TCP:
                {
                    if(CurSocketBox->m_socket->bytesAvailable() == 0 &&
                        !CurSocketBox->m_socket->waitForReadyRead(timeout))
                    {
                        BOSS_TRACE("Recv-TCP(-10035) - WSAEWOULDBLOCK");
                        return -10035; // WSAEWOULDBLOCK
                    }
                    Result = CurSocketBox->m_socket->read((char*) data, size);
                    BOSS_TRACE("Recv-TCP(%d)%s", Result, (0 <= Result)? "" : " - Failed");
                }
                break;
            case SocketBox::Type::UDP:
                {
                    auto UdpSocket = (QUdpSocket*) CurSocketBox->m_socket;
                    if(ip_udp || port_udp)
                    {
                        QHostAddress GetIP;
                        quint16 GetPort = 0;
                        Result = UdpSocket->readDatagram((char*) data, size, &GetIP, &GetPort);
                        if(ip_udp)
                        {
                            auto IPv4Address = GetIP.toIPv4Address();
                            ip_udp->ip[0] = (IPv4Address >> 24) & 0xFF;
                            ip_udp->ip[1] = (IPv4Address >> 16) & 0xFF;
                            ip_udp->ip[2] = (IPv4Address >>  8) & 0xFF;
                            ip_udp->ip[3] = (IPv4Address >>  0) & 0xFF;
                        }
                        if(port_udp) *port_udp = GetPort;
                    }
                    else Result = UdpSocket->readDatagram((char*) data, size);
                    BOSS_TRACE("Recv-UDP(%d)%s", Result, (0 <= Result)? "" : " - Failed");
                }
                break;
            case SocketBox::Type::WS:
            case SocketBox::Type::WSS:
                {
                    Result = Math::Min(size, CurSocketBox->m_wbytes.size());
                    Memory::Copy(data, CurSocketBox->m_wbytes.constData(), Result);
                    CurSocketBox->m_wbytes.remove(0, Result);
                }
                break;
            }
            return Result;
        }

        sint32 Platform::Socket::Send(id_socket socket, bytes data, sint32 size, sint32 timeout, bool utf8)
        {
            SocketBox* CurSocketBox = SocketBox::Access(socket);
            if(!CurSocketBox || !CurSocketBox->CheckState("Send"))
                return -1;

            switch(CurSocketBox->m_type)
            {
            case SocketBox::Type::TCP:
                {
                    if(CurSocketBox->m_socket->write((chars) data, size) < size ||
                        !CurSocketBox->m_socket->waitForBytesWritten(timeout))
                    {
                        BOSS_TRACE("Send-TCP(-1) - Failed");
                        return -1;
                    }
                    BOSS_TRACE("Send-TCP(%d)", size);
                }
                break;
            case SocketBox::Type::UDP:
                {
                    auto UdpSocket = (QUdpSocket*) CurSocketBox->m_socket;
                    if(UdpSocket->writeDatagram((chars) data, size, CurSocketBox->m_udpip, CurSocketBox->m_udpport) < size)
                    {
                        BOSS_TRACE("Send-UDP(-1) - Failed");
                        return -1;
                    }
                    BOSS_TRACE("Send-UDP(%d)", size);
                }
                break;
            case SocketBox::Type::WS:
            case SocketBox::Type::WSS:
                {
                    if(utf8)
                    {
                        if(CurSocketBox->m_wsocket->sendTextMessage(QString::fromUtf8((chars) data, size)) != size)
                        {
                            BOSS_TRACE("Send-WS-Text(-1) - Failed");
                            return -1;
                        }
                        BOSS_TRACE("Send-WS-Text(%d)", size);
                    }
                    else
                    {
                        if(CurSocketBox->m_wsocket->sendBinaryMessage(QByteArray((chars) data, size)) != size)
                        {
                            BOSS_TRACE("Send-WS-Binary(-1) - Failed");
                            return -1;
                        }
                        BOSS_TRACE("Send-WS-Binary(%d)", size);
                    }
                }
                break;
            }
            return size;
        }

        void* Platform::Socket::GetHostByName(chars name)
        {
            const QHostInfo& HostInfo = QHostInfo::fromName(name);
            const sint32 HostAddressCount = HostInfo.addresses().size();
            if(HostAddressCount == 0) return nullptr;

            Hostent& LastHostent = *BOSS_STORAGE_SYS(Hostent);
            LastHostent.Clear();

            // h_name
            String HostName = HostInfo.hostName().toUtf8().constData();
            char* NewName = new char[HostName.Length() + 1];
            Memory::Copy(NewName, (chars) HostName, HostName.Length() + 1);
            LastHostent.h_name = NewName;

            // h_addr_list
            LastHostent.h_addr_list = new bytes[HostAddressCount + 1];
            LastHostent.h_addr_list[HostAddressCount] = nullptr;
            for(sint32 i = 0; i < HostAddressCount; ++i)
            {
                const QHostAddress& CurAddress = HostInfo.addresses().at(i);
                const uint32 IPv4 = CurAddress.toIPv4Address();
                uint08* NewIPv4 = new uint08[4];
                Memory::Copy(NewIPv4, &IPv4, 4);
                LastHostent.h_addr_list[i] = NewIPv4;
            }
            return &LastHostent;
        }

        void* Platform::Socket::GetServByName(chars name, chars proto)
        {
            Servent& LastServent = *BOSS_STORAGE_SYS(Servent);
            LastServent.Clear();

            if(!LastServent.s_name)
            {
                // s_name
                String ServName = name;
                char* NewName = new char[ServName.Length() + 1];
                Memory::Copy(NewName, (chars) ServName, ServName.Length() + 1);
                LastServent.s_name = NewName;

                // s_port
                LastServent.s_port = (sint16) Parser(ServName).ReadInt();
            }
            return &LastServent;
        }

        ip4address Platform::Socket::GetLocalAddress(ip6address* ip6)
        {
            ip4address Result;
            Result.ip[0] = 127;
            Result.ip[1] = 0;
            Result.ip[2] = 0;
            Result.ip[3] = 1;
            #if !BOSS_WASM
                for(const QHostAddress& CurAddress : QNetworkInterface::allAddresses())
                {
                    if(CurAddress != QHostAddress(QHostAddress::LocalHost))
                    {
                        if(CurAddress.protocol() == QAbstractSocket::IPv4Protocol)
                        {
                            auto IPv4Address = CurAddress.toIPv4Address();
                            Result.ip[0] = (IPv4Address >> 24) & 0xFF;
                            Result.ip[1] = (IPv4Address >> 16) & 0xFF;
                            Result.ip[2] = (IPv4Address >>  8) & 0xFF;
                            Result.ip[3] = (IPv4Address >>  0) & 0xFF;
                        }
                        else if(ip6 && CurAddress.protocol() == QAbstractSocket::IPv6Protocol)
                        {
                            auto IPv6Address = CurAddress.toIPv6Address();
                            *ip6 = *((ip6address*) &IPv6Address);
                        }
                    }
                }
            #endif
            return Result;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SERVER
        ////////////////////////////////////////////////////////////////////////////////
        id_server Platform::Server::CreateTCP(bool sizefield)
        {
            auto NewServer = (TCPServerClass*) Buffer::AllocNoConstructorOnce<TCPServerClass>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewServer, 0, TCPServerClass, sizefield);
            return (id_server)(ServerClass*) NewServer;
        }

        id_server Platform::Server::CreateWS(chars name, bool use_wss)
        {
            auto NewServer = (WSServerClass*) Buffer::AllocNoConstructorOnce<WSServerClass>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewServer, 0, WSServerClass, name, use_wss);
            return (id_server)(ServerClass*) NewServer;
        }

        void Platform::Server::Release(id_server server)
        {
            Buffer::Free((buffer) server);
        }

        bool Platform::Server::Listen(id_server server, uint16 port)
        {
            if(auto CurAgent = (ServerClass*) server)
                return CurAgent->Listen(port);
            return false;
        }

        bool Platform::Server::TryNextPacket(id_server server)
        {
            if(auto CurAgent = (ServerClass*) server)
                return CurAgent->TryPacket();
            return false;
        }

        packettype Platform::Server::GetPacketType(id_server server)
        {
            if(auto CurAgent = (ServerClass*) server)
            {
                auto FocusedPacket = CurAgent->GetFocusedPacket();
                return FocusedPacket->Type;
            }
            return packettype_null;
        }

        sint32 Platform::Server::GetPacketPeerID(id_server server)
        {
            if(auto CurAgent = (ServerClass*) server)
            {
                auto FocusedPacket = CurAgent->GetFocusedPacket();
                return FocusedPacket->PeerID;
            }
            return -1;
        }

        bytes Platform::Server::GetPacketBuffer(id_server server, sint32* getsize)
        {
            if(auto CurAgent = (ServerClass*) server)
            {
                auto FocusedPacket = CurAgent->GetFocusedPacket();
                if(getsize) *getsize = Buffer::CountOf(FocusedPacket->Buffer);
                return (bytes) FocusedPacket->Buffer;
            }
            return nullptr;
        }

        bool Platform::Server::SendToPeer(id_server server, sint32 peerid, const void* buffer, sint32 buffersize, bool utf8)
        {
            if(auto CurAgent = (ServerClass*) server)
                return CurAgent->SendPacket(peerid, buffer, buffersize, utf8);
            return false;
        }

        bool Platform::Server::KickPeer(id_server server, sint32 peerid)
        {
            if(auto CurAgent = (ServerClass*) server)
                return CurAgent->KickPeer(peerid);
            return false;
        }

        bool Platform::Server::GetPeerInfo(id_server server, sint32 peerid, ip4address* ip4, ip6address* ip6, uint16* port)
        {
            if(auto CurAgent = (ServerClass*) server)
                return CurAgent->GetPeerAddress(peerid, ip4, ip6, port);
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // PIPE
        ////////////////////////////////////////////////////////////////////////////////
        id_pipe Platform::Pipe::Open(chars name, EventCB cb, payload data)
        {
            // 서버
            SharedMemoryClass* Semaphore = new SharedMemoryClass(name);
            if(!Semaphore->attach() && Semaphore->create(1))
            {
                QLocalServer* NewServer = new QLocalServer();
                if(NewServer->listen(name))
                    return (id_pipe)(PipeClass*) new PipeServerClass(NewServer, Semaphore, cb, data);
                delete NewServer;
            }
            delete Semaphore;

            // 클라이언트
            Semaphore = new SharedMemoryClass((chars) (String(name) + ".client"));
            if(!Semaphore->attach() && Semaphore->create(1))
                return (id_pipe)(PipeClass*) new PipeClientClass(name, Semaphore, cb, data);
            delete Semaphore;
            return nullptr;
        }

        void Platform::Pipe::Close(id_pipe pipe)
        {
            auto OldPipe = (PipeClass*) pipe;
            delete OldPipe;
        }

        bool Platform::Pipe::IsServer(id_pipe pipe)
        {
            auto CurPipe = (PipeClass*) pipe;
            if(!CurPipe) return false;
            return CurPipe->IsServer();
        }

        ConnectStatus Platform::Pipe::Status(id_pipe pipe)
        {
            auto CurPipe = (PipeClass*) pipe;
            if(!CurPipe) return CS_Disconnected;
            return CurPipe->Status();
        }

        sint32 Platform::Pipe::RecvAvailable(id_pipe pipe)
        {
            auto CurPipe = (PipeClass*) pipe;
            if(!CurPipe) return 0;
            return CurPipe->RecvAvailable();
        }

        sint32 Platform::Pipe::Recv(id_pipe pipe, uint08* data, sint32 size)
        {
            auto CurPipe = (PipeClass*) pipe;
            if(!CurPipe) return 0;
            return CurPipe->Recv(data, size);
        }

        const Context* Platform::Pipe::RecvJson(id_pipe pipe)
        {
            auto CurPipe = (PipeClass*) pipe;
            if(!CurPipe) return nullptr;
            return CurPipe->RecvJson();
        }

        bool Platform::Pipe::Send(id_pipe pipe, bytes data, sint32 size)
        {
            auto CurPipe = (PipeClass*) pipe;
            if(!CurPipe) return false;
            return CurPipe->Send(data, size);
        }

        bool Platform::Pipe::SendJson(id_pipe pipe, const String& json)
        {
            auto CurPipe = (PipeClass*) pipe;
            if(!CurPipe) return false;
            return CurPipe->SendJson(json);
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
            #ifdef QT_HAVE_SERIALPORT
                return SerialClass::EnumDevice(spec);
            #endif
            return Strings();
        }

        id_serial Platform::Serial::Open(chars name, sint32 baudrate, SerialDecodeCB dec, SerialEncodeCB enc)
        {
            #ifdef QT_HAVE_SERIALPORT
                SerialClass* NewSerial = new SerialClass(name, baudrate);
                if(NewSerial->IsValid())
                    return (id_serial) NewSerial;
                delete NewSerial;
            #endif
            return nullptr;
        }

        void Platform::Serial::Close(id_serial serial)
        {
            #ifdef QT_HAVE_SERIALPORT
                delete (SerialClass*) serial;
            #endif
        }

        bool Platform::Serial::Connected(id_serial serial)
        {
            #ifdef QT_HAVE_SERIALPORT
                if(serial)
                {
                    SerialClass* CurSerial = (SerialClass*) serial;
                    return CurSerial->Connected();
                }
            #endif
            return false;
        }

        bool Platform::Serial::ReadReady(id_serial serial, sint32* gettype)
        {
            #ifdef QT_HAVE_SERIALPORT
                if(serial)
                {
                    SerialClass* CurSerial = (SerialClass*) serial;
                    return CurSerial->ReadReady();
                }
            #endif
            return false;
        }

        sint32 Platform::Serial::ReadAvailable(id_serial serial)
        {
            #ifdef QT_HAVE_SERIALPORT
                if(serial)
                {
                    SerialClass* CurSerial = (SerialClass*) serial;
                    return CurSerial->ReadAvailable();
                }
            #endif
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

        sint32 Platform::Serial::ReadData(id_serial serial, uint08* data, const sint32 size)
        {
            #ifdef QT_HAVE_SERIALPORT
                if(serial)
                {
                    SerialClass* CurSerial = (SerialClass*) serial;
                    return CurSerial->ReadData(data, size);
                }
            #endif
            return 0;
        }

        bool Platform::Serial::WriteData(id_serial serial, bytes data, const sint32 size)
        {
            #ifdef QT_HAVE_SERIALPORT
                if(serial)
                {
                    SerialClass* CurSerial = (SerialClass*) serial;
                    CurSerial->WriteData(data, size);
                    return true;
                }
            #endif
            return false;
        }

        void Platform::Serial::WriteFlush(id_serial serial, sint32 type)
        {
            #ifdef QT_HAVE_SERIALPORT
                if(serial)
                {
                    SerialClass* CurSerial = (SerialClass*) serial;
                    CurSerial->WriteFlush();
                }
            #endif
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
