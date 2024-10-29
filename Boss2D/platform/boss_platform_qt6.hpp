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
    #include <QFile>
    #include <QFileInfo>
    #include <QDir>
    #include <QStandardPaths>

    #ifdef QT_HAVE_GRAPHICS
        #include <QMainWindow>
        #include <QWindow>
        #include <QPainter>
        #include <QCloseEvent>
        #include <QResizeEvent>
        #include <QPaintEvent>
        #include <QMouseEvent>
        #include <QWheelEvent>
        #include <QKeyEvent>
    #endif

    #include <QLocalSocket>
    #include <QLocalServer>
    #include <QSharedMemory>

    #define USER_FRAMECOUNT (60)

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
        inline bool is_font_ft() const {return mUseFontFT;}
        inline chars font_ft_nickname() const {return mFontFT.mNickName;}
        inline sint32 font_ft_height() const {return mFontFT.mHeight;}
        inline double zoom() const {return mPainter.transform().m11();}
        inline const QRect& scissor() const {return mScissor;}
        inline const QColor& color() const {return mColor;}
        inline const ShaderRole& shader() const {return mShader;}
        // Setter
        inline void SetFont(chars name, sint32 size)
        {mUseFontFT = false; mPainter.setFont(QFont(name, size));}
        inline void SetFontFT(chars nickname, sint32 height)
        {mUseFontFT = true; mFontFT.mNickName = nickname; mFontFT.mHeight = height;}
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
        bool mUseFontFT;
        FTFontClass mFontFT;
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
            setAttribute(Qt::WA_PaintOnScreen);
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
            h_view NewViewHandle = h_view::create_by_ptr(BOSS_DBG manager);
            mViewManager->SetView(NewViewHandle);
            mViewManager->SetCallback(
                [](payload data, sint32 count, chars arg)->void
                {((MainView*) data)->Update(count, arg);}, (payload) this);
            SendCreate();
            SendSizeWhenValid();
            return NewViewHandle;
        }

    private:
        void SendCreate()
        {
            mViewManager->OnCreate();
            mTickTimer.start(1000 / USER_FRAMECOUNT);
        }
        bool CanQuit()
        {
            return mViewManager->OnCanQuit();
        }
        void SendDestroy()
        {
            mTickTimer.stop();
            mViewManager->OnDestroy();
        }
        void SendActivate(bool actived)
        {
            mViewManager->OnActivate(actived);
        }
        void SendSizeWhenValid()
        {
            if(0 < mWidth && 0 < mHeight)
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
        void OnCloseEvent(QCloseEvent* event)
        {
            Platform::Popup::CloseAllTracker();
            if(mViewManager->OnCanQuit())
            {
                event->accept();
                CloseAllWindows();
            }
            event->ignore();
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
            if(mViewManager->OnCanQuit())
            {
                event->accept();
                setAttribute(Qt::WA_DeleteOnClose);
            }
            event->ignore();
        }
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            if(event->button() == Qt::LeftButton)
            {
                mViewManager->OnTouch(TT_Press, 0, event->x(), event->y());
                mLongpressTimer.start(500);
                mLongpressX = event->x();
                mLongpressY = event->y();
            }
            else if(event->button() == Qt::RightButton)
                mViewManager->OnTouch(TT_ExtendPress, 0, event->x(), event->y());
            else if(event->button() == Qt::MiddleButton)
                mViewManager->OnTouch(TT_WheelPress, 0, event->x(), event->y());
            mTooltipTimer.stop();
        }
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            mousePressEvent(event);
        }
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            if(event->buttons() == Qt::NoButton)
            {
                mViewManager->OnTouch(TT_Moving, 0, event->x(), event->y());
                mTooltipTimer.start(300);
                Platform::Popup::HideToolTip();
            }
            else
            {
                if(event->buttons() & Qt::LeftButton)
                    mViewManager->OnTouch(TT_Dragging, 0, event->x(), event->y());
                if(event->buttons() & Qt::RightButton)
                    mViewManager->OnTouch(TT_ExtendDragging, 0, event->x(), event->y());
                if(event->buttons() & Qt::MiddleButton)
                    mViewManager->OnTouch(TT_WheelDragging, 0, event->x(), event->y());
            }
            if(5 < Math::Distance(mLongpressX, mLongpressY, event->x(), event->y()))
                mLongpressTimer.stop();
        }
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            if(event->button() == Qt::LeftButton)
                mViewManager->OnTouch(TT_Release, 0, event->x(), event->y());
            else if(event->button() == Qt::RightButton)
                mViewManager->OnTouch(TT_ExtendRelease, 0, event->x(), event->y());
            else if(event->button() == Qt::MiddleButton)
                mViewManager->OnTouch(TT_WheelRelease, 0, event->x(), event->y());
            mLongpressTimer.stop();
        }
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE
        {
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
            if(!event->isAutoRepeat())
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
            if(!event->isAutoRepeat())
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
                        activateWindow(); // setFocus()는 작동하지 않는다!
                        raise(); // 부모기준 첫번째 자식으로 올림
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

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow()
        {
        }
        ~MainWindow()
        {
        }

    public:
        void InitForWidget(bool frameless, bool topmost)
        {
            mView = new MainView(this);
            setCentralWidget(mView);

            Qt::WindowFlags TypeCollector = Qt::Widget;
            if(frameless)
            {
                #if BOSS_MAC_OSX
                    TypeCollector |= Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint;
                #elif BOSS_WINDOWS | BOSS_LINUX
                    TypeCollector |= Qt::FramelessWindowHint;
                #endif
                setAttribute(Qt::WA_TranslucentBackground);
            }
            if(topmost)
                TypeCollector |= Qt::WindowStaysOnTopHint;
            if(TypeCollector != Qt::Widget)
                setWindowFlags(TypeCollector);
        }
        inline MainView* View()
        {
            return mView;
        }

    protected:
        void moveEvent(QMoveEvent* event) Q_DECL_OVERRIDE
        {
        }
        void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE
        {
            QWidget::resizeEvent(event);
        }
        void changeEvent(QEvent* event) Q_DECL_OVERRIDE
        {
            auto EventType = event->type();
            if(EventType == QEvent::ActivationChange)
                mView->OnActivateEvent(isActiveWindow());
            QWidget::changeEvent(event);
        }
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            mView->OnCloseEvent(event);
        }

    private:
        MainView* mView {nullptr};
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

#endif
