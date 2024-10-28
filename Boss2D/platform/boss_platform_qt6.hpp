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
        #include <QCloseEvent>
        #include <QResizeEvent>
        #include <QPaintEvent>
        #include <QMouseEvent>
        #include <QWheelEvent>
        #include <QKeyEvent>
    #endif

    #define USER_FRAMECOUNT (60)

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
            //if(CanvasClass::enabled()) return;
            //CanvasClass Canvas(getWidgetForPaint());
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

#endif
