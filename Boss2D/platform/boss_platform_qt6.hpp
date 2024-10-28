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
    #endif

    class MainView : public QWidget
    {
        Q_OBJECT

    public:
        MainView(QWidget* parent) : QWidget(parent)
        {
        }
        ~MainView()
        {
        }
    };

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow()
        {
            connect(&mMsecTimer, &QTimer::timeout, this, &MainWindow::MsecTimeout);
            mMsecTimer.start(1);
        }
        ~MainWindow()
        {
        }

    public:
        void initForWidget(bool frameless, bool topmost)
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

    private:
        void MsecTimeout()
        {
            const uint64 LimitMsec = Platform::Utility::CurrentTimeMsec() + 20;
            do {PlatformImpl::Core::FlushProcedure();}
            while(PlatformImpl::Core::CallProcedures(LimitMsec - Platform::Utility::CurrentTimeMsec()));
        }

    private:
        QTimer mMsecTimer;
        MainView* mView {nullptr};
    };

#endif
