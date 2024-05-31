#pragma once
#include <platform/boss_platform.hpp>
#include <platform/boss_platform_impl.hpp>

#undef size_t
#undef ssize_t

#ifdef BOSS_PLATFORM_QT5

    #include <element/boss_image.hpp>
    #include <format/boss_bmp.hpp>

    #include <QTimer>
    #include <QElapsedTimer>
    #include <QFile>
    #include <QFileInfo>
    #include <QDir>
    #include <QThread>
    #include <QEventLoop>
    #include <QEvent>
    #include <QSettings>
    #include <QCoreApplication>
    #include <QStandardPaths>
    #include <QDateTime>
    #include <QPoint>
    #include <QSize>
    #include <QRect>
    #include <QUrl>

    #ifdef QT_HAVE_GRAPHICS
        #include <QtWidgets>
        #include <QMainWindow>
        #include <QSystemTrayIcon>
        #include <QAction>
        #include <QStyle>
        #include <QToolTip>
        #include <QFont>
        #include <QFontMetrics>
        #include <QPalette>
        #include <QCursor>
        #include <QCloseEvent>
        #include <QResizeEvent>
        #include <QPaintEvent>
        #include <QMouseEvent>
        #include <QWheelEvent>
        #include <QKeyEvent>
        #include <QDesktopServices>
        #include <QOpenGLWidget>
        #include <QOpenGLFunctions>
        #include <QOpenGLShaderProgram>
        #include <QOpenGLFramebufferObject>

        // QPixmap을 GLuint타입 Texture로 바꾸기 위한 기능이 필요하여 include
        // auto CurContext = QGLContext::fromOpenGLContext(ctx);
        // GLuint CurTexture = CurContext->bindTexture(pixmap);
        #include <QGLWidget>
    #endif

    #include <QBuffer>
    #include <QHostInfo>
    #include <QTcpSocket>
    #include <QUdpSocket>
    #include <QTcpServer>
    #include <QNetworkInterface>
    #include <QtNetwork/QSslCertificate>
    #include <QtNetwork/QSslKey>
    #include <QWebSocket>
    #include <QWebSocketServer>
    #if !BOSS_WASM
        #include <QtBluetooth>
        #include <QLowEnergyController>
    #endif

    #ifdef QT_HAVE_SERIALPORT
        #include <QtSerialPort>
        #include <QSerialPortInfo>
    #endif

    #ifdef QT_HAVE_MULTIMEDIA
        #include <QMediaPlayer>
        #include <QMediaPlaylist>
        #include <QAudioOutput>
        #include <QAudioInput>

        #include <QCamera>
        #include <QCameraInfo>
        #include <QCameraImageCapture>
        #include <QAbstractVideoSurface>

        #include <QAudioRecorder>
        #include <QAudioProbe>
        #include <QAudioDeviceInfo>
    #endif

    #include <QLocalSocket>
    #include <QLocalServer>
    #include <QSharedMemory>

    #ifdef QT_HAVE_WEBENGINEWIDGETS
        #include <QWebEngineView>
        #include <QWebEngineProfile>
    #endif

    #ifdef QT_HAVE_PURCHASING
        #include <QtPurchasing>
    #endif

    #if BOSS_IPHONE
        #include <qpa/qplatformnativeinterface.h>
    #endif

    #if BOSS_ANDROID
        #include <QtAndroidExtras>
        #include <jni.h>
        #include <termios.h>
        #include <unistd.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <fcntl.h>
        #include <errno.h>
        #include <string.h>
        extern JNIEnv* GetAndroidJNIEnv();
    #endif

    #define USER_FRAMECOUNT (60)

    class MainData;
    extern MainData* g_data;
    extern QMainWindow* g_window;
    extern QWidget* g_view;
    extern bool g_event_blocked;
    extern sint32 g_argc;
    extern char** g_argv;

    class SizePolicy
    {
    public:
        SizePolicy();
        ~SizePolicy();
    public:
        sint32 m_minwidth;
        sint32 m_minheight;
        sint32 m_maxwidth;
        sint32 m_maxheight;
    };

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
        inline float zoom() const {return mPainter.transform().m11();}
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

    class ViewAPI : public QObject
    {
        Q_OBJECT

    public:
        enum ParentType {PT_Null, PT_GenericView, PT_GenericViewGL, PT_MainViewGL, PT_MainViewMDI};
        enum WidgetRequest {WR_Null, WR_NeedExit = -1, WR_NeedHide = -2, WR_NeedShow = -3, WR_NeedShowBy = -4};

    public:
        ViewAPI(ParentType type, buffer buf, View* manager, View::UpdaterCB cb, QWidget* data, QWidget* device)
        {
            m_parent_type = type;
            m_parent_buf = buf;
            m_parent_ptr = nullptr;
            m_view_manager = manager;
            m_next_manager = nullptr;
            m_view_cb = cb;
            m_view_data = data;
            m_paint_device = device;
            if(manager)
                manager->SetCallback(m_view_cb, m_view_data);

            m_width = 0;
            m_height = 0;
            m_request = WR_Null;
            m_paintcount = 0;
            m_longpress_x = 0;
            m_longpress_y = 0;

            connect(&m_tick_timer, &QTimer::timeout, this, &ViewAPI::tick_timeout);
            connect(&m_update_timer, &QTimer::timeout, this, &ViewAPI::update_timeout);
            connect(&m_tooltip_timer, &QTimer::timeout, this, &ViewAPI::tooltip_timeout);
            connect(&m_longpress_timer, &QTimer::timeout, this, &ViewAPI::longpress_timeout);
        }

        ViewAPI(const ViewAPI& rhs) = delete;
        ViewAPI& operator=(const ViewAPI& rhs) = delete;

        ~ViewAPI()
        {
            m_tick_timer.stop();
            m_update_timer.stop();
            m_tooltip_timer.stop();
            m_longpress_timer.stop();

            if(getParent()) sendDestroy();
            Buffer::Free(m_parent_buf);
            delete m_view_manager;
            delete m_next_manager;
        }

    public:
        inline ParentType getParentType() const
        {
            return m_parent_type;
        }

        inline void* getParent() const
        {
            BOSS_ASSERT("잘못된 시나리오입니다", (m_parent_buf != nullptr) != (m_parent_ptr != nullptr));
            return (void*) (((ublock) m_parent_buf) | ((ublock) m_parent_ptr));
        }

        inline void renewParent(void* ptr)
        {
            m_parent_buf = nullptr;
            m_parent_ptr = ptr;

            if(getParent())
                sendCreate();
        }

        inline bool parentIsPtr() const
        {
            return (m_parent_ptr != nullptr);
        }

        inline bool hasViewManager() const
        {
            return (m_view_manager != nullptr);
        }

        void setViewAndCreateAndSize(h_view view)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->SetView(view);

            sendCreate();
            sendSizeWhenValid();
        }

        h_view changeViewManagerAndDestroy(View* manager)
        {
            sendDestroy();
            h_view OldViewHandle;
            if(m_view_manager)
            {
                OldViewHandle = m_view_manager->SetView(h_view::null());
                delete m_view_manager;
            }

            m_view_manager = manager;
            m_view_manager->SetCallback(m_view_cb, m_view_data);
            return OldViewHandle;
        }

        inline void setNextViewManager(View* manager)
        {
            delete m_next_manager;
            m_next_manager = manager;
            update(1, nullptr);
        }

        inline void changeViewData(QWidget* data)
        {
            m_view_data = data;
            if(m_view_manager)
                m_view_manager->SetCallback(m_view_cb, m_view_data);
        }

        inline ViewClass* getClass() const
        {
            return (ViewClass*) m_view_manager->GetClass();
        }

        inline QWidget* getWidget() const
        {
            if(m_view_manager)
            {
                if(m_view_data)
                    return m_view_data;
                if(m_view_manager->IsNative())
                    return (QWidget*) m_view_manager->GetClass();
            }
            return (QWidget*) getParent();
        }

        inline QWidget* getWidgetForPaint() const
        {
            if(m_paint_device)
                return m_paint_device;
            return getWidget();
        }

        inline void render(sint32 width, sint32 height)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->OnRender(width, height, 0, 0, width, height);

            if(m_next_manager)
            {
                h_view OldViewHandle = changeViewManagerAndDestroy(m_next_manager);
                setViewAndCreateAndSize(OldViewHandle);
                m_next_manager = nullptr;
            }
        }

        inline void touch(TouchType type, sint32 id, sint32 x, sint32 y)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->OnTouch(type, id, x, y);
        }

        inline void key(sint32 code, chars text, bool pressed)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->OnKey(code, text, pressed);
        }

        inline void sendCreate()
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnCreate();
                m_tick_timer.start(1000 / USER_FRAMECOUNT);
            }
        }

        inline bool canQuit()
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                return m_view_manager->OnCanQuit();
            }
            return true;
        }

        inline void sendDestroy()
        {
            if(m_view_manager != nullptr)
            {
                m_tick_timer.stop();
                g_view = getWidget();
                m_view_manager->OnDestroy();
            }
        }

        inline void sendActivate(bool actived)
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnActivate(actived);
            }
        }

        inline void sendSizeWhenValid()
        {
            if(0 < m_width && 0 < m_height)
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnSize(m_width, m_height);
            }
        }

        inline void sendTick() const
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnTick();
            }
        }

        inline void sendNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out) const
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->SendNotify(type, topic, in, out, false);
        }

        inline void sendDirectNotify(NotifyType type, chars topic, id_share in) const
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->SendNotify(type, topic, in, nullptr, true);
        }

    public:
        void resize(sint32 width, sint32 height)
        {
            m_width = width;
            m_height = height;
            sendSizeWhenValid();
        }

        void paint()
        {
            // for assert dialog's recursive call
            if(CanvasClass::enabled()) return;
            CanvasClass Canvas(getWidgetForPaint());
            render(m_width, m_height);
        }

        void nextPaint()
        {
            if(0 < m_paintcount)
                m_paintcount--;
        }

        void dirtyAndUpdate()
        {
            m_view_manager->DirtyAllSurfaces();
            update(1, nullptr);
        }

        void update(sint32 count, chars arg)
        {
            if(count <= WR_NeedExit)
            {
                m_request = (WidgetRequest) count;
                if(arg) m_request_arg = arg;
            }
            else if(m_paintcount < count)
            {
                if(m_paintcount == 0)
                    m_update_timer.start(1000 / USER_FRAMECOUNT);
                m_paintcount = count;
            }
        }

        void onActivateEvent(bool actived)
        {
            sendActivate(actived);
        }

        bool closeEvent(QCloseEvent* event)
        {
            if(canQuit())
            {
                event->accept();
                return true;
            }
            event->ignore();
            return false;
        }

        void mousePressEvent(QMouseEvent* event)
        {
            if(g_event_blocked)
            {
                event->ignore();
                return;
            }

            if(event->button() == Qt::LeftButton)
            {
                touch(TT_Press, 0, event->x(), event->y());
                m_longpress_timer.start(500);
                m_longpress_x = event->x();
                m_longpress_y = event->y();
            }
            else if(event->button() == Qt::RightButton)
                touch(TT_ExtendPress, 0, event->x(), event->y());
            else if(event->button() == Qt::MidButton)
                touch(TT_WheelPress, 0, event->x(), event->y());

            m_tooltip_timer.stop();
        }

        void mouseMoveEvent(QMouseEvent* event)
        {
            if(g_event_blocked)
            {
                event->ignore();
                return;
            }

            if(event->buttons() == Qt::NoButton)
            {
                touch(TT_Moving, 0, event->x(), event->y());
                m_tooltip_timer.start(300);
                Platform::Popup::HideToolTip();
            }
            else
            {
                if(event->buttons() & Qt::LeftButton)
                    touch(TT_Dragging, 0, event->x(), event->y());
                if(event->buttons() & Qt::RightButton)
                    touch(TT_ExtendDragging, 0, event->x(), event->y());
                if(event->buttons() & Qt::MidButton)
                    touch(TT_WheelDragging, 0, event->x(), event->y());
            }

            if(5 < Math::Distance(m_longpress_x, m_longpress_y, event->x(), event->y()))
                m_longpress_timer.stop();
        }

        void mouseReleaseEvent(QMouseEvent* event)
        {
            if(g_event_blocked)
            {
                event->ignore();
                return;
            }

            if(event->button() == Qt::LeftButton)
                touch(TT_Release, 0, event->x(), event->y());
            else if(event->button() == Qt::RightButton)
                touch(TT_ExtendRelease, 0, event->x(), event->y());
            else if(event->button() == Qt::MidButton)
                touch(TT_WheelRelease, 0, event->x(), event->y());

            m_longpress_timer.stop();
        }

        void wheelEvent(QWheelEvent* event)
        {
            if(g_event_blocked)
            {
                event->ignore();
                return;
            }

            float WheelValue = event->angleDelta().y() / 120.0f;
            while (0 < WheelValue)
            {
                WheelValue = Math::MaxF(0, WheelValue - 1);
                touch(TT_WheelUp, 0, event->position().x(), event->position().y());
            }
            while (WheelValue < 0)
            {
                WheelValue = Math::MinF(WheelValue + 1, 0);
                touch(TT_WheelDown, 0, event->position().x(), event->position().y());
            }
        }

        void keyPressEvent(QKeyEvent* event)
        {
            if(g_event_blocked)
            {
                event->ignore();
                return;
            }
            if(!event->isAutoRepeat())
            {
                #if BOSS_WASM
                    key(event->key(), event->text().toUtf8().constData(), true);
                #else
                    key(event->nativeVirtualKey(), event->text().toUtf8().constData(), true);
                #endif
            }
        }

        void keyReleaseEvent(QKeyEvent* event)
        {
            if(g_event_blocked)
            {
                event->ignore();
                return;
            }
            if(!event->isAutoRepeat())
            {
                #if BOSS_WASM
                    key(event->key(), event->text().toUtf8().constData(), false);
                #else
                    key(event->nativeVirtualKey(), event->text().toUtf8().constData(), false);
                #endif
            }
        }

    private:
        void tick_timeout()
        {
            if(g_event_blocked)
                return;

            if(m_request == WR_Null)
                sendTick();
            else
            {
                if(m_request == WR_NeedExit)
                    QApplication::closeAllWindows();
                else if(m_request == WR_NeedHide)
                    getWidget()->hide();
                else if(m_request == WR_NeedShow)
                    getWidget()->show();
                else if(m_request == WR_NeedShowBy)
                {
                    auto Args = String::Split(m_request_arg);
                    BOSS_ASSERT("Args의 수량이 4개가 아닙니다", Args.Count() == 4);
                    getWidget()->setGeometry(
                        Parser::GetInt(Args[0]), Parser::GetInt(Args[1]),
                        Parser::GetInt(Args[2]), Parser::GetInt(Args[3]));
                    getWidget()->show();
                    getWidget()->activateWindow(); // setFocus()는 작동하지 않는다!
                    getWidget()->raise(); // 부모기준 첫번째 자식으로 올림
                }
                m_request = WR_Null;
                m_request_arg.Empty();
            }
        }

        void update_timeout()
        {
            if(g_event_blocked)
                return;
            if(m_paintcount == 0)
                m_update_timer.stop();
            getWidgetForPaint()->update();
        }

        void tooltip_timeout()
        {
            if(g_event_blocked)
                return;
            m_tooltip_timer.stop();
            point64 CursorPos;
            if(Platform::Utility::GetCursorPosInWindow(CursorPos))
                touch(TT_ToolTip, 0, CursorPos.x, CursorPos.y);
        }

        void longpress_timeout()
        {
            if(g_event_blocked)
                return;
            m_longpress_timer.stop();
            touch(TT_LongPress, 0, m_longpress_x, m_longpress_y);
        }

    private:
        ParentType m_parent_type;
        buffer m_parent_buf;
        void* m_parent_ptr;
        View* m_view_manager;
        View* m_next_manager;
        View::UpdaterCB m_view_cb;
        QWidget* m_view_data;
        QWidget* m_paint_device;

    private:
        sint32 m_width;
        sint32 m_height;
        WidgetRequest m_request;
        String m_request_arg;
        sint32 m_paintcount;
        QTimer m_tick_timer;
        QTimer m_update_timer;
        QTimer m_tooltip_timer;
        QTimer m_longpress_timer;
        sint32 m_longpress_x;
        sint32 m_longpress_y;
    };

    class GenericView : public QFrame
    {
        Q_OBJECT

    public:
        GenericView(QWidget* parent = nullptr) : QFrame(parent)
        {
            m_api = nullptr;

            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);
        }

        GenericView(View* manager, QString name, sint32 width, sint32 height, SizePolicy* policy)
        {
            BOSS_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr, nullptr);
            buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_GenericView, (buffer) this, manager, updater, this, nullptr);
            m_api = (ViewAPI*) NewAPI;

            m_name = name;
            m_firstwidth = width;
            m_firstheight = height;
            m_closing = false;
            setMinimumSize(policy->m_minwidth, policy->m_minheight);
            setMaximumSize(policy->m_maxwidth, policy->m_maxheight);

            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);
        }

        GenericView(h_view view)
        {
            takeView(view);

            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);
        }

        virtual ~GenericView()
        {
            if(m_api && m_api->parentIsPtr())
                Buffer::Free((buffer) m_api);
            m_window.set_buf(nullptr);
        }

        GenericView(const GenericView* rhs) {BOSS_ASSERT("사용금지", false);}
        GenericView& operator=(const GenericView& rhs) {BOSS_ASSERT("사용금지", false); return *this;}

    protected:
        void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE
        {
            m_api->resize(event->size().width(), event->size().height());
        }

        void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
                m_api->paint();
            else QFrame::paintEvent(event);
            m_api->nextPaint();
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            if(m_api->closeEvent(event))
            {
                m_closing = true;
                m_window.set_buf(nullptr);
                setAttribute(Qt::WA_DeleteOnClose);
            }
        }
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count, chars arg)
        {((GenericView*) data)->m_api->update(count, arg);}

    protected:
        void takeView(h_view view)
        {
            GenericView* OldGenericView = cast(view);
            m_api = OldGenericView->m_api;
            OldGenericView->m_api = nullptr;

            m_name = OldGenericView->m_name;
            m_firstwidth = OldGenericView->m_firstwidth;
            m_firstheight = OldGenericView->m_firstheight;
            m_closing = false;
            setMinimumSize(OldGenericView->minimumWidth(), OldGenericView->minimumHeight());
            setMaximumSize(OldGenericView->maximumWidth(), OldGenericView->maximumHeight());
            Buffer::Free((buffer) OldGenericView);

            m_api->renewParent(this);
            m_api->changeViewData(this);
            view.clear_buf();
            view.set_ptr(m_api); // 삭제책임은 GenericView에 이관하였기에
        }

    public:
        inline const QString getName() const
        {
            return m_name;
        }

        inline const QSize getFirstSize() const
        {
            return QSize(m_firstwidth, m_firstheight);
        }

        inline bool closing() const
        {
            return m_closing;
        }

        inline void attachWindow(h_window window)
        {
            m_closing = false;
            m_window = window;
        }

    public:
        static GenericView* cast(h_view view)
        {
            ViewAPI* CurViewAPI = (ViewAPI*) view.get();
            BOSS_ASSERT("타입정보가 일치하지 않습니다", CurViewAPI->getParentType() == ViewAPI::PT_GenericView);
            return (GenericView*) CurViewAPI->getParent();
        }

    public:
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
        ViewAPI* m_api;

    private:
        QString m_name;
        sint32 m_firstwidth;
        sint32 m_firstheight;
        bool m_closing;
        h_window m_window;
    };

    class GenericViewGL : public QOpenGLWidget, protected QOpenGLFunctions
    {
        Q_OBJECT

    public:
        GenericViewGL(QWidget* parent = nullptr) : QOpenGLWidget(parent)
        {
            m_api = nullptr;

            m_fbo_update = true;
            m_fbo = 0;
            m_fbo_render = 0;
            m_fbo_width = 0;
            m_fbo_height = 0;

            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);
        }

        GenericViewGL(View* manager, QString name, sint32 width, sint32 height, SizePolicy* policy)
        {
            BOSS_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr, nullptr);
            buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_GenericViewGL, (buffer) this, manager, updater, this, nullptr);
            m_api = (ViewAPI*) NewAPI;

            m_fbo_update = true;
            m_fbo = 0;
            m_fbo_render = 0;
            m_fbo_width = 0;
            m_fbo_height = 0;

            m_name = name;
            m_firstwidth = width;
            m_firstheight = height;
            m_closing = false;
            setMinimumSize(policy->m_minwidth, policy->m_minheight);
            setMaximumSize(policy->m_maxwidth, policy->m_maxheight);

            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);
        }

        GenericViewGL(h_view view)
        {
            takeView(view);

            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);
        }

        virtual ~GenericViewGL()
        {
            if(m_api && m_api->parentIsPtr())
                Buffer::Free((buffer) m_api);
            m_window.set_buf(nullptr);

            if(QOpenGLContext* ctx = QOpenGLContext::currentContext())
            {
                QOpenGLFunctions* f = ctx->functions();
                f->glDeleteFramebuffers(1, &m_fbo);
                f->glDeleteRenderbuffers(1, &m_fbo_render);
            }
        }

        GenericViewGL(const GenericViewGL* rhs) {BOSS_ASSERT("사용금지", false);}
        GenericViewGL& operator=(const GenericViewGL& rhs) {BOSS_ASSERT("사용금지", false); return *this;}

    protected:
        void initializeGL() Q_DECL_OVERRIDE
        {
            initializeOpenGLFunctions();
        }

        void resizeGL(int width, int height) Q_DECL_OVERRIDE
        {
            m_fbo_update = true;
            m_fbo_width = geometry().width();
            m_fbo_height = geometry().height();
            m_api->resize(m_fbo_width, m_fbo_height);
        }

        void paintGL() Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
            {
                if(m_fbo_update)
                {
                    m_fbo_update = false;
                    if(QOpenGLContext* ctx = QOpenGLContext::currentContext())
                    {
                        QOpenGLFunctions* f = ctx->functions();
                        if(m_fbo) f->glDeleteFramebuffers(1, &m_fbo);
                        if(m_fbo_render) f->glDeleteRenderbuffers(1, &m_fbo_render);

                        f->glGenRenderbuffers(1, &m_fbo_render);
                        f->glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_render);
                        f->glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, m_fbo_width, m_fbo_height);
                        f->glBindRenderbuffer(GL_RENDERBUFFER, 0);

                        GLuint prevFbo = 0;
                        f->glGenFramebuffers(1, &m_fbo);
                        f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &prevFbo);
                        f->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
                        f->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_fbo_render);
                        GLenum status = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);
                        BOSS_ASSERT("프레임버퍼의 생성에 실패하였습니다", status == GL_FRAMEBUFFER_COMPLETE);
                        f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
                    }
                }
                m_api->paint();
            }
            else QOpenGLWidget::paintGL();
            m_api->nextPaint();
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            if(m_api->closeEvent(event))
            {
                m_closing = true;
                m_window.set_buf(nullptr);
                setAttribute(Qt::WA_DeleteOnClose);
            }
        }
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count, chars arg)
        {((GenericViewGL*) data)->m_api->update(count, arg);}

    protected:
        void takeView(h_view view)
        {
            GenericViewGL* OldGenericViewGL = cast(view);
            m_api = OldGenericViewGL->m_api;
            OldGenericViewGL->m_api = nullptr;

            m_fbo_update = OldGenericViewGL->m_fbo_update;
            m_fbo = OldGenericViewGL->m_fbo;
            m_fbo_render = OldGenericViewGL->m_fbo_render;
            m_fbo_width = OldGenericViewGL->m_fbo_width;
            m_fbo_height = OldGenericViewGL->m_fbo_height;
            OldGenericViewGL->m_fbo = 0;
            OldGenericViewGL->m_fbo_render = 0;

            m_name = OldGenericViewGL->m_name;
            m_firstwidth = OldGenericViewGL->m_firstwidth;
            m_firstheight = OldGenericViewGL->m_firstheight;
            m_closing = false;
            setMinimumSize(OldGenericViewGL->minimumWidth(), OldGenericViewGL->minimumHeight());
            setMaximumSize(OldGenericViewGL->maximumWidth(), OldGenericViewGL->maximumHeight());
            Buffer::Free((buffer) OldGenericViewGL);

            m_api->renewParent(this);
            m_api->changeViewData(this);
            view.clear_buf();
            view.set_ptr(m_api); // 삭제책임은 GenericViewGL에 이관하였기에
        }

    public:
        inline const QString getName() const
        {
            return m_name;
        }

        inline const QSize getFirstSize() const
        {
            return QSize(m_firstwidth, m_firstheight);
        }

        inline bool closing() const
        {
            return m_closing;
        }

        inline void attachWindow(h_window window)
        {
            m_closing = false;
            m_window = window;
        }

    public:
        static GenericViewGL* cast(h_view view)
        {
            ViewAPI* CurViewAPI = (ViewAPI*) view.get();
            BOSS_ASSERT("타입정보가 일치하지 않습니다", CurViewAPI->getParentType() == ViewAPI::PT_GenericViewGL);
            return (GenericViewGL*) CurViewAPI->getParent();
        }

    public:
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
        ViewAPI* m_api;

    private:
        bool m_fbo_update;
        GLuint m_fbo;
        GLuint m_fbo_render;
        sint32 m_fbo_width;
        sint32 m_fbo_height;

    private:
        QString m_name;
        sint32 m_firstwidth;
        sint32 m_firstheight;
        bool m_closing;
        h_window m_window;
    };

    class MainViewGL : public QOpenGLWidget, protected QOpenGLFunctions
    {
        Q_OBJECT

    public:
        MainViewGL(QWidget* parent, bool webpaper) : QOpenGLWidget(parent)
        {
            BOSS_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr, nullptr);
            buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_MainViewGL, (buffer) this, nullptr, updater, this, nullptr);
            m_api = (ViewAPI*) NewAPI;

            m_fbo_update = true;
            m_fbo = 0;
            m_fbo_render = 0;
            m_fbo_width = 0;
            m_fbo_height = 0;

            //setAttribute(Qt::WA_PaintOnScreen); // 현재 OpenGL업데이트를 할 방법을 찾을 수 없음
            setAttribute(Qt::WA_NoSystemBackground);
            setAttribute(Qt::WA_AcceptTouchEvents);
            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);
            if(webpaper)
                setAttribute(Qt::WA_AlwaysStackOnTop);
        }

        virtual ~MainViewGL()
        {
            if(m_api->parentIsPtr())
                Buffer::Free((buffer) m_api);
            if(QOpenGLContext* ctx = QOpenGLContext::currentContext())
            {
                QOpenGLFunctions* f = ctx->functions();
                f->glDeleteFramebuffers(1, &m_fbo);
                f->glDeleteRenderbuffers(1, &m_fbo_render);
            }
        }

        MainViewGL(const MainViewGL* rhs) {BOSS_ASSERT("사용금지", false);}
        MainViewGL& operator=(const MainViewGL& rhs) {BOSS_ASSERT("사용금지", false); return *this;}

    public:
        void onActivateEvent(bool actived)
        {
            m_api->onActivateEvent(actived);
        }
        void onCloseEvent(QCloseEvent* event)
        {
            if(m_api->closeEvent(event))
                GenericView::CloseAllWindows();
        }

    protected:
        void initializeGL() Q_DECL_OVERRIDE
        {
            initializeOpenGLFunctions();
        }

        void resizeGL(int width, int height) Q_DECL_OVERRIDE
        {
            m_fbo_update = true;
            m_fbo_width = geometry().width();
            m_fbo_height = geometry().height();
            m_api->resize(m_fbo_width, m_fbo_height);
        }

        void paintGL() Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
            {
                if(m_fbo_update)
                {
                    m_fbo_update = false;
                    if(QOpenGLContext* ctx = QOpenGLContext::currentContext())
                    {
                        QOpenGLFunctions* f = ctx->functions();
                        if(m_fbo) f->glDeleteFramebuffers(1, &m_fbo);
                        if(m_fbo_render) f->glDeleteRenderbuffers(1, &m_fbo_render);

                        f->glGenRenderbuffers(1, &m_fbo_render);
                        f->glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_render);
                        f->glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, m_fbo_width, m_fbo_height);
                        f->glBindRenderbuffer(GL_RENDERBUFFER, 0);

                        GLuint prevFbo = 0;
                        f->glGenFramebuffers(1, &m_fbo);
                        f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &prevFbo);
                        f->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
                        f->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_fbo_render);
                        GLenum status = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);
                        BOSS_ASSERT("프레임버퍼의 생성에 실패하였습니다", status == GL_FRAMEBUFFER_COMPLETE);
                        f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
                    }
                }
                m_api->paint();
            }
            else QOpenGLWidget::paintGL();
            m_api->nextPaint();
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE {m_api->closeEvent(event);}
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count, chars arg)
        {
            ((MainViewGL*) data)->m_api->update(count, arg);
        }

    public:
        ViewAPI* m_api;

    private:
        bool m_fbo_update;
        GLuint m_fbo;
        GLuint m_fbo_render;
        sint32 m_fbo_width;
        sint32 m_fbo_height;
    };

    class MainViewMDI : public QMdiArea
    {
        Q_OBJECT

    public:
        MainViewMDI(QWidget* parent) : QMdiArea(parent)
        {
            BOSS_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr, nullptr);
            buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_MainViewMDI, (buffer) this, nullptr, updater, this, viewport());
            m_api = (ViewAPI*) NewAPI;

            m_width = 0;
            m_height = 0;

            setAttribute(Qt::WA_PaintOnScreen);
            setAttribute(Qt::WA_NoSystemBackground);
            setAttribute(Qt::WA_AcceptTouchEvents);
            setMouseTracking(true);
            setFocusPolicy(Qt::StrongFocus);
            setAutoFillBackground(false);
        }

        virtual ~MainViewMDI()
        {
            if(m_api->parentIsPtr())
                Buffer::Free((buffer) m_api);
        }

        MainViewMDI(const MainViewMDI* rhs) {BOSS_ASSERT("사용금지", false);}
        MainViewMDI& operator=(const MainViewMDI& rhs) {BOSS_ASSERT("사용금지", false); return *this;}

    public:
        void onActivateEvent(bool actived)
        {
            m_api->onActivateEvent(actived);
        }
        void onCloseEvent(QCloseEvent* event)
        {
            if(m_api->closeEvent(event))
                GenericView::CloseAllWindows();
        }

    protected:
        void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE
        {
            m_width = event->size().width();
            m_height = event->size().height();
            m_api->resize(m_width, m_height);
        }

        void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
                m_api->paint();
            else QMdiArea::paintEvent(event);
            m_api->nextPaint();
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE {m_api->closeEvent(event);}
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count, chars arg)
        {((MainViewMDI*) data)->m_api->update(count, arg);}

    public:
        ViewAPI* m_api;

    private:
        sint32 m_width;
        sint32 m_height;
    };

    class WidgetBox
    {
    public:
        WidgetBox()
        {
            m_view = nullptr;
            m_widget = nullptr;
        }
        ~WidgetBox()
        {
            if(m_view && !m_view->closing())
                delete m_view;
        }

        void setWidget(GenericView* view, QWidget* widget)
        {
            m_view = view;
            m_widget = widget;
        }

    private:
        GenericView* m_view;
        QWidget* m_widget;
    };

    class TrayIcon : public QSystemTrayIcon
    {
        Q_OBJECT

    public:
        TrayIcon(QWidget* menu)
        {
            m_ref_menu = menu;
            m_saved_size = m_ref_menu->size();
            Qt::WindowFlags TypeCollector = Qt::Dialog;
            TypeCollector |= Qt::FramelessWindowHint;
            TypeCollector |= Qt::WindowStaysOnTopHint;
            m_ref_menu->setWindowFlags(TypeCollector);
            m_ref_menu->installEventFilter(this);
            connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
        }
        ~TrayIcon()
        {
        }

    private slots:
        void iconActivated(QSystemTrayIcon::ActivationReason reason)
        {
            switch(reason)
            {
            case QSystemTrayIcon::Context: // R버튼 클릭
            case QSystemTrayIcon::Trigger: // L버튼 클릭
            case QSystemTrayIcon::MiddleClick: // 휠버튼 클릭
                {
                    const QPoint ClickPos = QCursor::pos();
                    const QRect GeometryRect = QApplication::desktop()->screenGeometry(ClickPos);
                    const float PosX = (GeometryRect.right() < ClickPos.x() + m_saved_size.width())?
                        ClickPos.x() - m_saved_size.width() : ClickPos.x();
                    const float PosY = (GeometryRect.bottom() < ClickPos.y() + m_saved_size.height())?
                        ClickPos.y() - m_saved_size.height() : ClickPos.y();
                    m_ref_menu->setGeometry(PosX, PosY, m_saved_size.width(), m_saved_size.height());
                    m_ref_menu->show();
                    m_ref_menu->activateWindow(); // setFocus()는 작동하지 않는다!
                    m_ref_menu->raise(); // 부모기준 첫번째 자식으로 올림
                    Platform::BroadcastNotify("show", nullptr, NT_TrayPopup, nullptr, true);
                }
                break;
            case QSystemTrayIcon::DoubleClick: // L버튼 더블클릭된 순간
                break;
            default:
                break;
            }
        }

    private:
        bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE
        {
            if(watched == m_ref_menu && event->type() == QEvent::FocusOut)
            {
                m_ref_menu->hide();
                Platform::BroadcastNotify("hide", nullptr, NT_TrayPopup, nullptr, true);
                return true;
            }
            return QObject::eventFilter(watched, event);
        }

    private:
        QWidget* m_ref_menu;
        QSize m_saved_size;
    };

    class TrayBox
    {
    public:
        TrayBox()
        {
            m_tray = nullptr;
        }
        ~TrayBox()
        {
            delete m_tray;
        }

        void setWidget(GenericView* view, QIcon* icon)
        {
            QWidget* OldWidget = view->m_api->getWidget();
            OldWidget->resize(view->getFirstSize());
            m_tray = new TrayIcon(OldWidget);
            if(icon) m_tray->setIcon(*icon);
            m_tray->show();
        }

        void setWidgetGL(GenericViewGL* view, QIcon* icon)
        {
            QWidget* OldWidget = view->m_api->getWidget();
            OldWidget->resize(view->getFirstSize());
            m_tray = new TrayIcon(OldWidget);
            if(icon) m_tray->setIcon(*icon);
            m_tray->show();
        }

    private:
        TrayIcon* m_tray;
    };

    class WebEnginePageForExtraDesktop : public QObject
    {
        Q_OBJECT

    public:
        enum RenderProcessTerminationStatus {
            NormalTerminationStatus = 0,
            AbnormalTerminationStatus,
            CrashedTerminationStatus,
            KilledTerminationStatus
        };
        enum PermissionPolicy {
            PermissionUnknown,
            PermissionGrantedByUser,
            PermissionDeniedByUser
        };
        enum Feature {
            Notifications = 0,
            Geolocation = 1,
            MediaAudioCapture = 2,
            MediaVideoCapture,
            MediaAudioVideoCapture,
            MouseLock,
            DesktopVideoCapture,
            DesktopAudioVideoCapture
        };

    public:
        WebEnginePageForExtraDesktop(QObject* parent = nullptr) {}
        virtual ~WebEnginePageForExtraDesktop() {}

    public:
        enum JavaScriptConsoleMessageLevel {InfoMessageLevel, WarningMessageLevel, ErrorMessageLevel};
        typedef std::function<void(const QVariant&)> WebEngineCallbackForExtraDesktop;

    public:
        void setFeaturePermission(const QUrl &securityOrigin, Feature feature, PermissionPolicy policy) {}
        void runJavaScript(const QString& scriptSource, const WebEngineCallbackForExtraDesktop& resultCallback) {}
        virtual void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID) {}
    };

    class WebEngineViewForExtraDesktop : public QObject
    {
        Q_OBJECT

    public:
        WebEngineViewForExtraDesktop(QWidget* parent = nullptr)
        {
            mPage = new WebEnginePageForExtraDesktop(this);
        }
        virtual ~WebEngineViewForExtraDesktop()
        {
            delete mPage;
        }

    public:
        void setPage(WebEnginePageForExtraDesktop* page)
        {
            delete mPage;
            mPage = page;
        }
        WebEnginePageForExtraDesktop* page() const {return mPage;}
        void setMouseTracking(...) {}
        virtual void closeEvent(QCloseEvent* event) {}

    public:
        WebEnginePageForExtraDesktop* mPage;
    };

    #ifdef QT_HAVE_WEBENGINEWIDGETS
        typedef QWebEnginePage WebEnginePageClass;
        typedef QWebEngineView WebEngineViewClass;
    #else
        typedef WebEnginePageForExtraDesktop WebEnginePageClass;
        typedef WebEngineViewForExtraDesktop WebEngineViewClass;
    #endif

    class MainWebPage : public WebEnginePageClass
    {
        Q_OBJECT

    public:
        MainWebPage(QObject* parent = nullptr) : WebEnginePageClass(parent) {}
        ~MainWebPage() override {}

    private:
        #ifdef QT_HAVE_WEBENGINEWIDGETS
            bool certificateError(const QWebEngineCertificateError& error) override
            {
                return true;
            }
        #endif

        void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID) override
        {
            QString Level;
            switch(level)
            {
            case InfoMessageLevel: Level = "Info"; break;
            case WarningMessageLevel: Level = "Warning"; break;
            case ErrorMessageLevel: Level = "Error"; break;
            }
            Platform::BroadcastNotify(String::Format("JSConsole[%s/%s/%d]:<%s>", Level.toUtf8().constData(),
                sourceID.toUtf8().constData(), lineNumber, message.toUtf8().constData()), nullptr, NT_WindowWeb);
        }
    };

    class MainData
    {
    public:
        enum class WindowType {Normal, Minimize, Maximize};
        WindowType m_lastWindowType;
        rect128 m_lastWindowNormalRect;

    public:
        MainData(QMainWindow* parent) :
            m_lastWindowType(WindowType::Normal), m_lastWindowNormalRect({0, 0, 0, 0})
        {
            m_parent = parent;
            m_viewGL = nullptr;
            m_viewMDI = nullptr;
            #ifdef QT_HAVE_WEBENGINEWIDGETS
                m_webPaper = nullptr;
            #endif
        }
        ~MainData() {}

    public:
        void onActivateEvent(bool actived)
        {
            if(m_viewGL)
                m_viewGL->onActivateEvent(actived);
            else if(m_viewMDI)
                m_viewMDI->onActivateEvent(actived);
        }

        void onCloseEvent(QCloseEvent* event)
        {
            Platform::Popup::CloseAllTracker();
            if(m_viewGL)
                m_viewGL->onCloseEvent(event);
            else if(m_viewMDI)
            {
                m_viewMDI->closeAllSubWindows();
                if(m_viewMDI->currentSubWindow())
                    event->ignore();
                else event->accept();
            }
        }

    public:
        void initForGL(bool frameless, bool topmost, chars url)
        {
            m_viewGL = new MainViewGL(m_parent, url != nullptr);
            m_viewGL->m_api->renewParent(m_viewGL);

            auto MainWidget = new QWidget();
            #ifdef QT_HAVE_WEBENGINEWIDGETS
                if(url)
                {
                    m_webPaper = new QWebEngineView(m_parent);
                    m_webPaper->setPage(new MainWebPage(m_webPaper));
                    m_webPaper->load(QUrl(QString::fromUtf8(url)));
                    auto MainLayout = new QStackedLayout();
                    MainLayout->setStackingMode(QStackedLayout::StackAll);
                    MainLayout->setMargin(0);
                    MainLayout->addWidget(m_webPaper);
                    MainLayout->addWidget(m_viewGL);
                    MainWidget->setLayout(MainLayout);
                }
                else
            #endif
            {
                auto MainLayout = new QHBoxLayout();
                MainLayout->setMargin(0);
                MainLayout->addWidget(m_viewGL);
                MainWidget->setLayout(MainLayout);
            }

            // Qt5.9.1부터는 m_parent->setCentralWidget(m_viewGL)식의 접합은
            // 윈도우즈OS에서 다중 모니터상황에서 레이아웃의 정렬불량 문제가 발생
            m_parent->setCentralWidget(MainWidget);

            Qt::WindowFlags TypeCollector = Qt::Widget;
            if(frameless)
            {
                #if BOSS_MAC_OSX
                    TypeCollector |= Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint;
                #elif BOSS_WINDOWS | BOSS_LINUX
                    TypeCollector |= Qt::FramelessWindowHint;
                #endif
                m_parent->setAttribute(Qt::WA_TranslucentBackground);
            }
            if(topmost)
                TypeCollector |= Qt::WindowStaysOnTopHint;
            if(TypeCollector != Qt::Widget)
                m_parent->setWindowFlags(TypeCollector);
        }

        void initForMDI(bool frameless, bool topmost)
        {
            m_viewMDI = new MainViewMDI(m_parent);
            m_viewMDI->m_api->renewParent(m_viewMDI);
            m_parent->setCentralWidget(m_viewMDI);

            Qt::WindowFlags TypeCollector = Qt::Widget;
            if(frameless)
            {
                #if BOSS_MAC_OSX
                    TypeCollector |= Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint;
                #elif BOSS_WINDOWS | BOSS_LINUX
                    TypeCollector |= Qt::FramelessWindowHint;
                #endif
                m_parent->setAttribute(Qt::WA_TranslucentBackground);
            }
            if(topmost)
                TypeCollector |= Qt::WindowStaysOnTopHint;
            if(TypeCollector != Qt::Widget)
                m_parent->setWindowFlags(TypeCollector);
        }

        void changeTopMost(bool enable)
        {
            m_parent->setWindowFlag(Qt::WindowStaysOnTopHint, enable);
            m_parent->show();
        }

        void setWindowWebUrl(chars url)
        {
            #ifdef QT_HAVE_WEBENGINEWIDGETS
                if(m_webPaper)
                {
                    if(url)
                    {
                        m_webPaper->load(QUrl(QString::fromUtf8(url)));
                        m_webPaper->show();
                    }
                    else
                    {
                        m_webPaper->load(QUrl(QString::fromUtf8("")));
                        m_webPaper->hide();
                    }
                }
            #endif
        }

        void sendWindowWebTouchEvent(TouchType type, sint32 x, sint32 y)
        {
            #ifdef QT_HAVE_WEBENGINEWIDGETS
                if(m_webPaper)
                {
                    QMouseEvent::Type CurType = QMouseEvent::None;
                    sint32 ButtonType = 0;
                    switch(type)
                    {
                    case TT_Moving: CurType = QMouseEvent::MouseMove; break;
                    case TT_Press: CurType = QMouseEvent::MouseButtonPress; ButtonType = 1; break;
                    case TT_Dragging: CurType = QMouseEvent::MouseMove; ButtonType = 1; break;
                    case TT_Release: CurType = QMouseEvent::MouseButtonRelease; ButtonType = 1; break;
                    case TT_ExtendPress: CurType = QMouseEvent::MouseButtonPress; ButtonType = 2; break;
                    case TT_ExtendDragging: CurType = QMouseEvent::MouseMove; ButtonType = 2; break;
                    case TT_ExtendRelease: CurType = QMouseEvent::MouseButtonRelease; ButtonType = 2; break;
                    default: BOSS_ASSERT("해당 case가 준비되지 않았습니다", false);
                    }
                    QMouseEvent NewEvent(CurType, QPoint(x, y),
                        (ButtonType == 0)? Qt::NoButton : ((ButtonType == 1)? Qt::LeftButton : Qt::RightButton),
                        (ButtonType == 0)? Qt::NoButton : ((ButtonType == 1)? Qt::LeftButton : Qt::RightButton),
                        Qt::NoModifier);
                    Q_FOREACH(QObject* obj, m_webPaper->page()->view()->children())
                    {
                        if(qobject_cast<QWidget*>(obj))
                            QApplication::sendEvent(obj, &NewEvent);
                    }
                }
            #endif
        }

        void sendWindowWebKeyEvent(sint32 code, chars text, bool pressed)
        {
            #ifdef QT_HAVE_WEBENGINEWIDGETS
                if(m_webPaper)
                {
                    if(auto CurWidget = m_webPaper->focusProxy())
                    {
                        QKeyEvent NewEvent((pressed)? QKeyEvent::KeyPress : QKeyEvent::KeyRelease, code, Qt::NoModifier, text);
                        QApplication::sendEvent(CurWidget, &NewEvent);
                    }
                }
            #endif
        }

        void callWindowWebJSFunction(chars script, sint32 matchid)
        {
            #ifdef QT_HAVE_WEBENGINEWIDGETS
                if(m_webPaper)
                {
                    m_webPaper->page()->runJavaScript(script,
                        [this, matchid](const QVariant& v)->void
                        {
                            Platform::BroadcastNotify(String::Format("JSResult[%d]:<%s>",
                                matchid, v.toString().toUtf8().constData()), nullptr, NT_WindowWeb);
                        });
                }
            #endif
        }

        ViewAPI* getMainAPI()
        {
            BOSS_ASSERT("GL/MDI의 초기화가 되어있지 않습니다", m_viewGL || m_viewMDI);
            if(m_viewGL) return m_viewGL->m_api;
            return m_viewMDI->m_api;
        }

        QOpenGLWidget* getGLWidget()
        {
            return m_viewGL;
        }

        QWidget* bindWidget(GenericView* view)
        {
            BOSS_ASSERT("GL/MDI의 초기화가 되어있지 않습니다", m_viewGL || m_viewMDI);
            if(m_viewGL) return view->m_api->getWidget();
            return m_viewMDI->addSubWindow(view->m_api->getWidget());
        }

    public:
        QMenu* getMenu(chars name)
        {
            QMenu** Result = m_menuMap.Access(name);
            if(!Result)
            {
                m_menuMap(name) = g_window->menuBar()->addMenu(QString::fromUtf8(name, -1));
                Result = m_menuMap.Access(name);
            }
            return *Result;
        }

        QToolBar* getToolbar(chars name)
        {
            QToolBar** Result = m_toolbarMap.Access(name);
            if(!Result)
            {
                m_toolbarMap(name) = g_window->addToolBar(QString::fromUtf8(name, -1));
                Result = m_toolbarMap.Access(name);
            }
            return *Result;
        }

    private:
        QMainWindow* m_parent;
        MainViewGL* m_viewGL;
        MainViewMDI* m_viewMDI;
        #ifdef QT_HAVE_WEBENGINEWIDGETS
            QWebEngineView* m_webPaper;
        #endif
        Map<QMenu*> m_menuMap;
        Map<QToolBar*> m_toolbarMap;
    };

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow() : m_window_pos({0, 0}), m_window_pos_old({0, 0}), m_window_size({0, 0}), m_window_size_old({0, 0})
        {
            setUnifiedTitleAndToolBarOnMac(true);
            g_data = new MainData(this);
            connect(&m_msec_timer, &QTimer::timeout, this, &MainWindow::msec_timeout);
            m_msec_timer.start(1);
            m_inited_platform = false;
            m_first_visible = true;
        }

        ~MainWindow()
        {
            m_msec_timer.stop();
            delete g_data;
            g_data = nullptr;
        }

    public:
        void SetInitedPlatform() {m_inited_platform = true;}
        bool initedPlatform() const {return m_inited_platform;}
        void SetFirstVisible(bool visible) {m_first_visible = visible;}
        bool firstVisible() const {return m_first_visible;}

    protected:
        void moveEvent(QMoveEvent* event) Q_DECL_OVERRIDE
        {
            m_window_pos_old = m_window_pos;
            auto CurPos = event->pos();
            m_window_pos.x = CurPos.x();
            m_window_pos.y = CurPos.y();
        }

        void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE
        {
            m_window_size_old = m_window_size;
            auto CurSize = event->size();
            m_window_size.w = CurSize.width();
            m_window_size.h = CurSize.height();
            QWidget::resizeEvent(event);
        }

        void changeEvent(QEvent* event) Q_DECL_OVERRIDE
        {
            auto EventType = event->type();
            if(EventType == QEvent::WindowStateChange)
            {
                switch(windowState())
                {
                case Qt::WindowNoState:
                    if(g_data->m_lastWindowType != MainData::WindowType::Normal)
                    {
                        g_data->m_lastWindowType = MainData::WindowType::Normal;
                        auto& CurRect = geometry();
                        const auto NewRect = QRect(g_data->m_lastWindowNormalRect.l, g_data->m_lastWindowNormalRect.t,
                            g_data->m_lastWindowNormalRect.r - g_data->m_lastWindowNormalRect.l,
                            g_data->m_lastWindowNormalRect.b - g_data->m_lastWindowNormalRect.t);
                        if(CurRect != NewRect)
                            setGeometry(NewRect);
                    }
                    break;
                case Qt::WindowMinimized:
                    if(g_data->m_lastWindowType == MainData::WindowType::Normal)
                    {
                        g_data->m_lastWindowNormalRect.l = m_window_pos.x;
                        g_data->m_lastWindowNormalRect.t = m_window_pos.y;
                        g_data->m_lastWindowNormalRect.r = m_window_pos.x + m_window_size.w;
                        g_data->m_lastWindowNormalRect.b = m_window_pos.y + m_window_size.h;
                    }
                    g_data->m_lastWindowType = MainData::WindowType::Minimize;
                    break;
                case Qt::WindowMaximized:
                case Qt::WindowFullScreen:
                    if(g_data->m_lastWindowType == MainData::WindowType::Normal)
                    {
                        g_data->m_lastWindowNormalRect.l = m_window_pos_old.x;
                        g_data->m_lastWindowNormalRect.t = m_window_pos_old.y;
                        g_data->m_lastWindowNormalRect.r = m_window_pos_old.x + m_window_size_old.w;
                        g_data->m_lastWindowNormalRect.b = m_window_pos_old.y + m_window_size_old.h;
                    }
                    g_data->m_lastWindowType = MainData::WindowType::Maximize;
                    break;
                }
            }
            else if(EventType == QEvent::ActivationChange)
                g_data->onActivateEvent(isActiveWindow());
            QWidget::changeEvent(event);
        }

        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            g_data->onCloseEvent(event);
        }

    private slots:
        void OnSlot()
        {
        }

    private:
        void msec_timeout()
        {
            if(!g_event_blocked)
            {
                const uint64 LimitMsec = Platform::Utility::CurrentTimeMsec() + 20;
                do {PlatformImpl::Core::FlushProcedure();}
                while(PlatformImpl::Core::CallProcedures(LimitMsec - Platform::Utility::CurrentTimeMsec()));
            }
        }

    private:
        point64 m_window_pos;
        point64 m_window_pos_old;
        size64 m_window_size;
        size64 m_window_size_old;
        QTimer m_msec_timer;
        bool m_inited_platform;
        bool m_first_visible;
    };

    class StackMessage
    {
    public:
        static StackMessage& Me()
        {static StackMessage _; return _;}

    public:
        void Push()
        {
            m_strings.AtAdding();
        }

        void Pop()
        {
            BOSS_ASSERT("StackMessage의 Pop이 초과됩니다", 0 < m_strings.Count());
            m_strings.SubtractionOne();
        }

        sint32 Count()
        {
            return m_strings.Count();
        }

        String Get(String str)
        {
            BOSS_ASSERT("StackMessage의 Push가 부족합니다", 0 < m_strings.Count());
            m_strings.At(-1) = str;
            String Result;
            for(sint32 i = 0, iend = m_strings.Count(); i < iend; ++i)
            {
                if(0 < i) Result += " → ";
                Result += m_strings[i];
            }
            return Result;
        }

    private:
        StackMessage()
        {
        }
        ~StackMessage()
        {
        }

        Strings m_strings;
    };

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

    class OpenGLClass
    {
    private:
        OpenGLClass()
        {
            InitAllShaders(QOpenGLContext::currentContext()->isOpenGLES());
            const uint08 SampleBGRA[4] = {255, 255, 255, 255};
            mFillTexture = Platform::Graphics::CreateTexture(false, false, 1, 1, SampleBGRA);
        }
        ~OpenGLClass()
        {
            Platform::Graphics::RemoveTexture(mFillTexture);
            TermShader();
        }

    public:
        static OpenGLClass& ST()
        {static OpenGLClass _; return _;}

    public:
        void FillRect(uint32 fbo, const BOSS::Rect& rect, const BOSS::Color& color)
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            QOpenGLFunctions* f = ctx->functions();

            GLuint prevFbo = 0;
            if(fbo != 0)
            {
                f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &prevFbo);
                f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                TestGL(BOSS_DBG 0);
            }

            GLint ViewPortValues[4] = {0};
            f->glGetIntegerv(GL_VIEWPORT, ViewPortValues);
            const GLint Width = ViewPortValues[2];
            const GLint Height = ViewPortValues[3];
            BOSS::Rect NewRect;
            NewRect.l = 2 * rect.l / Width - 1;
            NewRect.t = 1 - 2 * rect.t / Height;
            NewRect.r = 2 * rect.r / Width - 1;
            NewRect.b = 1 - 2 * rect.b / Height;

            f->glActiveTexture(GL_TEXTURE0);
            f->glBindTexture(GL_TEXTURE_2D, Platform::Graphics::GetTextureID(mFillTexture));
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            const sint32 CurProgram = CanvasClass::get()->shader();
            const GLfloat CurResolution[2] {1, 1};
            f->glUseProgram(mPrograms[CurProgram]); TestGL(BOSS_DBG 0);
            f->glBindBuffer(GL_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
            f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::Vertice); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::Color); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::TexCoords); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::Vertice, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].vertices[0]); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &mAttrib[0].colors[0]); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].texcoords[0]); TestGL(BOSS_DBG 0);
            f->glUniformMatrix4fv(mUniformMatrices[CurProgram], 1, GL_FALSE, (const GLfloat*) &mMatrix[0][0]); TestGL(BOSS_DBG 0);
            f->glUniform2fv(mUniformResolutions[CurProgram], 1, (const GLfloat*) &CurResolution[0]); TestGL(BOSS_DBG 0);

            f->glDisable(GL_CULL_FACE); TestGL(BOSS_DBG 0);
            f->glDisable(GL_DEPTH_TEST); TestGL(BOSS_DBG 0);
            f->glEnable(GL_BLEND); TestGL(BOSS_DBG 0);
            f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); TestGL(BOSS_DBG 0);
            f->glEnable(GL_SCISSOR_TEST); TestGL(BOSS_DBG 0);
            const float Zoom = CanvasClass::get()->zoom();
            const QRect& CurScissor = CanvasClass::get()->scissor();
            const int ScreenHeight = CanvasClass::get()->painter().window().height() / Zoom;
            f->glScissor(CurScissor.x() * Zoom,
                (ScreenHeight - (CurScissor.y() + CurScissor.height())) * Zoom,
                CurScissor.width() * Zoom,
                CurScissor.height() * Zoom);

            mAttrib[0].vertices[0] = NewRect.l;
            mAttrib[0].vertices[1] = NewRect.t;
            mAttrib[0].color32 = color.ToABGR();
            mAttrib[0].texcoords[0] = 0;
            mAttrib[0].texcoords[1] = 0;
            mAttrib[1].vertices[0] = NewRect.r;
            mAttrib[1].vertices[1] = NewRect.t;
            mAttrib[1].color32 = color.ToABGR();
            mAttrib[1].texcoords[0] = 1;
            mAttrib[1].texcoords[1] = 0;
            mAttrib[2].vertices[0] = NewRect.l;
            mAttrib[2].vertices[1] = NewRect.b;
            mAttrib[2].color32 = color.ToABGR();
            mAttrib[2].texcoords[0] = 0;
            mAttrib[2].texcoords[1] = 1;
            mAttrib[3].vertices[0] = NewRect.r;
            mAttrib[3].vertices[1] = NewRect.b;
            mAttrib[3].color32 = color.ToABGR();
            mAttrib[3].texcoords[0] = 1;
            mAttrib[3].texcoords[1] = 1;
            f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); TestGL(BOSS_DBG 0);

            if(fbo != 0)
            {
                f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
                TestGL(BOSS_DBG 0);
            }
        }
        void DrawPixmap(uint32 fbo, float x, float y, const BOSS::Point (&ps)[3],
            const QPixmap& pixmap, const BOSS::Point (&ips)[3], const BOSS::Color (&colors)[3])
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            QOpenGLFunctions* f = ctx->functions();

            GLuint prevFbo = 0;
            if(fbo != 0)
            {
                f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &prevFbo);
                f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                TestGL(BOSS_DBG 0);
            }

            GLint ViewPortValues[4] = {0};
            f->glGetIntegerv(GL_VIEWPORT, ViewPortValues);
            const GLint DstWidth = ViewPortValues[2];
            const GLint DstHeight = ViewPortValues[3];

            auto CurContext = QGLContext::fromOpenGLContext(ctx);
            GLuint CurTexture = CurContext->bindTexture(pixmap);
            f->glActiveTexture(GL_TEXTURE0);
            f->glBindTexture(GL_TEXTURE_2D, CurTexture);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            const sint32 CurProgram = CanvasClass::get()->shader();
            const GLfloat CurResolution[2] {(GLfloat) pixmap.width(), (GLfloat) pixmap.height()};
            f->glUseProgram(mPrograms[CurProgram]); TestGL(BOSS_DBG 0);
            f->glBindBuffer(GL_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
            f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::Vertice); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::Color); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::TexCoords); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::Vertice, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].vertices[0]); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &mAttrib[0].colors[0]); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].texcoords[0]); TestGL(BOSS_DBG 0);
            f->glUniformMatrix4fv(mUniformMatrices[CurProgram], 1, GL_FALSE, (const GLfloat*) &mMatrix[0][0]); TestGL(BOSS_DBG 0);
            f->glUniform2fv(mUniformResolutions[CurProgram], 1, (const GLfloat*) &CurResolution[0]); TestGL(BOSS_DBG 0);

            f->glDisable(GL_CULL_FACE); TestGL(BOSS_DBG 0);
            f->glDisable(GL_DEPTH_TEST); TestGL(BOSS_DBG 0);
            f->glEnable(GL_BLEND); TestGL(BOSS_DBG 0);
            f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); TestGL(BOSS_DBG 0);
            f->glEnable(GL_SCISSOR_TEST); TestGL(BOSS_DBG 0);
            const float Zoom = CanvasClass::get()->zoom();
            const QRect& CurScissor = CanvasClass::get()->scissor();
            const int ScreenHeight = CanvasClass::get()->painter().window().height() / Zoom;
            f->glScissor(CurScissor.x() * Zoom,
                (ScreenHeight - (CurScissor.y() + CurScissor.height())) * Zoom,
                CurScissor.width() * Zoom,
                CurScissor.height() * Zoom);

            const bool NeedReverse = true; //!(fbo == 0 && pixmap.devicePixelRatio() == 1);
            for(int i = 0; i < 3; ++i)
            {
                mAttrib[i].vertices[0] = 2 * (x + ps[i].x) * Zoom / DstWidth - 1;
                mAttrib[i].vertices[1] = 1 - 2 * (y + ps[i].y) * Zoom / DstHeight;
                mAttrib[i].color32 = colors[i].ToABGR();
                mAttrib[i].texcoords[0] = ips[i].x;
                mAttrib[i].texcoords[1] = (NeedReverse)? 1 - ips[i].y : ips[i].y;
            }
            f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 3); TestGL(BOSS_DBG 0);

            if(fbo != 0)
            {
                f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
                TestGL(BOSS_DBG 0);
            }
        }
        void DrawTexture(uint32 fbo, const BOSS::Rect& rect, id_texture_read tex, const BOSS::Rect& texrect, const BOSS::Color& color, orientationtype ori, bool antialiasing)
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            QOpenGLFunctions* f = ctx->functions();

            GLuint prevFbo = 0;
            if(fbo != 0)
            {
                f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &prevFbo);
                f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                TestGL(BOSS_DBG 0);
            }

            GLint ViewPortValues[4] = {0};
            f->glGetIntegerv(GL_VIEWPORT, ViewPortValues);
            const GLint DstWidth = ViewPortValues[2];
            const GLint DstHeight = ViewPortValues[3];
            BOSS::Rect NewRect;
            NewRect.l = 2 * rect.l / DstWidth - 1;
            NewRect.t = 1 - 2 * rect.t / DstHeight;
            NewRect.r = 2 * rect.r / DstWidth - 1;
            NewRect.b = 1 - 2 * rect.b / DstHeight;

            const bool IsNV21 = (mPrograms[SR_Nv21] && Platform::Graphics::IsTextureNV21(tex));
            if(IsNV21)
            {
                f->glActiveTexture(GL_TEXTURE0);
                f->glBindTexture(GL_TEXTURE_2D, Platform::Graphics::GetTextureID(tex, 0));
                f->glUniform1i(mUniformTextureY, 0);
                f->glActiveTexture(GL_TEXTURE1);
                f->glBindTexture(GL_TEXTURE_2D, Platform::Graphics::GetTextureID(tex, 1));
                f->glUniform1i(mUniformTextureUV, 1);
            }
            else
            {
                f->glActiveTexture(GL_TEXTURE0);
                f->glBindTexture(GL_TEXTURE_2D, Platform::Graphics::GetTextureID(tex));
            }
            if(antialiasing)
            {
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }

            const sint32 CurProgram = (IsNV21)? SR_Nv21 : CanvasClass::get()->shader();
            const GLint SrcWidth = Platform::Graphics::GetTextureWidth(tex);
            const GLint SrcHeight = Platform::Graphics::GetTextureHeight(tex);
            const GLfloat CurResolution[2] {(GLfloat) SrcWidth, (GLfloat) SrcHeight};
            f->glUseProgram(mPrograms[CurProgram]); TestGL(BOSS_DBG 0);
            f->glBindBuffer(GL_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
            f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::Vertice); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::Color); TestGL(BOSS_DBG 0);
            f->glEnableVertexAttribArray(AttribID::TexCoords); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::Vertice, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].vertices[0]); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &mAttrib[0].colors[0]); TestGL(BOSS_DBG 0);
            f->glVertexAttribPointer(AttribID::TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].texcoords[0]); TestGL(BOSS_DBG 0);
            f->glUniformMatrix4fv(mUniformMatrices[CurProgram], 1, GL_FALSE, (const GLfloat*) &mMatrix[0][0]); TestGL(BOSS_DBG 0);
            f->glUniform2fv(mUniformResolutions[CurProgram], 1, (const GLfloat*) &CurResolution[0]); TestGL(BOSS_DBG 0);

            f->glDisable(GL_CULL_FACE); TestGL(BOSS_DBG 0);
            f->glDisable(GL_DEPTH_TEST); TestGL(BOSS_DBG 0);
            f->glEnable(GL_BLEND); TestGL(BOSS_DBG 0);
            f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); TestGL(BOSS_DBG 0);
            f->glEnable(GL_SCISSOR_TEST); TestGL(BOSS_DBG 0);
            const float Zoom = CanvasClass::get()->zoom();
            const QRect& CurScissor = CanvasClass::get()->scissor();
            const int ScreenHeight = CanvasClass::get()->painter().window().height() / Zoom;
            f->glScissor(CurScissor.x() * Zoom,
                (ScreenHeight - (CurScissor.y() + CurScissor.height())) * Zoom,
                CurScissor.width() * Zoom,
                CurScissor.height() * Zoom);

            mAttrib[0].vertices[0] = NewRect.l;
            mAttrib[0].vertices[1] = NewRect.t;
            mAttrib[0].color32 = color.ToABGR();
            mAttrib[1].vertices[0] = NewRect.r;
            mAttrib[1].vertices[1] = NewRect.t;
            mAttrib[1].color32 = color.ToABGR();
            mAttrib[2].vertices[0] = NewRect.l;
            mAttrib[2].vertices[1] = NewRect.b;
            mAttrib[2].color32 = color.ToABGR();
            mAttrib[3].vertices[0] = NewRect.r;
            mAttrib[3].vertices[1] = NewRect.b;
            mAttrib[3].color32 = color.ToABGR();

            sint32 UV[4] = {2, 3, 1, 0};
            switch(ori)
            {
            case orientationtype_normal0: UV[0] = 2; UV[1] = 3; UV[2] = 1; UV[3] = 0; break;
            case orientationtype_normal90: UV[0] = 0; UV[1] = 2; UV[2] = 3; UV[3] = 1; break;
            case orientationtype_normal180: UV[0] = 1; UV[1] = 0; UV[2] = 2; UV[3] = 3; break;
            case orientationtype_normal270: UV[0] = 3; UV[1] = 1; UV[2] = 0; UV[3] = 2; break;
            case orientationtype_fliped0: UV[0] = 3; UV[1] = 2; UV[2] = 0; UV[3] = 1; break;
            case orientationtype_fliped90: UV[0] = 2; UV[1] = 0; UV[2] = 1; UV[3] = 3; break;
            case orientationtype_fliped180: UV[0] = 0; UV[1] = 1; UV[2] = 3; UV[3] = 2; break;
            case orientationtype_fliped270: UV[0] = 1; UV[1] = 3; UV[2] = 2; UV[3] = 0; break;
            }

            mAttrib[UV[0]].texcoords[0] = texrect.l / SrcWidth; // 좌측상단
            mAttrib[UV[0]].texcoords[1] = (SrcHeight - texrect.b) / SrcHeight;
            mAttrib[UV[1]].texcoords[0] = texrect.r / SrcWidth; // 우측상단
            mAttrib[UV[1]].texcoords[1] = (SrcHeight - texrect.b) / SrcHeight;
            mAttrib[UV[2]].texcoords[0] = texrect.r / SrcWidth; // 우측하단
            mAttrib[UV[2]].texcoords[1] = (SrcHeight - texrect.t) / SrcHeight;
            mAttrib[UV[3]].texcoords[0] = texrect.l / SrcWidth; // 좌측하단
            mAttrib[UV[3]].texcoords[1] = (SrcHeight - texrect.t) / SrcHeight;
            f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); TestGL(BOSS_DBG 0);

            if(fbo != 0)
            {
                f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
                TestGL(BOSS_DBG 0);
            }
        }

    private:
        void InitShader(ShaderRole role, chars vsource, chars fsource)
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            QOpenGLFunctions* f = ctx->functions();

            mVShaders[role] = f->glCreateShader(GL_VERTEX_SHADER); TestGL(BOSS_DBG 0);
            f->glShaderSource(mVShaders[role], 1, &vsource, NULL); TestGL(BOSS_DBG 0);
            f->glCompileShader(mVShaders[role]); TestShader(BOSS_DBG mVShaders[role]);

            mFShaders[role] = f->glCreateShader(GL_FRAGMENT_SHADER); TestGL(BOSS_DBG 0);
            f->glShaderSource(mFShaders[role], 1, &fsource, NULL); TestGL(BOSS_DBG 0);
            f->glCompileShader(mFShaders[role]); TestShader(BOSS_DBG mFShaders[role]);

            mPrograms[role] = f->glCreateProgram(); TestGL(BOSS_DBG 0);
            f->glAttachShader(mPrograms[role], mVShaders[role]); TestShader(BOSS_DBG mVShaders[role]);
            f->glAttachShader(mPrograms[role], mFShaders[role]); TestShader(BOSS_DBG mFShaders[role]);

            f->glLinkProgram(mPrograms[role]); TestProgram(BOSS_DBG mPrograms[role]);
            f->glValidateProgram(mPrograms[role]); TestProgram(BOSS_DBG mPrograms[role]);

            LoadIdentity();
            f->glUseProgram(mPrograms[role]); TestProgram(BOSS_DBG mPrograms[role]);
            mUniformMatrices[role] = f->glGetUniformLocation(mPrograms[role], "u_matrix"); TestGL(BOSS_DBG 0);
            mUniformResolutions[role] = f->glGetUniformLocation(mPrograms[role], "u_resolution"); TestGL(BOSS_DBG 0);
            if(role == SR_Nv21)
            {
                mUniformTextureY = f->glGetUniformLocation(mPrograms[role], "u_texture_y"); TestGL(BOSS_DBG 0);
                mUniformTextureUV = f->glGetUniformLocation(mPrograms[role], "u_texture_uv"); TestGL(BOSS_DBG 0);
            }
        }
        void InitAllShaders(bool es)
        {
            chars VSourceNormal = "#version %s\n"
                "layout (location = %d) in highp vec2 a_position;\n"
                "layout (location = %d) in highp vec4 a_color;\n"
                "layout (location = %d) in highp vec2 a_texcoord;\n"
                "uniform highp mat4 u_matrix;\n"
                "uniform highp vec2 u_resolution;\n"
                "out mediump vec4 v_fragmentColor;\n"
                "out mediump vec2 v_texCoord;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = u_matrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
                "    v_fragmentColor = a_color;\n"
                "    v_texCoord = a_texcoord;\n"
                "}";

            chars VersionCode = (es)? "300 es" : "330 core";
            InitShader(SR_Normal,
                String::Format(VSourceNormal,
                    VersionCode, AttribID::Vertice, AttribID::Color, AttribID::TexCoords),
                String::Format(
                    "#version %s\n"
                    "layout (location = 0) out highp vec4 oColour;\n"
                    "uniform highp mat4 u_matrix;\n"
                    "uniform highp vec2 u_resolution;\n"
                    "uniform highp sampler2D u_texture;\n"
                    "in mediump vec4 v_fragmentColor;\n"
                    "in mediump vec2 v_texCoord;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    oColour = v_fragmentColor * texture(u_texture, v_texCoord);\n"
                    "}", VersionCode));

            InitShader(SR_Nv21,
                String::Format(
                    "#version %s\n"
                    "layout (location = %d) in highp vec2 a_position;\n"
                    "layout (location = %d) in highp vec4 a_color;\n"
                    "layout (location = %d) in highp vec2 a_texcoord;\n"
                    "uniform highp mat4 u_matrix;\n"
                    "uniform highp vec2 u_resolution;\n"
                    "uniform highp sampler2D u_texture_y;\n"
                    "uniform highp sampler2D u_texture_uv;\n"
                    "out mediump vec4 v_fragmentColor;\n"
                    "out mediump vec2 v_texCoord;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    gl_Position = u_matrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
                    "    v_fragmentColor = a_color;\n"
                    "    v_texCoord = a_texcoord;\n"
                    "}", VersionCode, AttribID::Vertice, AttribID::Color, AttribID::TexCoords),
                String::Format(
                    "#version %s\n"
                    "layout (location = 0) out highp vec4 oColour;\n"
                    "uniform highp mat4 u_matrix;\n"
                    "uniform highp vec2 u_resolution;\n"
                    "uniform highp sampler2D u_texture_y;\n"
                    "uniform highp sampler2D u_texture_uv;\n"
                    "in mediump vec4 v_fragmentColor;\n"
                    "in mediump vec2 v_texCoord;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    highp float y = texture(u_texture_y, v_texCoord).r;\n"
                    "    highp float u = texture(u_texture_uv, v_texCoord).a - 0.5;\n"
                    "    highp float v = texture(u_texture_uv, v_texCoord).r - 0.5;\n"
                    "    highp float r = y + 1.403 * v;\n"
                    "    highp float g = y - 0.344 * u - 0.714 * v;\n"
                    "    highp float b = y + 1.770 * u;\n"
                    "    oColour = v_fragmentColor * vec4(r, g, b, 1.0);\n"
                    "}", VersionCode));

            chars FSourceBlur = "#version %s\n"
                "layout (location = 0) out highp vec4 oColour;\n"
                "uniform highp mat4 u_matrix;\n"
                "uniform highp vec2 u_resolution;\n"
                "uniform highp sampler2D u_texture;\n"
                "in mediump vec4 v_fragmentColor;\n"
                "in mediump vec2 v_texCoord;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    float Pi2 = 6.28318530718;\n"
                "    float Direction = %lf;\n"
                "    float Quality = %lf;\n"
                "    vec2 Radius = %lf / u_resolution.xy;\n"
                "    vec4 Sum = texture2D(u_texture, v_texCoord);\n"
                "    float SumRate = 1.0;\n"
                "    for(float d = 0.0; d < Pi2; d += Pi2 / Direction)\n"
                "    {\n"
                "        for(float i = 1.0 / Quality; i < 1.0; i += 1.0 / Quality)\n"
                "        {\n"
                "            Sum += texture2D(u_texture, v_texCoord + vec2(cos(d), sin(d)) * Radius * i) * (1.0 - i);\n"
                "            SumRate += 1.0 - i;\n"
                "        }\n"
                "    }\n"
                "    oColour = v_fragmentColor * (Sum / SumRate);\n"
                "}";

            // 약-블러 : 8.0, 2.0, 4.0 // 8 x 2 = 16회 반복
            InitShader(SR_BlurWeak,
                String::Format(VSourceNormal,
                    VersionCode, AttribID::Vertice, AttribID::Color, AttribID::TexCoords),
                String::Format(FSourceBlur,
                    VersionCode, 8.0, 2.0, 4.0));

            // 중-블러 : 16.0, 4.0, 8.0 // 16 x 4 = 64회 반복
            InitShader(SR_BlurMedium,
                String::Format(VSourceNormal,
                    VersionCode, AttribID::Vertice, AttribID::Color, AttribID::TexCoords),
                String::Format(FSourceBlur,
                    VersionCode, 16.0, 4.0, 8.0));

            // 강-블러 : 128.0, 32.0, 64.0 // 128 x 32 = 4096회 반복
            InitShader(SR_BlurStrong,
                String::Format(VSourceNormal,
                    VersionCode, AttribID::Vertice, AttribID::Color, AttribID::TexCoords),
                String::Format(FSourceBlur,
                    VersionCode, 128.0, 32.0, 64.0));
        }
        void TermShader()
        {
            if(QOpenGLContext* ctx = QOpenGLContext::currentContext())
            {
                QOpenGLFunctions* f = ctx->functions();
                for(sint32 i = 0; i < SR_Max; ++i)
                {
                    if(mPrograms[i]) {f->glDeleteProgram(mPrograms[i]); TestGL(BOSS_DBG 0); mPrograms[i] = 0;}
                    if(mVShaders[i]) {f->glDeleteShader(mVShaders[i]); TestGL(BOSS_DBG 0); mVShaders[i] = 0;}
                    if(mFShaders[i]) {f->glDeleteShader(mFShaders[i]); TestGL(BOSS_DBG 0); mFShaders[i] = 0;}
                }
            }
        }

    public:
        static void TestGL(BOSS_DBG_PRM sint32 nouse)
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            QOpenGLFunctions* f = ctx->functions();
            if(auto errorCode = f->glGetError())
                BOSS_ASSERT_PRM(String::Format("TestGL(error:%d) is failed", errorCode), false);
        }
        static void TestShader(BOSS_DBG_PRM GLuint shader)
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            QOpenGLFunctions* f = ctx->functions();

            GLint status;
            f->glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            if(status == GL_FALSE)
            {
                GLchar log[4096];
                GLsizei s;
                f->glGetShaderInfoLog(shader, 4096, &s, log);
                BOSS_ASSERT_PRM(String::Format("TestShader(%s) is failed", log), false);
            }
            else if(auto errorCode = f->glGetError())
                BOSS_ASSERT_PRM(String::Format("TestShader(error:%d) is failed", errorCode), false);
        }
        static void TestProgram(BOSS_DBG_PRM GLuint program)
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            QOpenGLFunctions* f = ctx->functions();

            GLint linked;
            f->glGetProgramiv(program, GL_LINK_STATUS, &linked);
            if(!linked)
            {
                int i32InfoLogLength, i32CharsWritten;
                f->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
                char* pszInfoLog = new char[i32InfoLogLength];
                f->glGetProgramInfoLog(program, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
                BOSS_ASSERT_PRM(String::Format("TestProgram(%s) is failed", pszInfoLog), false);
                delete [] pszInfoLog;
            }
            else if(auto errorCode = f->glGetError())
                BOSS_ASSERT_PRM(String::Format("TestProgram(error:%d) is failed", errorCode), false);
        }

    private:
        void LoadIdentity()
        {
            mMatrix[0][0] = 1; mMatrix[0][1] = 0; mMatrix[0][2] = 0; mMatrix[0][3] = 0;
            mMatrix[1][0] = 0; mMatrix[1][1] = 1; mMatrix[1][2] = 0; mMatrix[1][3] = 0;
            mMatrix[2][0] = 0; mMatrix[2][1] = 0; mMatrix[2][2] = 1; mMatrix[2][3] = 0;
            mMatrix[3][0] = 0; mMatrix[3][1] = 0; mMatrix[3][2] = 0; mMatrix[3][3] = 1;
        }
        void Multiply(
            const float m00, const float m01, const float m02, const float m03,
            const float m10, const float m11, const float m12, const float m13,
            const float m20, const float m21, const float m22, const float m23,
            const float m30, const float m31, const float m32, const float m33)
        {
            GLfloat m[4][4];
            Memory::Copy(&m[0][0], &mMatrix[0][0], sizeof(GLfloat) * 16);
            mMatrix[0][0] = m[0][0] * m00 + m[0][1] * m10 + m[0][2] * m20 + m[0][3] * m30;
            mMatrix[0][1] = m[0][0] * m01 + m[0][1] * m11 + m[0][2] * m21 + m[0][3] * m31;
            mMatrix[0][2] = m[0][0] * m02 + m[0][1] * m12 + m[0][2] * m22 + m[0][3] * m32;
            mMatrix[0][3] = m[0][0] * m03 + m[0][1] * m13 + m[0][2] * m23 + m[0][3] * m33;
            mMatrix[1][0] = m[1][0] * m00 + m[1][1] * m10 + m[1][2] * m20 + m[1][3] * m30;
            mMatrix[1][1] = m[1][0] * m01 + m[1][1] * m11 + m[1][2] * m21 + m[1][3] * m31;
            mMatrix[1][2] = m[1][0] * m02 + m[1][1] * m12 + m[1][2] * m22 + m[1][3] * m32;
            mMatrix[1][3] = m[1][0] * m03 + m[1][1] * m13 + m[1][2] * m23 + m[1][3] * m33;
            mMatrix[2][0] = m[2][0] * m00 + m[2][1] * m10 + m[2][2] * m20 + m[2][3] * m30;
            mMatrix[2][1] = m[2][0] * m01 + m[2][1] * m11 + m[2][2] * m21 + m[2][3] * m31;
            mMatrix[2][2] = m[2][0] * m02 + m[2][1] * m12 + m[2][2] * m22 + m[2][3] * m32;
            mMatrix[2][3] = m[2][0] * m03 + m[2][1] * m13 + m[2][2] * m23 + m[2][3] * m33;
            mMatrix[3][0] = m[3][0] * m00 + m[3][1] * m10 + m[3][2] * m20 + m[3][3] * m30;
            mMatrix[3][1] = m[3][0] * m01 + m[3][1] * m11 + m[3][2] * m21 + m[3][3] * m31;
            mMatrix[3][2] = m[3][0] * m02 + m[3][1] * m12 + m[3][2] * m22 + m[3][3] * m32;
            mMatrix[3][3] = m[3][0] * m03 + m[3][1] * m13 + m[3][2] * m23 + m[3][3] * m33;
        }

    private:
        GLuint mPrograms[SR_Max] {0,};
        GLuint mVShaders[SR_Max] {0,};
        GLuint mFShaders[SR_Max] {0,};
        GLint mUniformMatrices[SR_Max] {-1,};
        GLint mUniformResolutions[SR_Max] {-1,};
        GLint mUniformTextureY {-1};
        GLint mUniformTextureUV {-1};
        id_texture mFillTexture {nullptr};

    private:
        enum AttribID {Vertice = 0, Color, TexCoords, Resolution};
        struct Attrib
        {
            GLfloat vertices[2];
            union
            {
                GLubyte colors[4];
                GLuint color32;
            };
            GLfloat texcoords[2];
            GLfloat resolution[2];
        };
        Attrib mAttrib[4];
        GLfloat mMatrix[4][4];
    };

    class TextureClass
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(TextureClass)

    public:
        TextureClass()
        {
            mRefCount = 1;
            mNV21 = false;
            mWidth = 0;
            mHeight = 0;
            mTexture[0] = mTexture[1] = 0;
        }
        ~TextureClass()
        {
            BOSS_ASSERT("잘못된 시나리오입니다", mRefCount == 0);
            Remove();
        }

    public:
        void Init(bool nv21, bool bitmapcache, sint32 width, sint32 height, const void* bits)
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            BOSS_ASSERT("OpenGL의 Context접근에 실패하였습니다", ctx);
            if(ctx)
            {
                QOpenGLFunctions* f = ctx->functions();
                mNV21 = nv21;
                mWidth = width;
                mHeight = height;
                if(nv21)
                {
                    const sint32 Level[2] = {GL_LUMINANCE, GL_LUMINANCE_ALPHA};
                    const sint32 Widths[2]  = {mWidth, mWidth / 2};
                    const sint32 Heights[2] = {mHeight, mHeight / 2};
                    const void* Bits[2] = {bits, (bits)? &((bytes) bits)[mWidth * mHeight] : nullptr};
                    const sint32 BitsSizes[2] = {1 * Widths[0] * Heights[0], 2 * Widths[1] * Heights[1]};
                    f->glGenTextures(2, mTexture);
                    for(sint32 i = 0; i < 2; ++i)
                    {
                        f->glBindTexture(GL_TEXTURE_2D, mTexture[i]);
                        f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                        f->glTexImage2D(GL_TEXTURE_2D,
                            0, Level[i], Widths[i], Heights[i],
                            0, Level[i], GL_UNSIGNED_BYTE, Bits[i]);
                        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        if(bitmapcache && Bits[i]) // CreateBitmap()을 위해 저장해 둠
                            Memory::Copy(mBits[i].AtDumping(0, BitsSizes[i]), Bits[i], BitsSizes[i]);
                    }
                }
                else
                {
                    const sint32 BitsSize = 4 * mWidth * mHeight;
                    f->glGenTextures(1, mTexture);
                    f->glBindTexture(GL_TEXTURE_2D, mTexture[0]);
                    f->glTexImage2D(GL_TEXTURE_2D,
                        0, GL_RGBA8, mWidth, mHeight,
                        0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
                    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    if(bitmapcache && bits) // CreateBitmap()을 위해 저장해 둠
                        Memory::Copy(mBits[0].AtDumping(0, BitsSize), bits, BitsSize);
                }
            }
        }
        void Remove()
        {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            if(ctx)
            {
                QOpenGLFunctions* f = ctx->functions();
                for(sint32 i = 0; i < 2; ++i)
                {
                    if(mTexture[i])
                    {
                        f->glDeleteTextures(1, &mTexture[i]);
                        mTexture[i] = 0;
                    }
                }
            }
        }
        id_bitmap CreateBitmapByGL() const
        {
            id_bitmap NewBitmap = nullptr;
            if(mNV21)
                BOSS_ASSERT("NV21방식은 CreateBitmapByGL를 지원하지 않습니다", false);
            else
            {
                QOpenGLContext* ctx = QOpenGLContext::currentContext();
                if(ctx)
                {
                    QOpenGLFunctions* f = ctx->functions();
                    GLuint fbo = 0, prevFbo = 0;
                    f->glGenFramebuffers(1, &fbo);
                    f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &prevFbo);
                    f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                    NewBitmap = Bmp::Create(4, mWidth, mHeight);
                    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture[0], 0);
                    f->glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, Bmp::GetBits(NewBitmap));
                    f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
                    f->glDeleteFramebuffers(1, &fbo);
                    Bmp::SwapRedBlue(NewBitmap);
                }
                else BOSS_ASSERT("OpenGL의 Context접근에 실패하였습니다", ctx);
            }
            return NewBitmap;
        }
        id_bitmap CreateBitmapByCopy() const
        {
            id_bitmap NewBitmap = nullptr;
            if(mNV21)
            {
                const sint32 Widths[2]  = {mWidth, mWidth / 2};
                const sint32 Heights[2] = {mHeight, mHeight / 2};
                const sint32 BitsSizes[2] = {1 * Widths[0] * Heights[0], 2 * Widths[1] * Heights[1]};
                if(mBits[0].Count() == BitsSizes[0] && mBits[1].Count() == BitsSizes[1])
                    NewBitmap = Bmp::CloneFromNV21(&mBits[0][0], (uv16s) &mBits[1][0], mWidth, mHeight);
                else BOSS_ASSERT("미리 저장된 mBits가 없어서 CreateBitmapByCopy에 실패하였습니다", false);
            }
            else
            {
                const sint32 BitsSize = 4 * mWidth * mHeight;
                if(mBits[0].Count() == BitsSize)
                {
                    NewBitmap = Bmp::Create(4, mWidth, mHeight);
                    Memory::Copy(Bmp::GetBits(NewBitmap), &mBits[0][0], BitsSize);
                }
                else BOSS_ASSERT("미리 저장된 mBits가 없어서 CreateBitmapByCopy에 실패하였습니다", false);
            }
            return NewBitmap;
        }
        void CopyFromBitmap(sint32 x, sint32 y, sint32 width, sint32 height, const Bmp::bitmappixel* bits)
        {
            BOSS_ASSERT("복사영역이 텍스쳐영역을 벗어납니다", 0 <= x && 0 <= y && x + width <= mWidth && y + height <= mHeight);

            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            BOSS_ASSERT("OpenGL의 Context접근에 실패하였습니다", ctx);
            if(ctx)
            {
                QOpenGLFunctions* f = ctx->functions();
                if(mNV21)
                    BOSS_ASSERT("개발이 필요합니다!", false);
                else
                {
                    f->glBindTexture(GL_TEXTURE_2D, mTexture[0]);
                    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    f->glTexSubImage2D(GL_TEXTURE_2D, 0,
                        x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bits);
                }
            }
        }

    public:
        inline TextureClass* clone() {++mRefCount; return this;}
        inline bool release() {return (--mRefCount == 0);}
        inline bool nv21() const {return mNV21;}
        inline sint32 width() const {return mWidth;}
        inline sint32 height() const {return mHeight;}
        inline uint32 id(sint32 i) const {return mTexture[i];}

    public:
        void ResetDirectly(uint32 texture, sint32 width, sint32 height)
        {
            mRefCount = 1;
            mNV21 = false;
            mWidth = width;
            mHeight = height;
            mTexture[0] = texture;
        }
        void ClearDirectly()
        {
            mRefCount = 0;
            mTexture[0] = 0;
        }

    private:
        sint32 mRefCount;
        bool mNV21;
        sint32 mWidth;
        sint32 mHeight;
        uint32 mTexture[2];
        uint08s mBits[2];
    };

    class SurfaceClass
    {
    public:
        SurfaceClass() : mFBO(0, 0, QOpenGLFramebufferObjectFormat()), mDevice(0, 0)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", false);
            mSavedSurface = nullptr;
            mIsValidLastImage = false;
        }
        SurfaceClass(sint32 width, sint32 height, QOpenGLFramebufferObjectFormat* format)
            : mFBO(width, height, *format), mDevice(width, height)
        {
            BOSS_ASSERT("FBO생성에 실패하였습니다", mFBO.isValid());
            mSavedSurface = nullptr;
            mIsValidLastImage = false;
        }
        ~SurfaceClass()
        {
            mFBO.release();
            mLastTexture.ClearDirectly();
        }

    public:
        SurfaceClass(const SurfaceClass& rhs) : mFBO(0, 0, QOpenGLFramebufferObjectFormat()), mDevice(0, 0)
        {
            operator=(rhs);
        }
        SurfaceClass& operator=(const SurfaceClass& rhs)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", false);
            return *this;
        }

    public:
        inline uint32 fbo() const {return mFBO.handle();}
        inline id_texture_read texture() const
        {
            mLastTexture.ResetDirectly(mFBO.texture(), mFBO.width(), mFBO.height());
            return (id_texture_read) &mLastTexture;
        }
        inline sint32 width() const {return mFBO.width();}
        inline sint32 height() const {return mFBO.height();}
        inline QPainter* painter() {return &mCanvas.painter();}

    public:
        void BindGraphics()
        {
            BOSS_ASSERT("mSavedSurface는 nullptr이어야 합니다", !mSavedSurface);
            mSavedSurface = ST();
            mCanvas.Bind(&mDevice);
            mFBO.bind();
            ST() = this;
        }
        void UnbindGraphics()
        {
            BOSS_ASSERT("SurfaceClass는 스택식으로 해제해야 합니다", ST() == this);
            mIsValidLastImage = false;
            mCanvas.Unbind();
            if(ST() = mSavedSurface)
            {
                mSavedSurface->mFBO.bind();
                mSavedSurface = nullptr;
            }
        }
        const QImage& GetLastImage() const
        {
            if(!mIsValidLastImage)
            {
                mIsValidLastImage = true;
                mLastImage = mFBO.toImage();
            }
            return mLastImage;
        }

    public:
        static void LockForGL()
        {
            BOSS_ASSERT("STGL()는 nullptr이어야 합니다", !STGL());
            STGL() = ST();
            ST() = nullptr;
        }
        static void UnlockForGL()
        {
            BOSS_ASSERT("ST()는 nullptr이어야 합니다", !ST());
            if(ST() = STGL())
            {
                STGL()->mFBO.bind();
                STGL() = nullptr;
            }
        }

    private:
        static inline SurfaceClass*& ST() {static SurfaceClass* _ = nullptr; return _;}
        static inline SurfaceClass*& STGL() {static SurfaceClass* _ = nullptr; return _;}

    private:
        SurfaceClass* mSavedSurface;
        QOpenGLFramebufferObject mFBO;
        QOpenGLPaintDevice mDevice;
        CanvasClass mCanvas;

    private:
        mutable bool mIsValidLastImage;
        mutable QImage mLastImage;
        mutable TextureClass mLastTexture;
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
        static sint64 GetLocalTimeMSecFromUtc() {static sint64 _ = (sint64) (QDateTime::currentDateTime().utcOffset() * 1000); return _;}

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

    #ifdef QT_HAVE_MULTIMEDIA
        class SoundClass
        {
        public:
            SoundClass(chars path, bool loop, bool url_or_file)
            {
                m_volume_pos = 0;
                m_volume_target = 0;
                m_volume_pos_msec = 0;
                m_volume_target_msec = 0;
                m_player = new QMediaPlayer();
                m_playlist = nullptr;
                m_output = nullptr;
                m_outputdevice = nullptr;
                m_outputmutex = nullptr;

                if(loop)
                {
                    m_playlist = new QMediaPlaylist();
                    m_playlist->addMedia((url_or_file)? QUrl(path) : QUrl::fromLocalFile(path));
                    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
                    m_player->setPlaylist(m_playlist);
                }
                else m_player->setMedia((url_or_file)? QUrl(path) : QUrl::fromLocalFile(path));
                m_player->setVolume(100);
            }
            SoundClass(sint32 channel, sint32 sample_rate, sint32 sample_size)
            {
                QAudioFormat AudioFormat;
                AudioFormat.setCodec("audio/pcm");
                AudioFormat.setChannelCount(channel);
                AudioFormat.setSampleRate(sample_rate);
                AudioFormat.setSampleSize(sample_size);
                AudioFormat.setSampleType(QAudioFormat::SignedInt);
                AudioFormat.setByteOrder(QAudioFormat::LittleEndian);

                m_volume_pos = 0;
                m_volume_target = 0;
                m_volume_pos_msec = 0;
                m_volume_target_msec = 0;
                m_player = nullptr;
                m_playlist = nullptr;
                m_output = new QAudioOutput(AudioFormat);
                m_output->setBufferSize(sample_rate);
                m_output->setVolume(100);
                m_outputdevice = nullptr;
                m_outputmutex = Mutex::Open();
            }
            ~SoundClass()
            {
                Stop();
                delete m_player;
                delete m_playlist;
                delete m_output;
                if(m_outputmutex)
                    Mutex::Close(m_outputmutex);
            }

        public:
            void SetVolume(float volume, sint32 apply_msec)
            {
                const sint32 NewVolume = Math::Clamp(100 * volume, 0, 100);
                if(apply_msec == 0 || m_volume_pos == NewVolume)
                {
                    m_volume_pos = NewVolume;
                    m_volume_target = NewVolume;
                    m_volume_pos_msec = 0;
                    m_volume_target_msec = 0;
                    if(m_player)
                        m_player->setVolume(m_volume_pos);
                }
                else
                {
                    m_volume_target = NewVolume;
                    m_volume_pos_msec = Platform::Utility::CurrentTimeMsec();
                    m_volume_target_msec = m_volume_pos_msec + apply_msec;
                }
            }
            sint32 CalcedVolume() const
            {
                if(m_volume_target_msec == 0)
                    return m_volume_pos;
                const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
                return Math::Clamp(100 * (CurMsec - m_volume_pos_msec) / (m_volume_target_msec - m_volume_pos_msec), 0, 100);
            }
            bool ApplyVolumeOnce()
            {
                if(m_volume_target_msec == 0)
                    return false;
                const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
                if(m_volume_target_msec <= CurMsec)
                {
                    m_volume_pos = m_volume_target;
                    m_volume_pos_msec = 0;
                    m_volume_target_msec = 0;
                    if(m_player)
                        m_player->setVolume(m_volume_pos);
                    return false;
                }
                if(m_player)
                    m_player->setVolume(100 * (CurMsec - m_volume_pos_msec) / (m_volume_target_msec - m_volume_pos_msec));
                return true;
            }
            void Play()
            {
                branch;
                jump(m_player)
                {
                    m_player->setVolume(CalcedVolume());
                    m_player->play();
                }
                jump(m_output)
                {
                    if(!m_outputdevice)
                    {
                        Mutex::Lock(m_outputmutex);
                        m_outputdevice = m_output->start();
                        Mutex::Unlock(m_outputmutex);
                    }
                }
            }
            void Stop()
            {
                branch;
                jump(m_player) m_player->stop();
                jump(m_output)
                {
                    if(m_outputdevice)
                    {
                        Mutex::Lock(m_outputmutex);
                        m_output->stop();
                        m_outputdevice = nullptr;
                        Mutex::Unlock(m_outputmutex);
                    }
                }
            }
            bool NowPlaying()
            {
                branch;
                jump(m_player)
                {
                    if(m_player->state() == QMediaPlayer::PlayingState)
                        return true;
                }
                jump(m_output)
                {
                    if(m_outputdevice && m_output->state() == QAudio::ActiveState)
                        return true;
                }
                return false;
            }
            sint32 AddStreamForPlay(bytes raw, sint32 size, sint32 timeout)
            {
                Mutex::Lock(m_outputmutex);
                sint32 WrittenBytes = -1;
                if(m_outputdevice)
                    WrittenBytes = m_outputdevice->write((chars) raw, size);
                Mutex::Unlock(m_outputmutex);

                if(WrittenBytes == -1)
                    return -1;
                else if(WrittenBytes < size)
                {
                    uint64 BeginMsec = Platform::Utility::CurrentTimeMsec();
                    while(Platform::Utility::CurrentTimeMsec() < BeginMsec + timeout)
                    {
                        Platform::Utility::Sleep(1, false, false, true);
                        raw += WrittenBytes;
                        size -= WrittenBytes;

                        Mutex::Lock(m_outputmutex);
                        WrittenBytes = -1;
                        if(m_outputdevice)
                            WrittenBytes = m_outputdevice->write((chars) raw, size);
                        Mutex::Unlock(m_outputmutex);

                        if(WrittenBytes == -1)
                            return -1;
                        if(WrittenBytes == size)
                            return CalcedVolume();
                    }
                    return -1; // Timeout
                }
                return CalcedVolume();
            }

        private:
            sint32 m_volume_pos;
            sint32 m_volume_target;
            uint64 m_volume_pos_msec;
            uint64 m_volume_target_msec;
            QMediaPlayer* m_player;
            QMediaPlaylist* m_playlist;
            QAudioOutput* m_output;
            QIODevice* m_outputdevice;
            id_mutex m_outputmutex;
        };
    #endif

    class PeerData : public QObjectUserData
    {
    public:
        const sint32 ID;
        sint32 PacketNeeds;

    public:
        PeerData() : ID(MakeID()) {PacketNeeds = 0;}
        virtual ~PeerData() {}

    public:
        static uint ClassID()
        {static uint _ = QObject::registerUserData(); return _;}
        static int MakeID() {static int _ = -1; return ++_;}
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
            PeerData* NewData = new PeerData();
            mPeers[NewData->ID] = Peer;
            Peer->setUserData(PeerData::ClassID(), NewData);
            mPacketQueue.Enqueue(new PeerPacket(packettype_entrance, NewData->ID, 0));

            if(!mUsingSizeField) connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeer()));
            else connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
            connect(Peer, SIGNAL(disconnected()), this, SLOT(disconnected()));
            Platform::BroadcastNotify("entrance", nullptr, NT_SocketReceive, nullptr, true);
        }

        void readyPeer()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            PeerData* Data = (PeerData*) Peer->userData(PeerData::ClassID());
            const sint64 PacketSize = Peer->bytesAvailable();

            if(0 < PacketSize)
            {
                PeerPacket* NewPacket = new PeerPacket(packettype_message, Data->ID, PacketSize);
                Peer->read((char*) NewPacket->Buffer, PacketSize);
                mPacketQueue.Enqueue(NewPacket);
            }
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive, nullptr, true);
        }

        void readyPeerWithSizeField()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            PeerData* Data = (PeerData*) Peer->userData(PeerData::ClassID());
            sint64 PacketSize = Peer->bytesAvailable();

            while(0 < PacketSize)
            {
                if(Data->PacketNeeds == 0)
                {
                    if(4 <= PacketSize)
                    {
                        PacketSize -= 4;
                        int GetPacketSize = 0;
                        Peer->read((char*) &GetPacketSize, 4);
                        Data->PacketNeeds = GetPacketSize;
                    }
                    else break;
                }
                if(0 < Data->PacketNeeds)
                {
                    if(Data->PacketNeeds <= PacketSize)
                    {
                        PacketSize -= Data->PacketNeeds;
                        PeerPacket* NewPacket = new PeerPacket(packettype_message, Data->ID, Data->PacketNeeds);
                        Peer->read((char*) NewPacket->Buffer, Data->PacketNeeds);
                        mPacketQueue.Enqueue(NewPacket);
                        Data->PacketNeeds = 0;
                    }
                    else break;
                }
            }
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive, nullptr, true);
        }

        void disconnected()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            PeerData* Data = (PeerData*) Peer->userData(PeerData::ClassID());

            mPeers.Remove(Data->ID);
            mPacketQueue.Enqueue(new PeerPacket(packettype_leaved, Data->ID, 0));
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
            PeerData* NewData = new PeerData();
            mPeers[NewData->ID] = Peer;
            Peer->setUserData(PeerData::ClassID(), NewData);
            mPacketQueue.Enqueue(new PeerPacket(packettype_entrance, NewData->ID, 0));

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
            PeerData* Data = (PeerData*) Peer->userData(PeerData::ClassID());
            const sint64 PacketSize = frame.size();

            if(0 < PacketSize)
            {
                PeerPacket* NewPacket = new PeerPacket(packettype_message, Data->ID, PacketSize);
                Memory::Copy((void*) NewPacket->Buffer, frame.constData(), PacketSize);
                mPacketQueue.Enqueue(NewPacket);
            }
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive, nullptr, true);
        }

        void disconnected()
        {
            QWebSocket* Peer = (QWebSocket*) sender();
            PeerData* Data = (PeerData*) Peer->userData(PeerData::ClassID());

            mPeers.Remove(Data->ID);
            mPacketQueue.Enqueue(new PeerPacket(packettype_leaved, Data->ID, 0));
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

    class WebPageClass : public WebEnginePageClass
    {
        Q_OBJECT

    public:
        WebPageClass(QObject* parent = nullptr) : WebEnginePageClass(parent)
        {
            mCb = nullptr;
            mData = nullptr;
        }
        ~WebPageClass() override {}

    public:
        void SetCallback(Platform::Web::EventCB cb, payload data)
        {
            mCb = cb;
            mData = data;
        }

    private:
        #ifdef QT_HAVE_WEBENGINEWIDGETS
            bool certificateError(const QWebEngineCertificateError& error) override
            {
                return true;
            }
        #endif

        void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID) override
        {
            if(mCb)
                mCb(mData, String::Format("JSConsole:%d", level), message.toUtf8().constData());
        }

    private:
        Platform::Web::EventCB mCb;
        payload mData;
    };

    class WebViewClass : public WebEngineViewClass
    {
        Q_OBJECT

    public:
        WebViewClass(QWidget* parent = nullptr) : WebEngineViewClass(parent), mHandle(h_web::null())
        {
            mNowLoading = false;
            mLoadingProgress = 100;
            mLoadingRate = 1;
            mCb = nullptr;
            mData = nullptr;

            setPage(new WebPageClass(this));
            setMouseTracking(true);
            #ifdef QT_HAVE_WEBENGINEWIDGETS
                // 배경의 투명화
                setStyleSheet("background-color:transparent;");
                page()->setBackgroundColor(Qt::transparent);
            #endif

            connect(this, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));
            connect(this, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
            connect(this, SIGNAL(loadStarted()), SLOT(onLoadStarted()));
            connect(this, SIGNAL(loadProgress(int)), SLOT(onLoadProgress(int)));
            connect(this, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished(bool)));
            connect(this, SIGNAL(renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus, int)),
                SLOT(onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus, int)));
            connect(page(), SIGNAL(featurePermissionRequested(QUrl, QWebEnginePage::Feature)),
                SLOT(onFeaturePermissionRequested(QUrl, QWebEnginePage::Feature)));
        }
        virtual ~WebViewClass()
        {
            mHandle.set_buf(nullptr);
        }

    public:
        WebViewClass(const WebViewClass&) {BOSS_ASSERT("사용금지", false);}
        WebViewClass& operator=(const WebViewClass&) {BOSS_ASSERT("사용금지", false); return *this;}

    public:
        void SetCallback(Platform::Web::EventCB cb, payload data)
        {
            mCb = cb;
            mData = data;
            ((WebPageClass*) page())->SetCallback(cb, data);
        }
        void CallJSFunction(chars script, sint32 matchid)
        {
            page()->runJavaScript(script,
                [this, matchid](const QVariant& v)->void
                {
                    if(mCb)
                    {
                        const String Result = v.toString().toUtf8().constData();
                        if(0 < Result.Length())
                            mCb(mData, String::Format("JSFunction:%d", matchid), Result);
                    }
                });
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            event->accept();
            mHandle.set_buf(nullptr);
        }

    private slots:
        void onTitleChanged(const QString& title)
        {
            if(mCb)
                mCb(mData, "TitleChanged", title.toUtf8().constData());
        }
        void onUrlChanged(const QUrl& url)
        {
            if(mCb)
                mCb(mData, "UrlChanged", url.url().toUtf8().constData());
        }
        void onLoadStarted()
        {
            mNowLoading = true;
            mLoadingProgress = 0;
            mLoadingRate = 0;
        }
        void onLoadProgress(int progress)
        {
            mNowLoading = true;
            mLoadingProgress = progress;
        }
        void onLoadFinished(bool)
        {
            mNowLoading = false;
            mLoadingProgress = 100;
            mLoadingRate = 1;
            if(mCb)
                mCb(mData, "LoadFinished", "");
        }
        void renderProcessTerminated(WebEnginePageClass::RenderProcessTerminationStatus terminationStatus, int exitCode)
        {
            if(mCb)
            {
                switch(terminationStatus)
                {
                case WebEnginePageClass::NormalTerminationStatus: mCb(mData, "RenderTerminated", "Normal"); break;
                case WebEnginePageClass::AbnormalTerminationStatus: mCb(mData, "RenderTerminated", "Abnormal"); break;
                case WebEnginePageClass::CrashedTerminationStatus: mCb(mData, "RenderTerminated", "Crashed"); break;
                case WebEnginePageClass::KilledTerminationStatus: mCb(mData, "RenderTerminated", "Killed"); break;
                }
            }
        }
        void onFeaturePermissionRequested(QUrl q, WebEnginePageClass::Feature f)
        {
            page()->setFeaturePermission(q, f, WebEnginePageClass::PermissionGrantedByUser);
        }

    public:
        h_web mHandle;
        bool mNowLoading;
        int mLoadingProgress;
        float mLoadingRate;

    private:
        Platform::Web::EventCB mCb;
        payload mData;
    };

    #ifdef QT_HAVE_WEBENGINEWIDGETS
        class WebClassForDesktop
        {
        public:
            WebClassForDesktop()
            {
                mProxy = mScene.addWidget(&mView);
                mLastTextureGL = nullptr;
            }
            ~WebClassForDesktop()
            {
                mScene.removeItem(mProxy);
                mLastTexture.ClearDirectly();
                delete mLastTextureGL;
            }

        public:
            WebClassForDesktop(const WebClassForDesktop&) {BOSS_ASSERT("사용금지", false);}
            WebClassForDesktop& operator=(const WebClassForDesktop&) {BOSS_ASSERT("사용금지", false); return *this;}

        public:
            void AttachHandle(h_web web)
            {
                mView.mHandle = web;
            }
            void ClearCookies()
            {
                mView.page()->profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
            }
            void Reload(chars url)
            {
                mView.load(QUrl(QString::fromUtf8(url)));
            }
            bool NowLoading(float* rate)
            {
                if(rate)
                {
                    mView.mLoadingRate = mView.mLoadingRate * 0.9f + mView.mLoadingProgress / 100.0f * 0.1f;
                    *rate = mView.mLoadingRate;
                }
                return mView.mNowLoading && (mView.mLoadingProgress < 100); // LoadingProgress만 100%가 되어도 로딩이 끝난것으로 간주
            }
            bool Resize(sint32 width, sint32 height)
            {
                if(width != mLastImage.width() || height != mLastImage.height())
                {
                    mView.resize(width, height);
                    mLastImage = QImage(width, height, QImage::Format_ARGB32);
                    return true;
                }
                return false;
            }
            void SetCallback(Platform::Web::EventCB cb, payload data)
            {
                mView.SetCallback(cb, data);
            }
            void SendTouchEvent(TouchType type, sint32 x, sint32 y)
            {
                QMouseEvent::Type CurType = QMouseEvent::None;
                sint32 ButtonType = 0;
                switch(type)
                {
                case TT_Moving: CurType = QMouseEvent::MouseMove; break;
                case TT_Press: CurType = QMouseEvent::MouseButtonPress; ButtonType = 1; break;
                case TT_Dragging: CurType = QMouseEvent::MouseMove; ButtonType = 1; break;
                case TT_Release: CurType = QMouseEvent::MouseButtonRelease; ButtonType = 1; break;
                case TT_ExtendPress: CurType = QMouseEvent::MouseButtonPress; ButtonType = 2; break;
                case TT_ExtendDragging: CurType = QMouseEvent::MouseMove; ButtonType = 2; break;
                case TT_ExtendRelease: CurType = QMouseEvent::MouseButtonRelease; ButtonType = 2; break;
                default: BOSS_ASSERT("해당 case가 준비되지 않았습니다", false);
                }
                QMouseEvent NewEvent(CurType, QPoint(x, y),
                    (ButtonType == 0)? Qt::NoButton : ((ButtonType == 1)? Qt::LeftButton : Qt::RightButton),
                    (ButtonType == 0)? Qt::NoButton : ((ButtonType == 1)? Qt::LeftButton : Qt::RightButton),
                    Qt::NoModifier);
                Q_FOREACH(QObject* obj, mView.page()->view()->children())
                {
                    if(qobject_cast<QWidget*>(obj))
                        QApplication::sendEvent(obj, &NewEvent);
                }
            }
            void SendKeyEvent(sint32 code, chars text, bool pressed)
            {
                if(auto CurWidget = mView.focusProxy())
                {
                    QKeyEvent NewEvent((pressed)? QKeyEvent::KeyPress : QKeyEvent::KeyRelease, code, Qt::NoModifier, text);
                    QApplication::sendEvent(CurWidget, &NewEvent);
                }
            }
            void CallJSFunction(chars script, sint32 matchid)
            {
                mView.CallJSFunction(script, matchid);
            }
            id_texture_read GetTexture()
            {
                delete mLastTextureGL;
                mLastTextureGL = new QOpenGLTexture(GetImage());
                mLastTexture.ResetDirectly(mLastTextureGL->textureId(), mLastTextureGL->width(), mLastTextureGL->height());
                return (id_texture_read) &mLastTexture;
            }
            const QPixmap GetPixmap()
            {
                return QPixmap::fromImage(GetImage());
            }
            const QImage& GetImage()
            {
                mLastImage.fill(QColor(0, 0, 0, 0));
                CanvasClass CurCanvas(&mLastImage);
                mView.update();
                mView.render(&CurCanvas.painter());
                return mLastImage;
            }

        private:
            WebViewClass mView;
            QGraphicsProxyWidget* mProxy;
            QGraphicsScene mScene;
            QImage mLastImage;
            TextureClass mLastTexture;
            QOpenGLTexture* mLastTextureGL;
        };
        typedef WebClassForDesktop WebClass;
    #else
        class WebClassForExtraDesktop
        {
        public:
            WebClassForExtraDesktop()
            {
            }
            ~WebClassForExtraDesktop()
            {
            }

        public:
            WebClassForExtraDesktop(const WebClassForExtraDesktop&) {BOSS_ASSERT("사용금지", false);}
            WebClassForExtraDesktop& operator=(const WebClassForExtraDesktop&) {BOSS_ASSERT("사용금지", false); return *this;}

        public:
            void AttachHandle(h_web web)
            {
            }
            void ClearCookies()
            {
            }
            void Reload(chars url)
            {
            }
            bool NowLoading(float* rate)
            {
                if(rate) *rate = 1;
                return false;
            }
            bool Resize(sint32 width, sint32 height)
            {
                if(width != mLastImage.width() || height != mLastImage.height())
                {
                    mLastImage = QImage(width, height, QImage::Format_ARGB32);
                    return true;
                }
                return false;
            }
            void SetCallback(Platform::Web::EventCB cb, payload data)
            {
            }
            void SendTouchEvent(TouchType type, sint32 x, sint32 y)
            {
            }
            void SendKeyEvent(sint32 code, chars text, bool pressed)
            {
            }
            void CallJSFunction(chars script, sint32 matchid)
            {
            }
            id_texture_read GetTexture()
            {
                return nullptr;
            }
            const QPixmap GetPixmap()
            {
                return QPixmap::fromImage(GetImage());
            }
            const QImage& GetImage()
            {
                CanvasClass CurCanvas(&mLastImage);
                const QRect CurRect(0, 0, mLastImage.width(), mLastImage.height());
                return mLastImage;
            }

        private:
            QImage mLastImage;
        };
        typedef WebClassForExtraDesktop WebClass;
    #endif

    #ifdef QT_HAVE_PURCHASING
        class PurchaseClass : public QInAppStore
        {
            Q_OBJECT

        public:
            PurchaseClass(QWidget* parent = nullptr) : QInAppStore(parent)
            {
                mProduct = nullptr;
                connect(this, SIGNAL(productRegistered(QInAppProduct*)), SLOT(onProductRegistered(QInAppProduct*)));
                connect(this, SIGNAL(productUnknown(QInAppProduct::ProductType, const QString&)), SLOT(onProductUnknown(QInAppProduct::ProductType, const QString&)));
                connect(this, SIGNAL(transactionReady(QInAppTransaction*)), SLOT(onTransactionReady(QInAppTransaction*)));
            }
            ~PurchaseClass()
            {
            }

        public:
            PurchaseClass(const PurchaseClass&) {BOSS_ASSERT("사용금지", false);}
            PurchaseClass& operator=(const PurchaseClass&) {BOSS_ASSERT("사용금지", false); return *this;}

        public:
            bool Register(chars name, PurchaseType type)
            {
                mName = name;
                switch(type)
                {
                case PT_Consumable:
                    registerProduct(QInAppProduct::Consumable, name);
                    return true;
                case PT_Unlockable:
                    registerProduct(QInAppProduct::Unlockable, name);
                    return true;
                }
                return false;
            }
            bool Purchase(PurchaseCB cb)
            {
                if(!mProduct)
                    mProduct = registeredProduct((chars) mName);
                if(mProduct)
                {
                    mProduct->purchase();
                    return true;
                }
                return false;
            }

        private slots:
            void onProductRegistered(QInAppProduct* product)
            {
            }
            void onProductUnknown(QInAppProduct::ProductType productType, const QString& identifier)
            {
            }
            void onTransactionReady(QInAppTransaction* transaction)
            {
            }

        public:
            String mName;
            QInAppProduct* mProduct;
        };
    #else
        class PurchaseClass : public QObject {Q_OBJECT};
    #endif

    #if !BOSS_WASM
        class BluetoothSearchClass : public QObject
        {
            Q_OBJECT

        public:
            static void DeviceBegin()
            {
                DeviceEnd();

                auto& Self = ST();
                Mutex::Lock(Self.mDiscoverMutex);
                {
                    Self.mDiscoveryDeviceAgent = new QBluetoothDeviceDiscoveryAgent();
                    Self.mDiscoveryDeviceAgent->setLowEnergyDiscoveryTimeout(5000);
                    Self.mDiscoveryDeviceAgent->start();
                }
                Mutex::Unlock(Self.mDiscoverMutex);

                connect(Self.mDiscoveryDeviceAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                    &Self, &BluetoothSearchClass::deviceDiscovered);
                connect(Self.mDiscoveryDeviceAgent, &QBluetoothDeviceDiscoveryAgent::finished,
                    &Self, &BluetoothSearchClass::scanDeviceFinished);
                connect(Self.mDiscoveryDeviceAgent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
                    &Self, &BluetoothSearchClass::scanDeviceErrorOccurred);

                Platform::BroadcastNotify("ScanDeviceStarted", nullptr, NT_BluetoothSearch);
            }
            static void DeviceEnd()
            {
                auto& Self = ST();
                Mutex::Lock(Self.mDiscoverMutex);
                {
                    Self.mDiscoveredDevices.empty();

                    if(Self.mDiscoveryDeviceAgent)
                    {
                        disconnect(Self.mDiscoveryDeviceAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                            &Self, &BluetoothSearchClass::deviceDiscovered);
                        disconnect(Self.mDiscoveryDeviceAgent, &QBluetoothDeviceDiscoveryAgent::finished,
                            &Self, &BluetoothSearchClass::scanDeviceFinished);
                        disconnect(Self.mDiscoveryDeviceAgent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
                            &Self, &BluetoothSearchClass::scanDeviceErrorOccurred);

                        if(Self.mDiscoveryDeviceAgent->isActive())
                        {
                            Self.mDiscoveryDeviceAgent->stop();
                            Self.scanDeviceFinished();
                        }
                        delete Self.mDiscoveryDeviceAgent;
                        Self.mDiscoveryDeviceAgent = nullptr;
                    }
                }
                Mutex::Unlock(Self.mDiscoverMutex);
            }

        public:
            static void ServiceBegin(chars deviceaddress, Strings uuidfilters)
            {
                ServiceEnd();

                auto& Self = ST();
                Mutex::Lock(Self.mDiscoverMutex);
                {
                    Self.mFocusedDeviceAddress = deviceaddress;

                    // 디바이스타겟 등록
                    if(deviceaddress)
                    {
                        if(auto NewDevice = CloneOrCreateDevice(deviceaddress, false))
                        {
                            if(NewDevice->coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
                            {
                                Self.mDiscoveryServiceAgent_BLE = QLowEnergyController::createCentral(*NewDevice, &Self);
                                Self.mDiscoveryServiceAgent_BLE->setRemoteAddressType(QLowEnergyController::PublicAddress);
                            }
                            else
                            {
                                QBluetoothLocalDevice LocalDevice;
                                QBluetoothAddress LocalAdapter = LocalDevice.address();
                                Self.mDiscoveryServiceAgent_BT = new QBluetoothServiceDiscoveryAgent(LocalAdapter);
                                Self.mDiscoveryServiceAgent_BT->setRemoteAddress(NewDevice->address());
                            }
                            delete NewDevice;
                        }
                    }

                    if(Self.mDiscoveryServiceAgent_BT)
                    {
                        // UUID필터 등록
                        if(uuidfilters.Count() == 1)
                        {
                            const QBluetoothUuid NewUUID(QString::fromUtf8((chars) uuidfilters[0]));
                            Self.mDiscoveryServiceAgent_BT->setUuidFilter(NewUUID);
                        }
                        else if(1 < uuidfilters.Count())
                        {
                            QList<QBluetoothUuid> NewUUIDs;
                            for(sint32 i = 0, iend = uuidfilters.Count(); i < iend; ++i)
                                NewUUIDs += QBluetoothUuid(QString::fromUtf8((chars) uuidfilters[i]));
                            Self.mDiscoveryServiceAgent_BT->setUuidFilter(NewUUIDs);
                        }
                    }
                }
                Mutex::Unlock(Self.mDiscoverMutex);

                if(Self.mDiscoveryServiceAgent_BT)
                {
                    connect(Self.mDiscoveryServiceAgent_BT, &QBluetoothServiceDiscoveryAgent::serviceDiscovered,
                        &Self, &BluetoothSearchClass::serviceDiscovered_BT);
                    connect(Self.mDiscoveryServiceAgent_BT, &QBluetoothServiceDiscoveryAgent::finished,
                        &Self, &BluetoothSearchClass::scanServiceFinished_BT);
                    connect(Self.mDiscoveryServiceAgent_BT, QOverload<QBluetoothServiceDiscoveryAgent::Error>::of(&QBluetoothServiceDiscoveryAgent::error),
                        &Self, &BluetoothSearchClass::scanServiceErrorOccurred_BT);
                    Self.mDiscoveryServiceAgent_BT->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
                }
                else if(Self.mDiscoveryServiceAgent_BLE)
                {
                    connect(Self.mDiscoveryServiceAgent_BLE, &QLowEnergyController::serviceDiscovered,
                        &Self, &BluetoothSearchClass::serviceDiscovered_BLE);
                    connect(Self.mDiscoveryServiceAgent_BLE, &QLowEnergyController::discoveryFinished,
                        &Self, &BluetoothSearchClass::scanServiceFinished_BLE);
                    connect(Self.mDiscoveryServiceAgent_BLE, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
                        &Self, &BluetoothSearchClass::scanServiceErrorOccurred_BLE);
                    connect(Self.mDiscoveryServiceAgent_BLE, &QLowEnergyController::connected,
                        &Self, &BluetoothSearchClass::deviceConnected_BLE);
                    connect(Self.mDiscoveryServiceAgent_BLE, &QLowEnergyController::disconnected,
                        &Self, &BluetoothSearchClass::deviceDisconnected_BLE);
                    Self.mDiscoveryServiceAgent_BLE->connectToDevice();
                }

                Platform::BroadcastNotify("ScanServiceStarted", nullptr, NT_BluetoothSearch);
            }
            static void ServiceEnd()
            {
                auto& Self = ST();
                Mutex::Lock(Self.mDiscoverMutex);
                {
                    Self.mFocusedDeviceAddress.clear();
                    Self.mDiscoveredServices_BT.empty();

                    if(Self.mDiscoveryServiceAgent_BT)
                    {
                        disconnect(Self.mDiscoveryServiceAgent_BT, &QBluetoothServiceDiscoveryAgent::serviceDiscovered,
                            &Self, &BluetoothSearchClass::serviceDiscovered_BT);
                        disconnect(Self.mDiscoveryServiceAgent_BT, &QBluetoothServiceDiscoveryAgent::finished,
                            &Self, &BluetoothSearchClass::scanServiceFinished_BT);
                        disconnect(Self.mDiscoveryServiceAgent_BT, QOverload<QBluetoothServiceDiscoveryAgent::Error>::of(&QBluetoothServiceDiscoveryAgent::error),
                            &Self, &BluetoothSearchClass::scanServiceErrorOccurred_BT);

                        if(Self.mDiscoveryServiceAgent_BT->isActive())
                        {
                            Self.mDiscoveryServiceAgent_BT->stop();
                            Self.scanServiceFinished_BT();
                        }
                        delete Self.mDiscoveryServiceAgent_BT;
                        Self.mDiscoveryServiceAgent_BT = nullptr;
                    }
                    else if(Self.mDiscoveryServiceAgent_BLE)
                    {
                        disconnect(Self.mDiscoveryServiceAgent_BLE, &QLowEnergyController::serviceDiscovered,
                            &Self, &BluetoothSearchClass::serviceDiscovered_BLE);
                        disconnect(Self.mDiscoveryServiceAgent_BLE, &QLowEnergyController::discoveryFinished,
                            &Self, &BluetoothSearchClass::scanServiceFinished_BLE);
                        disconnect(Self.mDiscoveryServiceAgent_BLE, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
                            &Self, &BluetoothSearchClass::scanServiceErrorOccurred_BLE);
                        disconnect(Self.mDiscoveryServiceAgent_BLE, &QLowEnergyController::connected,
                            &Self, &BluetoothSearchClass::deviceConnected_BLE);
                        disconnect(Self.mDiscoveryServiceAgent_BLE, &QLowEnergyController::disconnected,
                            &Self, &BluetoothSearchClass::deviceDisconnected_BLE);

                        if(Self.mDiscoveryServiceAgent_BLE->state() != QLowEnergyController::UnconnectedState)
                        {
                            Self.mDiscoveryServiceAgent_BLE->disconnectFromDevice();
                            Self.scanServiceFinished_BLE();
                        }
                        delete Self.mDiscoveryServiceAgent_BLE;
                        Self.mDiscoveryServiceAgent_BLE = nullptr;
                    }
                }
                Mutex::Unlock(Self.mDiscoverMutex);
            }

        public:
            static QBluetoothDeviceInfo* GetClonedSearchedDevice(chars address, bool locking)
            {
                auto& Self = ST();
                QBluetoothDeviceInfo* NewDeviceInfo = nullptr;
                if(locking) Mutex::Lock(Self.mDiscoverMutex);
                {
                    auto Result = Self.mDiscoveredDevices.find(QString::fromUtf8(address));
                    if(Result != Self.mDiscoveredDevices.end())
                        NewDeviceInfo = new QBluetoothDeviceInfo(Result.value());
                }
                if(locking) Mutex::Unlock(Self.mDiscoverMutex);
                return NewDeviceInfo;
            }
            static QBluetoothDeviceInfo* CreateDevice(chars devicename, chars address)
            {
                const QBluetoothAddress NewAddress(QString::fromUtf8(address));
                QBluetoothDeviceInfo* NewDeviceInfo = new QBluetoothDeviceInfo(NewAddress, QString::fromUtf8(devicename), 0);
                return NewDeviceInfo;
            }
            static QBluetoothDeviceInfo* CloneOrCreateDevice(chars devicename_and_address, bool locking)
            {
                // 주소로만 구성된 경우 저장된 장치정보에서 검색
                if(auto Result = BluetoothSearchClass::GetClonedSearchedDevice(devicename_and_address, locking))
                    return Result;

                // "장치명/주소"로 구성된 경우 장치정보 신규생성
                const String DeviceNameAndAddress = devicename_and_address;
                sint32 SlashPos = -1;
                sint32 NextSlashPos = 0;
                while(NextSlashPos != -1)
                {
                    NextSlashPos = DeviceNameAndAddress.Find(SlashPos + 1, "/");
                    if(NextSlashPos != -1)
                        SlashPos = NextSlashPos;
                }
                if(SlashPos != -1)
                    return BluetoothSearchClass::CreateDevice(
                        DeviceNameAndAddress.Left(SlashPos), DeviceNameAndAddress.Offset(SlashPos + 1));

                return nullptr;
            }
            static QBluetoothServiceInfo* GetClonedSearchedService_BT(chars uuid, bool locking)
            {
                auto& Self = ST();
                QBluetoothServiceInfo* NewServiceInfo = nullptr;
                if(locking) Mutex::Lock(Self.mDiscoverMutex);
                {
                    auto Result = Self.mDiscoveredServices_BT.find(QString::fromUtf8(uuid));
                    if(Result != Self.mDiscoveredServices_BT.end())
                        NewServiceInfo = new QBluetoothServiceInfo(Result.value());
                }
                if(locking) Mutex::Unlock(Self.mDiscoverMutex);
                return NewServiceInfo;
            }
            static QLowEnergyController* CreateController_BLE(QObject* parent, bool locking)
            {
                auto& Self = ST();
                QLowEnergyController* NewController = nullptr;
                if(locking) Mutex::Lock(Self.mDiscoverMutex);
                {
                    if(auto NewDevice = CloneOrCreateDevice(Self.mFocusedDeviceAddress.toUtf8().constData(), false))
                    {
                        if(NewDevice->coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
                        {
                            NewController = QLowEnergyController::createCentral(*NewDevice, parent);
                            NewController->setRemoteAddressType(QLowEnergyController::PublicAddress);
                        }
                        delete NewDevice;
                    }
                }
                if(locking) Mutex::Unlock(Self.mDiscoverMutex);
                return NewController;
            }

        private slots:
            void deviceDiscovered(const QBluetoothDeviceInfo& device)
            {
                const String NewAddress = device.address().toString().toUtf8().constData();
                Mutex::Lock(mDiscoverMutex);
                {
                    mDiscoveredDevices.insert(device.address().toString(), device);
                }
                Mutex::Unlock(mDiscoverMutex);

                String DeviceName = device.name().toUtf8().constData();
                if(DeviceName.Length() == 0)
                    DeviceName += "Unknown Device";
                if(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
                    DeviceName += " (BLE)";
                Platform::BroadcastNotify(DeviceName, NewAddress, NT_BluetoothDevice);
            }
            void scanDeviceFinished()
            {
                Platform::BroadcastNotify("ScanDeviceFinished", nullptr, NT_BluetoothSearch);
            }
            void scanDeviceErrorOccurred(QBluetoothDeviceDiscoveryAgent::Error error)
            {
                Platform::BroadcastNotify("ScanDeviceError", String::Format("code:%d", (sint32) error), NT_BluetoothSearch);
            }

            void serviceDiscovered_BT(const QBluetoothServiceInfo& service)
            {
                String ServiceName = service.serviceName().toUtf8().constData();
                if(ServiceName.Length() == 0) // 안드로이드에서는 윈도우와 달리 서비스명이 나오지 않는 경우도 있음
                    ServiceName = "Unknown Service";

                Strings UuidCollector;
                for(const auto& CurUuid : service.serviceClassUuids())
                    UuidCollector.AtAdding() = CurUuid.toString().toUtf8().constData();
                if(UuidCollector.Count() == 0) // 안드로이드에서는 윈도우와 달리 UUID가 나오지 않는 경우도 있음
                {
                    static sint32 BlankUUID = 0;
                    UuidCollector.AtAdding() = String::Format("{63ce%04d-ebad-467d-b939-86d3229de522}", ++BlankUUID % 10000);
                }

                Mutex::Lock(mDiscoverMutex);
                {
                    for(sint32 i = 0, iend = UuidCollector.Count(); i < iend; ++i)
                        mDiscoveredServices_BT.insert(QString((chars) UuidCollector[i]), service);
                }
                Mutex::Unlock(mDiscoverMutex);

                Platform::BroadcastNotify(ServiceName, UuidCollector, NT_BluetoothService);
            }
            void scanServiceFinished_BT()
            {
                Platform::BroadcastNotify("ScanServiceFinished", nullptr, NT_BluetoothSearch);
            }
            void scanServiceErrorOccurred_BT(QBluetoothServiceDiscoveryAgent::Error error)
            {
                Platform::BroadcastNotify("ScanServiceError", String::Format("code:%d", (sint32) error), NT_BluetoothSearch);
            }

            void serviceDiscovered_BLE(const QBluetoothUuid& serviceUuid)
            {
                String ServiceName = "Unknown Service";
                if(QLowEnergyService* NewService = mDiscoveryServiceAgent_BLE->createServiceObject(serviceUuid))
                {
                    const String NewServiceName = NewService->serviceName().toUtf8().constData();
                    if(0 < NewServiceName.Length()) // 안드로이드에서는 윈도우와 달리 서비스명이 나오지 않는 경우도 있음
                        ServiceName = NewServiceName;
                    delete NewService;
                }

                Strings UuidCollector;
                UuidCollector.AtAdding() = serviceUuid.toString().toUtf8().constData();

                Platform::BroadcastNotify(ServiceName, UuidCollector, NT_BluetoothService);
            }
            void scanServiceFinished_BLE()
            {
                Platform::BroadcastNotify("ScanServiceFinished", nullptr, NT_BluetoothSearch);
            }
            void scanServiceErrorOccurred_BLE(QLowEnergyController::Error error)
            {
                Platform::BroadcastNotify("ScanServiceError", String::Format("code:%d", (sint32) error), NT_BluetoothSearch);
            }
            void deviceConnected_BLE()
            {
                mDiscoveryServiceAgent_BLE->discoverServices();
            }
            void deviceDisconnected_BLE()
            {
            }

        private:
            BluetoothSearchClass()
            {
                mDiscoverMutex = Mutex::Open();
                mDiscoveryDeviceAgent = nullptr;
                mDiscoveryServiceAgent_BT = nullptr;
                mDiscoveryServiceAgent_BLE = nullptr;
            }
            ~BluetoothSearchClass()
            {
                delete mDiscoveryDeviceAgent;
                delete mDiscoveryServiceAgent_BT;
                delete mDiscoveryServiceAgent_BLE;
                Mutex::Close(mDiscoverMutex);
            }

        private:
            static BluetoothSearchClass& ST()
            {static BluetoothSearchClass _; return _;}

        private:
            id_mutex mDiscoverMutex;
            QBluetoothDeviceDiscoveryAgent* mDiscoveryDeviceAgent;
            QMap<QString, QBluetoothDeviceInfo> mDiscoveredDevices;
            QString mFocusedDeviceAddress;
            QBluetoothServiceDiscoveryAgent* mDiscoveryServiceAgent_BT;
            QMap<QString, QBluetoothServiceInfo> mDiscoveredServices_BT;
            QLowEnergyController* mDiscoveryServiceAgent_BLE;
        };

        class BluetoothClass : public QObject
        {
            Q_OBJECT

        public:
            virtual bool Connected() = 0;
            virtual sint32 ReadAvailable() = 0;
            virtual sint32 Read(uint08* data, const sint32 size) = 0;
            virtual bool Write(const uint08* data, const sint32 size) = 0;

        public:
            BluetoothClass()
            {
            }
            virtual ~BluetoothClass()
            {
            }
        };

        class BluetoothServerClass : public BluetoothClass
        {
            Q_OBJECT

        public:
            bool Init(chars service, chars uuid, chars description = "-Description-", chars provider = "-Provider-")
            {
                mServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
                const QBluetoothAddress LocalAdapter = QBluetoothAddress();
                connect(mServer, &QBluetoothServer::newConnection, this, &BluetoothServerClass::OnClientConnected);
                mServer->listen(LocalAdapter);

                SetServiceInfo(service, uuid, description, provider);
                mServiceInfo.registerService(LocalAdapter);
                return true;
            }

        public:
            bool Connected() override
            {
                return (mClient != nullptr);
            }
            sint32 ReadAvailable() override
            {
                return mRecvData.count();
            }
            sint32 Read(uint08* data, const sint32 size) override
            {
                if(mClient)
                {
                    const sint32 MinSize = Math::Min(mRecvData.count(), size);
                    Memory::Copy(data, mRecvData.constData(), MinSize);
                    mRecvData.remove(0, MinSize);
                    return MinSize;
                }
                return -1;
            }
            bool Write(const uint08* data, const sint32 size) override
            {
                if(mClient)
                {
                    mClient->write((chars) data, size);
                    mClient->waitForBytesWritten(3000);
                    return (mClient->state() == QAbstractSocket::ConnectedState);
                }
                return false;
            }

        private slots:
            void OnClientConnected()
            {
                if(mServer && !mClient)
                {
                    mClient = mServer->nextPendingConnection();
                    connect(mClient, &QBluetoothSocket::disconnected, this, &BluetoothServerClass::OnDisconnected);
                    connect(mClient, &QBluetoothSocket::readyRead, this, &BluetoothServerClass::OnReadyRead);
                    Platform::BroadcastNotify("connected", nullptr, NT_BluetoothReceive);
                }
            }
            void OnDisconnected()
            {
                mClient->deleteLater();
                mClient = nullptr;
                Platform::BroadcastNotify("disconnected", nullptr, NT_BluetoothReceive);
            }
            void OnReadyRead()
            {
                auto ReadSize = mClient->bytesAvailable();
                if(0 < ReadSize)
                {
                    mRecvData += mClient->read(ReadSize);
                    Platform::BroadcastNotify("message", nullptr, NT_BluetoothReceive);
                }
            }

        private:
            void SetServiceInfo(chars service, chars uuid, chars description, chars provider)
            {
                // Profile구성
                QBluetoothServiceInfo::Sequence ProfileSequence;
                QBluetoothServiceInfo::Sequence ClassID;
                ClassID << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
                ClassID << QVariant::fromValue(quint16(0x100));
                ProfileSequence.append(QVariant::fromValue(ClassID));
                mServiceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList, ProfileSequence);

                // Service구성
                ClassID.clear();
                ClassID << QVariant::fromValue(QBluetoothUuid(QString::fromUtf8(uuid)));
                ClassID << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
                mServiceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, ClassID);
                mServiceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr(service));
                mServiceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription, tr(description));
                mServiceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr(provider));
                mServiceInfo.setServiceUuid(QBluetoothUuid(QString::fromUtf8(uuid)));

                // Browse구성
                QBluetoothServiceInfo::Sequence PublicBrowse;
                PublicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
                mServiceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList, PublicBrowse);

                // Protocol구성
                QBluetoothServiceInfo::Sequence ProtocolDescriptorList;
                QBluetoothServiceInfo::Sequence Protocol;
                Protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
                ProtocolDescriptorList.append(QVariant::fromValue(Protocol));
                Protocol.clear();
                Protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm));
                Protocol << QVariant::fromValue(quint8(mServer->serverPort()));
                ProtocolDescriptorList.append(QVariant::fromValue(Protocol));
                mServiceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList, ProtocolDescriptorList);
            }

        public:
            BluetoothServerClass()
            {
                mServer = nullptr;
                mClient = nullptr;
            }
            ~BluetoothServerClass() override
            {
                delete mServer;
            }

        private:
            QBluetoothServer* mServer;
            QBluetoothServiceInfo mServiceInfo;
            QBluetoothSocket* mClient;
            QByteArray mRecvData;
        };

        // 미완성
        class BluetoothLEServerClass : public BluetoothClass
        {
            Q_OBJECT

        public:
            bool Init(chars service, chars uuid)
            {
                mServiceName = QString(service);
                mServiceUuid = QBluetoothUuid(QString(uuid));
                if(mController = QLowEnergyController::createPeripheral())
                {
                    mAdvertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
                    mAdvertisingData.setIncludePowerLevel(true);
                    mAdvertisingData.setLocalName(mServiceName);
                    mAdvertisingData.setServices(QList<QBluetoothUuid>() << mServiceUuid);

                    mWriteCharData.setUuid(QBluetoothUuid(QString("f377fa5e-dd91-4427-8606-5cdddce19977")));
                    mWriteCharData.setProperties(QLowEnergyCharacteristic::Write);

                    mReadCharData.setUuid(QBluetoothUuid(QString("f377fa5e-dd91-4427-8606-5cdddce19988")));
                    mReadCharData.setProperties(QLowEnergyCharacteristic::Read);

                    mServiceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
                    mServiceData.setUuid(mServiceUuid);
                    mServiceData.addCharacteristic(mWriteCharData);
                    mServiceData.addCharacteristic(mReadCharData);

                    connect(mController, &QLowEnergyController::connected, this, &BluetoothLEServerClass::OnConnected);
                    connect(mController, &QLowEnergyController::disconnected, this, &BluetoothLEServerClass::OnDisconnected);
                    mReadTimer = new QTimer(this);
                    connect(mReadTimer, &QTimer::timeout, this, &BluetoothLEServerClass::OnRead);
                    mReadTimer->start(100);
                    SetServiceInfo();
                    return true;
                }
                return false;
            }

        public:
            bool Connected() override
            {
                if(mController)
                    return (mController->state() == QLowEnergyController::ConnectedState);
                return false;
            }
            sint32 ReadAvailable() override
            {
                return mReadCount;
            }
            sint32 Read(uint08* data, const sint32 size) override
            {
                if(mService)
                {
                    if(!mReadQueue.isEmpty())
                    {
                        QByteArray CurBytes = mReadQueue.dequeue();
                        const sint32 CopySize = Math::Min(size, CurBytes.length());
                        Memory::Copy(data, CurBytes.constData(), CopySize);
                        mReadCount -= CurBytes.length();
                        return CopySize;
                    }
                    return 0;
                }
                return -1;
            }
            bool Write(const uint08* data, const sint32 size) override
            {
                if(mService)
                {
                    QLowEnergyCharacteristic WriteChar = mService->characteristic(QBluetoothUuid(QString("f377fa5e-dd91-4427-8606-5cdddce19977")));
                    if(WriteChar.isValid())
                    {
                        mService->writeCharacteristic(WriteChar, QByteArray((chars) data, size));
                        return true;
                    }
                }
                return false;
            }

        private slots:
            void OnConnected()
            {
            }
            void OnDisconnected()
            {
                SetServiceInfo();
            }
            void OnRead()
            {
                if(mService)
                {
                    QLowEnergyCharacteristic ReadChar = mService->characteristic(QBluetoothUuid(QString("f377fa5e-dd91-4427-8606-5cdddce19988")));
                    if(ReadChar.isValid())
                        mService->readCharacteristic(ReadChar);
                }
            }

            void OnCharacteristicChanged(const QLowEnergyCharacteristic& info, const QByteArray& value)
            {
                mReadCount += value.length();
                mReadQueue.enqueue(value);
                Platform::BroadcastNotify("message", nullptr, NT_BluetoothReceive);
            }
            void OnCharacteristicRead(const QLowEnergyCharacteristic& info, const QByteArray& value)
            {
                mReadCount += value.length();
                mReadQueue.enqueue(value);
                Platform::BroadcastNotify("message", nullptr, NT_BluetoothReceive);
            }
            void OnCharacteristicWritten(const QLowEnergyCharacteristic& info, const QByteArray& value)
            {
            }

        private:
            void SetServiceInfo()
            {
                delete mService;
                mService = mController->addService(mServiceData);
                connect(mService, &QLowEnergyService::characteristicChanged, this, &BluetoothLEServerClass::OnCharacteristicChanged);
                connect(mService, &QLowEnergyService::characteristicRead, this, &BluetoothLEServerClass::OnCharacteristicRead);
                connect(mService, &QLowEnergyService::characteristicWritten, this, &BluetoothLEServerClass::OnCharacteristicWritten);
                mController->startAdvertising(QLowEnergyAdvertisingParameters(), mAdvertisingData, mAdvertisingData);
            }

        public:
            BluetoothLEServerClass()
            {
                mController = nullptr;
                mService = nullptr;
                mReadTimer = nullptr;
                mReadCount = 0;
            }
            ~BluetoothLEServerClass() override
            {
                if(mReadTimer)
                {
                    mReadTimer->deleteLater();
                    delete mReadTimer;
                }

                delete mService;
                delete mController;
            }

        private:
            QString mServiceName;
            QBluetoothUuid mServiceUuid;
            QLowEnergyAdvertisingData mAdvertisingData;
            QLowEnergyCharacteristicData mWriteCharData;
            QLowEnergyCharacteristicData mReadCharData;
            QLowEnergyServiceData mServiceData;
            QLowEnergyController* mController;
            QLowEnergyService* mService;
            QTimer* mReadTimer;
            QQueue<QByteArray> mReadQueue;
            sint32 mReadCount;
        };

        class BluetoothClientClass : public BluetoothClass
        {
            Q_OBJECT

        public:
            bool Init(chars uuid)
            {
                if(auto NewService = BluetoothSearchClass::GetClonedSearchedService_BT(uuid, true))
                {
                    mSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
                    connect(mSocket, &QBluetoothSocket::connected, this, &BluetoothClientClass::OnConnected);
                    connect(mSocket, &QBluetoothSocket::disconnected, this, &BluetoothClientClass::OnDisconnected);
                    connect(mSocket, &QBluetoothSocket::readyRead, this, &BluetoothClientClass::OnReadyRead);
                    connect(mSocket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error),
                        this, &BluetoothClientClass::OnErrorOccurred);
                    mSocket->connectToService(*NewService);
                    delete NewService;
                    return true;
                }
                return false;
            }

        public:
            bool Connected() override
            {
                return mConnected;
            }
            sint32 ReadAvailable() override
            {
                return mRecvData.count();
            }
            sint32 Read(uint08* data, const sint32 size) override
            {
                if(mSocket)
                {
                    const sint32 MinSize = Math::Min(mRecvData.count(), size);
                    Memory::Copy(data, mRecvData.constData(), MinSize);
                    mRecvData.remove(0, MinSize);
                    return MinSize;
                }
                return -1;
            }
            bool Write(const uint08* data, const sint32 size) override
            {
                if(mSocket)
                {
                    mSocket->write((chars) data, size);
                    mSocket->waitForBytesWritten(3000);
                    return (mSocket->state() == QAbstractSocket::ConnectedState);
                }
                return false;
            }

        private slots:
            void OnConnected()
            {
                if(mSocket)
                {
                    mConnected = true;
                    mPeerName = mSocket->peerName();
                    Platform::BroadcastNotify("connected", nullptr, NT_BluetoothReceive);
                }
            }
            void OnDisconnected()
            {
                mConnected = false;
                mPeerName.clear();
                Platform::BroadcastNotify("disconnected", nullptr, NT_BluetoothReceive);
            }
            void OnReadyRead()
            {
                if(mSocket)
                {
                    auto ReadSize = mSocket->bytesAvailable();
                    if(0 < ReadSize)
                    {
                        mRecvData += mSocket->read(ReadSize);
                        Platform::BroadcastNotify("message", nullptr, NT_BluetoothReceive);
                    }
                }
            }
            void OnErrorOccurred(QBluetoothSocket::SocketError error)
            {
                mConnected = false;
                Platform::BroadcastNotify("error", nullptr, NT_BluetoothReceive);
            }

        public:
            BluetoothClientClass()
            {
                mSocket = nullptr;
                mConnected = false;
            }
            ~BluetoothClientClass() override
            {
                delete mSocket;
            }

        private:
            QBluetoothSocket* mSocket;
            bool mConnected;
            QString mPeerName;
            QByteArray mRecvData;
        };

        class BluetoothLEClientClass : public BluetoothClass
        {
            Q_OBJECT

        public:
            bool Init(chars uuid)
            {
                mServiceUuid = QBluetoothUuid(QString(uuid));
                if(mController = BluetoothSearchClass::CreateController_BLE(this, true))
                {
                    connect(mController, &QLowEnergyController::serviceDiscovered,
                        this, &BluetoothLEClientClass::OnServiceDiscovered);
                    connect(mController, &QLowEnergyController::discoveryFinished,
                        this, &BluetoothLEClientClass::OnScanServiceFinished);
                    connect(mController, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
                        this, &BluetoothLEClientClass::OnScanServiceErrorOccurred);
                    connect(mController, &QLowEnergyController::connected,
                        this, &BluetoothLEClientClass::OnDeviceConnected);
                    connect(mController, &QLowEnergyController::disconnected,
                        this, &BluetoothLEClientClass::OnDeviceDisconnected);
                    mController->connectToDevice();
                    return true;
                }
                return false;
            }

        public:
            bool Connected() override
            {
                return mConnected;
            }
            sint32 ReadAvailable() override
            {
                return mReadCount;
            }
            sint32 Read(uint08* data, const sint32 size) override
            {
                if(mService)
                {
                    if(!mReadQueue.isEmpty())
                    {
                        QByteArray CurBytes = mReadQueue.dequeue();
                        const sint32 CopySize = Math::Min(size, CurBytes.length());
                        Memory::Copy(data, CurBytes.constData(), CopySize);
                        mReadCount -= CurBytes.length();
                        return CopySize;
                    }
                    return 0;
                }
                return -1;
            }
            bool Write(const uint08* data, const sint32 size) override
            {
                if(mService)
                {
                    mWriteQueue.enqueue(QByteArray((chars) data, size));
                    if(mConnected)
                        WriteFlush();
                    return true;
                }
                return false;
            }

        private slots:
            void OnServiceDiscovered(const QBluetoothUuid& serviceUuid)
            {
            }
            void OnScanServiceFinished()
            {
                if(mService = mController->createServiceObject(mServiceUuid, this))
                {
                    connect(mService, &QLowEnergyService::stateChanged, this, &BluetoothLEClientClass::OnServiceStateChanged);
                    connect(mService, &QLowEnergyService::characteristicChanged, this, &BluetoothLEClientClass::OnCharacteristicChanged);
                    connect(mService, &QLowEnergyService::characteristicRead, this, &BluetoothLEClientClass::OnCharacteristicRead);
                    connect(mService, &QLowEnergyService::characteristicWritten, this, &BluetoothLEClientClass::OnCharacteristicWritten);
                    connect(mService, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
                        this, &BluetoothLEClientClass::OnErrorOccurred);
                    mService->discoverDetails();
                }
            }
            void OnScanServiceErrorOccurred(QLowEnergyController::Error error)
            {
            }
            void OnDeviceConnected()
            {
                mController->discoverServices();
            }
            void OnDeviceDisconnected()
            {
            }

            void OnServiceStateChanged(QLowEnergyService::ServiceState state)
            {
                if(state == QLowEnergyService::ServiceDiscovered)
                {
                    if(InitCharacteristic())
                    {
                        mConnected = true;
                        Platform::BroadcastNotify("connected", nullptr, NT_BluetoothReceive);
                        WriteFlush();
                    }
                    else
                    {
                        mConnected = false;
                        Platform::BroadcastNotify("error", nullptr, NT_BluetoothReceive);
                    }
                }
                else if(state == QLowEnergyService::InvalidService)
                {
                    mConnected = false;
                    Platform::BroadcastNotify("error", nullptr, NT_BluetoothReceive);
                }
            }
            void OnCharacteristicChanged(const QLowEnergyCharacteristic& info, const QByteArray& value)
            {
                mReadCount += value.length();
                mReadQueue.enqueue(value);
                Platform::BroadcastNotify("message", nullptr, NT_BluetoothReceive);
            }
            void OnCharacteristicRead(const QLowEnergyCharacteristic& info, const QByteArray& value)
            {
                mReadCount += value.length();
                mReadQueue.enqueue(value);
                Platform::BroadcastNotify("message", nullptr, NT_BluetoothReceive);
            }
            void OnCharacteristicWritten(const QLowEnergyCharacteristic& info, const QByteArray& value)
            {
            }
            void OnErrorOccurred(QLowEnergyService::ServiceError error)
            {
                mConnected = false;
                Platform::BroadcastNotify("error", nullptr, NT_BluetoothReceive);
            }
            void OnRead()
            {
                if(mReadChar.isValid())
                    mService->readCharacteristic(mReadChar);
            }

        private:
            bool InitCharacteristic()
            {
                bool Success = false;
                if(mService)
                foreach(QLowEnergyCharacteristic c, mService->characteristics())
                {
                    if(c.isValid())
                    {
                        if((c.properties() & QLowEnergyCharacteristic::WriteNoResponse) ||
                            (c.properties() & QLowEnergyCharacteristic::Write))
                        {
                            Success = true;
                            mWriteChar = c;
                            if(c.properties() & QLowEnergyCharacteristic::Write)
                                mWriteMode = QLowEnergyService::WriteWithResponse;
                            else mWriteMode = QLowEnergyService::WriteWithoutResponse;
                        }
                        if(c.properties() & QLowEnergyCharacteristic::Read)
                        {
                            Success = true;
                            mReadChar = c;
                            if(!mReadTimer)
                            {
                                mReadTimer = new QTimer(this);
                                connect(mReadTimer, &QTimer::timeout, this, &BluetoothLEClientClass::OnRead);
                                mReadTimer->start(100);
                            }
                        }
                        mNotiDesc = c.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
                        if(mNotiDesc.isValid())
                            mService->writeDescriptor(mNotiDesc, QByteArray::fromHex("0100"));
                    }
                }
                return Success;
            }
            void WriteFlush()
            {
                if(mWriteChar.isValid())
                while(!mWriteQueue.isEmpty())
                {
                    QByteArray CurBytes = mWriteQueue.dequeue();
                    if(20 < CurBytes.length())
                    {
                        sint32 SentBytes = 0;
                        while(SentBytes < CurBytes.length())
                        {
                            mService->writeCharacteristic(mWriteChar, CurBytes.mid(SentBytes, 20), mWriteMode);
                            SentBytes += 20;
                            if(mWriteMode == QLowEnergyService::WriteWithResponse)
                            {
                                WaitForWrite();
                                if(mService->error() != QLowEnergyService::NoError)
                                {
                                    mConnected = false;
                                    Platform::BroadcastNotify("error", nullptr, NT_BluetoothReceive);
                                    return;
                                }
                            }
                        }
                    }
                    else mService->writeCharacteristic(mWriteChar, CurBytes, mWriteMode);
                }
            }
            void WaitForWrite()
            {
                QEventLoop Loop;
                connect(mService, SIGNAL(characteristicWritten(QLowEnergyCharacteristic, QByteArray)), &Loop, SLOT(quit()));
                Loop.exec();
            }

        public:
            BluetoothLEClientClass()
            {
                mController = nullptr;
                mService = nullptr;
                mConnected = false;
                mReadTimer = nullptr;
                mReadCount = 0;
            }
            ~BluetoothLEClientClass() override
            {
                if(mReadTimer)
                {
                    mReadTimer->deleteLater();
                    delete mReadTimer;
                }

                if(mService && mNotiDesc.isValid())
                    mService->writeDescriptor(mNotiDesc, QByteArray::fromHex("0000"));

                delete mService;
                delete mController;
            }

        private:
            QBluetoothUuid mServiceUuid;
            QLowEnergyController* mController;
            QLowEnergyService* mService;
            bool mConnected;
            QLowEnergyCharacteristic mWriteChar;
            QLowEnergyService::WriteMode mWriteMode;
            QLowEnergyCharacteristic mReadChar;
            QTimer* mReadTimer;
            QLowEnergyDescriptor mNotiDesc;
            QQueue<QByteArray> mWriteQueue;
            QQueue<QByteArray> mReadQueue;
            sint32 mReadCount;
        };

        #if BOSS_ANDROID
            class BluetoothContentForAndroid
            {
            public:
                static void MusicPlayWithSelector()
                {
                    QAndroidJniObject Activity = QtAndroid::androidActivity();
                    QAndroidJniObject::callStaticMethod<void>("com/boss2d/BossBluetoothSco", "init",
                        "(Landroid/app/Activity;)V", Activity.object<jobject>());
                    QAndroidJniObject NewIntent = QAndroidJniObject::callStaticObjectMethod("com/boss2d/BossBluetoothSco", "open",
                        "(Landroid/app/Activity;)Landroid/content/Intent;", Activity.object<jobject>());

                    QtAndroid::startActivity(NewIntent, 2345, [](int requestCode, int resultCode, const QAndroidJniObject& data)->void
                    {
                        const QAndroidJniObject Context = QtAndroid::androidContext();
                        QAndroidJniObject::callStaticMethod<void>("com/boss2d/BossBluetoothSco", "play",
                            "(Landroid/content/Context;Landroid/content/Intent;)V", Context.object<jobject>(), data.object<jobject>());
                    });
                }
                static void MusicStop()
                {
                    QAndroidJniObject::callStaticMethod<void>("com/boss2d/BossBluetoothSco", "stop", "()V");
                }
                static void SetVolume(float value)
                {
                    const QAndroidJniObject Context = QtAndroid::androidContext();
                    QAndroidJniObject::callStaticMethod<void>("com/boss2d/BossBluetoothSco", "setVolume",
                        "(Landroid/content/Context;F)V", Context.object<jobject>(), (jfloat) value);
                }
            };
            typedef BluetoothContentForAndroid BluetoothContentClass;
        #else
            class BluetoothContentForBlank
            {
            public:
                static void MusicPlayWithSelector()
                {
                }
                static void MusicStop()
                {
                }
                static void SetVolume(float value)
                {
                }
            };
            typedef BluetoothContentForBlank BluetoothContentClass;
        #endif
    #else
        class BluetoothSearchClass : public QObject
        {
            Q_OBJECT
        public:
            static void DeviceBegin() {}
            static void DeviceEnd() {}
        public:
            static void ServiceBegin(chars deviceaddress, Strings uuidfilters) {}
            static void ServiceEnd() {}
        };
        class BluetoothClass : public QObject
        {
            Q_OBJECT
        public:
            virtual bool Connected() {return false;}
            virtual sint32 ReadAvailable() {return 0;}
            virtual sint32 Read(uint08* data, const sint32 size) {return -1;}
            virtual bool Write(const uint08* data, const sint32 size) {return false;}

        public:
            BluetoothClass()
            {
            }
            virtual ~BluetoothClass()
            {
            }
        };
        class BluetoothServerClass : public BluetoothClass
        {
            Q_OBJECT
        public:
            bool Init(chars service, chars uuid, chars description = "-Description-", chars provider = "-Provider-") {return false;}
        };
        class BluetoothLEServerClass : public BluetoothClass
        {
            Q_OBJECT
        public:
            bool Init(chars service, chars uuid) {return false;}
        };
        class BluetoothClientClass : public BluetoothClass
        {
            Q_OBJECT
        public:
            bool Init(chars uuid) {return false;}
        };
        class BluetoothLEClientClass : public BluetoothClass
        {
            Q_OBJECT
        public:
            bool Init(chars uuid) {return false;}
        };
        class BluetoothContentClass
        {
        public:
            static void MusicPlayWithSelector() {}
            static void MusicStop() {}
            static void SetVolume(float value) {}
        };
    #endif

    #if defined(QT_HAVE_SERIALPORT)
        typedef QSerialPortInfo SerialPortInfoClass;
        typedef QSerialPort SerialPortClass;
    #else
        class SerialPortInfoForBlank
        {
        public:
            SerialPortInfoForBlank()
            {
            }
            ~SerialPortInfoForBlank()
            {
            }
        public:
            static const QList<SerialPortInfoForBlank>& availablePorts()
            {
                static QList<SerialPortInfoForBlank> Result;
                return Result;
            }
        public:
            QString portName() const {return QString();}
            QString description() const {return QString();}
            QString systemLocation() const {return QString();}
            QString manufacturer() const {return QString();}
            QString serialNumber() const {return QString();}
        };
        typedef SerialPortInfoForBlank SerialPortInfoClass;
        class SerialPortForBlank : public QObject
        {
            Q_OBJECT

        public:
            enum BaudRate {Baud115200};
            enum DataBits {Data8};
            enum Parity {NoParity};
            enum StopBits {OneStop};
            enum FlowControl {NoFlowControl};
            enum SerialPortError {NoError};
        public:
            SerialPortForBlank()
            {
            }
            SerialPortForBlank(const SerialPortInfoClass& info)
            {
            }
            ~SerialPortForBlank()
            {
            }
        public:
            bool open(QIODevice::OpenModeFlag flag) {return false;}
            void close() {}
            QByteArray readAll() {return QByteArray();}
            qint64 write(const char* data, qint64 len) {return 0;}
            bool flush() {return false;}
        public: // setter
            void setPort(const SerialPortInfoClass &serialPortInfo) {}
            void setPortName(const QString &name) {}
            void setBaudRate(BaudRate baudrate) {}
            void setDataBits(DataBits databits) {}
            void setParity(Parity parity) {}
            void setStopBits(StopBits stopbits) {}
            void setFlowControl(FlowControl flowcontrol) {}
        public: // getter
            QString portName() {return QString();}
            QString errorString() {return "NoError";}
            SerialPortError error() {return NoError;}
        };
        typedef SerialPortForBlank SerialPortClass;
    #endif

    class SerialClass : public SerialPortClass
    {
        Q_OBJECT

    private:
        SerialDecodeCB mDec;
        SerialEncodeCB mEnc;
        uint08s mReadStream;
        uint08s mWriteStream;
        uint08s mLastDecodedData;
        sint32 mLastDecodedDataFocus;

    private:
        class Chunk
        {
        public:
            uint08* mPtr;
            bytes mPtrConst;
            sint32 mLength;
            bool mBigEndian;
        public:
            Chunk() {mPtr = nullptr; mPtrConst = nullptr; mLength = 0; mBigEndian = false;}
            Chunk(uint08* ptr, sint32 length, bool bigendian) {Init(ptr, length, bigendian);}
            Chunk(bytes ptr, sint32 length, bool bigendian) {InitConst(ptr, length, bigendian);}
            ~Chunk() {}
            void Init(uint08* ptr, sint32 length, bool bigendian)
            {
                mPtr = ptr;
                mPtrConst = nullptr;
                mLength = length;
                mBigEndian = bigendian;
            }
            void InitConst(bytes ptr, sint32 length, bool bigendian)
            {
                mPtr = nullptr;
                mPtrConst = ptr;
                mLength = length;
                mBigEndian = bigendian;
            }
            sint64 GetLength() const
            {
                sint64 Result;
                Load(&Result, sizeof(sint64));
                return Result;
            }
            void Load(void* outptr, sint32 length) const
            {
                Memory::Set(outptr, 0, length);
                const sint32 CopyLength = Math::Min(mLength, length);
                if(!mBigEndian) Memory::Copy(outptr, mPtrConst, CopyLength);
                else
                {
                    uint08* Dest = ((uint08*) outptr) + CopyLength - 1;
                    for(sint32 i = 0; i < CopyLength; ++i)
                        Dest[-i] = mPtrConst[i];
                }
            }
            void Save(bytes ptr, sint32 length)
            {
                Memory::Set(mPtr, 0, mLength);
                const sint32 CopyLength = Math::Min(mLength, length);
                if(!mBigEndian) Memory::Copy(mPtr, ptr, CopyLength);
                else
                {
                    uint08* Dest = ((uint08*) mPtr) + CopyLength - 1;
                    for(sint32 i = 0; i < CopyLength; ++i)
                        Dest[-i] = ptr[i];
                }
            }
        };

    public:
        static Strings GetList(String* spec)
        {
            Strings Result;
            Context SpecCollector;
            const QList<SerialPortInfoClass>& AllPorts = SerialPortInfoClass::availablePorts();
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
        SerialClass()
        {
            mDec = DefaultSerialDecode;
            mEnc = DefaultSerialEncode;
            mLastDecodedDataFocus = 0;
        }
        SerialClass(chars name, sint32 baudrate, SerialDecodeCB dec, SerialEncodeCB enc)
        {
            mDec = (dec)? dec : DefaultSerialDecode;
            mEnc = (enc)? enc : DefaultSerialEncode;
            mLastDecodedDataFocus = 0;
            const QList<SerialPortInfoClass>& AllPorts = SerialPortInfoClass::availablePorts();
            foreach(const auto& CurPort, AllPorts)
            {
                if(*name == '\0' || CurPort.portName() == name || CurPort.description() == name)
                {
                    if(baudrate != -1)
                    {
                        setPortName(CurPort.portName());
                        setBaudRate((SerialPortClass::BaudRate) baudrate);
                        setDataBits(SerialPortClass::Data8);
                        setParity(SerialPortClass::NoParity);
                        setStopBits(SerialPortClass::OneStop);
                        setFlowControl(SerialPortClass::NoFlowControl);
                    }
                    else setPort(CurPort);

                    if(!open(QIODevice::ReadWrite))
                    {
                        BOSS_TRACE("SerialClass error: port=%s, error=%s(%d)",
                            portName().toUtf8().constData(), errorString().toUtf8().constData(), (sint32) error());
                        if(*name == '\0')
                            continue;
                    }
                    #if !BOSS_WASM
                        else
                        {
                            connect(this, &QSerialPort::errorOccurred, this, &SerialClass::OnErrorOccurred);
                            connect(this, &QSerialPort::readyRead, this, &SerialClass::OnRead);
                        }
                    #endif
                    break;
                }
            }
        }

        ~SerialClass()
        {
        }

    private slots:
        void OnErrorOccurred(SerialPortClass::SerialPortError error)
        {
            Platform::BroadcastNotify("error", nullptr, NT_Serial);
        }

        void OnRead()
        {
            QByteArray NewArray = readAll();
            if(0 < NewArray.length())
            {
                Memory::Copy(mReadStream.AtDumpingAdded(NewArray.length()), NewArray.constData(), NewArray.length());
                Platform::BroadcastNotify("message", nullptr, NT_Serial);
            }
        }

    public:
        bool IsValid()
        {
            #if !BOSS_WASM
                return isOpen();
            #else
                return false;
            #endif
        }

        bool Connected()
        {
            auto ErrorCode = error();
            return (ErrorCode == SerialPortClass::NoError);
        }

        bool ReadReady(sint32* gettype)
        {
            OnRead();
            // 읽기스트림의 처리
            if(0 < mReadStream.Count())
            {
                // 디코딩과정
                const sint32 UsedLength = mDec(&mReadStream[0], mReadStream.Count(), mLastDecodedData, gettype);
                if(0 < UsedLength)
                {
                    if(UsedLength <= mReadStream.Count())
                        BOSS_TRACE("SerialDecodeCB의 리턴값이 허용범위를 초과합니다");
                    const sint32 CopyLength = mReadStream.Count() - UsedLength;
                    if(0 < CopyLength)
                    {
                        uint08s NewReadStream;
                        Memory::Copy(NewReadStream.AtDumpingAdded(CopyLength), &mReadStream[UsedLength], CopyLength);
                        mReadStream = NewReadStream;
                    }
                    else mReadStream.SubtractionAll();
                    return (0 < mLastDecodedData.Count());
                }
            }
            return false;
        }

        sint32 ReadAvailable()
        {
            return mLastDecodedData.Count() - mLastDecodedDataFocus;
        }

        sint32 Read(chars format, va_list args)
        {
            Map<Chunk> Cache;
            chars Pos = format;
            bool CurEndian = false;
            sint32 CurIndex = -1;
            while(*Pos)
            {
                if(*Pos == '#')
                {
                    CurIndex++;
                    // 인용길이
                    sint64 LastLength = 0;
                         if('1' <= Pos[1] && Pos[1] <= '8') LastLength = Pos[1] - '0';
                    else if('a' <= Pos[1] && Pos[1] <= 'z') LastLength = Cache[Pos[1] - 'a'].GetLength();
                    else if('A' <= Pos[1] && Pos[1] <= 'Z') LastLength = Cache[Pos[1] - 'A'].GetLength();
                    else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                    if(ReadAvailable() < LastLength) return BOSS_SERIAL_ERROR_SHORT_STREAM - CurIndex;
                    Pos += 2;

                    bytes LastPtr = &mLastDecodedData[mLastDecodedDataFocus];
                    // 캐시로 읽기
                    if(*Pos == '=')
                    {
                             if('a' <= Pos[1] && Pos[1] <= 'z') Cache[Pos[1] - 'a'].InitConst(LastPtr, LastLength, CurEndian);
                        else if('A' <= Pos[1] && Pos[1] <= 'Z') Cache[Pos[1] - 'A'].InitConst(LastPtr, LastLength, CurEndian);
                        else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        Pos += 2;
                    }
                    // 타입힌트식 읽기
                    else if(*Pos == ':')
                    {
                        if(Pos[1] == 's' && Pos[2] == 'k' && Pos[3] == 'i' && Pos[4] == 'p') Pos += 5;
                        else
                        {
                            const Chunk NewChunk(LastPtr, LastLength, CurEndian);
                            if(Pos[1] == 's')
                            {
                                     if(Pos[2] == '1') NewChunk.Load(va_arg(args, sint08*), sizeof(sint08));
                                else if(Pos[2] == '2') NewChunk.Load(va_arg(args, sint16*), sizeof(sint16));
                                else if(Pos[2] == '4') NewChunk.Load(va_arg(args, sint32*), sizeof(sint32));
                                else if(Pos[2] == '8') NewChunk.Load(va_arg(args, sint64*), sizeof(sint64));
                                else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                            }
                            else if(Pos[1] == 'u')
                            {
                                     if(Pos[2] == '1') NewChunk.Load(va_arg(args, uint08*), sizeof(uint08));
                                else if(Pos[2] == '2') NewChunk.Load(va_arg(args, uint16*), sizeof(uint16));
                                else if(Pos[2] == '4') NewChunk.Load(va_arg(args, uint32*), sizeof(uint32));
                                else if(Pos[2] == '8') NewChunk.Load(va_arg(args, uint64*), sizeof(uint64));
                                else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                            }
                            else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                            Pos += 3;
                        }
                    }
                    // 일반적인 읽기
                    else Chunk(LastPtr, LastLength, CurEndian).Load(va_arg(args, void*), LastLength);

                    // 데이터처리
                    mLastDecodedDataFocus += LastLength;
                    if(mLastDecodedData.Count() == mLastDecodedDataFocus)
                    {
                        mLastDecodedData.SubtractionAll();
                        mLastDecodedDataFocus = 0;
                    }
                }
                else if(*Pos == '[')
                {
                         if(Pos[1] == 'b' && Pos[2] == 'e' && Pos[3] == ']') CurEndian = true;
                    else if(Pos[1] == 'l' && Pos[2] == 'e' && Pos[3] == ']') CurEndian = false;
                    else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                    Pos += 4;
                }
                else Pos++;
            }
            return ReadAvailable();
        }

        sint32 Write(chars format, va_list args)
        {
            #if BOSS_ANDROID
                #define va_sint08 sint32
                #define va_sint16 sint32
                #define va_sint32 sint32
                #define va_sint64 sint64
                #define va_uint08 uint32
                #define va_uint16 uint32
                #define va_uint32 uint32
                #define va_uint64 uint64
            #else
                #define va_sint08 sint08
                #define va_sint16 sint16
                #define va_sint32 sint32
                #define va_sint64 sint64
                #define va_uint08 uint08
                #define va_uint16 uint16
                #define va_uint32 uint32
                #define va_uint64 uint64
            #endif

            chars Pos = format;
            bool CurEndian = false;
            while(*Pos)
            {
                if(*Pos == '#')
                {
                    // 인용길이
                    sint64 LastLength = 0;
                    if('1' <= Pos[1] && Pos[1] <= '8') LastLength = Pos[1] - '0';
                    else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                    Pos += 2;

                    uint08* LastPtr = mWriteStream.AtDumpingAdded(LastLength);
                    // 상수로 쓰기
                    if(*Pos == '=')
                    {
                        const sint32 SavedPos = &Pos[-1] - format;
                        uint08s Value;
                        if(Pos[1] == '0' && (Pos[2] == 'x' || Pos[2] == 'X'))
                        {
                            Pos += 2;
                            bool AddingTurn = false;
                            while(*(++Pos))
                            {
                                if(AddingTurn = !AddingTurn)
                                {
                                         if('0' <= *Pos && *Pos <= '9') Value.AtAdding() = ((*Pos - '0') & 0xF) << 4;
                                    else if('a' <= *Pos && *Pos <= 'f') Value.AtAdding() = ((*Pos - 'a' + 10) & 0xF) << 4;
                                    else if('A' <= *Pos && *Pos <= 'F') Value.AtAdding() = ((*Pos - 'A' + 10) & 0xF) << 4;
                                    else break;
                                }
                                else
                                {
                                         if('0' <= *Pos && *Pos <= '9') Value.At(-1) |= (*Pos - '0') & 0xF;
                                    else if('a' <= *Pos && *Pos <= 'f') Value.At(-1) |= (*Pos - 'a' + 10) & 0xF;
                                    else if('A' <= *Pos && *Pos <= 'F') Value.At(-1) |= (*Pos - 'A' + 10) & 0xF;
                                    else break;
                                }
                            }
                        }
                        else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        // 16진수는 큰수가 메모리의 앞쪽에 오므로 CurEndian을 반대로 적용
                        Chunk(LastPtr, LastLength, !CurEndian).Save(&Value[0], Value.Count());
                    }
                    // 타입힌트식 쓰기
                    else if(*Pos == ':')
                    {
                        Chunk NewChunk(LastPtr, LastLength, CurEndian);
                        if(Pos[1] == 's')
                        {
                                 if(Pos[2] == '1') {auto v = va_arg(args, va_sint08); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '2') {auto v = va_arg(args, va_sint16); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '4') {auto v = va_arg(args, va_sint32); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '8') {auto v = va_arg(args, va_sint64); NewChunk.Save((bytes) &v, sizeof(v));}
                            else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        }
                        else if(Pos[1] == 'u')
                        {
                                 if(Pos[2] == '1') {auto v = va_arg(args, va_uint08); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '2') {auto v = va_arg(args, va_uint16); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '4') {auto v = va_arg(args, va_uint32); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '8') {auto v = va_arg(args, va_uint64); NewChunk.Save((bytes) &v, sizeof(v));}
                            else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        }
                        else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        Pos += 3;
                    }
                    // 일반적인 쓰기
                    else
                    {
                        Chunk NewChunk(LastPtr, LastLength, CurEndian);
                             if(LastLength == 1) {auto v = va_arg(args, va_uint08); NewChunk.Save((bytes) &v, sizeof(v));}
                        else if(LastLength == 2) {auto v = va_arg(args, va_uint16); NewChunk.Save((bytes) &v, sizeof(v));}
                        else if(LastLength == 4) {auto v = va_arg(args, va_uint32); NewChunk.Save((bytes) &v, sizeof(v));}
                        else if(LastLength == 8) {auto v = va_arg(args, va_uint64); NewChunk.Save((bytes) &v, sizeof(v));}
                        else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[-1] - format);
                    }
                }
                else if(*Pos == '[')
                {
                         if(Pos[1] == 'b' && Pos[2] == 'e' && Pos[3] == ']') CurEndian = true;
                    else if(Pos[1] == 'l' && Pos[2] == 'e' && Pos[3] == ']') CurEndian = false;
                    else return BOSS_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                    Pos += 4;
                }
                else Pos++;
            }
            return mWriteStream.Count();
        }

        void WriteFlush(sint32 type)
        {
            // 쓰기스트림의 처리
            if(0 < mWriteStream.Count())
            {
                static uint08s NewEncodedData;
                NewEncodedData.SubtractionAll();
                mEnc(&mWriteStream[0], mWriteStream.Count(), NewEncodedData, type);
                mWriteStream.SubtractionAll();

                if(0 < NewEncodedData.Count())
                {
                    write((const char*) &NewEncodedData[0], NewEncodedData.Count());
                    flush();
                }
            }
        }

    private:
        static sint32 DefaultSerialDecode(bytes data, sint32 length, uint08s& outdata, sint32* outtype)
        {
            Memory::Copy(outdata.AtDumpingAdded(length), data, length);
            return length;
        }

        static void DefaultSerialEncode(bytes data, sint32 length, uint08s& outdata, sint32 type)
        {
            Memory::Copy(outdata.AtDumpingAdded(length), data, length);
        }
    };

    #ifdef QT_HAVE_MULTIMEDIA
        class CameraSurface : public QAbstractVideoSurface
        {
            Q_OBJECT

        private:
            QCamera mCamera;
            id_mutex mMutex;
            QVideoFrame::PixelFormat mPixelFormat;
            bool mNeedFlip;
            uint08s mLastImage;
            sint32 mLastImageWidth;
            sint32 mLastImageHeight;

        public:
            CameraSurface(const QCameraInfo& info) : mCamera(info)
            {
                mMutex = Mutex::Open();
                mNeedFlip = false;
                mLastImageWidth = 0;
                mLastImageHeight = 0;
            }
            ~CameraSurface()
            {
                StopCamera();
                Mutex::Close(mMutex);
            }

        public:
            void StartCamera() {mCamera.start();}
            void StopCamera() {mCamera.stop();}
            const QCameraViewfinderSettings GetSettings() {return mCamera.viewfinderSettings();}
            void SetSettings(const QCameraViewfinderSettings& settings) {mCamera.setViewfinderSettings(settings);}
            const QList<QCameraViewfinderSettings> GetSupportedAllSettings() {return mCamera.supportedViewfinderSettings();}

        protected:
            void StartPreview(bool preview) {mCamera.setViewfinder(this);}
            void StopPreview() {mCamera.setViewfinder((QAbstractVideoSurface*) nullptr);}
            sint32 GetLastImageWidth() const {return mLastImageWidth;}
            sint32 GetLastImageHeight() const {return mLastImageHeight;}
            id_texture CreateLastTexture(bool bitmapcache)
            {
                if(0 < mLastImageWidth && 0 < mLastImageHeight && 0 < mLastImage.Count())
                {
                    uint08s mDecodedBits;
                    switch(mPixelFormat)
                    {
                    case QVideoFrame::Format_RGB32:
                        mDecodedBits = mLastImage;
                        break;
                    case QVideoFrame::Format_BGR32:
                        ToARGB32<QVideoFrame::Format_BGR32, false>((bytes) &mLastImage[0], mLastImageWidth, mLastImageHeight, 4 * mLastImageWidth,
                            mDecodedBits.AtDumpingAdded(4 * mLastImageWidth * mLastImageHeight));
                        break;
                    case QVideoFrame::Format_YUYV:
                        ToARGB32<QVideoFrame::Format_YUYV, false>((bytes) &mLastImage[0], mLastImageWidth / 2, mLastImageHeight, 4 * mLastImageWidth,
                            mDecodedBits.AtDumpingAdded(4 * mLastImageWidth * mLastImageHeight));
                        break;
                    case QVideoFrame::Format_Jpeg:
                        ToARGB32<QVideoFrame::Format_Jpeg, false>((bytes) &mLastImage[0], mLastImageWidth, mLastImageHeight, mLastImage.Count(),
                            mDecodedBits.AtDumpingAdded(4 * mLastImageWidth * mLastImageHeight));
                        break;
                    }
                    return Platform::Graphics::CreateTexture(false, bitmapcache, mLastImageWidth, mLastImageHeight, &mDecodedBits[0]);
                }
                return nullptr;
            }

        protected:
            virtual bool CaptureEnabled() {return false;}
            virtual void BufferFlush() {}
            virtual void AddPictureShotCount() {}
            virtual void AddPreviewShotCount() {}

        protected:
            void DecodeImage(sint32& width, sint32& height, uint08s& bits)
            {
                Mutex::Lock(mMutex);
                if(0 < mLastImage.Count())
                {
                    width = mLastImageWidth;
                    height = mLastImageHeight;
                    bits.SubtractionAll();
                    if(mNeedFlip)
                    {
                        switch(mPixelFormat)
                        {
                        case QVideoFrame::Format_RGB32:
                            ToARGB32<QVideoFrame::Format_RGB32, true>((bytes) &mLastImage[0], width, height, 4 * width,
                                bits.AtDumpingAdded(4 * width * height));
                            break;
                        case QVideoFrame::Format_BGR32:
                            ToARGB32<QVideoFrame::Format_BGR32, true>((bytes) &mLastImage[0], width, height, 4 * width,
                                bits.AtDumpingAdded(4 * width * height));
                            break;
                        case QVideoFrame::Format_YUYV:
                            ToARGB32<QVideoFrame::Format_YUYV, true>((bytes) &mLastImage[0], width / 2, height, 4 * width,
                                bits.AtDumpingAdded(4 * width * height));
                            break;
                        case QVideoFrame::Format_Jpeg:
                            ToARGB32<QVideoFrame::Format_Jpeg, true>((bytes) &mLastImage[0], width, height, mLastImage.Count(),
                                bits.AtDumpingAdded(4 * width * height));
                            break;
                        }
                    }
                    else switch(mPixelFormat)
                    {
                    case QVideoFrame::Format_RGB32:
                        ToARGB32<QVideoFrame::Format_RGB32, false>((bytes) &mLastImage[0], width, height, 4 * width,
                            bits.AtDumpingAdded(4 * width * height));
                        break;
                    case QVideoFrame::Format_BGR32:
                        ToARGB32<QVideoFrame::Format_BGR32, false>((bytes) &mLastImage[0], width, height, 4 * width,
                            bits.AtDumpingAdded(4 * width * height));
                        break;
                    case QVideoFrame::Format_YUYV:
                        ToARGB32<QVideoFrame::Format_YUYV, false>((bytes) &mLastImage[0], width / 2, height, 4 * width,
                            bits.AtDumpingAdded(4 * width * height));
                        break;
                    case QVideoFrame::Format_Jpeg:
                        ToARGB32<QVideoFrame::Format_Jpeg, false>((bytes) &mLastImage[0], width, height, mLastImage.Count(),
                            bits.AtDumpingAdded(4 * width * height));
                        break;
                    }
                    //단편화방지차원: mLastImage.Clear();
                }
                Mutex::Unlock(mMutex);
            }

        private:
            QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override
            {
                Q_UNUSED(handleType);
                return QList<QVideoFrame::PixelFormat>()
                    << QVideoFrame::Format_RGB32
                    << QVideoFrame::Format_BGR32;
            }

        private slots:
            bool present(const QVideoFrame& frame) override
            {
                AddPictureShotCount();
                bool Result = false;
                if(!CaptureEnabled())
                {
                    auto FrameType = frame.handleType();
                    if(FrameType != QAbstractVideoBuffer::GLTextureHandle)
                        QVideoFrame(frame).unmap();
                }
                else if(frame.isValid())
                {
                    auto FrameType = frame.handleType();
                    if(FrameType != QAbstractVideoBuffer::GLTextureHandle)
                    {
                        QVideoFrame ClonedFrame(frame);
                        if(ClonedFrame.map(QAbstractVideoBuffer::ReadOnly))
                        {
                            Mutex::Lock(mMutex);
                            {
                                mPixelFormat = ClonedFrame.pixelFormat();
                                mNeedFlip = true;
                                mLastImage.SubtractionAll();
                                mLastImageWidth = ClonedFrame.width();
                                mLastImageHeight = ClonedFrame.height();
                                Memory::Copy(mLastImage.AtDumpingAdded(ClonedFrame.mappedBytes()),
                                    ClonedFrame.bits(), ClonedFrame.mappedBytes());
                            }
                            Mutex::Unlock(mMutex);
                            BufferFlush();
                            ClonedFrame.unmap();
                            Result = true;
                        }
                    }
                    #ifndef BOSS_SILENT_NIGHT_IS_ENABLED
                        // QVideoFrame::map함수의 버그에 따른 수동패치
                        else if(QOpenGLContext* ctx = QOpenGLContext::currentContext())
                        {
                            QOpenGLFunctions* f = ctx->functions();
                            GLuint textureId = frame.handle().toUInt();
                            GLuint fbo = 0, prevFbo = 0;
                            f->glGenFramebuffers(1, &fbo);
                            f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &prevFbo);
                            f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                            f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
                            Mutex::Lock(mMutex);
                            {
                                mPixelFormat = frame.pixelFormat();
                                mNeedFlip = false;
                                mLastImage.SubtractionAll();
                                mLastImageWidth = frame.width();
                                mLastImageHeight = frame.height();
                                f->glReadPixels(0, 0, mLastImageWidth, mLastImageHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                                    mLastImage.AtDumpingAdded(4 * mLastImageWidth * mLastImageHeight));
                            }
                            Mutex::Unlock(mMutex);
                            BufferFlush();
                            f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
                            f->glDeleteFramebuffers(1, &fbo);
                            Result = true;
                        }
                    #endif
                }
                return Result;
            }

        private:
            template<sint32 FORMAT, bool FLIP>
            void ToARGB32(bytes srcbits, const sint32 width, const sint32 height, const sint32 rowbytes, uint08* dstbits)
            {
                if(FORMAT == (sint32) QVideoFrame::Format_Jpeg)
                {
                    if(id_bitmap NewBitmap = AddOn::Jpg::ToBmp(srcbits, rowbytes))
                    {
                        bytes SrcBits = Bmp::GetBits(NewBitmap);
                        const sint32 RowBytes = 4 * width;
                        if(FLIP)
                        {
                            for(sint32 y = 0, yend = height; y < yend; ++y)
                            {
                                Bmp::bitmappixel* CurDst = (Bmp::bitmappixel*) &dstbits[RowBytes * y];
                                bytes CurSrc = &SrcBits[RowBytes * (yend - 1 - y)];
                                Memory::Copy(CurDst, CurSrc, RowBytes);
                            }
                        }
                        else Memory::Copy(dstbits, SrcBits, RowBytes * height);
                    }
                }
                else
                {
                    const sint32 SrcRowBytes = (FORMAT == (sint32)QVideoFrame::Format_YUYV)? rowbytes / 2 : rowbytes;
                    for(sint32 y = 0, yend = height; y < yend; ++y)
                    {
                        Bmp::bitmappixel* CurDst = (Bmp::bitmappixel*) &dstbits[rowbytes * y];
                        bytes CurSrc = (FLIP)? &srcbits[SrcRowBytes * (yend - 1 - y)] : &srcbits[SrcRowBytes * y];
                        for(sint32 x = 0, xend = width; x < xend; ++x)
                        {
                            if(FORMAT == (sint32) QVideoFrame::Format_RGB32)
                            {
                                CurDst->a = 0xFF;
                                CurDst->r = CurSrc[2];
                                CurDst->g = CurSrc[1];
                                CurDst->b = CurSrc[0];
                                CurDst++; CurSrc += 4;
                            }
                            if(FORMAT == (sint32) QVideoFrame::Format_BGR32)
                            {
                                CurDst->a = 0xFF;
                                CurDst->r = CurSrc[0];
                                CurDst->g = CurSrc[1];
                                CurDst->b = CurSrc[2];
                                CurDst++; CurSrc += 4;
                            }
                            if(FORMAT == (sint32) QVideoFrame::Format_YUYV)
                            {
                                const sint32 RAdd = 1.4065 * (CurSrc[3] - 128);
                                const sint32 GSub = 0.3455 * (CurSrc[1] - 128) + 0.7169 * (CurSrc[3] - 128);
                                const sint32 BAdd = 1.1790 * (CurSrc[1] - 128);
                                CurDst->a = 0xFF;
                                CurDst->r = Math::Clamp(CurSrc[0] + RAdd, 0, 255);
                                CurDst->g = Math::Clamp(CurSrc[0] - GSub, 0, 255);
                                CurDst->b = Math::Clamp(CurSrc[0] + BAdd, 0, 255);
                                CurDst++;
                                CurDst->a = 0xFF;
                                CurDst->r = Math::Clamp(CurSrc[2] + RAdd, 0, 255);
                                CurDst->g = Math::Clamp(CurSrc[2] - GSub, 0, 255);
                                CurDst->b = Math::Clamp(CurSrc[2] + BAdd, 0, 255);
                                CurDst++; CurSrc += 4;
                            }
                        }
                    }
                }
            }
        };

        #if BOSS_ANDROID
            class CameraSurfaceForAndroid
            {
            private:
                QCameraViewfinderSettings mSettings;
                QList<QCameraViewfinderSettings> mAllSettings;
                id_mutex mMutex;
                uint08s mLastImage;
                sint32 mLastImageWidth;
                sint32 mLastImageHeight;
                id_texture mCamTexture;

            public:
                CameraSurfaceForAndroid(const QCameraInfo& info)
                {
                    JNIEnv* env = GetAndroidJNIEnv();
                    // 카메라 정보수집
                    QAndroidJniObject Collector = QAndroidJniObject::callStaticObjectMethod("com/boss2d/BossCameraManager", "info", "()Ljava/lang/String;");
                    String Result = Collector.toString().toUtf8().constData();
                    chars ResultPtr = Result;

                    // 정보입력
                    QList<QSize> SupportedResolutions;
                    for(sint32 i = 0; (i = Result.Find(i, "Size_")) != -1;)
                    {
                        QSize NewResolution;
                        i += 5; // Size_
                        NewResolution.setWidth(Parser::GetInt(ResultPtr, -1, &i));
                        i += 1; // x
                        NewResolution.setHeight(Parser::GetInt(ResultPtr, -1, &i));
                        i += 1; // ;
                        SupportedResolutions.append(NewResolution);
                    }
                    foreach(const auto& CurResolution, SupportedResolutions)
                    {
                        QCameraViewfinderSettings NewSettings = mSettings;
                        NewSettings.setResolution(CurResolution);
                        mAllSettings.append(NewSettings);
                    }
                    // "Format_JPEG;" -> Format_Jpeg
                    // "Format_NV21;" -> Format_NV21
                    // "Format_RGB_565;" -> Format_RGB565
                    // "Format_YUV_420_888;" -> Format_YUV420P
                    // "Format_YUV_444_888;" -> Format_YUV444
                    // "Format_YUY2;" -> Format_YUYV
                    // "Format_YV12;" -> Format_YV12

                    mMutex = Mutex::Open();
                    mLastImageWidth = 0;
                    mLastImageHeight = 0;
                    mCamTexture = nullptr;
                    // 콜백함수 연결
                    JNINativeMethod methods[] {
                        {"OnPictureTaken", "([BIII)V", reinterpret_cast<void*>(OnPictureTaken)},
                        {"OnPreviewTaken", "([BIII)V", reinterpret_cast<void*>(OnPreviewTaken)}};
                    jclass BossCameraManagerClass = env->FindClass("com/boss2d/BossCameraManager");
                    env->RegisterNatives(BossCameraManagerClass, methods, sizeof(methods) / sizeof(methods[0]));
                    env->DeleteLocalRef(BossCameraManagerClass);
                    SavedMe() = this;
                }
                ~CameraSurfaceForAndroid()
                {
                    Platform::Graphics::RemoveTexture(mCamTexture);
                    StopCamera();
                    SavedMe() = nullptr;
                    Mutex::Close(mMutex);
                }

            public:
                void StartCamera()
                {
                    mCamTexture = Platform::Graphics::CreateTexture(false, false, mSettings.resolution().width(), mSettings.resolution().height());
                    BOSS_TRACE("StartCamera: GenTexture - LastTexture: %d, width: %d, height: %d",
                        Platform::Graphics::GetTextureID(mCamTexture), mSettings.resolution().width(), mSettings.resolution().height());
                    QAndroidJniObject::callStaticMethod<void>("com/boss2d/BossCameraManager", "init", "(III)V",
                        Platform::Graphics::GetTextureID(mCamTexture), mSettings.resolution().width(), mSettings.resolution().height());
                }
                void StopCamera()
                {
                    QAndroidJniObject::callStaticMethod<void>("com/boss2d/BossCameraManager", "quit", "()V");
                    Platform::Graphics::RemoveTexture(mCamTexture);
                    mCamTexture = nullptr;
                    BOSS_TRACE("StopCamera: DeleteTexture done");
                }
                const QCameraViewfinderSettings GetSettings() {return mSettings;}
                void SetSettings(const QCameraViewfinderSettings& settings) {mSettings = settings;}
                const QList<QCameraViewfinderSettings> GetSupportedAllSettings() {return mAllSettings;}

            protected:
                void StartPreview(bool preview) {QAndroidJniObject::callStaticMethod<void>("com/boss2d/BossCameraManager", "play", "(I)V", (preview)? 1 : 2);}
                void StopPreview() {QAndroidJniObject::callStaticMethod<void>("com/boss2d/BossCameraManager", "stop", "()V");}
                sint32 GetLastImageWidth() const {return mLastImageWidth;}
                sint32 GetLastImageHeight() const {return mLastImageHeight;}
                id_texture CreateLastTexture(bool bitmapcache)
                {
                    if(0 < mLastImageWidth && 0 < mLastImageHeight)
                    {
                        switch(*((uint32*) &mLastImage[0]))
                        {
                        case codeid("NV21"):
                            if(12 < mLastImage.Count())
                                return Platform::Graphics::CreateTexture(true, bitmapcache, mLastImageWidth, mLastImageHeight, &mLastImage[12]);
                            break;
                        }
                    }
                    return nullptr;
                }

            protected:
                virtual bool CaptureEnabled() {return false;}
                virtual void BufferFlush() {}
                virtual void AddPictureShotCount() {}
                virtual void AddPreviewShotCount() {}

            protected:
                void DecodeImage(sint32& width, sint32& height, uint08s& bits)
                {
                    BOSS_TRACE("DecodeImage - Begin(%d)", mLastImage.Count());
                    Mutex::Lock(mMutex);
                    if(0 < mLastImage.Count())
                    {
                        switch(*((uint32*) &mLastImage[0]))
                        {
                        case codeid("NV21"):
                            {
                                BOSS_TRACE("DecodeImage(RAW8) - Mid-A (%llu)", Platform::Utility::CurrentTimeMsec());
                                width = *((sint32*) &mLastImage[4]);
                                height = *((sint32*) &mLastImage[8]);
                                bits.SubtractionAll();
                                Bmp::bitmappixel* Dst = (Bmp::bitmappixel*)
                                    bits.AtDumpingAdded(sizeof(Bmp::bitmappixel) * width * height);
                                auto SrcY = (const uint08*) &mLastImage[12];
                                auto SrcUVBegin = (const uint08*) &mLastImage[12 + width * height];
                                const uint08* SrcUVPair[2] = {SrcUVBegin, SrcUVBegin};
                                for(sint32 y = 0; y < height; ++y)
                                {
                                    const uint08*& SrcUV = SrcUVPair[y & 1];
                                    for(sint32 x = 0; x < width; x += 2)
                                    {
                                        const sint32 Y1 = *(SrcY++) & 0xFF;
                                        const sint32 Y2 = *(SrcY++) & 0xFF;
                                        const sint32 V = (*(SrcUV++) & 0xFF) - 128;
                                        const sint32 U = (*(SrcUV++) & 0xFF) - 128;
                                        const sint32 Y1Value = (sint32)(1.164f * (Y1 - 16));
                                        const sint32 Y2Value = (sint32)(1.164f * (Y2 - 16));
                                        const sint32 RValue = (sint32)(1.596f * V);
                                        const sint32 GValue = (sint32)(0.391f * U + 0.813f * V);
                                        const sint32 BValue = (sint32)(2.018f * U);
                                        Dst->a = 0xFF;
                                        Dst->r = (uint08) Math::Max(0, Math::Min(Y1Value + RValue, 255));
                                        Dst->g = (uint08) Math::Max(0, Math::Min(Y1Value - GValue, 255));
                                        Dst->b = (uint08) Math::Max(0, Math::Min(Y1Value + BValue, 255));
                                        Dst++;
                                        Dst->a = 0xFF;
                                        Dst->r = (uint08) Math::Max(0, Math::Min(Y2Value + RValue, 255));
                                        Dst->g = (uint08) Math::Max(0, Math::Min(Y2Value - GValue, 255));
                                        Dst->b = (uint08) Math::Max(0, Math::Min(Y2Value + BValue, 255));
                                        Dst++;
                                    }
                                }
                                BOSS_TRACE("DecodeImage(RAW8) - Mid-B (%llu)", Platform::Utility::CurrentTimeMsec());
                            }
                            break;
                        case codeid("JPEG"):
                            BOSS_TRACE("DecodeImage(JPEG)");
                            if(id_bitmap NewBitmap = AddOn::Jpg::ToBmp(&mLastImage[4], mLastImage.Count() - 4))
                            {
                                BOSS_TRACE("DecodeImage(JPEG) - Mid-A");
                                width = Bmp::GetWidth(NewBitmap);
                                height = Bmp::GetHeight(NewBitmap);
                                bits.SubtractionAll();
                                Bmp::bitmappixel* Dst = (Bmp::bitmappixel*)
                                    bits.AtDumpingAdded(sizeof(Bmp::bitmappixel) * width * height);
                                auto Src = (const Bmp::bitmappixel*) Bmp::GetBits(NewBitmap);
                                for(sint32 y = 0; y < height; ++y)
                                    Memory::Copy(&Dst[width * y], &Src[width * (height - 1 - y)],
                                        sizeof(Bmp::bitmappixel) * width);
                                Bmp::Remove(NewBitmap);
                                BOSS_TRACE("DecodeImage(JPEG) - Mid-B");
                            }
                            break;
                        }
                        //단편화방지차원: mLastImage.Clear();
                    }
                    else
                    {
                        width = 1;
                        height = 1;
                        bits.SubtractionAll();
                        Bmp::bitmappixel* Dst = (Bmp::bitmappixel*)
                            bits.AtDumpingAdded(sizeof(Bmp::bitmappixel) * width * height);
                        Dst->a = 0xFF;
                        Dst->r = 0xFF;
                        Dst->g = 0x00;
                        Dst->b = 0x00;
                    }
                    Mutex::Unlock(mMutex);
                    BOSS_TRACE("DecodeImage - End(%d)", bits.Count());
                }

            private:
                static CameraSurfaceForAndroid*& SavedMe()
                {
                    static CameraSurfaceForAndroid* _ = nullptr;
                    return _;
                }
                static void OnPictureTaken(JNIEnv* env, jobject thiz, jbyteArray data, jint length, jint width, jint height)
                {
                    BOSS_TRACE("OnPictureTaken Begin(%llu)", Platform::Utility::CurrentTimeMsec());
                    bytes paramData = (bytes) env->GetByteArrayElements(data, nullptr);
                    CameraSurfaceForAndroid* Me = SavedMe();
                    BOSS_TRACE("OnPictureTaken - width: %d, height: %d, paramData: %08X, length: %d, Me: %08X",
                        width, height, paramData, length, Me);
                    if(Me)
                    {
                        Me->AddPictureShotCount();
                        if(Me->CaptureEnabled())
                        if(paramData && 0 < length)
                        {
                            if(16 <= length)
                            BOSS_TRACE("OnPictureTaken - %08X %08X %08X %08X... (%d)",
                                ((sint32*) paramData)[0], ((sint32*) paramData)[1],
                                ((sint32*) paramData)[2], ((sint32*) paramData)[3], length);
                            Mutex::Lock(Me->mMutex);
                            {
                                Me->mLastImage.SubtractionAll();
                                Me->mLastImageWidth = width;
                                Me->mLastImageHeight = height;
                                const uint32 TypeCode = codeid("JPEG");
                                Memory::Copy(Me->mLastImage.AtDumpingAdded(4), &TypeCode, 4);
                                Memory::Copy(Me->mLastImage.AtDumpingAdded(length), paramData, length);
                            }
                            Mutex::Unlock(Me->mMutex);
                            Me->BufferFlush();
                        }
                    }
                    env->ReleaseByteArrayElements(data, (jbyte*) paramData, JNI_ABORT);
                    BOSS_TRACE("OnPictureTaken End(%llu)", Platform::Utility::CurrentTimeMsec());
                }
                static void OnPreviewTaken(JNIEnv* env, jobject thiz, jbyteArray data, jint length, jint width, jint height)
                {
                    BOSS_TRACE("OnPreviewTaken Begin(%llu)", Platform::Utility::CurrentTimeMsec());
                    bytes paramData = (bytes) env->GetByteArrayElements(data, nullptr);
                    CameraSurfaceForAndroid* Me = SavedMe();
                    BOSS_TRACE("OnPreviewTaken - width: %d, height: %d, paramData: %08X, length: %d, Me: %08X",
                        width, height, paramData, length, Me);
                    if(Me)
                    {
                        Me->AddPreviewShotCount();
                        if(Me->CaptureEnabled())
                        if(paramData && 0 < length)
                        {
                            if(16 <= length)
                            BOSS_TRACE("OnPreviewTaken - %08X %08X %08X %08X... (%d)",
                                ((sint32*) paramData)[0], ((sint32*) paramData)[1],
                                ((sint32*) paramData)[2], ((sint32*) paramData)[3], length);
                            Mutex::Lock(Me->mMutex);
                            {
                                Me->mLastImage.SubtractionAll();
                                Me->mLastImageWidth = width;
                                Me->mLastImageHeight = height;
                                const uint32 TypeCode = codeid("NV21");
                                Memory::Copy(Me->mLastImage.AtDumpingAdded(4), &TypeCode, 4);
                                Memory::Copy(Me->mLastImage.AtDumpingAdded(4), &width, 4);
                                Memory::Copy(Me->mLastImage.AtDumpingAdded(4), &height, 4);
                                Memory::Copy(Me->mLastImage.AtDumpingAdded(length), paramData, length);
                            }
                            Mutex::Unlock(Me->mMutex);
                            Me->BufferFlush();
                        }
                    }
                    env->ReleaseByteArrayElements(data, (jbyte*) paramData, JNI_ABORT);
                    BOSS_TRACE("OnPreviewTaken End(%llu)", Platform::Utility::CurrentTimeMsec());
                }
            };
            typedef CameraSurfaceForAndroid CameraSurfaceClass;
        #else
            typedef CameraSurface CameraSurfaceClass;
        #endif

        class CameraService : public CameraSurfaceClass
        {
        private:
            enum CaptureOrder {CaptureOrder_None, CaptureOrder_ShotOnly, CaptureOrder_ShotAndStop, CaptureOrder_NeedStop};
            id_mutex mMutex;
            bool mStartPreview;
            CaptureOrder mCaptureOrder;
            sint32 mDecodedWidth;
            sint32 mDecodedHeight;
            uint08s mDecodedBits;
            bool mUpdateForImage;
            bool mUpdateForBitmap;
            uint64 mUpdateTimeMsec;
            float mUpdateAvgMsec;
            sint32 mPictureShotCount;
            sint32 mPreviewShotCount;

        public:
            CameraService(const QCameraInfo& info) : CameraSurfaceClass(info)
            {
                mMutex = Mutex::Open();
                mStartPreview = false;
                mCaptureOrder = CaptureOrder_None;
                mDecodedWidth = 0;
                mDecodedHeight = 0;
                mUpdateForImage = false;
                mUpdateForBitmap = false;
                mUpdateTimeMsec = Platform::Utility::CurrentTimeMsec();
                mUpdateAvgMsec = 0;
                mPictureShotCount = 0;
                mPreviewShotCount = 0;
            }
            ~CameraService()
            {
                Mutex::Close(mMutex);
            }

        private:
            virtual bool CaptureEnabled() override
            {
                bool Result = false;
                Mutex::Lock(mMutex);
                {
                    Result = (mCaptureOrder == CaptureOrder_ShotOnly
                        || mCaptureOrder == CaptureOrder_ShotAndStop);
                }
                Mutex::Unlock(mMutex);
                return Result;
            }
            virtual void BufferFlush() override
            {
                Mutex::Lock(mMutex);
                {
                    if(mCaptureOrder == CaptureOrder_ShotOnly)
                        mCaptureOrder = CaptureOrder_None;
                    else if(mCaptureOrder == CaptureOrder_ShotAndStop)
                        mCaptureOrder = CaptureOrder_NeedStop;
                    mUpdateForImage = true;
                    mUpdateForBitmap = true;
                }
                Mutex::Unlock(mMutex);
            }
            virtual void AddPictureShotCount() override
            {
                mPictureShotCount++;
                const uint64 NewTimeMsec = Platform::Utility::CurrentTimeMsec();
                mUpdateAvgMsec = (mUpdateAvgMsec * 19 + Math::Max(1, NewTimeMsec - mUpdateTimeMsec)) / 20;
                mUpdateTimeMsec = NewTimeMsec;
            }
            virtual void AddPreviewShotCount() override
            {
                mPreviewShotCount++;
                const uint64 NewTimeMsec = Platform::Utility::CurrentTimeMsec();
                mUpdateAvgMsec = (mUpdateAvgMsec * 19 + Math::Max(1, NewTimeMsec - mUpdateTimeMsec)) / 20;
                mUpdateTimeMsec = NewTimeMsec;
            }

        public:
            void Capture(bool preview, bool needstop)
            {
                Mutex::Lock(mMutex);
                {
                    mCaptureOrder = (needstop)? CaptureOrder_ShotAndStop : CaptureOrder_ShotOnly;
                    mStartPreview = true;
                    StartPreview(preview);
                }
                Mutex::Unlock(mMutex);
            }
            id_texture CloneCapturedTexture(bool bitmapcache)
            {
                id_texture Result = nullptr;
                Mutex::Lock(mMutex);
                {
                    Result = CreateLastTexture(bitmapcache);
                }
                Mutex::Unlock(mMutex);
                return Result;
            }
            void GetCapturedImage(QPixmap& pixmap, sint32 maxwidth, sint32 maxheight, sint32 rotate)
            {
                Mutex::Lock(mMutex);
                {
                    if(mCaptureOrder == CaptureOrder_NeedStop)
                    {
                        mCaptureOrder = CaptureOrder_None;
                        if(mStartPreview)
                        {
                            mStartPreview = false;
                            StopPreview();
                        }
                    }
                    if(mUpdateForImage)
                    {
                        mUpdateForImage = false;
                        if(mUpdateForBitmap) DecodeImage(mDecodedWidth, mDecodedHeight, mDecodedBits);
                        QImage NewImage(&mDecodedBits[0], mDecodedWidth, mDecodedHeight, QImage::Format_ARGB32);

                        if(maxwidth == -1 && maxheight == -1)
                        {maxwidth = mDecodedWidth; maxheight = mDecodedHeight;}
                        else if(maxwidth == -1) maxwidth = mDecodedWidth * maxheight / mDecodedHeight;
                        else if(maxheight == -1) maxheight = mDecodedHeight * maxwidth / mDecodedWidth;

                        // 스케일링
                        if(maxwidth < mDecodedWidth || maxheight < mDecodedHeight)
                            NewImage = NewImage.scaled(Math::Min(maxwidth, mDecodedWidth), Math::Min(maxheight, mDecodedHeight));

                        // 회전
                        if(0 < rotate)
                        {
                            QMatrix NewMatrix;
                            NewMatrix.translate(mDecodedWidth / 2, mDecodedHeight / 2);
                            NewMatrix.rotate(rotate);
                            NewImage = NewImage.transformed(NewMatrix);
                        }
                        pixmap.convertFromImage(NewImage);
                        //단편화방지차원: if(!mUpdateForBitmap) mDecodedBits.Clear();
                    }
                }
                Mutex::Unlock(mMutex);
            }
            void GetCapturedBitmap(id_bitmap& bitmap, orientationtype ori)
            {
                Mutex::Lock(mMutex);
                {
                    if(mCaptureOrder == CaptureOrder_NeedStop)
                    {
                        mCaptureOrder = CaptureOrder_None;
                        if(mStartPreview)
                        {
                            mStartPreview = false;
                            StopPreview();
                        }
                    }
                    if(mUpdateForBitmap)
                    {
                        mUpdateForBitmap = false;
                        if(mUpdateForImage) DecodeImage(mDecodedWidth, mDecodedHeight, mDecodedBits);
                        bitmap = Bmp::CloneFromBits(&mDecodedBits[0], mDecodedWidth, mDecodedHeight, 32, ori, bitmap);
                        //단편화방지차원: if(!mUpdateForImage) mDecodedBits.Clear();
                    }
                }
                Mutex::Unlock(mMutex);
            }
            size64 GetCapturedSize() const
            {
                size64 Result = {0, 0};
                Mutex::Lock(mMutex);
                {
                    Result.w = GetLastImageWidth();
                    Result.h = GetLastImageHeight();
                }
                Mutex::Unlock(mMutex);
                return Result;
            }
            uint64 GetCapturedTimeMsec(sint32* avgmsec) const
            {
                uint64 Result = 0;
                Mutex::Lock(mMutex);
                {
                    Result = mUpdateTimeMsec;
                    if(avgmsec) *avgmsec = Math::Max(1, (sint32) mUpdateAvgMsec);
                }
                Mutex::Unlock(mMutex);
                return Result;
            }
            sint32 GetPictureShotCount() const
            {
                return mPictureShotCount;
            }
            sint32 GetPreviewShotCount() const
            {
                return mPreviewShotCount;
            }
        };

        class CameraClass
        {
        private:
            sint32 mRefCount;
            QCameraInfo mCameraInfo;
            CameraService* mCameraService;
            QPixmap mLastPixmap;
            id_bitmap mLastBitmap;

        public:
            static Strings GetList(String* spec)
            {
                Strings Result;
                Context SpecCollector;
                const QList<QCameraInfo>& AllCameras = QCameraInfo::availableCameras();
                foreach(const auto& CurCamera, AllCameras)
                {
                    String CurName = CurCamera.description().toUtf8().constData();
                    if(CurName.Length() == 0)
                        CurName = CurCamera.deviceName().toUtf8().constData();
                    Result.AtAdding() = CurName;
                    if(spec)
                    {
                        Context& NewChild = SpecCollector.At(SpecCollector.LengthOfIndexable());
                        NewChild.At("description").Set(CurCamera.description().toUtf8().constData());
                        NewChild.At("devicename").Set(CurCamera.deviceName().toUtf8().constData());
                        NewChild.At("position").Set(String::FromInteger(CurCamera.position()));
                        NewChild.At("orientation").Set(String::FromInteger(CurCamera.orientation()));
                    }
                }
                if(spec)
                    *spec = SpecCollector.SaveJson(*spec);
                return Result;
            }

        public:
            CameraClass(chars name, sint32 width, sint32 height)
            {
                mRefCount = 1;
                mCameraService = nullptr;
                mLastBitmap = nullptr;
                const QList<QCameraInfo>& AllCameraInfos = QCameraInfo::availableCameras();
                foreach(const auto& CurCameraInfo, AllCameraInfos)
                {
                    if(*name == '\0' || CurCameraInfo.description() == name || CurCameraInfo.deviceName() == name)
                    {
                        mCameraInfo = CurCameraInfo;
                        mCameraService = new CameraService(mCameraInfo);

                        sint32 BestValue = 0, SavedWidth = -1, SavedHeight = -1;
                        double SavedMinFR = -1, SavedMaxFR = -1;
                        auto AllSettings = mCameraService->GetSupportedAllSettings();
                        BOSS_TRACE("Created Camera: %s(%s) [Included %d settings]",
                            CurCameraInfo.deviceName().toUtf8().constData(),
                            CurCameraInfo.description().toUtf8().constData(), AllSettings.size());
                        BOSS_TRACE(" - Setting Count: %d", AllSettings.size());
                        foreach(const auto& CurSetting, AllSettings)
                        {
                            const sint32 CurWidth = CurSetting.resolution().width();
                            const sint32 CurHeight = CurSetting.resolution().height();
                            const double CurMinFR = CurSetting.minimumFrameRate();
                            const double CurMaxFR = CurSetting.maximumFrameRate();
                            BOSS_TRACE(" - 1.Supported Resolution: %dx%d", CurWidth, CurHeight);
                            BOSS_TRACE(" - 2.Supported PixelFormat: %d", (sint32) CurSetting.pixelFormat());
                            BOSS_TRACE(" - 3.Supported FrameRate: %f~%f", (float) CurMinFR, (float) CurMaxFR);
                            bool IsFinded = false;
                            if(width == -1 && height == -1)
                            {
                                if(BestValue <= CurWidth * CurHeight)
                                {
                                    BestValue = CurWidth * CurHeight;
                                    IsFinded = true;
                                }
                            }
                            else if(width == -1)
                            {
                                if(height == CurHeight && BestValue <= CurWidth)
                                {
                                    BestValue = CurWidth;
                                    IsFinded = true;
                                }
                            }
                            else if(height == -1)
                            {
                                if(width == CurWidth && BestValue <= CurHeight)
                                {
                                    BestValue = CurHeight;
                                    IsFinded = true;
                                }
                            }
                            else if(width == CurWidth && height == CurHeight)
                                IsFinded = true;
                            if(IsFinded)
                            {
                                SavedWidth = CurWidth;
                                SavedHeight = CurHeight;
                                SavedMinFR = CurMinFR;
                                SavedMaxFR = CurMaxFR;
                            }
                        }

                        QCameraViewfinderSettings NewSettings = mCameraService->GetSettings();
                        if(SavedWidth != -1 && SavedHeight != -1)
                            NewSettings.setResolution(SavedWidth, SavedHeight);
                        else NewSettings.setResolution(640, 480);
                        if(SavedMinFR != -1) NewSettings.setMinimumFrameRate(SavedMinFR);
                        if(SavedMaxFR != -1) NewSettings.setMaximumFrameRate(SavedMaxFR);
                        mCameraService->SetSettings(NewSettings);
                        mCameraService->StartCamera();
                        break;
                    }
                }
            }
            ~CameraClass()
            {
                delete mCameraService;
                Bmp::Remove(mLastBitmap);
            }

        public:
            CameraClass* AddRef()
            {
                mRefCount++;
                return this;
            }
            bool SubRef()
            {
                return (--mRefCount == 0);
            }
            bool IsValid() const
            {
                return (mCameraService != nullptr);
            }
            void Capture(bool preview, bool needstop)
            {
                if(mCameraService)
                    mCameraService->Capture(preview, needstop);
            }
            id_texture CloneCapturedTexture(bool bitmapcache)
            {
                if(mCameraService)
                    return mCameraService->CloneCapturedTexture(bitmapcache);
                return nullptr;
            }
            id_image_read LastCapturedImage(sint32 maxwidth, sint32 maxheight, sint32 rotate)
            {
                if(mCameraService)
                {
                    mCameraService->GetCapturedImage(mLastPixmap, maxwidth, maxheight, rotate);
                    return (id_image_read) &mLastPixmap;
                }
                return nullptr;
            }
            id_bitmap_read LastCapturedBitmap(orientationtype ori)
            {
                if(mCameraService)
                {
                    mCameraService->GetCapturedBitmap(mLastBitmap, ori);
                    return mLastBitmap;
                }
                return nullptr;
            }
            size64 LastCapturedSize() const
            {
                if(mCameraService)
                    return mCameraService->GetCapturedSize();
                return {0, 0};
            }
            uint64 LastCapturedTimeMsec(sint32* avgmsec) const
            {
                if(mCameraService)
                    return mCameraService->GetCapturedTimeMsec(avgmsec);
                return 0;
            }
            sint32 TotalPictureShotCount() const
            {
                if(mCameraService)
                    return mCameraService->GetPictureShotCount();
                return 0;
            }
            sint32 TotalPreviewShotCount() const
            {
                if(mCameraService)
                    return mCameraService->GetPreviewShotCount();
                return 0;
            }
        };
    #else
        class CameraSurface : public QObject {Q_OBJECT};
    #endif

    #ifdef QT_HAVE_MULTIMEDIA
        class MicrophoneClass : public QObject
        {
            Q_OBJECT

        private:
            class Data
            {
            public:
                Data() {mChannel = 1;}
                ~Data() {}
            public:
                Data(const Data& rhs) {operator=(rhs);}
                Data& operator=(const Data& rhs)
                {
                    mChannel = rhs.mChannel;
                    mPulseCodes = rhs.mPulseCodes;
                    mTimeMsec = rhs.mTimeMsec;
                    return *this;
                }
                operator void*() const {return nullptr;}
                bool operator!() const {return (mPulseCodes.Count() == 0);}
            public:
                sint32 mChannel;
                floats mPulseCodes; // [Channel * Code]
                uint64 mTimeMsec;
            };

        private:
            QAudioRecorder* mRecorder;
            QAudioProbe* mProbe;
            QAudioEncoderSettings mAudioSettings;
            const sint32 mMaxQueueCount;
            Queue<Data> mDataQueue;
            Data mLastData;

        public:
            static Strings GetList(String* spec)
            {
                Strings Result;
                auto OneRecorder = new QAudioRecorder();
                foreach(QString Input, OneRecorder->audioInputs())
                    Result.AtAdding() = Input.toUtf8().constData();

                if(spec)
                {
                    Context SpecCollector;
                    foreach(QString Input, OneRecorder->audioInputs())
                        SpecCollector.At("input").AtAdding().Set(Input.toUtf8().constData());
                    foreach(QString AudioCodec, OneRecorder->supportedAudioCodecs())
                        SpecCollector.At("codec").AtAdding().Set(AudioCodec.toUtf8().constData());
                    foreach(QString Container, OneRecorder->supportedContainers())
                        SpecCollector.At("container").AtAdding().Set(Container.toUtf8().constData());
                    foreach(int SampleRate, OneRecorder->supportedAudioSampleRates())
                        SpecCollector.At("samplerate").AtAdding().Set(String::FromInteger(SampleRate));
                    *spec = SpecCollector.SaveJson(*spec);
                }
                delete OneRecorder;
                return Result;
            }

        public:
            MicrophoneClass(chars name, sint32 samplerate, sint32 channelcount, sint32 maxqueue) : mMaxQueueCount(maxqueue)
            {
                mRecorder = new QAudioRecorder();
                mProbe = new QAudioProbe();
                connect(mProbe, &QAudioProbe::audioBufferProbed, this, &MicrophoneClass::processBuffer);
                mProbe->setSource(mRecorder);

                QString SelectedInput = "";
                if(*name != '\0')
                {
                    QStringList Inputs = mRecorder->audioInputs();
                    foreach(QString Input, Inputs)
                        if(!String::Compare(name, Input.toUtf8().constData()))
                            SelectedInput = Input;
                }
                else SelectedInput = mRecorder->defaultAudioInput();
                if(SelectedInput.length() == 0)
                {
                    delete mProbe;
                    mProbe = nullptr;
                    delete mRecorder;
                    mRecorder = nullptr;
                    return;
                }

                mRecorder->setAudioInput(SelectedInput);
                mAudioSettings.setCodec("audio/x-raw");
                mAudioSettings.setSampleRate(samplerate);
                mAudioSettings.setBitRate(128000);
                mAudioSettings.setChannelCount(channelcount);
                mAudioSettings.setQuality(QMultimedia::HighQuality);
                mAudioSettings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
                mRecorder->setEncodingSettings(mAudioSettings);
                mRecorder->record();
            }
            ~MicrophoneClass()
            {
                delete mProbe;
                delete mRecorder;
                while(0 < mDataQueue.Count())
                    mDataQueue.Dequeue();
            }

        public:
            bool IsValid()
            {
                return (mRecorder != nullptr);
            }
            bool TryLastData()
            {
                if(0 < mDataQueue.Count())
                {
                    mLastData = mDataQueue.Dequeue();
                    return true;
                }
                return false;
            }
            const floats& GetLastData(sint32* channel, uint64* timems) const
            {
                if(channel) *channel = mLastData.mChannel;
                if(timems) *timems = mLastData.mTimeMsec;
                return mLastData.mPulseCodes;
            }
            const QAudioEncoderSettings& GetAudioSettings() const
            {return mAudioSettings;}

        private:
            template<bool SIGNED = true, typename TYPE>
            void copyBuffer(Data& dst, const TYPE* src, int frames, int channels, double peak)
            {
                dst.mChannel = channels;
                auto Dst = dst.mPulseCodes.AtDumping(0, frames * channels);
                double PeakHalf = peak / 2;
                for(sint32 i = 0; i < frames; ++i)
                for(sint32 j = 0; j < channels; ++j)
                {
                    if(SIGNED)
                        Dst[i * channels + j] = src[i * channels + j] / peak;
                    else Dst[i * channels + j] = (src[i * channels + j] - PeakHalf) / PeakHalf;
                }
            }

        private slots:
            void processBuffer(const QAudioBuffer& buffer)
            {
                // 한계처리
                while(mMaxQueueCount < mDataQueue.Count())
                    mDataQueue.Dequeue();

                // 데이터적재
                if(buffer.isValid())
                if(buffer.format().codec() == "audio/pcm")
                {
                    Data NewData;
                    switch(buffer.format().sampleType())
                    {
                    case QAudioFormat::Unknown:
                    case QAudioFormat::UnSignedInt:
                        if(buffer.format().sampleSize() == 32)
                            copyBuffer<false>(NewData, buffer.constData<quint32>(), buffer.frameCount(), buffer.format().channelCount(), 0xFFFFFFFF);
                        else if(buffer.format().sampleSize() == 16)
                            copyBuffer<false>(NewData, buffer.constData<quint16>(), buffer.frameCount(), buffer.format().channelCount(), 0xFFFF);
                        else if(buffer.format().sampleSize() == 8)
                            copyBuffer<false>(NewData, buffer.constData<quint8>(), buffer.frameCount(), buffer.format().channelCount(), 0xFF);
                        break;
                    case QAudioFormat::Float:
                        if(buffer.format().sampleSize() == 32)
                            copyBuffer(NewData, buffer.constData<float>(), buffer.frameCount(), buffer.format().channelCount(), 1.00003); // 1.00003은 QT예제
                        break;
                    case QAudioFormat::SignedInt:
                        if(buffer.format().sampleSize() == 32)
                            copyBuffer(NewData, buffer.constData<qint32>(), buffer.frameCount(), buffer.format().channelCount(), 0x7FFFFFFF);
                        else if(buffer.format().sampleSize() == 16)
                            copyBuffer(NewData, buffer.constData<qint16>(), buffer.frameCount(), buffer.format().channelCount(), 0x7FFF);
                        else if(buffer.format().sampleSize() == 8)
                            copyBuffer(NewData, buffer.constData<qint8>(), buffer.frameCount(), buffer.format().channelCount(), 0x7F);
                        break;
                    }
                    NewData.mTimeMsec = Platform::Utility::CurrentTimeMsec();
                    mDataQueue.Enqueue(NewData);
                }
            }
        };
    #else
        class MicrophoneClass : public QObject {Q_OBJECT};
    #endif

#elif defined(_MSC_VER) && defined(QT_DLL)

    // 플랫폼매크로를 변경하였을때 아래 코드가 없으면
    // Qt는 moc를 다시 빌드하지 않아 링크에러를 유발.
    #include <QtWidgets>
    #include <QMainWindow>
    #include <QSystemTrayIcon>
    #include <QHostInfo>
    #include <QTcpSocket>
    #include <QUdpSocket>
    #include <QTcpServer>
    #include <QGLWidget>
    #include <QGLFunctions>
    #include <QGLShaderProgram>
    #include <QAbstractVideoSurface>
    #include <QAudioProbe>
    #include <QWebEngineView>

    class ViewAPI : public QObject {Q_OBJECT};
    class GenericView : public QFrame {Q_OBJECT};
    class GenericViewGL : public QGLWidget {Q_OBJECT};
    class MainViewGL : public QGLWidget {Q_OBJECT};
    class MainViewMDI : public QMdiArea {Q_OBJECT};
    class TrayIcon : public QSystemTrayIcon {Q_OBJECT};
    class MainWebPage : public WebEnginePageClass {Q_OBJECT};
    class MainWindow : public QMainWindow {Q_OBJECT};
    class EditTracker : public QLineEdit {Q_OBJECT};
    class ListTracker : public QListWidget {Q_OBJECT};
    class ThreadClass : public QThread {Q_OBJECT};
    class ServerClass : public QObject {Q_OBJECT};
    class TCPServerClass : public ServerClass {Q_OBJECT};
    class WSServerClass : public ServerClass {Q_OBJECT};
    class PipeClass : public QObject {Q_OBJECT};
    class PipeServerClass : public PipeClass {Q_OBJECT};
    class PipeClientClass : public PipeClass {Q_OBJECT};
    class WebEngineViewForExtraDesktop : public QObject {Q_OBJECT};
    class WebViewClass : public WebEngineViewClass {Q_OBJECT};
    class PurchaseClass : public QInAppStore {Q_OBJECT};
    class BluetoothSearchClass : public QObject {Q_OBJECT};
    class BluetoothClass : public QObject {Q_OBJECT};
    class BluetoothServerClass : public BluetoothClass {Q_OBJECT};
    class BluetoothLEServerClass : public BluetoothClass {Q_OBJECT};
    class BluetoothClientClass : public BluetoothClass {Q_OBJECT};
    class BluetoothLEClientClass : public BluetoothClass {Q_OBJECT};
    class SerialClass : public SerialPortClass {Q_OBJECT};
    class CameraSurface : public QAbstractVideoSurface {Q_OBJECT};
    class MicrophoneClass : public QAudioProbe {Q_OBJECT};

#endif
