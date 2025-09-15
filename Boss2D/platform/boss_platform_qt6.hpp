#pragma once
#include <platform/boss_platform.hpp>
#include <platform/boss_platform_impl.hpp>

#undef size_t
#undef ssize_t

#ifdef BOSS_PLATFORM_QT6

    #include <QApplication>
    #include <QCoreApplication>
    #include <QGuiApplication>
    #include <QScreen>
    #include <QTimer>
    #include <QDateTime>
    #include <QDir>
    #include <QFile>
    #include <QFileInfo>
    #include <QFileDialog>
    #include <QInputDialog>
    #include <QColorDialog>
    #include <QThread>
    #include <QDateTime>
    #include <QStandardPaths>
    #include <QRandomGenerator>
    #include <QClipboard>
    #include <QFontDatabase>
    #include <QMessageBox>
    #include <QSettings>
    #if !BOSS_WASM
        #include <QException>
    #endif

    #ifdef QT_HAVE_GRAPHICS
        #include <QMainWindow>
        #include <QWindow>
        #include <QPainter>
        #include <QPainterPath>
        #include <QCloseEvent>
        #include <QResizeEvent>
        #include <QPaintEvent>
        #include <QMouseEvent>
        #include <QWheelEvent>
        #include <QKeyEvent>
        #include <QLineEdit>
        #include <QListWidget>
        #include <QToolTip>
        #include <QSplashScreen>
        #include <QStackedLayout>
        #include <QGridLayout>
    #endif

    #include <QLocalSocket>
    #include <QLocalServer>
    #include <QSharedMemory>
    #include <QHostInfo>
    #include <QTcpSocket>
    #include <QUdpSocket>
    #include <QWebSocket>
    #include <QNetworkInterface>
    #include <QSslCertificate>
    #include <QSslKey>
    #include <QTcpServer>
    #include <QWebSocketServer>

    #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
        #include <QCefContext.h>
        #include <QCefView.h>
    #endif

    #ifdef QT_HAVE_SERIALPORT
        #include <QSerialPort>
        #include <QSerialPortInfo>
        #if BOSS_LINUX
            #include <libudev.h>
            #include <QSocketNotifier>
        #endif
    #endif

    #if BOSS_WINDOWS
        #include <Windows.h>
        #include <Dbt.h>
    #endif

    #define USER_FRAMECOUNT (60)
    extern h_view g_view;

    class FTFontClass
    {
    public:
        FTFontClass()
        {
            mHeight = 0;
        }
        ~FTFontClass()
        {
        }
        FTFontClass(const FTFontClass& rhs)
        {
            operator=(rhs);
        }
        FTFontClass& operator=(const FTFontClass& rhs)
        {
            mNickName = ToReference(rhs.mNickName);
            mHeight = ToReference(rhs.mHeight);
            return *this;
        }

    public:
        String mNickName;
        sint32 mHeight;
    };

    class CanvasClass
    {
    public:
        CanvasClass();
        CanvasClass(QPaintDevice* device);
        ~CanvasClass();

    public:
        void Bind(QPaintDevice* device);
        void Unbind();

    private:
        void BindCore(QPaintDevice* device);
        void UnbindCore();

    public:
        // Getter
        static inline bool enabled() {return (ST() != ST_FIRST());}
        static inline CanvasClass* get() {return ST();}
        inline QPainter& painter() {return mPainter;}
        inline sint32 painter_width() const {return mPainterWidth;}
        inline sint32 painter_height() const {return mPainterHeight;}
        inline double zoom() const {return mPainter.transform().m11();}
        inline const QRect& scissor() const {return mScissor;}
        inline const QColor& color() const {return mColor;}
        inline const ShaderRole& shader() const {return mShader;}
        // Setter
        inline void SetFont(chars name, sint32 size)
        {
            const String FamilyAndStyle(name);
            const sint32 DotPos = FamilyAndStyle.Find(0, ".");
            if(DotPos != -1)
                mPainter.setFont(QFontDatabase::font((chars) FamilyAndStyle.Left(DotPos),
                    (chars) FamilyAndStyle.Offset(DotPos + 1), size));
            else mPainter.setFont(QFontDatabase::font(name, "", size));
        }
        inline void SetScissor(sint32 l, sint32 t, sint32 r, sint32 b)
        {
            mScissor = QRect(l, t, r - l, b - t);
            mPainter.setClipRect(mScissor);
        }
        inline void SetColor(uint08 r, uint08 g, uint08 b, uint08 a)
        {mColor.setRgb(r, g, b, a);}
        inline void SetMask(QPainter::CompositionMode mask)
        {mPainter.setCompositionMode(mask);}
        inline void SetShader(ShaderRole role)
        {mShader = role;}

    private:
        static inline CanvasClass* ST_FIRST() {static CanvasClass _; return &_;}
        static inline CanvasClass*& ST() {static CanvasClass* _ = ST_FIRST(); return _;}

    private: // 서피스관련
        const bool mIsTypeSurface;
        bool mIsSurfaceBinded;

    private: // 페인터관련
        CanvasClass* mSavedCanvas;
        ZoomState mSavedZoom;
        QFont mSavedFont;
        QPainter::CompositionMode mSavedMask;
        QPainter mPainter;
        sint32 mPainterWidth;
        sint32 mPainterHeight;
    private: // 오리지널옵션
        QRect mScissor;
        QColor mColor;
        ShaderRole mShader;
    };

    class MainView : public QWidget
    {
        Q_OBJECT

    public:
        enum WidgetRequest {WR_Null, WR_NeedExit = -1, WR_NeedHide = -2, WR_NeedShow = -3, WR_NeedShowBy = -4};

    public:
        MainView(QWidget* parent) : QWidget(parent)
        {
            #if BOSS_WINDOWS
                setAttribute(Qt::WA_PaintOnScreen);
            #endif
            setAttribute(Qt::WA_NoSystemBackground);
            setAttribute(Qt::WA_AcceptTouchEvents);
            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);

            connect(&mTickTimer, &QTimer::timeout, this, &MainView::TickTimeout);
            connect(&mUpdateTimer, &QTimer::timeout, this, &MainView::UpdateTimeout);
            connect(&mTooltipTimer, &QTimer::timeout, this, &MainView::TooltipTimeout);
            connect(&mLongpressTimer, &QTimer::timeout, this, &MainView::LongpressTimeout);
        }
        ~MainView()
        {
            if(mViewManager)
            {
                SendDestroy();
                mViewManager->SetView(h_view::null());
                delete mViewManager;
            }
        }

    public:
        h_view ChangeViewManager(View* manager)
        {
            if(mViewManager)
            {
                SendDestroy();
                mViewManager->SetView(h_view::null());
                delete mViewManager;
            }
            mViewManager = manager;
            if(mViewManager)
            {
                g_view = h_view::create_by_ptr(BOSS_DBG manager);
                mViewManager->SetView(g_view);
                mViewManager->SetCallback(
                    [](payload data, sint32 count, chars arg)->void
                    {((MainView*) data)->Update(count, arg);}, (payload) this);
                SendCreate();
                SendSizeWhenValid();
                return g_view;
            }
            return h_view::null();
        }

    private:
        void SendCreate()
        {
            if(mViewManager)
                mViewManager->OnCreate();
            mTickTimer.start(1000 / USER_FRAMECOUNT);
        }
        bool CanQuit()
        {
            if(mViewManager)
                return mViewManager->OnCanQuit();
            return true;
        }
        void SendDestroy()
        {
            mTickTimer.stop();
            if(mViewManager)
                mViewManager->OnDestroy();
        }
        void SendActivate(bool actived)
        {
            if(mViewManager)
                mViewManager->OnActivate(actived);
        }
        void SendDeviceArrival(bool connected)
        {
            if(mViewManager)
                mViewManager->OnDeviceArrival(connected);
        }
        void SendSizeWhenValid()
        {
            if(mViewManager && 0 < mWidth && 0 < mHeight)
                mViewManager->OnSize(mWidth, mHeight);
        }
        void Update(sint32 count, chars arg)
        {
            if(count <= WR_NeedExit)
            {
                mRequest = (WidgetRequest) count;
                if(arg) mRequestArg = arg;
            }
            else if(mPaintCount < count)
            {
                if(mPaintCount == 0)
                    mUpdateTimer.start(1000 / USER_FRAMECOUNT);
                mPaintCount = count;
            }
        }
        static bool CloseAllWindows()
        {
            QList<QWindow*> processedWindows;
            while(auto w = QApplication::activeModalWidget())
            {
                if(QWindow* window = w->windowHandle())
                {
                    if(!window->close()) return false;
                    processedWindows.append(window);
                }
            }
            for(auto w : QApplication::topLevelWidgets())
            {
                if(w->windowType() == Qt::Desktop)
                    continue;
                if(QWindow* window = w->windowHandle())
                {
                    if(!window->close()) return false;
                    processedWindows.append(window);
                }
            }
            return true;
        }

    public:
        void OnActivateEvent(bool actived)
        {
            SendActivate(actived);
        }
        void OnDeviceArrivalEvent(bool connected)
        {
            SendDeviceArrival(connected);
        }
        void OnCloseEvent(QCloseEvent* event)
        {
            Platform::Popup::CloseAllTracker();
            if(!mViewManager || mViewManager->OnCanQuit())
            {
                event->accept();
                CloseAllWindows();
            }
            else event->ignore();
        }
        void ForcedKeyClick(sint32 code, chars text)
        {
            if(mViewManager)
            {
                mViewManager->OnKey(code, text, true);
                mViewManager->OnKey(code, text, false);
            }
        }

    protected:
        void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE
        {
            mWidth = event->size().width();
            mHeight = event->size().height();
            SendSizeWhenValid();
        }
        void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE
        {
            // for assert dialog's recursive call
            if(CanvasClass::enabled()) return;
            CanvasClass Canvas(this);
            if(mViewManager)
                mViewManager->OnRender(mWidth, mHeight, 0, 0, mWidth, mHeight);

            /*if(m_next_manager)
            {
                h_view OldViewHandle = changeViewManagerAndDestroy(m_next_manager);
                setViewAndCreateAndSize(OldViewHandle);
                m_next_manager = nullptr;
            }*/
            if(0 < mPaintCount)
                mPaintCount--;
        }
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            if(!mViewManager || mViewManager->OnCanQuit())
            {
                event->accept();
                setAttribute(Qt::WA_DeleteOnClose);
            }
            else event->ignore();
        }
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            if(!mViewManager) return;
            if(event->button() == Qt::LeftButton)
            {
                mViewManager->OnTouch(TT_Press, 0, event->position().x(), event->position().y());
                mLongpressTimer.start(500);
                mLongpressX = event->position().x();
                mLongpressY = event->position().y();
            }
            else if(event->button() == Qt::RightButton)
                mViewManager->OnTouch(TT_ExtendPress, 0, event->position().x(), event->position().y());
            else if(event->button() == Qt::MiddleButton)
                mViewManager->OnTouch(TT_WheelPress, 0, event->position().x(), event->position().y());
            mTooltipTimer.stop();
        }
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            mousePressEvent(event);
        }
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            if(!mViewManager) return;
            if(event->buttons() == Qt::NoButton)
            {
                mViewManager->OnTouch(TT_Moving, 0, event->position().x(), event->position().y());
                mTooltipTimer.start(300);
                Platform::Popup::HideToolTip();
            }
            else
            {
                if(event->buttons() & Qt::LeftButton)
                    mViewManager->OnTouch(TT_Dragging, 0, event->position().x(), event->position().y());
                if(event->buttons() & Qt::RightButton)
                    mViewManager->OnTouch(TT_ExtendDragging, 0, event->position().x(), event->position().y());
                if(event->buttons() & Qt::MiddleButton)
                    mViewManager->OnTouch(TT_WheelDragging, 0, event->position().x(), event->position().y());
            }
            if(5 < Math::Distance(mLongpressX, mLongpressY, event->position().x(), event->position().y()))
                mLongpressTimer.stop();
        }
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            if(!mViewManager) return;
            if(event->button() == Qt::LeftButton)
                mViewManager->OnTouch(TT_Release, 0, event->position().x(), event->position().y());
            else if(event->button() == Qt::RightButton)
                mViewManager->OnTouch(TT_ExtendRelease, 0, event->position().x(), event->position().y());
            else if(event->button() == Qt::MiddleButton)
                mViewManager->OnTouch(TT_WheelRelease, 0, event->position().x(), event->position().y());
            mLongpressTimer.stop();
        }
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE
        {
            if(!mViewManager) return;
            float WheelValue = event->angleDelta().y() / 120.0f;
            while (0 < WheelValue)
            {
                WheelValue = Math::MaxF(0, WheelValue - 1);
                mViewManager->OnTouch(TT_WheelUp, 0, event->position().x(), event->position().y());
            }
            while (WheelValue < 0)
            {
                WheelValue = Math::MinF(WheelValue + 1, 0);
                mViewManager->OnTouch(TT_WheelDown, 0, event->position().x(), event->position().y());
            }
        }
        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE
        {
            if(mViewManager && !event->isAutoRepeat())
            {
                #if BOSS_WASM
                    mViewManager->OnKey(event->key(), event->text().toUtf8().constData(), true);
                #else
                    mViewManager->OnKey(event->nativeVirtualKey(), event->text().toUtf8().constData(), true);
                #endif
            }
        }
        void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE
        {
            if(mViewManager && !event->isAutoRepeat())
            {
                #if BOSS_WASM
                    mViewManager->OnKey(event->key(), event->text().toUtf8().constData(), false);
                #else
                    mViewManager->OnKey(event->nativeVirtualKey(), event->text().toUtf8().constData(), false);
                #endif
            }
        }

    private:
        void TickTimeout()
        {
            // Procedure
            const uint64 LimitMsec = Platform::Utility::CurrentTimeMsec() + 20;
            do {PlatformImpl::Core::FlushProcedure();}
            while(PlatformImpl::Core::CallProcedures(LimitMsec - Platform::Utility::CurrentTimeMsec()));

            // Tick
            if(mRequest == WR_Null)
                mViewManager->OnTick();
            else
            {
                if(mRequest == WR_NeedExit)
                    QApplication::closeAllWindows();
                else if(mRequest == WR_NeedHide)
                    hide();
                else if(mRequest == WR_NeedShow)
                    show();
                else if(mRequest == WR_NeedShowBy)
                {
                    auto Args = String::Split(mRequestArg);
                    if(Args.Count() == 4)
                    {
                        setGeometry(
                            Parser::GetInt(Args[0]), Parser::GetInt(Args[1]),
                            Parser::GetInt(Args[2]), Parser::GetInt(Args[3]));
                        show();
                        activateWindow();
                        raise();
                    }
                }
                mRequest = WR_Null;
                mRequestArg.Empty();
            }
        }
        void UpdateTimeout()
        {
            if(mPaintCount == 0)
                mUpdateTimer.stop();
            update();
        }
        void TooltipTimeout()
        {
            mTooltipTimer.stop();
            point64 CursorPos;
            if(Platform::Utility::GetCursorPosInWindow(CursorPos))
                mViewManager->OnTouch(TT_ToolTip, 0, CursorPos.x, CursorPos.y);
        }
        void LongpressTimeout()
        {
            mLongpressTimer.stop();
            mViewManager->OnTouch(TT_LongPress, 0, mLongpressX, mLongpressY);
        }

    private:
        View* mViewManager {nullptr};
        sint32 mWidth {0};
        sint32 mHeight {0};
        QTimer mTickTimer;
        QTimer mUpdateTimer;
        QTimer mTooltipTimer;
        QTimer mLongpressTimer;
        sint32 mLongpressX {0};
        sint32 mLongpressY {0};
        sint32 mPaintCount {0};
        WidgetRequest mRequest {WR_Null};
        String mRequestArg;
    };

    #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
        class CefWebView : public QCefView
        {
            Q_OBJECT

        public:
            CefWebView(const QString url, const QCefSetting* setting, QWidget* parent = nullptr)
                : QCefView(url, setting, parent)
            {
                //QPalette pal = palette();
                //pal.setColor(QPalette::Window, Qt::green);
                //setPalette(pal);

                //setContextMenuPolicy(Qt::NoContextMenu);
                //setContextMenuPolicy(Qt::ActionsContextMenu);
                //setContextMenuPolicy(Qt::CustomContextMenu);
                //setContextMenuPolicy(Qt::PreventContextMenu);
                //setContextMenuPolicy(Qt::DefaultContextMenu);

                //setAcceptDrops(true);
                //setAllowDrag(true);

                connect(this, &QCefView::draggableRegionChanged, this, &CefWebView::onDraggableRegionChanged);
                connect(this, &QCefView::nativeBrowserCreated, this, &CefWebView::onNativeBrowserWindowCreated);
                connect(this, &QCefView::invokeMethod, this, &CefWebView::onInvokeMethod);
                connect(this, &QCefView::cefUrlRequest, this, &CefWebView::onQCefUrlRequest);
                connect(this, &QCefView::cefQueryRequest, this, &CefWebView::onQCefQueryRequest);
                connect(this, &QCefView::reportJavascriptResult, this, &CefWebView::onJavascriptResult);
                connect(this, &QCefView::loadStart, this, &CefWebView::onLoadStart);
                connect(this, &QCefView::loadEnd, this, &CefWebView::onLoadEnd);
                connect(this, &QCefView::loadError, this, &CefWebView::onLoadError);
            }
            ~CefWebView()
            {
            }

        public:
            void SendPythonStart(chars pid, chars filename, chars args)
            {
                executeJavascriptWithResult(QCefView::MainFrameID,
                    (chars) String::Format("boss_start('%s','%s',`%s`);", pid, filename, args), "", "BossChannel");
            }
            void SendPythonStop(chars pid)
            {
                executeJavascriptWithResult(QCefView::MainFrameID,
                    (chars) String::Format("boss_stop('%s');", pid), "", "BossChannel");
            }
            void SendPythonStopAll()
            {
                executeJavascriptWithResult(QCefView::MainFrameID,
                    "boss_stop_all();", "", "BossChannel");
            }
            void SendPythonCall(chars pid, chars func, chars args)
            {
                executeJavascriptWithResult(QCefView::MainFrameID,
                    (chars) String::Format("boss_call('%s','%s',%s);", pid, func, args), "", "BossChannel");
            }

        public:
            void load(const QUrl &url)
            {
                navigateToUrl(url.toEncoded());
            }
            void reload()
            {
                browserReload();
            }

        protected slots:
            void onScreenChanged(QScreen* screen)
            {
            }
            void onNativeBrowserWindowCreated(QWindow* window)
            {
                if(window)
                    connect(this->window()->windowHandle(), SIGNAL(screenChanged(QScreen*)), this, SLOT(onScreenChanged(QScreen*)));
            }
            void onDraggableRegionChanged(const QRegion& draggableRegion, const QRegion& nonDraggableRegion)
            {
                m_draggableRegion = draggableRegion;
                m_nonDraggableRegion = nonDraggableRegion;
            }
            void onInvokeMethod(const QCefBrowserId& browserId, const QCefFrameId& frameId, const QString& method, const QVariantList& arguments)
            {
                branch;
                // DOM처리
                jump(method == "boss_log")
                {
                    const String Text = arguments.value(0).toString().toUtf8().constData();
                    ((View*) g_view.get())->SendNotify(NT_WindowWeb, "PythonLog", Text, nullptr, false);
                }
                jump(method == "boss_set")
                {
                    Strings Args;
                    Args.AtAdding() = arguments.value(0).toString().toUtf8().constData(); // Key
                    Args.AtAdding() = arguments.value(1).toString().toUtf8().constData(); // Value
                    ((View*) g_view.get())->SendNotify(NT_WindowWeb, "PythonSet", Args, nullptr, false);
                }
                jump(method == "boss_setjson")
                {
                    Strings Args;
                    Args.AtAdding() = arguments.value(0).toString().toUtf8().constData(); // KeyHead
                    Args.AtAdding() = arguments.value(1).toString().toUtf8().constData(); // Json
                    ((View*) g_view.get())->SendNotify(NT_WindowWeb, "PythonSetJson", Args, nullptr, false);
                }
                jump(method == "boss_get")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const String Key = arguments.value(1).toString().toUtf8().constData();
                    id_cloned_share Out = nullptr;
                    ((View*) g_view.get())->SendNotify(NT_WindowWeb, "PythonGet", Key, &Out, true);
                    if(Out)
                    {
                        const String Value(Out);
                        executeJavascriptWithResult(frameId, (chars) String::Format(
                            "boss_get_return(%d,'%s');", CallID, (chars) Value), "", "BossChannel");
                    }
                    else executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_get_return(%d,'');", CallID), "", "BossChannel");
                }
                jump(method == "boss_rem")
                {
                    const String KeyHead = arguments.value(0).toString().toUtf8().constData();
                    ((View*) g_view.get())->SendNotify(NT_WindowWeb, "PythonRem", KeyHead, nullptr, false);
                }
                jump(method == "boss_gluecall")
                {
                    Strings Args;
                    Args.AtAdding() = arguments.value(0).toString().toUtf8().constData(); // Name
                    for(sint32 i = 1, iend = arguments.count(); i < iend; ++i)
                        Args.AtAdding() = arguments.value(i).toString().toUtf8().constData(); // Params
                    ((View*) g_view.get())->SendNotify(NT_WindowWeb, "PythonGlueCall", Args, nullptr, false);
                }
                jump(method == "boss_jumpcall")
                {
                    const String Name = arguments.value(0).toString().toUtf8().constData();
                    ((View*) g_view.get())->SendNotify(NT_WindowWeb, "PythonJumpCall", Name, nullptr, false);
                }
                // 파일처리
                jump(method == "boss_findfile")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const String DirPath = String("webpython/") + arguments.value(1).toString().toUtf8().constData();
                    Context Files;
                    if(auto NewPath = AssetPath::Create())
                    {
                        AssetPath::AddPath(NewPath, DirPath);
                        AssetPath::Search(NewPath, [](chars filepath, chars filename, payload data)->void
                            {
                                auto& Files = *((Context*) data);
                                Files.AtAdding().Set(filename);
                            }, &Files);
                        AssetPath::Release(NewPath);
                    }
                    if(0 < Files.LengthOfIndexable())
                        executeJavascriptWithResult(frameId, (chars) String::Format(
                            "boss_findfile_return(%d,%s);", CallID, (chars) Files.SaveJson()), "", "BossChannel");
                    else executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_findfile_return(%d,[]);", CallID), "", "BossChannel");
                }
                jump(method == "boss_pickupfile")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const WString FilterW = WString::FromChars(String(arguments.value(1).toString().toUtf8().constData()).Trim().Replace("/", ";*."));
                    const String FileTitle = String("webpython/") + arguments.value(2).toString().toUtf8().constData();
                    wchararray FilterScriptW;
                    if(FilterW.Length() == 0)
                        Memory::Copy(FilterScriptW.AtDumpingAdded(20), L"All Files(*.*)\0*.*\0", sizeof(wchar_t) * 20);
                    else
                    {
                        Memory::Copy(FilterScriptW.AtDumpingAdded(13), L"Some files\0*.", sizeof(wchar_t) * 13);
                        Memory::Copy(FilterScriptW.AtDumpingAdded(FilterW.Length()), (wchars) FilterW, sizeof(wchar_t) * FilterW.Length());
                        Memory::Copy(FilterScriptW.AtDumpingAdded(2), L"\0", sizeof(wchar_t) * 2);
                    }
                    String TargetFilePath;
                    String FilePath;
                    if(Platform::Popup::FileDialog(DST_FileOpen, FilePath, nullptr, "Please select a file", &FilterScriptW[0]))
                    if(auto OldFile = Platform::File::OpenForRead(FilePath))
                    {
                        auto ExtName = String::FromWChars(Platform::File::GetExtensionName(WString::FromChars(FilePath)));
                        auto FileSize = Platform::File::Size(OldFile);
                        auto NewBuffer = Buffer::Alloc(BOSS_DBG FileSize);
                        Platform::File::Read(OldFile, (uint08*) NewBuffer, FileSize);
                        Platform::File::Close(OldFile);
                        if(auto NewAsset = Asset::OpenForWrite(FileTitle + ExtName, true))
                        {
                            Asset::Write(NewAsset, (bytes) NewBuffer, FileSize);
                            Asset::Close(NewAsset);
                            TargetFilePath = FileTitle + ExtName;
                        }
                    }
                    executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_pickupfile_return(%d,'%s');", CallID, (chars) TargetFilePath), "", "BossChannel");
                }
                jump(method == "boss_infofile")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const String FilePath = String("webpython/") + arguments.value(1).toString().toUtf8().constData();
                    uint64 Size = 0, CTime = 0, ATime = 0, MTime = 0;
                    if(Asset::Exist(FilePath, nullptr, &Size, &CTime, &ATime, &MTime))
                        executeJavascriptWithResult(frameId, (chars) String::Format(
                            "boss_infofile_return(%d,{'exist':true,'size':%llu,'ctime':%llu,'atime':%llu,'mtime':%llu});",
                            CallID, Size, CTime, ATime, MTime), "", "BossChannel");
                    else executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_infofile_return(%d,{'exist':false});", CallID), "", "BossChannel");
                }
                jump(method == "boss_readfile")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const String FilePath = String("webpython/") + arguments.value(1).toString().toUtf8().constData();
                    if(auto NewBuffer = Asset::ToBuffer(FilePath))
                    {
                        const sint32 FileSize = Buffer::CountOf(NewBuffer);
                        executeJavascriptWithResult(frameId, (chars) String::Format("boss_readfile_return(%d,'%s');",
                            CallID, AddOn::Ssl::ToBASE64((bytes) NewBuffer, FileSize)), "", "BossChannel");
                        Buffer::Free(NewBuffer);
                    }
                    else executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_readfile_return(%d,'');", CallID), "", "BossChannel");
                }
                jump(method == "boss_writefile")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const String FilePath = String("webpython/") + arguments.value(1).toString().toUtf8().constData();
                    const String Base64 = arguments.value(2).toString().toUtf8().constData();
                    sint32 FileSize = 0;
                    if(auto NewBuffer = AddOn::Ssl::FromBASE64(Base64))
                    if(auto NewAsset = Asset::OpenForWrite(FilePath, true))
                    {
                        FileSize = Buffer::CountOf(NewBuffer);
                        Asset::Write(NewAsset, (bytes) NewBuffer, FileSize);
                        Asset::Close(NewAsset);
                    }
                    executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_writefile_return(%d,%d);", CallID, FileSize), "", "BossChannel");
                }
                jump(method == "boss_resetfile")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const String FilePath = String("webpython/") + arguments.value(1).toString().toUtf8().constData();
                    const bool Success = Asset::RemoveForWrite(FilePath, true);
                    executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_resetfile_return(%d,%s);", CallID, (Success)? "true" : "false"), "", "BossChannel");
                }
                jump(method == "boss_movefile")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const String OldFilePath = String("webpython/") + arguments.value(1).toString().toUtf8().constData();
                    const String NewFilePath = String("webpython/") + arguments.value(2).toString().toUtf8().constData();
                    const bool Success = Asset::RenameForWrite(OldFilePath, NewFilePath);
                    executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_movefile_return(%d,%s);", CallID, (Success)? "true" : "false"), "", "BossChannel");
                }
                jump(method == "boss_hashfile")
                {
                    const sint32 CallID = arguments.value(0).toInt();
                    const String FilePath = String("webpython/") + arguments.value(1).toString().toUtf8().constData();
                    if(auto NewBuffer = Asset::ToBuffer(FilePath))
                    {
                        const sint32 FileSize = Buffer::CountOf(NewBuffer);
                        executeJavascriptWithResult(frameId, (chars) String::Format("boss_hashfile_return(%d,'%s');",
                            CallID, AddOn::Ssl::ToSHA256((bytes) NewBuffer, FileSize, true)), "", "BossChannel");
                        Buffer::Free(NewBuffer);
                    }
                    else executeJavascriptWithResult(frameId, (chars) String::Format(
                        "boss_hashfile_return(%d,'');", CallID), "", "BossChannel");
                }
            }
            void onQCefUrlRequest(const QCefBrowserId& browserId, const QCefFrameId& frameId, const QString& url)
            {
            }
            void onQCefQueryRequest(const QCefBrowserId& browserId, const QCefFrameId& frameId, const QCefQuery& query)
            {
            }
            void onJavascriptResult(const QCefBrowserId& browserId, const QCefFrameId& frameId, const QString& context, const QVariant& result)
            {
            }
            void onLoadStart(const QCefBrowserId& browserId, const QCefFrameId& frameId, bool isMainFrame, int transitionType)
            {
            }
            void onLoadEnd(const QCefBrowserId& browserId, const QCefFrameId& frameId, bool isMainFrame, int httpStatusCode)
            {
            }
            void onLoadError(const QCefBrowserId& browserId, const QCefFrameId& frameId, bool isMainFrame, int errorCode, const QString& errorMsg, const QString& failedUrl)
            {
            }

        protected:
            bool onNewPopup(const QCefFrameId& sourceFrameId, const QString& targetUrl, QString& targetFrameName,
                QCefView::CefWindowOpenDisposition targetDisposition, QRect& rect, QCefSetting& settings, bool& disableJavascriptAccess) override
            {
                return false;
            }
            void onNewDownloadItem(const QSharedPointer<QCefDownloadItem>& item, const QString& suggestedName) override
            {
                //DownloadManager::getInstance().AddNewDownloadItem(item);
            }
            void onUpdateDownloadItem(const QSharedPointer<QCefDownloadItem>& item) override
            {
                //DownloadManager::getInstance().UpdateDownloadItem(item);
            }

        protected:
            void resizeEvent(QResizeEvent* event) override
            {
                QCefView::resizeEvent(event);
            }
            void mousePressEvent(QMouseEvent* event) override
            {
                QCefView::mousePressEvent(event);
            }

        private:
            QRegion m_draggableRegion;
            QRegion m_nonDraggableRegion;
        };
    #endif

    class GroupingWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        typedef std::function<void(bool active)> ActivatorCB;
        void SetActivator(ActivatorCB cb) {mActivatorCB = cb;}

    protected:
        void changeEvent(QEvent* event) Q_DECL_OVERRIDE
        {
            auto EventType = event->type();
            if(EventType == QEvent::ActivationChange)
                mActivatorCB(isActiveWindow());
            QWidget::changeEvent(event);
        }

    private:
        ActivatorCB mActivatorCB;
    };

    class MainWindow : public GroupingWindow
    {
        Q_OBJECT

    public:
        MainWindow(QApplication* app = nullptr) : mRefApplication(app)
        {
            setUnifiedTitleAndToolBarOnMac(true);
            mWindowRect.setRect(0, 0, 640, 480);

            #if defined(QT_HAVE_SERIALPORT) && BOSS_LINUX
                if(mUsbUdev = udev_new())
                if(mUsbUdevMon = udev_monitor_new_from_netlink(mUsbUdev, "udev"))
                {
                    udev_monitor_filter_add_match_subsystem_devtype(mUsbUdevMon, "usb", "usb_device");
                    if(0 <= udev_monitor_enable_receiving(mUsbUdevMon))
                    {
                        int UdevMonFD = udev_monitor_get_fd(mUsbUdevMon);
                        mUsbNotifier = new QSocketNotifier(UdevMonFD, QSocketNotifier::Read, this);
                        connect(mUsbNotifier, &QSocketNotifier::activated, this, &MainWindow::onUdevActivated);
                    }
                }
            #endif
        }
        ~MainWindow()
        {
            delete mView;
            delete mBgWindow;
            delete mWebWindow;

            #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
                delete mWebConfig;
                delete mWebContext;
            #endif

            #if defined(QT_HAVE_SERIALPORT) && BOSS_LINUX
                if(mUsbNotifier) mUsbNotifier->deleteLater();
                if(mUsbUdevMon) udev_monitor_unref(mUsbUdevMon);
                if(mUsbUdev) udev_unref(mUsbUdev);
            #endif
        }

    public:
        void SetInitedPlatform() {mInited = true;}
        bool InitedPlatform() const {return mInited;}
        void SetFirstVisible(bool visible) {mNeedVisible = visible;}
        bool FirstVisible() const {return mNeedVisible;}

    public:
        void InitForWidget(bool frameless, bool topmost, QWidget* bgwidget, chars bgweb)
        {
            mView = new MainView(this);
            SetDefaultActivator(this);
            setCentralWidget(mView);
            Qt::WindowFlags TypeCollector = Qt::Widget;
            if(frameless)
            {
                setAttribute(Qt::WA_TranslucentBackground);
                #if BOSS_MAC_OSX
                    TypeCollector |= Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint;
                #elif BOSS_WINDOWS | BOSS_LINUX
                    TypeCollector |= Qt::FramelessWindowHint;
                #endif
            }
            if(topmost)
                TypeCollector |= Qt::WindowStaysOnTopHint;
            if(TypeCollector != Qt::Widget)
                setWindowFlags(TypeCollector);
            setGeometry(mWindowRect);

            if(bgwidget)
            {
                mBgWindow = NewGroupingWindow(bgwidget, mWindowRect, true);
                bgwidget->setParent(mBgWindow);
            }
            if(bgweb)
            {
                #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
                    mWebConfig = new QCefConfig();
                    mWebConfig->setUserAgent("BossCefView");
                    mWebConfig->setLogLevel(QCefConfig::LOGSEVERITY_DEFAULT);
                    mWebConfig->setBridgeObjectName("BossChannel");
                    mWebConfig->setBuiltinSchemeName("CefView");
                    mWebConfig->setRemoteDebuggingPort(9000);
                    mWebConfig->setWindowlessRenderingEnabled(false);
                    mWebConfig->setStandaloneMessageLoopEnabled(true);
                    mWebConfig->setSandboxDisabled(true);
                    mWebConfig->addCommandLineSwitch("use-mock-keychain");
                    mWebConfig->addCommandLineSwitch("allow-file-access");
                    mWebConfig->addCommandLineSwitch("allow-file-access-from-files");
                    //mWebConfig->addCommandLineSwitch("disable-gpu");
                    //mWebConfig->addCommandLineSwitch("enable-media-stream");
                    //mWebConfig->addCommandLineSwitch("disable-spell-checking");
                    //mWebConfig->addCommandLineSwitch("disable-site-isolation-trials");
                    //mWebConfig->addCommandLineSwitch("enable-aggressive-domstorage-flushing");
                    mWebConfig->addCommandLineSwitchWithValue("renderer-process-limit", "1");
                    mWebConfig->addCommandLineSwitchWithValue("remote-allow-origins", "*");
                    mWebConfig->setCachePath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/BossCefView");
                    mWebContext = new QCefContext(mRefApplication, 0, nullptr, mWebConfig);

                    QDir dir = QCoreApplication::applicationDirPath();
                    QString webResourceDir = QDir::toNativeSeparators(dir.filePath("webres"));
                    QCefContext::instance()->addLocalFolderResource(webResourceDir, "http://QCefViewDoc");

                    QCefSetting WebSetting;
                    WebSetting.setHardwareAccelerationEnabled(true);
                    WebSetting.setWindowlessFrameRate(60);
                    mWebView = new CefWebView(bgweb, &WebSetting);
                    mWebWindow = NewGroupingWindow(mWebView, mWindowRect, false);
                    mWebView->setParent(mWebWindow);
                #endif
            }
        }
        inline MainView* View()
        {
            return mView;
        }
        void SetWindowWebUrl(chars bgweb)
        {
            #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
                if(mWebView)
                {
                    if(bgweb && *bgweb)
                    {
                        mWebView->load(QUrl(bgweb));
                        mWebView->show();
                    }
                    else
                    {
                        mWebView->hide();
                    }
                }
            #endif
        }
        void ReloadWindowWeb()
        {
            #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
                if(mWebView)
                    mWebView->reload();
            #endif
        }
        void SendWindowWebPythonStart(chars pid, chars filename, chars args)
        {
            #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
                if(mWebView)
                    mWebView->SendPythonStart(pid, filename, args);
            #endif
        }
        void SendWindowWebPythonStop(chars pid)
        {
            #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
                if(mWebView)
                    mWebView->SendPythonStop(pid);
            #endif
        }
        void SendWindowWebPythonStopAll()
        {
            #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
                if(mWebView)
                    mWebView->SendPythonStopAll();
            #endif
        }
        void SendWindowWebPythonCall(chars pid, chars func, chars args)
        {
            #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
                if(mWebView)
                    mWebView->SendPythonCall(pid, func, args);
            #endif
        }
        QRect GetWindowRect() const
        {
            return mWindowRect;
        }
        void SetWindowRect(sint32 x, sint32 y, sint32 width, sint32 height)
        {
            const bool EnableMove = (mWindowRect.x() != x || mWindowRect.y() != y);
            const bool EnableSize = (mWindowRect.width() != width || mWindowRect.height() != height);
            mWindowRect.setRect(x, y, width, height);
            // 논리좌표계를 DeferWindowPos에서 사용할 물리좌료계로 제대로 바뀌지 않아서
            // 일단 SetWindowRect에는 사용보류
            //if(mView && !SetGroupGeometry(EnableMove, EnableSize, x, y, width, height))
            {
                setGeometry(x, y, width, height);
                if(mBgWindow) mBgWindow->setGeometry(x, y, width, height);
                if(mWebWindow) mWebWindow->setGeometry(x, y, width, height);
            }
        }
        bool SetGroupGeometry(bool enable_move, bool enable_size, sint32 x = 0, sint32 y = 0, sint32 width = 0, sint32 height = 0)
        {
            #if BOSS_WINDOWS
                if(auto ViewWinId = reinterpret_cast<HWND>(winId()))
                {
                    if(enable_move || enable_size)
                    {
                        POINT TopLeft = {x, y};
                        POINT BottomRight = {x + width, y + height};
                        LogicalToPhysicalPointForPerMonitorDPI(ViewWinId, &TopLeft);
                        LogicalToPhysicalPointForPerMonitorDPI(ViewWinId, &BottomRight);
                        x = TopLeft.x;
                        y = TopLeft.y;
                        width = BottomRight.x - TopLeft.x;
                        height = BottomRight.y - TopLeft.y;
                    }
                    auto BgWinId = (mBgWindow)? reinterpret_cast<HWND>(mBgWindow->winId()) : NULL;
                    auto WebWinId = (mWebWindow)? reinterpret_cast<HWND>(mWebWindow->winId()) : NULL;
                    if(auto WinPos = BeginDeferWindowPos(1 + (BgWinId != NULL) + (WebWinId != NULL)))
                    {
                        const UINT Flag = SWP_NOACTIVATE | ((enable_move)? 0 : SWP_NOMOVE) | ((enable_size)? 0 : SWP_NOSIZE);
                        WinPos = DeferWindowPos(WinPos, ViewWinId, HWND_TOP, x, y, width, height, Flag);
                        if(BgWinId) WinPos = DeferWindowPos(WinPos, BgWinId, ViewWinId, x, y, width, height, Flag);
                        if(WebWinId) WinPos = DeferWindowPos(WinPos, WebWinId, (BgWinId)? BgWinId : ViewWinId, x, y, width, height, Flag);
                        EndDeferWindowPos(WinPos);
                        return true;
                    }
                }
            #endif
            return false;
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            mView->OnCloseEvent(event);
        }
        bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) Q_DECL_OVERRIDE
        {
            #ifdef Q_OS_WIN
                MSG* msg = static_cast<MSG*>(message);
                if(msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN)
                {
                    if(msg->wParam == VK_HANGUL || msg->wParam == VK_HANJA)
                    {
                        mView->ForcedKeyClick(msg->wParam, "");
                        *result = 1;
                        return true;
                    }
                }
                else if(msg->message == WM_DEVICECHANGE)
                {
                    if(msg->wParam == DBT_DEVICEARRIVAL)
                        mView->OnDeviceArrivalEvent(true);
                    else if(msg->wParam == DBT_DEVICEREMOVECOMPLETE)
                        mView->OnDeviceArrivalEvent(false);
                }
            #endif
            return QMainWindow::nativeEvent(eventType, message, result);
        }
        #if defined(QT_HAVE_SERIALPORT) && BOSS_LINUX
            private slots:
            void onUdevActivated(int fd)
            {
                while(true)
                {
                    udev_device* CurDev = udev_monitor_receive_device(mUsbUdevMon);
                    if(!CurDev) break;
                    if(const char* CurAction = udev_device_get_action(CurDev))
                    {
                        if(!String::Compare(CurAction, "add"))
                            mView->OnDeviceArrivalEvent(true);
                        else if(!String::Compare(CurAction, "remove"))
                            mView->OnDeviceArrivalEvent(false);
                    }
                    udev_device_unref(CurDev);
                }
            }
        #endif

    private:
        GroupingWindow* NewGroupingWindow(QWidget* widget, QRect rect, bool transparency)
        {
            auto NewWindow = new GroupingWindow();
            SetDefaultActivator(NewWindow);
            NewWindow->setCentralWidget(widget);
            if(transparency)
                NewWindow->setAttribute(Qt::WA_TranslucentBackground);
            #if BOSS_MAC_OSX
                NewWindow->setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
            #elif BOSS_WINDOWS | BOSS_LINUX
                NewWindow->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
            #endif
            NewWindow->setGeometry(rect);
            NewWindow->show();
            return NewWindow;
        }
        void SetDefaultActivator(GroupingWindow* window)
        {
            window->SetActivator([this](bool active)->void
            {
                if(active)
                {
                    mView->OnActivateEvent(true);
                    SetGroupGeometry(false, false);
                }
                else mView->OnActivateEvent(false);
            });
        }

    private:
        QApplication* const mRefApplication;
        MainView* mView {nullptr};
        GroupingWindow* mBgWindow {nullptr};
        GroupingWindow* mWebWindow {nullptr};
        #if !BOSS_WASM & BOSS_NEED_CEF_WEBVIEW
            CefWebView* mWebView {nullptr};
            QCefConfig* mWebConfig {nullptr};
            QCefContext* mWebContext {nullptr};
        #endif
        QRect mWindowRect;
        bool mInited {false};
        bool mNeedVisible {true};
        #if defined(QT_HAVE_SERIALPORT) && BOSS_LINUX
            struct udev* mUsbUdev {nullptr};
            struct udev_monitor* mUsbUdevMon {nullptr};
            QSocketNotifier* mUsbNotifier {nullptr};
        #endif
    };

    class ThreadClass : public QThread
    {
        Q_OBJECT

    public:
        static void Begin(ThreadCB cb, payload data, prioritytype priority)
        {
            auto pThread = new ThreadClass(cb, nullptr, data, priority);

            connect(pThread, &ThreadClass::started,pThread, &ThreadClass::runslot);
            QObject::connect(pThread, &ThreadClass::finished, pThread, &QObject::deleteLater);
            pThread->moveToThread(pThread);
            pThread->start();
        }
        static void* BeginEx(ThreadExCB cb, payload data, prioritytype priority)
        {
            return new ThreadClass(nullptr, cb, data, priority);
        }

    private:
        ThreadClass(ThreadCB cb1, ThreadExCB cb2, payload data, prioritytype priority)
        {
            BOSS_ASSERT("cb1와 cb2가 모두 nullptr일 순 없습니다", cb1 || cb2);
            m_cb1 = cb1;
            m_cb2 = cb2;
            m_data = data;
            connect(this, SIGNAL(finished()), SLOT(OnFinished()));
            switch(priority)
            {
            case prioritytype_lowest: start(QThread::Priority::LowestPriority); break;
            case prioritytype_low: start(QThread::Priority::LowPriority); break;
            case prioritytype_normal: start(QThread::Priority::NormalPriority); break;
            case prioritytype_high: start(QThread::Priority::HighPriority); break;
            case prioritytype_highest: start(QThread::Priority::HighestPriority); break;
            }
        }
        virtual ~ThreadClass() {}

    private slots:
        void runslot()
        {
            if (m_cb1) this->m_cb1(m_data);
            else if (m_cb2) this->m_cb2(m_data);
        }

        void OnFinished()
        {
            delete this;
        }

    private:
        ThreadCB m_cb1;
        ThreadExCB m_cb2;
        payload m_data;
    };

    class ClockClass
    {
    public:
        ClockClass()
        {
            m_prev = GetHead();
            m_next = m_prev->m_next;
            m_prev->m_next = this;
            m_next->m_prev = this;
            if(!GetTimer().isValid())
            {
                GetTotalMSecFromJulianDay() = ((sint64) EpochToJulian(QDateTime::currentMSecsSinceEpoch())) + GetLocalTimeMSecFromUtc();
                GetTimer().start();
            }
            m_laptime = GetTimer().nsecsElapsed();
        }
        ClockClass(const ClockClass& rhs)
        {
            m_prev = GetHead();
            m_next = m_prev->m_next;
            m_prev->m_next = this;
            m_next->m_prev = this;
            operator=(rhs);
        }
        ~ClockClass()
        {
            m_prev->m_next = m_next;
            m_next->m_prev = m_prev;
        }

    public:
        void SetClock(sint32 year, sint32 month, sint32 day, sint32 hour, sint32 min, sint32 sec, sint64 nsec)
        {
            QDateTime NewTime = QDateTime::fromString(
                (chars) String::Format("%04d-%02d-%02d %02d:%02d:%02d",
                                       year, month, day, hour, min, sec), "yyyy-MM-dd HH:mm:ss");
            const sint64 NewClockMSec = ((sint64) EpochToJulian(NewTime.toMSecsSinceEpoch())) + GetLocalTimeMSecFromUtc();
            m_laptime = (NewClockMSec - GetTotalMSecFromJulianDay()) * 1000000 + nsec;
        }
        void SetClock(uint64 windowtime_msec)
        {
            const uint64 NewWindowMSec = WindowToEpoch(windowtime_msec);
            const sint64 NewClockMSec = ((sint64) EpochToJulian(NewWindowMSec)) + GetLocalTimeMSecFromUtc();
            m_laptime = (NewClockMSec - GetTotalMSecFromJulianDay()) * 1000000;
        }

    public:
        inline ClockClass& operator=(const ClockClass& rhs)
        {m_laptime = rhs.m_laptime; return *this;}

    public:
        inline sint64 GetLap() const {return m_laptime;}
        inline void AddLap(sint64 nsec) {m_laptime += nsec;}
        inline sint64 GetTotalSec() const
        {return GetTotalMSecFromJulianDay() / 1000 + m_laptime / 1000000000;}
        inline sint64 GetNSecInSec() const
        {return ((GetTotalMSecFromJulianDay() % 1000) * 1000000 + 1000000000 + (m_laptime % 1000000000)) % 1000000000;}

    public:
        static void SetBaseTime(chars timestring)
        {
            QDateTime CurrentTime = QDateTime::fromString(timestring, "yyyy-MM-dd HH:mm:ss");
            GetTotalMSecFromJulianDay() = ((sint64) EpochToJulian(CurrentTime.toMSecsSinceEpoch())) + GetLocalTimeMSecFromUtc();

            const sint64 ChangedNSec = GetTimer().restart() * 1000000;
            ClockClass* CurNode = GetHead();
            while((CurNode = CurNode->m_next) != GetHead())
                CurNode->m_laptime -= ChangedNSec;
        }
        static sint64 GetLocalTimeMSecFromUtc() {static sint64 _ = (sint64) (QDateTime::currentDateTime().offsetFromUtc() * 1000); return _;}

    private:
        static ClockClass* GetHead() {static ClockClass _(0); return &_;}
        static QElapsedTimer& GetTimer() {static QElapsedTimer _; return _;}
        static sint64& GetTotalMSecFromJulianDay() {static sint64 _ = 0; return _;}

    private:
        ClockClass(int) // Head전용
        {
            m_prev = this;
            m_next = this;
        }

    private:
        ClockClass* m_prev;
        ClockClass* m_next;
        sint64 m_laptime;
    };

    #if !BOSS_WASM
        typedef QSharedMemory SharedMemoryClass;
        class PipeClass : public QObject
        {
            Q_OBJECT

        public:
            PipeClass(SharedMemoryClass* semaphore, Platform::Pipe::EventCB cb, payload data)
            {
                mStatus = CS_Connecting;
                mTempContext = nullptr;
                mSemaphore = semaphore;
                mEventCB = cb;
                mCBData = data;
            }
            virtual ~PipeClass()
            {
                delete mTempContext;
                delete mSemaphore;
            }

        public:
            inline ConnectStatus Status() const {return mStatus;}
            inline sint32 RecvAvailable() const {return mData.Count();}
            sint32 Recv(uint08* data, sint32 size)
            {
                const sint32 MinSize = Math::Min(size, mData.Count());
                Memory::Copy(data, &mData[0], MinSize);
                if(MinSize == mData.Count()) mData.SubtractionAll();
                else mData.SubtractionSection(0, MinSize);
                return MinSize;
            }
            bool Send(bytes data, sint32 size)
            {
                bool Result = SendCore(data, size);
                FlushCore();
                return Result;
            }

        public:
            virtual bool IsServer() const = 0;
            virtual bool SendCore(bytes data, sint32 size) = 0;
            virtual void FlushCore() = 0;

        public:
            const Context* RecvJson()
            {
                static const String JsonBegin("#json begin");
                static const String JsonEnd("#json end");
                delete mTempContext;
                mTempContext = nullptr;

                if(JsonEnd.Length() < mData.Count())
                {
                    String Message(mData);
                    sint32 BeginPos = 0, EndPos = 0;
                    if((EndPos = Message.Find(0, JsonEnd)) != -1)
                    {
                        if((BeginPos = Message.Find(0, JsonBegin)) != -1)
                        {
                            const sint32 JsonPos = BeginPos + JsonBegin.Length();
                            const sint32 JsonSize = EndPos - JsonPos;
                            mTempContext = new Context(ST_Json, SO_NeedCopy, &mData[JsonPos], JsonSize);
                        }
                        mData.SubtractionSection(0, EndPos + JsonEnd.Length());
                    }
                }
                return mTempContext;
            }
            bool SendJson(const String& json)
            {
                bool Result = true;
                Result &= SendCore((bytes) "#json begin", 11);
                Result &= SendCore((bytes) (chars) json, json.Length());
                Result &= SendCore((bytes) "#json end", 9);
                FlushCore();
                return Result;
            }

        protected:
            ConnectStatus mStatus;
            chararray mData;
            Context* mTempContext;
            SharedMemoryClass* mSemaphore;
            Platform::Pipe::EventCB mEventCB;
            payload mCBData;
        };

        class PipeServerClass : public PipeClass
        {
            Q_OBJECT

        public:
            PipeServerClass(QLocalServer* server, SharedMemoryClass* semaphore, Platform::Pipe::EventCB cb, payload data) : PipeClass(semaphore, cb, data)
            {
                mServer = server;
                mLastClient = nullptr;
                connect(server, &QLocalServer::newConnection, this, &PipeServerClass::OnNewConnection);
            }
            ~PipeServerClass() override
            {
                delete mServer;
            }

        private:
            bool IsServer() const override
            {
                return true;
            }
            bool SendCore(bytes data, sint32 size) override
            {
                if(mLastClient)
                    return (mLastClient->write((chars) data, size) == size);
                return false;
            }
            void FlushCore() override
            {
                if(mLastClient)
                    mLastClient->flush();
            }

        private slots:
            void OnNewConnection()
            {
                QLocalSocket* NewClient = mServer->nextPendingConnection();
                if(mStatus == CS_Connecting)
                {
                    mStatus = CS_Connected;
                    mLastClient = NewClient;
                    if(mEventCB)
                        mEventCB(Platform::Pipe::Connected, mCBData);
                    connect(mLastClient, &QLocalSocket::readyRead, this, &PipeServerClass::OnReadyRead);
                    connect(mLastClient, &QLocalSocket::disconnected, this, &PipeServerClass::OnDisconnected);
                }
                else NewClient->disconnectFromServer();
            }
            void OnReadyRead()
            {
                mLastClient = (QLocalSocket*) sender();
                if(sint64 PacketSize = mLastClient->bytesAvailable())
                    mLastClient->read((char*) mData.AtDumpingAdded(PacketSize), PacketSize);
                if(mEventCB)
                    mEventCB(Platform::Pipe::Received, mCBData);
            }
            void OnDisconnected()
            {
                mStatus = CS_Connecting;
                mLastClient = nullptr;
                if(mEventCB)
                    mEventCB(Platform::Pipe::Disconnected, mCBData);
            }

        private:
            QLocalServer* mServer;
            QLocalSocket* mLastClient;
        };

        class PipeClientClass : public PipeClass
        {
            Q_OBJECT

        public:
            PipeClientClass(chars name, SharedMemoryClass* semaphore, Platform::Pipe::EventCB cb, payload data) : PipeClass(semaphore, cb, data)
            {
                mClient = new QLocalSocket();
                connect(mClient, &QLocalSocket::readyRead, this, &PipeClientClass::OnReadyRead);
                connect(mClient, &QLocalSocket::connected, this, &PipeClientClass::OnConnected);
                connect(mClient, &QLocalSocket::disconnected, this, &PipeClientClass::OnDisconnected);

                mClient->abort();
                mClient->connectToServer(name);
            }
            ~PipeClientClass() override
            {
                delete mClient;
            }

        private:
            bool IsServer() const override
            {
                return false;
            }
            bool SendCore(bytes data, sint32 size) override
            {
                return (mClient->write((chars) data, size) == size);
            }
            void FlushCore() override
            {
                mClient->flush();
            }

        private slots:
            void OnReadyRead()
            {
                if(sint64 PacketSize = mClient->bytesAvailable())
                    mClient->read((char*) mData.AtDumpingAdded(PacketSize), PacketSize);
                if(mEventCB)
                    mEventCB(Platform::Pipe::Received, mCBData);
            }
            void OnConnected()
            {
                mStatus = CS_Connected;
                if(mEventCB)
                    mEventCB(Platform::Pipe::Connected, mCBData);
            }
            void OnDisconnected()
            {
                mStatus = CS_Disconnected;
                if(mEventCB)
                    mEventCB(Platform::Pipe::Disconnected, mCBData);
            }

        private:
            QLocalSocket* mClient;
        };
    #else
        class SharedMemoryClass
        {
        public:
            SharedMemoryClass(chars name) {}
            ~SharedMemoryClass() {}
        public:
            bool attach() {return true;}
            bool create(sint32 size) {return false;}
        };
        class PipeClass : public QObject
        {
            Q_OBJECT
        public:
            inline ConnectStatus Status() const {return CS_Disconnected;}
            inline sint32 RecvAvailable() const {return 0;}
            sint32 Recv(uint08* data, sint32 size) {return 0;}
            bool Send(bytes data, sint32 size) {return false;}
        public:
            virtual bool IsServer() const {return false;}
            virtual bool SendCore(bytes data, sint32 size) {return false;}
            virtual void FlushCore() {}
        public:
            const Context* RecvJson() {return nullptr;}
            bool SendJson(const String& json) {return false;}
        };
        class PipeServerClass : public PipeClass
        {
            Q_OBJECT
        public:
            PipeServerClass(QLocalServer* server, SharedMemoryClass* semaphore, Platform::Pipe::EventCB cb, payload data) {}
            ~PipeServerClass() {}
        };
        class PipeClientClass : public PipeClass
        {
            Q_OBJECT
        public:
            PipeClientClass(chars name, SharedMemoryClass* semaphore, Platform::Pipe::EventCB cb, payload data) {}
            ~PipeClientClass() {}
        };
    #endif

    class Tracker
    {
        typedef void (*ExitCB)(void*);

    public:
        Tracker(QWidget* parent, ExitCB exit, void* data)
        {
            m_parent = parent;
            m_exit = exit;
            m_data = data;
            m_next = nullptr;
            if(m_parent)
                Top().Insert(this);
        }
        ~Tracker()
        {
            if(m_parent)
                Top().Remove(this);
        }

    public:
        void Lock()
        {
            m_loop.exec();
        }
        void Unlock()
        {
            m_loop.quit();
        }
        static void CloseAll(QWidget* parent)
        {
            Tracker* CurNode = Top().m_next;
            while(CurNode)
            {
                Tracker* NextNode = CurNode->m_next;
                if(CurNode->m_parent == parent)
                {
                    Top().Remove(CurNode);
                    CurNode->m_exit(CurNode->m_data);
                }
                CurNode = NextNode;
            }
        }
        static bool HasAnyone(QWidget* parent)
        {
            Tracker* CurNode = Top().m_next;
            while(CurNode)
            {
                Tracker* NextNode = CurNode->m_next;
                if(CurNode->m_parent == parent)
                    return true;
                CurNode = NextNode;
            }
            return false;
        }

    private:
        void Insert(Tracker* rhs)
        {
            rhs->m_next = m_next;
            m_next = rhs;
        }
        void Remove(Tracker* rhs)
        {
            Tracker* CurNode = this;
            while(CurNode)
            {
                if(CurNode->m_next == rhs)
                {
                    CurNode->m_next = CurNode->m_next->m_next;
                    break;
                }
                CurNode = CurNode->m_next;
            }
        }
        static Tracker& Top()
        {static Tracker _(nullptr, nullptr, nullptr); return _;}

    private:
        QWidget* m_parent;
        QEventLoop m_loop;
        ExitCB m_exit;
        void* m_data;
        Tracker* m_next;
    };

    class EditTracker : public QLineEdit
    {
        Q_OBJECT

    public:
        enum TrackerClosingType {TCT_Null, TCT_Enter, TCT_Escape, TCT_FocusOut, TCT_ForcedExit};

    public:
        EditTracker(UIEditType type, const QString& contents, QWidget* parent)
            : QLineEdit(contents, nullptr), m_tracker(parent, OnExit, this)
        {
            if(parent)
                m_parentpos = parent->mapToGlobal(QPoint(0, 0));
            else m_parentpos = QPoint(0, 0);

            m_closing = TCT_Null;
            #if BOSS_WINDOWS
                setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
            #else
                setWindowFlags(Qt::FramelessWindowHint);
            #endif
            setFrame(false);
            selectAll();

            switch(type)
            {
            case UIET_Int: setValidator(new QIntValidator(this)); break;
            case UIET_Float: setValidator(new QDoubleValidator(this)); break;
            default: break;
            }

            QPalette Palette = palette();
            Palette.setColor(QPalette::Base, QColor(255, 255, 255));
            Palette.setColor(QPalette::Text, QColor(0, 0, 0));
            setPalette(Palette);
        }
        ~EditTracker() override
        {
        }
        TrackerClosingType Popup(sint32 x, sint32 y, sint32 w, sint32 h)
        {
            move(m_parentpos.x() + x, m_parentpos.y() + y);
            resize(w, h);
            show();
            activateWindow();
            setFocus();
            m_tracker.Lock();
            return m_closing;
        }

    protected:
        static void OnExit(void* data)
        {
            EditTracker* This = (EditTracker*) data;
            if(This->m_closing == TCT_Null)
                This->m_closing = TCT_ForcedExit;
            This->close();
            This->m_tracker.Unlock();
        }

    private:
        void focusOutEvent(QFocusEvent* event) Q_DECL_OVERRIDE
        {
            if(m_closing == TCT_Null)
                m_closing = TCT_FocusOut;
            close();
            m_tracker.Unlock();
        }

        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE
        {
            if(event->nativeVirtualKey() == 13) // Enter
            {
                if(m_closing == TCT_Null)
                    m_closing = TCT_Enter;
                close();
                m_tracker.Unlock();
            }
            else if(event->nativeVirtualKey() == 27) // Esc
            {
                if(m_closing == TCT_Null)
                    m_closing = TCT_Escape;
                close();
                m_tracker.Unlock();
            }
            else QLineEdit::keyPressEvent(event);
        }

    private:
        Tracker m_tracker;
        TrackerClosingType m_closing;
        QPoint m_parentpos;
    };

    class ListTracker : public QListWidget
    {
        Q_OBJECT

    public:
        ListTracker(const Strings& strings, QWidget* parent)
            : QListWidget(nullptr), m_tracker(parent, OnExit, this)
        {
            for(sint32 i = 0, iend = strings.Count(); i < iend; ++i)
            {
                QListWidgetItem* NewItem = new QListWidgetItem(QString::fromUtf8(strings[i], -1), this);
                NewItem->setTextAlignment(Qt::AlignCenter);
                addItem(NewItem);
            }

            if(parent)
                m_parentpos = parent->mapToGlobal(QPoint(0, 0));
            else m_parentpos = QPoint(0, 0);

            m_select = -1;
            #if BOSS_WINDOWS
                setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
            #else
                setWindowFlags(Qt::FramelessWindowHint);
            #endif

            connect(this, SIGNAL(itemPressed(QListWidgetItem*)), SLOT(onItemPressed(QListWidgetItem*)));
        }
        ~ListTracker()
        {
        }
        sint32 Popup(sint32 x, sint32 y, sint32 w, sint32 h)
        {
            move(m_parentpos.x() + x, m_parentpos.y() + y);
            resize(w, h);
            show();
            activateWindow();
            setFocus();
            m_tracker.Lock();
            return m_select;
        }

    protected:
        static void OnExit(void* data)
        {
            ListTracker* This = (ListTracker*) data;
            This->m_select = -1;
            This->close();
            This->m_tracker.Unlock();
        }

    private:
        void focusOutEvent(QFocusEvent* event) Q_DECL_OVERRIDE
        {
            close();
            m_tracker.Unlock();
        }

    private slots:
        void onItemPressed(QListWidgetItem* item)
        {
            m_select = row(item);
            close();
            m_tracker.Unlock();
        }

    private:
        Tracker m_tracker;
        sint32 m_select;
        QPoint m_parentpos;
    };

    class PeerPacket
    {
    public:
        const packettype Type;
        const sint32 PeerID;
        buffer Buffer;

    public:
        PeerPacket(packettype type, sint32 peerid, sint32 buffersize)
            : Type(type), PeerID(peerid) {Buffer = Buffer::Alloc(BOSS_DBG buffersize);}
        ~PeerPacket() {Buffer::Free(Buffer);}

    public:
        void DestroyMe() {delete this;}
        static sint32 MakeID() {static sint32 _ = -1; return ++_;}
    };

    class ServerClass : public QObject
    {
        Q_OBJECT

    protected:
        Queue<PeerPacket*> mPacketQueue;
        PeerPacket* mFocusedPacket;

    public:
        ServerClass()
        {
            mFocusedPacket = new PeerPacket(packettype_null, -1, 0);
        }
        virtual ~ServerClass()
        {
            for(PeerPacket* UnusedPacket = nullptr; UnusedPacket = mPacketQueue.Dequeue();)
                UnusedPacket->DestroyMe();
            delete mFocusedPacket;
        }

    public:
        bool TryPacket()
        {
            PeerPacket* PopPacket = mPacketQueue.Dequeue();
            if(!PopPacket) return false;
            delete mFocusedPacket;
            mFocusedPacket = PopPacket;
            return true;
        }

        PeerPacket* GetFocusedPacket()
        {
            return mFocusedPacket;
        }

    public:
        virtual bool Listen(uint16 port) = 0;
        virtual bool SendPacket(int peerid, const void* buffer, sint32 buffersize, bool utf8) = 0;
        virtual bool KickPeer(int peerid) = 0;
        virtual bool GetPeerAddress(int peerid, ip4address* ip4, ip6address* ip6, uint16* port) = 0;
    };

    class TCPServerClass : public ServerClass
    {
        Q_OBJECT

    private:
        QTcpServer* mServer;
        bool mUsingSizeField;
        Map<QTcpSocket*> mPeers;

    public:
        TCPServerClass(bool sizefield = false)
        {
            mServer = new QTcpServer(this);
            mUsingSizeField = sizefield;
            connect(mServer, SIGNAL(newConnection()), this, SLOT(acceptPeer()));
        }

        ~TCPServerClass() override
        {
            mServer->close();
        }

        TCPServerClass& operator=(const TCPServerClass&)
        {
            BOSS_ASSERT("호출불가", false);
            return *this;
        }

    private slots:
        void acceptPeer()
        {
            QTcpSocket* Peer = mServer->nextPendingConnection();
            const sint32 NewPeerID = PeerPacket::MakeID();
            Peer->setProperty("id", NewPeerID);
            Peer->setProperty("needs", 0);
            mPacketQueue.Enqueue(new PeerPacket(packettype_entrance, NewPeerID, 0));

            if(!mUsingSizeField) connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeer()));
            else connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
            connect(Peer, SIGNAL(disconnected()), this, SLOT(disconnected()));
            Platform::BroadcastNotify("entrance", nullptr, NT_SocketReceive, nullptr, true);
        }

        void readyPeer()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            const sint32 PeerID = Peer->property("id").toInt();
            const sint64 PacketSize = Peer->bytesAvailable();

            if(0 < PacketSize)
            {
                PeerPacket* NewPacket = new PeerPacket(packettype_message, PeerID, PacketSize);
                Peer->read((char*) NewPacket->Buffer, PacketSize);
                mPacketQueue.Enqueue(NewPacket);
            }
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive, nullptr, true);
        }

        void readyPeerWithSizeField()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            const sint32 PeerID = Peer->property("id").toInt();
            sint32 PeerNeeds = Peer->property("needs").toInt();
            sint64 PacketSize = Peer->bytesAvailable();

            while(0 < PacketSize)
            {
                if(PeerNeeds == 0)
                {
                    if(4 <= PacketSize)
                    {
                        PacketSize -= 4;
                        int GetPacketSize = 0;
                        Peer->read((char*) &GetPacketSize, 4);
                        PeerNeeds = GetPacketSize;
                    }
                    else break;
                }
                if(0 < PeerNeeds)
                {
                    if(PeerNeeds <= PacketSize)
                    {
                        PacketSize -= PeerNeeds;
                        PeerPacket* NewPacket = new PeerPacket(packettype_message, PeerID, PeerNeeds);
                        Peer->read((char*) NewPacket->Buffer, PeerNeeds);
                        mPacketQueue.Enqueue(NewPacket);
                        PeerNeeds = 0;
                    }
                    else break;
                }
            }
            Peer->setProperty("needs", PeerNeeds);
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive, nullptr, true);
        }

        void disconnected()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            const sint32 PeerID = Peer->property("id").toInt();

            mPeers.Remove(PeerID);
            mPacketQueue.Enqueue(new PeerPacket(packettype_leaved, PeerID, 0));
            Platform::BroadcastNotify("leaved", nullptr, NT_SocketReceive);
        }

    private:
        bool Listen(uint16 port) override
        {
            if(mServer->isListening()) return true;
            return mServer->listen(QHostAddress::Any, port);
        }

        bool SendPacket(int peerid, const void* buffer, sint32 buffersize, bool utf8) override
        {
            if(QTcpSocket** Peer = mPeers.Access(peerid))
            {
                if((*Peer)->write((chars) buffer, buffersize) == buffersize)
                    return true;

                mPeers.Remove(peerid);
                mPacketQueue.Enqueue(new PeerPacket(packettype_leaved, peerid, 0));
                Platform::BroadcastNotify("leaved", nullptr, NT_SocketReceive);
            }
            return false;
        }

        bool KickPeer(int peerid) override
        {
            if(QTcpSocket** Peer = mPeers.Access(peerid))
            {
                (*Peer)->disconnectFromHost();
                (*Peer)->deleteLater();
                mPeers.Remove(peerid);
                mPacketQueue.Enqueue(new PeerPacket(packettype_kicked, peerid, 0));
                Platform::BroadcastNotify("kicked", nullptr, NT_SocketReceive);
                return true;
            }
            return false;
        }

        bool GetPeerAddress(int peerid, ip4address* ip4, ip6address* ip6, uint16* port) override
        {
            if(QTcpSocket** Peer = mPeers.Access(peerid))
            {
                if(ip4)
                {
                    auto IPv4Address = (*Peer)->peerAddress().toIPv4Address();
                    ip4->ip[0] = (IPv4Address >> 24) & 0xFF;
                    ip4->ip[1] = (IPv4Address >> 16) & 0xFF;
                    ip4->ip[2] = (IPv4Address >>  8) & 0xFF;
                    ip4->ip[3] = (IPv4Address >>  0) & 0xFF;
                }
                if(ip6)
                {
                    auto IPv6Address = (*Peer)->peerAddress().toIPv6Address();
                    *ip6 = *((ip6address*) &IPv6Address);
                }
                if(port) *port = (*Peer)->peerPort();
                return true;
            }
            return false;
        }
    };

    class WSServerClass : public ServerClass
    {
        Q_OBJECT

    private:
        QWebSocketServer* mServer;
        Map<QWebSocket*> mPeers;

    public:
        WSServerClass(chars name = "noname", bool use_wss = false)
        {
            #if !BOSS_WASM
                mServer = new QWebSocketServer(name, (use_wss)? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode, this);
            #else
                mServer = new QWebSocketServer(name, QWebSocketServer::NonSecureMode, this);
            #endif
            connect(mServer, &QWebSocketServer::newConnection, this, &WSServerClass::acceptPeer);

            #if !BOSS_WASM
                if(use_wss)
                {
                    connect(mServer, &QWebSocketServer::sslErrors, this, &WSServerClass::sslErrors);
                    if(QSslSocket::supportsSsl())
                    {
                        const QByteArray CertText = QByteArray::fromStdString(
                            "-----BEGIN CERTIFICATE-----\r\n"
                            "MIIEBzCCAr+gAwIBAgIQLqRnqmW9PgQ0XO4HOU1bGTANBgkqhkiG9w0BAQUFADA8\r\n"
                            "MQswCQYDVQQGEwJHQjEZMBcGA1UEChMQV2VzdHBvaW50IENBIEtleTESMBAGA1UE\r\n"
                            "ChMJV2VzdHBvaW50MB4XDTEyMTIxOTE3MjIwNVoXDTEyMTIxOTE3MjIwNVowOzEL\r\n"
                            "MAkGA1UEBhMCR0IxEjAQBgNVBAoTCVdlc3Rwb2ludDEYMBYGA1UEAxMPd3d3LmV4\r\n"
                            "YW1wbGUuY29tMIIBUjANBgkqhkiG9w0BAQEFAAOCAT8AMIIBOgKCATEAuxSykwJE\r\n"
                            "8S0sNP07NAEvD9zvLqZ4OIXCTnEEe3zrvQjZP9HRygE1Lp8uFmRu1U0vJi4VlQko\r\n"
                            "ebcbk0ts2oPZxCQw1VRmVi5V83fDzd0qHpo5MF8YUWt/nl+DB99vMv9mddyESU3Q\r\n"
                            "z0Boj77NvIcDIq9GQU4ds1leHO6LTRfKXC5aeBzZpoTc6NMFHFEy36PKG1Tjh2HK\r\n"
                            "P/voiP3D1fZ12m0kj8JM0MZXqxYdcA0/Dyl24CLUpnGKfK1tABwEjtN/Ck4neFqv\r\n"
                            "m8kJLE2dbrMhOlclnqWPeIl0o11E3BiBD3fxxbG4hSXWPZC6hvZCDWd/zD91N8CW\r\n"
                            "ocfP9Qdfj22O0/P8uauGNV5CHXWTxLd6ab/KGxVyA7tfRiWAb9bMvIIV9PfRLcF6\r\n"
                            "D5/Hbz6zQY8wdwIDAQABo4GlMIGiMCwGA1UdEQQlMCOCD3d3dy5leGFtcGxlLmNv\r\n"
                            "bYEQdGVzdEBleGFtcGxlLmNvbTAMBgNVHRMBAf8EAjAAMBMGA1UdJQQMMAoGCCsG\r\n"
                            "AQUFBwMBMA8GA1UdDwEB/wQFAwMHKAAwHQYDVR0OBBYEFBJA2exOAIVq7qe+UC7Q\r\n"
                            "aiiWhOOAMB8GA1UdIwQYMBaAFJiD+YDrKjcu2I6T2dWGIwNbjjPzMA0GCSqGSIb3\r\n"
                            "DQEBBQUAA4IBMQCYRGhuU6tuH3X4NtWBfI3cPlqo/O0KzTgHzy9tUD015NOWunb9\r\n"
                            "r5wum3V36JUDS2BcRHWI0mz9LZZlvx7a+xhlmcD3yXIawjrjNbbKTHmgWvKFsIau\r\n"
                            "XwnxIbMcN+s5MMvcIw0U3QczxmpuqVQKrlBcPdzwSQYlQ5S08QOFcXKbsNOifxJW\r\n"
                            "7FNOmQYZVuYZ+MWmes/Ppk8SF+wE3bFgWrTc6o5e0RprhUHKLsPRKLbMQmIFdGI4\r\n"
                            "TCgC5+VxZb/Qaf+/J/KNABwHXPqpQxJkXfYQCjeuOFEEnA8sAEeuQFXZTjtC0gnH\r\n"
                            "6z2+erl20gitjt/AEDvW65cblahpIWS2F/OCSEzrVS/YY0AjYZUHJCUDLjv306qv\r\n"
                            "NeiqhoJa7CNjb+4/gVolKRoFmCjMzR4kXML3\r\n"
                            "-----END CERTIFICATE-----");
                        const QByteArray KeyText = QByteArray::fromStdString(
                            "-----BEGIN RSA PRIVATE KEY-----\r\n"
                            "MIIFfAIBAAKCATEAuxSykwJE8S0sNP07NAEvD9zvLqZ4OIXCTnEEe3zrvQjZP9HR\r\n"
                            "ygE1Lp8uFmRu1U0vJi4VlQkoebcbk0ts2oPZxCQw1VRmVi5V83fDzd0qHpo5MF8Y\r\n"
                            "UWt/nl+DB99vMv9mddyESU3Qz0Boj77NvIcDIq9GQU4ds1leHO6LTRfKXC5aeBzZ\r\n"
                            "poTc6NMFHFEy36PKG1Tjh2HKP/voiP3D1fZ12m0kj8JM0MZXqxYdcA0/Dyl24CLU\r\n"
                            "pnGKfK1tABwEjtN/Ck4neFqvm8kJLE2dbrMhOlclnqWPeIl0o11E3BiBD3fxxbG4\r\n"
                            "hSXWPZC6hvZCDWd/zD91N8CWocfP9Qdfj22O0/P8uauGNV5CHXWTxLd6ab/KGxVy\r\n"
                            "A7tfRiWAb9bMvIIV9PfRLcF6D5/Hbz6zQY8wdwIDAQABAoIBMQCp4FN/NlJQBcrc\r\n"
                            "mw3FXUXUy7PM0pDcEmmsPOfrEjYlwwEy+F3dZldabGS3JJ+XxKyJqNMkL9q3G1RI\r\n"
                            "3faMPanid1J4hFkg7JZTrG76Yle6ziQcDl3QoSKTNvuOjI826b+qSoE85xIy/7Ny\r\n"
                            "w7mh9Z8dQbcz2bESiJXzA5EugenY5qZz5w6kLIVETUmYuwrNFLeTNfQdnOjAS3qB\r\n"
                            "azGIZp+kqm0p9zay+OUyvoQGxpr+vYV8JaHU4eBpxslr97GclJ88sk8CpsYy7mRd\r\n"
                            "81ONGE8nwFHRm4uZDL2o5NzjawHQo3adNpRum/1PCmqJAK2xjPBswwOuYHbS7utc\r\n"
                            "mfuvRuXaY9FQDPmp7B7nZ5iQD2ned5vAR86gg/f6d09EaFw3i2ARSf4QdbZWY9Nn\r\n"
                            "qPXTDejhAoGZAOdDwcsNhfJYqU3+fUfyENBfw2sQ5PnH/WPhIn5yO672gzU2Iuno\r\n"
                            "9tAP32FwloKbOpq5TiczIZgW0LblJk9E07F/dTbSiLxUQMxctY+U5XYMZ0uJyKZm\r\n"
                            "1pWLBJQXAsXFxnV19WwMYys4aTWEHnIw02zLvKrvYrOIWX4BICTsNqw8FQv0wG+r\r\n"
                            "VcrwLc5WkleUxdHhFF1X/leZAoGZAM8XJWaGCUmdyV75zTpxmZvtXUjTajxBctRG\r\n"
                            "/cx75ZSUj+p3Fv6q9oVZL6IN+G4s+WT1PY5fYJSFZK61uQtoY2w8pe20LhHAkeif\r\n"
                            "bpkOMK7ie6+NiF1GCiPMBFtEZOawdq3x2EwbNbqpbgJOz1yNAT/nEenx1cDxyqyr\r\n"
                            "K+4ZIh4Dufs9eoYftMyjO5BrXWSOGNFliaGZH5KPAoGZAKdAvJYCiL31WzR5+dcf\r\n"
                            "fQOGTolPJZp0BZDHkK/MI9fsloXUSjnK7z0YTaBl0aRRaXfezmPROdmJnpa3cRZh\r\n"
                            "G4zCNl5YsuUpNdfWsMRPlgfi/o2F72RQ+Z4bdUs4vRuVZmsqzTzAVLQ8TvKSQ4ao\r\n"
                            "Qy/qxN8G0+YtlTNo0vuBDiVQKpSEBOx+CEUFoMsalynaAZtBYf4EFoD5AoGYVfl+\r\n"
                            "BXpQEMf4+f0rPsA6zYlV2Q4sZKenTInMhEBLp8ulk+mtGj1P6zyDkfvKz7LMNyW5\r\n"
                            "UIo4RnC6w+2dzSahYCYtnOnY1nXkHXdTKyfA/ln4j4Fqw454VzQz+tACM+O+4agt\r\n"
                            "7Cq/u6brjPm7DOldQ3Ji9YT5AQlg4x6NNmQozd0uMSfs3hH7tZlu+R4Zv81ecFB/\r\n"
                            "Ox+fA/ECgZhPv9P/wK1ZrgOcpfvbLOYOfxo2WET/oxuqh2SDgW6Ar0bURJDJQIUX\r\n"
                            "7LaK4cNFHjmLYN0N/5hdcuSFTw5ZPtXWdcJFJ9I3cYNZ/tMCxqL1lgu7Oj4tO6Rr\r\n"
                            "9H2YCWY3G2D4VKR9rJNqMm/ZWpJbuV0DbbC7OpHcU5HcMMk39ojuAEWpqd7e6xpY\r\n"
                            "JyITLVhZp3B9M8WfKXFylw==\r\n"
                            "-----END RSA PRIVATE KEY-----");

                        const QSslCertificate Certificate(CertText, QSsl::Pem);
                        const QSslKey SslKey(KeyText, QSsl::Rsa, QSsl::Pem);
                        QSslConfiguration SslConfiguration = QSslConfiguration::defaultConfiguration();
                        SslConfiguration.setLocalCertificate(Certificate);
                        SslConfiguration.setPrivateKey(SslKey);
                        SslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
                        SslConfiguration.setProtocol(QSsl::SecureProtocols);
                        mServer->setSslConfiguration(SslConfiguration);
                    }
                }
            #endif
        }

        ~WSServerClass() override
        {
            mServer->close();
        }

        WSServerClass& operator=(const WSServerClass&)
        {
            BOSS_ASSERT("호출불가", false);
            return *this;
        }

    private slots:
        void acceptPeer()
        {
            QWebSocket* Peer = mServer->nextPendingConnection();
            const sint32 NewPeerID = PeerPacket::MakeID();
            Peer->setProperty("id", NewPeerID);
            Peer->setProperty("needs", 0);
            mPacketQueue.Enqueue(new PeerPacket(packettype_entrance, NewPeerID, 0));

            connect(Peer, &QWebSocket::textFrameReceived, this, &WSServerClass::textReceived);
            connect(Peer, &QWebSocket::binaryFrameReceived, this, &WSServerClass::binaryReceived);
            connect(Peer, &QWebSocket::disconnected, this, &WSServerClass::disconnected);
            Platform::BroadcastNotify("entrance", nullptr, NT_SocketReceive, nullptr, true);
        }

        void textReceived(const QString& frame, bool isLastFrame)
        {
            binaryReceived(frame.toUtf8(), isLastFrame);
        }

        void binaryReceived(const QByteArray& frame, bool isLastFrame)
        {
            QWebSocket* Peer = (QWebSocket*) sender();
            const sint32 PeerID = Peer->property("id").toInt();
            const sint64 PacketSize = frame.size();

            if(0 < PacketSize)
            {
                PeerPacket* NewPacket = new PeerPacket(packettype_message, PeerID, PacketSize);
                Memory::Copy((void*) NewPacket->Buffer, frame.constData(), PacketSize);
                mPacketQueue.Enqueue(NewPacket);
            }
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive, nullptr, true);
        }

        void disconnected()
        {
            QWebSocket* Peer = (QWebSocket*) sender();
            const sint32 PeerID = Peer->property("id").toInt();

            mPeers.Remove(PeerID);
            mPacketQueue.Enqueue(new PeerPacket(packettype_leaved, PeerID, 0));
            Platform::BroadcastNotify("leaved", nullptr, NT_SocketReceive);
        }

        #if !BOSS_WASM
            void sslErrors(const QList<QSslError>& errors)
            {
                foreach(const auto& CurError, errors)
                {
                    String ErrorText = CurError.errorString().toUtf8().constData();
                    Platform::BroadcastNotify("error", ErrorText, NT_SocketReceive);
                }
            }
        #endif

    private:
        bool Listen(uint16 port) override
        {
            if(mServer->isListening()) return true;
            return mServer->listen(QHostAddress::Any, port);
        }

        bool SendPacket(int peerid, const void* buffer, sint32 buffersize, bool utf8) override
        {
            if(QWebSocket** Peer = mPeers.Access(peerid))
            {
                if(utf8)
                {
                    if((*Peer)->sendTextMessage(QString::fromUtf8((chars) buffer, buffersize)) == buffersize)
                        return true;
                }
                else if((*Peer)->sendBinaryMessage(QByteArray((chars) buffer, buffersize)) == buffersize)
                    return true;

                mPeers.Remove(peerid);
                mPacketQueue.Enqueue(new PeerPacket(packettype_leaved, peerid, 0));
                Platform::BroadcastNotify("leaved", nullptr, NT_SocketReceive);
            }
            return false;
        }

        bool KickPeer(int peerid) override
        {
            if(QWebSocket** Peer = mPeers.Access(peerid))
            {
                (*Peer)->disconnect();
                (*Peer)->deleteLater();
                mPeers.Remove(peerid);
                mPacketQueue.Enqueue(new PeerPacket(packettype_kicked, peerid, 0));
                Platform::BroadcastNotify("kicked", nullptr, NT_SocketReceive);
                return true;
            }
            return false;
        }

        bool GetPeerAddress(int peerid, ip4address* ip4, ip6address* ip6, uint16* port) override
        {
            if(QWebSocket** Peer = mPeers.Access(peerid))
            {
                if(ip4)
                {
                    auto IPv4Address = (*Peer)->peerAddress().toIPv4Address();
                    ip4->ip[0] = (IPv4Address >> 24) & 0xFF;
                    ip4->ip[1] = (IPv4Address >> 16) & 0xFF;
                    ip4->ip[2] = (IPv4Address >>  8) & 0xFF;
                    ip4->ip[3] = (IPv4Address >>  0) & 0xFF;
                }
                if(ip6)
                {
                    auto IPv6Address = (*Peer)->peerAddress().toIPv6Address();
                    *ip6 = *((ip6address*) &IPv6Address);
                }
                if(port) *port = (*Peer)->peerPort();
                return true;
            }
            return false;
        }
    };

    class SocketBox : public QObject
    {
        Q_OBJECT

    public:
        SocketBox()
        {
            m_type = Type::NIL;
            m_socket = nullptr;
            m_wsocket = nullptr;
            m_udpip.clear();
            m_udpport = 0;
        }
        virtual ~SocketBox()
        {
            delete m_socket;
            delete m_wsocket;
        }

    public:
        void Init(chars type)
        {
            if(!String::Compare(type, "TCP"))
            {
                m_type = Type::TCP;
                m_socket = (QAbstractSocket*) new QTcpSocket();
            }
            else if(!String::Compare(type, "UDP"))
            {
                m_type = Type::UDP;
                m_socket = (QAbstractSocket*) new QUdpSocket();
            }
            else if(!String::Compare(type, "WS"))
            {
                m_type = Type::WS;
                m_wsocket = new QWebSocket();
            }
            else if(!String::Compare(type, "WSS"))
            {
                m_type = Type::WSS;
                m_wsocket = new QWebSocket();
            }

            if(m_socket)
            {
                connect(m_socket, &QAbstractSocket::connected, this, &SocketBox::OnConnected);
                connect(m_socket, &QAbstractSocket::disconnected, this, &SocketBox::OnDisconnected);
                connect(m_socket, &QAbstractSocket::readyRead, this, &SocketBox::OnReadyRead);
            }
            else if(m_wsocket)
            {
                connect(m_wsocket, &QWebSocket::connected, this, &SocketBox::OnWebConnected);
                connect(m_wsocket, &QWebSocket::disconnected, this, &SocketBox::OnWebDisconnected);
                connect(m_wsocket, &QWebSocket::textMessageReceived, this, &SocketBox::OnWebTextMessageReceived);
                connect(m_wsocket, &QWebSocket::binaryMessageReceived, this, &SocketBox::OnWebBinaryMessageReceived);
            }
        }
        bool CheckState(chars name)
        {
            switch(m_type)
            {
            case Type::TCP:
                if(!m_socket->isValid())
                    return false;
                if(m_socket->state() == QAbstractSocket::UnconnectedState)
                    return false;
                return true;
            case Type::UDP:
                return true;
            case Type::WS:
            case Type::WSS:
                if(!m_wsocket->isValid())
                    return false;
                if(m_wsocket->state() == QAbstractSocket::UnconnectedState)
                    return false;
                return true;
            }
            return false;
        }

    private slots:
        void OnConnected()
        {
            Platform::BroadcastNotify("connected", nullptr, NT_SocketReceive, nullptr, true);
        }
        void OnDisconnected()
        {
            Platform::BroadcastNotify("disconnected", nullptr, NT_SocketReceive, nullptr, true);
        }
        void OnReadyRead()
        {
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive, nullptr, true);
        }
        void OnWebConnected()
        {
            Platform::BroadcastNotify("connected", nullptr, NT_SocketReceive, nullptr, true);
        }
        void OnWebDisconnected()
        {
            Platform::BroadcastNotify("disconnected", nullptr, NT_SocketReceive, nullptr, true);
        }
        void OnWebTextMessageReceived(const QString& message)
        {
            OnWebBinaryMessageReceived(message.toUtf8());
        }
        void OnWebBinaryMessageReceived(const QByteArray& message)
        {
            m_wbytes += message;
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive, nullptr, true);
        }

    public:
        enum class Type {NIL, TCP, UDP, WS, WSS} m_type;
        QAbstractSocket* m_socket;
        QWebSocket* m_wsocket;
        QByteArray m_wbytes;
        QHostAddress m_udpip;
        quint16 m_udpport;

    private:
        class STClass
        {
        public:
            STClass()
            {
                static uint64 LastThreadID = 0;
                m_lastId = (++LastThreadID) << 20;
            }
            ~STClass() {}
        public:
            Map<SocketBox> m_map;
            ublock m_lastId;
        };
        static inline STClass& ST() {return *BOSS_STORAGE_SYS(STClass);}

    public:
        static void Create(id_socket& result, chars type)
        {
            STClass& CurClass = ST();
            CurClass.m_map[++CurClass.m_lastId].Init(type);
            result = (id_socket) AnyTypeToPtr(CurClass.m_lastId);
        }
        static SocketBox* Access(id_socket id)
        {
            if(id == nullptr) return nullptr;
            BOSS_ASSERT("타 스레드에서 생성된 소켓입니다", (ST().m_lastId >> 20) == (((ublock) id) >> 20));
            return ST().m_map.Access(PtrToUint64(id));
        }
        static void Remove(id_socket id)
        {
            ST().m_map.Remove(PtrToUint64(id));
        }
    };

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
            Clear();
        }

    public:
        void Clear()
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
            h_name = nullptr;
            h_aliases = nullptr;
            h_addr_list = nullptr;
        }

    public:
        chars h_name;
        chars* h_aliases;
        const sint16 h_addrtype;
        const sint16 h_length;
        bytes* h_addr_list;
    };

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
            Clear();
        }

    public:
        void Clear()
        {
            delete[] s_name;
            for(chars* ptr_aliases = s_aliases; ptr_aliases; ++ptr_aliases)
                delete[] *ptr_aliases;
            delete[] s_aliases;
            delete[] s_proto;
            s_name = nullptr;
            s_aliases = nullptr;
            s_port = 0;
            s_proto = nullptr;
        }

    public:
        chars s_name;
        chars* s_aliases;
        sint16 s_port;
        chars s_proto;
    };

    #ifdef QT_HAVE_SERIALPORT
        class SerialClass : public QSerialPort
        {
            Q_OBJECT

        public:
            static Strings EnumDevice(String* spec)
            {
                Strings Result;
                Context SpecCollector;
                const QList<QSerialPortInfo>& AllPorts = QSerialPortInfo::availablePorts();
                foreach(const auto& CurPort, AllPorts)
                {
                    String CurName = CurPort.portName().toUtf8().constData();
                    if(CurName.Length() == 0)
                        CurName = CurPort.description().toUtf8().constData();
                    Result.AtAdding() = CurName;
                    if(spec)
                    {
                        Context& NewChild = SpecCollector.At(SpecCollector.LengthOfIndexable());
                        NewChild.At("portname").Set(CurPort.portName().toUtf8().constData());
                        NewChild.At("description").Set(CurPort.description().toUtf8().constData());
                        NewChild.At("systemlocation").Set(CurPort.systemLocation().toUtf8().constData());
                        NewChild.At("manufacturer").Set(CurPort.manufacturer().toUtf8().constData());
                        NewChild.At("serialnumber").Set(CurPort.serialNumber().toUtf8().constData());
                    }
                }
                if(spec)
                    *spec = SpecCollector.SaveJson(*spec);
                return Result;
            }

        public:
            SerialClass(chars port, sint32 baudrate)
            {
                mPort = port;
				mReadFocus = 0;
                mReadMutex = Mutex::Open();
                mWriteMutex = Mutex::Open();
                const QList<QSerialPortInfo>& AllPorts = QSerialPortInfo::availablePorts();
                foreach(const auto& CurPort, AllPorts)
                {
                    if(*port == '\0' || CurPort.portName() == port)
                    {
                        if(baudrate != -1)
                        {
                            setPortName(port);
                            setBaudRate((QSerialPort::BaudRate) baudrate);
                            setDataBits(QSerialPort::Data8);
                            setParity(QSerialPort::NoParity);
                            setStopBits(QSerialPort::OneStop);
                            setFlowControl(QSerialPort::NoFlowControl);
                        }
                        else setPort(CurPort);
                        break;
                    }
                }
                if(open(QIODevice::ReadWrite))
                {
                    connect(this, &QSerialPort::errorOccurred, this, &SerialClass::OnErrorOccurred);
                    connect(this, &QSerialPort::readyRead, this, &SerialClass::OnRead);
                }
                else OnErrorOccurred(error());
            }
            ~SerialClass()
            {
                Mutex::Close(mReadMutex);
				Mutex::Close(mWriteMutex);
            }

        private slots:
            void OnErrorOccurred(QSerialPort::SerialPortError error)
            {
                const String ErrorText = String::Format(":%s at %s", errorString().toUtf8().constData(), portName().toUtf8().constData());
                BOSS_TRACE("시리얼통신에서 에러가 발생하였습니다(%s)", (chars) ErrorText);
                Platform::BroadcastNotify("error", mPort + ErrorText, NT_Serial);
            }
            void OnRead()
            {
                QByteArray NewArray = readAll();
                if(0 < NewArray.length())
                {
                    Mutex::Lock(mReadMutex);
                    {
                        if(0 < mReadFocus)
                        {
                            const sint32 CopyLength = mReadStream.Count() - mReadFocus;
                            if(0 < CopyLength)
								mReadStream.SubtractionSection(0, mReadFocus);
                            else mReadStream.SubtractionAll();
                            mReadFocus = 0;
                        }
                        Memory::Copy(mReadStream.AtDumpingAdded(NewArray.length()), NewArray.constData(), NewArray.length());
                    }
                    Mutex::Unlock(mReadMutex);
                    Platform::BroadcastNotify("message", mPort, NT_Serial);
                }
            }

        public:
            bool IsValid()
            {
                return isOpen();
            }
            bool Connected()
            {
                auto ErrorCode = error();
                return (ErrorCode == QSerialPort::NoError);
            }
            bool ReadReady()
            {
                return (0 < ReadAvailable());
            }
            sint32 ReadAvailable()
            {
                sint32 ReadSize = 0;
                Mutex::Lock(mReadMutex);
                {
                    ReadSize = mReadStream.Count() - mReadFocus;
                }
                Mutex::Unlock(mReadMutex);
                return ReadSize;
            }
            sint32 ReadData(uint08* data, const sint32 size)
            {
                sint32 CopySize = 0;
                Mutex::Lock(mReadMutex);
                {
                    CopySize = Math::Min(size, mReadStream.Count() - mReadFocus);
                    Memory::Copy(data, &mReadStream[mReadFocus], CopySize);
                    mReadFocus += CopySize;
                }
                Mutex::Unlock(mReadMutex);
                return CopySize;
            }
            void WriteData(bytes data, const sint32 size)
            {
				Mutex::Lock(mWriteMutex);
				{
					Memory::Copy(mWriteStream.AtDumpingAdded(size), data, size);
				}
				Mutex::Unlock(mWriteMutex);
            }
            bool WriteFlush(uint08s* get = nullptr)
            {
				Mutex::Lock(mWriteMutex);
				{
					write((const char*) &mWriteStream[0], mWriteStream.Count());
					if(get) *get = ToReference(mWriteStream);
					else mWriteStream.SubtractionAll();
				}
				Mutex::Unlock(mWriteMutex);
				return flush();;
            }

        private:
            String mPort;
			sint32 mReadFocus;
            id_mutex mReadMutex;
            id_mutex mWriteMutex;
            uint08s mReadStream;
            uint08s mWriteStream;
        };
    #endif

#endif
