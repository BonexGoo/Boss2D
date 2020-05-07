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
        #include <QGLWidget>
        #include <QGLFunctions>
        #include <QGLShaderProgram>
        #include <QGLFramebufferObject>
    #endif

    #include <QBuffer>
    #include <QHostInfo>
    #include <QTcpSocket>
    #include <QUdpSocket>
    #include <QTcpServer>
    #include <QNetworkInterface>

    #ifdef QT_HAVE_PURCHASING
        #include <QtPurchasing>
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

    #if BOSS_IPHONE
        #include <qpa/qplatformnativeinterface.h>
    #endif

    #if BOSS_ANDROID
        #include <QtAndroidExtras/QAndroidJniObject>
        #include <QtAndroidExtras/QAndroidJniEnvironment>
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

    #ifndef BOSS_SILENT_NIGHT_IS_ENABLED
        typedef WId WIdPrivate;
        typedef QColor ColorPrivate;
        typedef QMatrix MatrixPrivate;
        typedef QFocusEvent FocusEventPrivate;
        typedef QCloseEvent CloseEventPrivate;
        typedef QMoveEvent MoveEventPrivate;
        typedef QResizeEvent ResizeEventPrivate;
        typedef QPaintEvent PaintEventPrivate;
        typedef QInputEvent InputEventPrivate;
        typedef QMouseEvent MouseEventPrivate;
        typedef QWheelEvent WheelEventPrivate;
        typedef QKeyEvent KeyEventPrivate;
        typedef QDesktopServices DesktopServicesPrivate;
        typedef QFont FontPrivate;
        typedef QFontMetrics FontMetricsPrivate;
        typedef QPalette PalettePrivate;
        typedef QCursor CursorPrivate;
        typedef QPaintDevice PaintDevicePrivate;
        typedef QPainter PainterPrivate;
        typedef QPainterPath PainterPathPrivate;
        typedef QImage ImagePrivate;
        typedef QBitmap BitmapPrivate;
        typedef QPixmap PixmapPrivate;
        typedef QStyle StylePrivate;
        typedef QScreen ScreenForPrivate;
        typedef QIcon IconPrivate;
        typedef QAction ActionPrivate;
        typedef QWindow WindowPrivate;
        typedef QList<QWindow*> WindowListPrivate;
        typedef QWidget WidgetPrivate;
        typedef QList<QWidget*> WidgetListPrivate;
        typedef QDesktopWidget DesktopWidgetPrivate;
        typedef QGuiApplication GuiApplicationPrivate;
        typedef QApplication ApplicationPrivate;
        typedef QFrame FramePrivate;
        typedef QMenu MenuPrivate;
        typedef QMenuBar MenuBarPrivate;
        typedef QStatusBar StatusBarPrivate;
        typedef QToolBar ToolBarPrivate;
        typedef QDockWidget DockWidgetPrivate;
        typedef QMainWindow MainWindowPrivate;
        typedef QAbstractScrollArea AbstractScrollAreaPrivate;
        typedef QMdiArea MdiAreaPrivate;
        typedef QValidator ValidatorPrivate;
        typedef QIntValidator IntValidatorPrivate;
        typedef QDoubleValidator DoubleValidatorPrivate;
        typedef QSystemTrayIcon SystemTrayIconPrivate;
        typedef QHBoxLayout HBoxLayoutPrivate;
        typedef QLineEdit LineEditPrivate;
        typedef QListWidgetItem ListWidgetItemPrivate;
        typedef QListWidget ListWidgetPrivate;
        typedef QInputDialog InputDialogPrivate;
        typedef QColorDialog ColorDialogPrivate;
        typedef QGLWidget GLWidgetPrivate;
    #else
        class WIdForEmpty;
        typedef WIdForEmpty WIdPrivate;
        class ColorForEmpty;
        typedef ColorForEmpty ColorPrivate;
        class MatrixForEmpty;
        typedef MatrixForEmpty MatrixPrivate;
        class FocusEventForEmpty;
        typedef FocusEventForEmpty FocusEventPrivate;
        class CloseEventForEmpty;
        typedef CloseEventForEmpty CloseEventPrivate;
        class MoveEventForEmpty;
        typedef MoveEventForEmpty MoveEventPrivate;
        class ResizeEventForEmpty;
        typedef ResizeEventForEmpty ResizeEventPrivate;
        class PaintEventForEmpty;
        typedef PaintEventForEmpty PaintEventPrivate;
        class InputEventForEmpty;
        typedef InputEventForEmpty InputEventPrivate;
        class MouseEventForEmpty;
        typedef MouseEventForEmpty MouseEventPrivate;
        class WheelEventForEmpty;
        typedef WheelEventForEmpty WheelEventPrivate;
        class KeyEventForEmpty;
        typedef KeyEventForEmpty KeyEventPrivate;
        class DesktopServicesForEmpty;
        typedef DesktopServicesForEmpty DesktopServicesPrivate;
        class FontForEmpty;
        typedef FontForEmpty FontPrivate;
        class FontMetricsForEmpty;
        typedef FontMetricsForEmpty FontMetricsPrivate;
        class PaletteForEmpty;
        typedef PaletteForEmpty PalettePrivate;
        class CursorForEmpty;
        typedef CursorForEmpty CursorPrivate;
        class PaintDeviceForEmpty;
        typedef PaintDeviceForEmpty PaintDevicePrivate;
        class PainterForEmpty;
        typedef PainterForEmpty PainterPrivate;
        class PainterPathForEmpty;
        typedef PainterPathForEmpty PainterPathPrivate;
        class ImageForEmpty;
        typedef ImageForEmpty ImagePrivate;
        class BitmapForEmpty;
        typedef BitmapForEmpty BitmapPrivate;
        class PixmapForEmpty;
        typedef PixmapForEmpty PixmapPrivate;
        class StyleForEmpty;
        typedef StyleForEmpty StylePrivate;
        class ScreenForEmpty;
        typedef ScreenForEmpty ScreenForPrivate;
        class IconForEmpty;
        typedef IconForEmpty IconPrivate;
        class ActionForEmpty;
        typedef ActionForEmpty ActionPrivate;
        class WindowForEmpty;
        typedef WindowForEmpty WindowPrivate;
        typedef QList<WindowPrivate*> WindowListPrivate;
        class WidgetForEmpty;
        typedef WidgetForEmpty WidgetPrivate;
        typedef QList<WidgetPrivate*> WidgetListPrivate;
        class DesktopWidgetForEmpty;
        typedef DesktopWidgetForEmpty DesktopWidgetPrivate;
        class GuiApplicationForEmpty;
        typedef GuiApplicationForEmpty GuiApplicationPrivate;
        class ApplicationForEmpty;
        typedef ApplicationForEmpty ApplicationPrivate;
        class FrameForEmpty;
        typedef FrameForEmpty FramePrivate;
        class MenuForEmpty;
        typedef MenuForEmpty MenuPrivate;
        class MenuBarForEmpty;
        typedef MenuBarForEmpty MenuBarPrivate;
        class StatusBarForEmpty;
        typedef StatusBarForEmpty StatusBarPrivate;
        class ToolBarForEmpty;
        typedef ToolBarForEmpty ToolBarPrivate;
        class DockWidgetForEmpty;
        typedef DockWidgetForEmpty DockWidgetPrivate;
        class MainWindowForEmpty;
        typedef MainWindowForEmpty MainWindowPrivate;
        class AbstractScrollAreaForEmpty;
        typedef AbstractScrollAreaForEmpty AbstractScrollAreaPrivate;
        class MdiAreaForEmpty;
        typedef MdiAreaForEmpty MdiAreaPrivate;
        class ValidatorForEmpty;
        typedef ValidatorForEmpty ValidatorPrivate;
        class IntValidatorForEmpty;
        typedef IntValidatorForEmpty IntValidatorPrivate;
        class DoubleValidatorForEmpty;
        typedef DoubleValidatorForEmpty DoubleValidatorPrivate;
        class SystemTrayIconForEmpty;
        typedef SystemTrayIconForEmpty SystemTrayIconPrivate;
        class HBoxLayoutForEmpty;
        typedef HBoxLayoutForEmpty HBoxLayoutPrivate;
        class LineEditForEmpty;
        typedef LineEditForEmpty LineEditPrivate;
        class ListWidgetItemForEmpty;
        typedef ListWidgetItemForEmpty ListWidgetItemPrivate;
        class ListWidgetForEmpty;
        typedef ListWidgetForEmpty ListWidgetPrivate;
        class InputDialogForEmpty;
        typedef InputDialogForEmpty InputDialogPrivate;
        class ColorDialogForEmpty;
        typedef ColorDialogForEmpty ColorDialogPrivate;
        class GLWidgetForEmpty;
        typedef GLWidgetForEmpty GLWidgetPrivate;
    #endif

    class WIdForEmpty
    {
    public:
        WIdForEmpty() {}
        WIdForEmpty(ublock wid) {mWId = wid;}
        ~WIdForEmpty() {}

    public:
        operator ublock() {return mWId;}

    public:
        ublock mWId;
    };

    class ColorForEmpty
    {
    public:
        ColorForEmpty() {}
        ColorForEmpty(Qt::GlobalColor color) {}
        ColorForEmpty(int r, int g, int b, int a = 255) {}
        ~ColorForEmpty() {}

    public:
        int alpha() const {return 0;}
        int red() const {return 0;}
        int green() const {return 0;}
        int blue() const {return 0;}
        void setRgb(int r, int g, int b, int a = 255) {}
        bool isValid() const {return false;}
    };

    class MatrixForEmpty
    {
    public:
        MatrixForEmpty() {}
        MatrixForEmpty(qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy) {}
        ~MatrixForEmpty() {}

    public:
        MatrixForEmpty& translate(qreal dx, qreal dy) {return *this;}
        MatrixForEmpty& scale(qreal sx, qreal sy) {return *this;}
        MatrixForEmpty& shear(qreal sh, qreal sv) {return *this;}
        MatrixForEmpty& rotate(qreal a) {return *this;}
        qreal m11() const {return 0;}
        qreal m12() const {return 0;}
        qreal m21() const {return 0;}
        qreal m22() const {return 0;}
        qreal dx() const {return 0;}
        qreal dy() const {return 0;}
    };

    class FocusEventForEmpty : public QEvent
    {
        Q_GADGET

    public:
        FocusEventForEmpty(Type type) : QEvent(type) {}
        ~FocusEventForEmpty() {}
    };

    class CloseEventForEmpty : public QEvent
    {
        Q_GADGET

    public:
        CloseEventForEmpty(Type type) : QEvent(type) {}
        ~CloseEventForEmpty() {}
    };

    class MoveEventForEmpty : public QEvent
    {
        Q_GADGET

    public:
        MoveEventForEmpty(Type type) : QEvent(type) {}
        ~MoveEventForEmpty() {}

    public:
        const QPoint& pos() const {return mPos;}

    public:
        QPoint mPos;
    };

    class ResizeEventForEmpty : public QEvent
    {
        Q_GADGET

    public:
        ResizeEventForEmpty(Type type) : QEvent(type) {}
        ~ResizeEventForEmpty() {}

    public:
        const QSize& size() const {return mSize;}

    public:
        QSize mSize;
    };

    class PaintEventForEmpty : public QEvent
    {
        Q_GADGET

    public:
        PaintEventForEmpty(Type type) : QEvent(type) {}
        ~PaintEventForEmpty() {}
    };

    class InputEventForEmpty : public QEvent
    {
        Q_GADGET

    public:
        InputEventForEmpty(Type type) : QEvent(type) {}
        ~InputEventForEmpty() {}
    };

    class MouseEventForEmpty : public InputEventForEmpty
    {
        Q_GADGET

    public:
        MouseEventForEmpty(Type type) : InputEventForEmpty(type) {}
        ~MouseEventForEmpty() {}

    public:
        Qt::MouseButton button() const {return Qt::MouseButton();}
        Qt::MouseButtons buttons() const {return Qt::MouseButtons();}
        QPoint pos() const {return QPoint();}
        int x() const {return 0;}
        int y() const {return 0;}
    };

    class WheelEventForEmpty : public InputEventForEmpty
    {
        Q_GADGET

    public:
        WheelEventForEmpty(Type type) : InputEventForEmpty(type) {}
        ~WheelEventForEmpty() {}
    public:
        int delta() const {return 0;}
        QPoint pos() const {return QPoint();}
        int x() const {return 0;}
        int y() const {return 0;}
    };

    class KeyEventForEmpty : public InputEventForEmpty
    {
        Q_GADGET

    public:
        KeyEventForEmpty(Type type) : InputEventForEmpty(type) {}
        ~KeyEventForEmpty() {}

    public:
        int key() const {return 0;}
        QString text() const {return QString();}
        bool isAutoRepeat() const {return false;}
    };

    class DesktopServicesForEmpty
    {
    public:
        DesktopServicesForEmpty() {}
        ~DesktopServicesForEmpty() {}

    public:
        static bool openUrl(const QUrl& url) {return false;}
    };

    class FontForEmpty
    {
    public:
        FontForEmpty() {}
        FontForEmpty(const FontForEmpty&) {}
        FontForEmpty(const QString& family, int pointSize = -1, int weight = -1, bool italic = false) {}
        ~FontForEmpty() {}

    public:
        FontForEmpty& operator=(const FontForEmpty&) {return *this;}
    };

    class FontMetricsForEmpty
    {
    public:
        FontMetricsForEmpty(const FontPrivate&) {}
        ~FontMetricsForEmpty() {}

    public:
        int ascent() const {return 0;}
        int width(const QString&, int len = -1) const {return 0;}
        int height() const {return 0;}
    };

    class PaletteForEmpty
    {
        Q_GADGET

    public:
        enum ColorGroup { Active, Disabled, Inactive, NColorGroups, Current, All, Normal = Active };
        Q_ENUM(ColorGroup)
        enum ColorRole { WindowText, Button, Light, Midlight, Dark, Mid,
            Text, BrightText, ButtonText, Base, Window, Shadow,
            Highlight, HighlightedText,
            Link, LinkVisited,
            AlternateBase,
            NoRole,
            ToolTipBase, ToolTipText,
            NColorRoles = ToolTipText + 1,
            Foreground = WindowText, Background = Window };
        Q_ENUM(ColorRole)

    public:
        PaletteForEmpty() {}
        ~PaletteForEmpty() {}

    public:
        void setColor(ColorRole cr, const ColorPrivate& color) {}
    };

    class CursorForEmpty
    {
    public:
        CursorForEmpty() {}
        ~CursorForEmpty() {}

    public:
        static QPoint pos() {return QPoint();}
    };

    class PaintDeviceForEmpty : public QObject
    {
        Q_OBJECT

    public:
        PaintDeviceForEmpty() {}
        ~PaintDeviceForEmpty() {}
    };

    class PainterForEmpty : public QObject
    {
        Q_OBJECT

    public:
        enum CompositionMode
        {
            CompositionMode_SourceOver,
            CompositionMode_DestinationOver,
            CompositionMode_Clear,
            CompositionMode_Source,
            CompositionMode_Destination,
            CompositionMode_SourceIn,
            CompositionMode_DestinationIn,
            CompositionMode_SourceOut,
            CompositionMode_DestinationOut,
            CompositionMode_SourceAtop,
            CompositionMode_DestinationAtop,
            CompositionMode_Xor,
            //svg 1.2 blend modes
            CompositionMode_Plus,
            CompositionMode_Multiply,
            CompositionMode_Screen,
            CompositionMode_Overlay,
            CompositionMode_Darken,
            CompositionMode_Lighten,
            CompositionMode_ColorDodge,
            CompositionMode_ColorBurn,
            CompositionMode_HardLight,
            CompositionMode_SoftLight,
            CompositionMode_Difference,
            CompositionMode_Exclusion,
            // ROPs
            RasterOp_SourceOrDestination,
            RasterOp_SourceAndDestination,
            RasterOp_SourceXorDestination,
            RasterOp_NotSourceAndNotDestination,
            RasterOp_NotSourceOrNotDestination,
            RasterOp_NotSourceXorDestination,
            RasterOp_NotSource,
            RasterOp_NotSourceAndDestination,
            RasterOp_SourceAndNotDestination,
            RasterOp_NotSourceOrDestination,
            RasterOp_SourceOrNotDestination,
            RasterOp_ClearDestination,
            RasterOp_SetDestination,
            RasterOp_NotDestination
        };
        enum RenderHint
        {
            Antialiasing = 0x01,
            TextAntialiasing = 0x02,
            SmoothPixmapTransform = 0x04,
            HighQualityAntialiasing = 0x08,
            NonCosmeticDefaultPen = 0x10,
            Qt4CompatiblePainting = 0x20
        };
        Q_FLAG(RenderHint)

        Q_DECLARE_FLAGS(RenderHints, RenderHint)
        Q_FLAG(RenderHints)

    public:
        PainterForEmpty() : mFontMetrics(mFont) {}
        ~PainterForEmpty() {}

    public:
        PaintDevicePrivate* device() const {return nullptr;}
        bool begin(PaintDevicePrivate*) {return false;}
        bool end() {return false;}
        void setRenderHint(RenderHint hint, bool on = true) {}
        void setRenderHints(RenderHints hints, bool on = true) {}
        void setMatrix(const MatrixPrivate& matrix, bool combine = false) {mMatrix = matrix;}
        const MatrixPrivate& matrix() const {return mMatrix;}
        void setClipRect(const QRectF&) {}
        QRectF clipBoundingRect() const {return QRectF();}
        const FontPrivate& font() const {return mFont;}
        void setFont(const FontPrivate& f) {mFont = f;}
        FontMetricsPrivate fontMetrics() const {return mFontMetrics;}

    public:
        MatrixPrivate mMatrix;
        FontPrivate mFont;
        FontMetricsPrivate mFontMetrics;
    };
    Q_DECLARE_OPERATORS_FOR_FLAGS(PainterForEmpty::RenderHints)

    class PainterPathForEmpty : public QObject
    {
        Q_OBJECT

    public:
        PainterPathForEmpty() {}
        ~PainterPathForEmpty() {}
    };

    class ImageForEmpty : public PaintDeviceForEmpty
    {
        Q_OBJECT

    public:
        enum Format {
            Format_Invalid,
            Format_Mono,
            Format_MonoLSB,
            Format_Indexed8,
            Format_RGB32,
            Format_ARGB32,
            Format_ARGB32_Premultiplied,
            Format_RGB16,
            Format_ARGB8565_Premultiplied,
            Format_RGB666,
            Format_ARGB6666_Premultiplied,
            Format_RGB555,
            Format_ARGB8555_Premultiplied,
            Format_RGB888,
            Format_RGB444,
            Format_ARGB4444_Premultiplied,
            Format_RGBX8888,
            Format_RGBA8888,
            Format_RGBA8888_Premultiplied,
            Format_BGR30,
            Format_A2BGR30_Premultiplied,
            Format_RGB30,
            Format_A2RGB30_Premultiplied,
            Format_Alpha8,
            Format_Grayscale8,
            #if 0
                // reserved for future use
                Format_Grayscale16,
            #endif
            #ifndef Q_QDOC
                NImageFormats
            #endif
        };

    public:
        ImageForEmpty() {}
        ImageForEmpty(const ImageForEmpty&) {}
        ImageForEmpty(int width, int height, Format format) {}
        ImageForEmpty(bytes data, int width, int height, Format format) {}
        ~ImageForEmpty() {}

    public:
        uint08* bits() {return nullptr;}
        bytes bits() const {return nullptr;}
        bytes constBits() const {return nullptr;}
        int bitPlaneCount() const {return 0;}
        int	width() const {return 0;}
        int	height() const {return 0;}
        ImageForEmpty scaled(int w, int h) const {return ImageForEmpty();}
        ImageForEmpty transformed(const MatrixPrivate& matrix) const {return ImageForEmpty();}
        ImageForEmpty convertToFormat(Format f) const {return ImageForEmpty();}

    public:
        ImageForEmpty& operator=(const ImageForEmpty&) {return *this;}
    };

    class BitmapForEmpty : public PaintDeviceForEmpty
    {
        Q_OBJECT

    public:
        BitmapForEmpty() {}
        BitmapForEmpty(const BitmapForEmpty&) {}
        ~BitmapForEmpty() {}
    };

    class PixmapForEmpty : public PaintDeviceForEmpty
    {
        Q_OBJECT

    public:
        PixmapForEmpty() {}
        PixmapForEmpty(const PixmapForEmpty&) {}
        ~PixmapForEmpty() {}

    public:
        static PixmapForEmpty fromImage(const ImagePrivate& image) {return PixmapForEmpty();}
        bool convertFromImage(const ImagePrivate& img) {return false;}
        int	width() const {return 0;}
        int	height() const {return 0;}
        ImagePrivate toImage() const {return ImagePrivate();}
        BitmapPrivate mask() const {return BitmapPrivate();}

    public:
        PixmapForEmpty& operator=(const PixmapForEmpty&) {return *this;}
    };

    class StyleForEmpty : public QObject
    {
        Q_OBJECT

    public:
        enum PixelMetric {
            PM_ButtonMargin,
            PM_ButtonDefaultIndicator,
            PM_MenuButtonIndicator,
            PM_ButtonShiftHorizontal,
            PM_ButtonShiftVertical,
            PM_DefaultFrameWidth,
            PM_SpinBoxFrameWidth,
            PM_ComboBoxFrameWidth,
            PM_MaximumDragDistance,
            PM_ScrollBarExtent,
            PM_ScrollBarSliderMin,
            PM_SliderThickness,             // total slider thickness
            PM_SliderControlThickness,      // thickness of the business part
            PM_SliderLength,                // total length of slider
            PM_SliderTickmarkOffset,        //
            PM_SliderSpaceAvailable,        // available space for slider to move
            PM_DockWidgetSeparatorExtent,
            PM_DockWidgetHandleExtent,
            PM_DockWidgetFrameWidth,
            PM_TabBarTabOverlap,
            PM_TabBarTabHSpace,
            PM_TabBarTabVSpace,
            PM_TabBarBaseHeight,
            PM_TabBarBaseOverlap,
            PM_ProgressBarChunkWidth,
            PM_SplitterWidth,
            PM_TitleBarHeight,
            PM_MenuScrollerHeight,
            PM_MenuHMargin,
            PM_MenuVMargin,
            PM_MenuPanelWidth,
            PM_MenuTearoffHeight,
            PM_MenuDesktopFrameWidth,
            PM_MenuBarPanelWidth,
            PM_MenuBarItemSpacing,
            PM_MenuBarVMargin,
            PM_MenuBarHMargin,
            PM_IndicatorWidth,
            PM_IndicatorHeight,
            PM_ExclusiveIndicatorWidth,
            PM_ExclusiveIndicatorHeight,
            PM_DialogButtonsSeparator,
            PM_DialogButtonsButtonWidth,
            PM_DialogButtonsButtonHeight,
            PM_MdiSubWindowFrameWidth,
            PM_MDIFrameWidth = PM_MdiSubWindowFrameWidth,         // ### Qt 6: remove
            PM_MdiSubWindowMinimizedWidth,
            PM_MDIMinimizedWidth = PM_MdiSubWindowMinimizedWidth, // ### Qt 6: remove
            PM_HeaderMargin,
            PM_HeaderMarkSize,
            PM_HeaderGripMargin,
            PM_TabBarTabShiftHorizontal,
            PM_TabBarTabShiftVertical,
            PM_TabBarScrollButtonWidth,
            PM_ToolBarFrameWidth,
            PM_ToolBarHandleExtent,
            PM_ToolBarItemSpacing,
            PM_ToolBarItemMargin,
            PM_ToolBarSeparatorExtent,
            PM_ToolBarExtensionExtent,
            PM_SpinBoxSliderHeight,
            PM_DefaultTopLevelMargin, // ### Qt 6: remove
            PM_DefaultChildMargin,    // ### Qt 6: remove
            PM_DefaultLayoutSpacing,  // ### Qt 6: remove
            PM_ToolBarIconSize,
            PM_ListViewIconSize,
            PM_IconViewIconSize,
            PM_SmallIconSize,
            PM_LargeIconSize,
            PM_FocusFrameVMargin,
            PM_FocusFrameHMargin,
            PM_ToolTipLabelFrameWidth,
            PM_CheckBoxLabelSpacing,
            PM_TabBarIconSize,
            PM_SizeGripSize,
            PM_DockWidgetTitleMargin,
            PM_MessageBoxIconSize,
            PM_ButtonIconSize,
            PM_DockWidgetTitleBarButtonMargin,
            PM_RadioButtonLabelSpacing,
            PM_LayoutLeftMargin,
            PM_LayoutTopMargin,
            PM_LayoutRightMargin,
            PM_LayoutBottomMargin,
            PM_LayoutHorizontalSpacing,
            PM_LayoutVerticalSpacing,
            PM_TabBar_ScrollButtonOverlap,
            PM_TextCursorWidth,
            PM_TabCloseIndicatorWidth,
            PM_TabCloseIndicatorHeight,
            PM_ScrollView_ScrollBarSpacing,
            PM_ScrollView_ScrollBarOverlap,
            PM_SubMenuOverlap,
            PM_TreeViewIndentation,
            PM_HeaderDefaultSectionSizeHorizontal,
            PM_HeaderDefaultSectionSizeVertical,
            PM_TitleBarButtonIconSize,
            PM_TitleBarButtonSize,
            // do not add any values below/greater than this
            PM_CustomBase = 0xf0000000
        };
        Q_ENUM(PixelMetric)

    public:
        StyleForEmpty() {}
        ~StyleForEmpty() {}

    public:
        int pixelMetric(PixelMetric metric) const {return 0;}
    };

    class ScreenForEmpty : public QObject
    {
        Q_OBJECT

    public:
        ScreenForEmpty() {}
        ~ScreenForEmpty() {}

    public:
        PixmapPrivate grabWindow(WIdPrivate window, int x = 0, int y = 0, int w = -1, int h = -1) {return PixmapPrivate();}
    };

    class IconForEmpty
    {
    public:
        IconForEmpty() {}
        IconForEmpty(const QString& fileName) {}
        ~IconForEmpty() {}
    };

    class ActionForEmpty : public QObject
    {
        Q_OBJECT

    public:
        ActionForEmpty(QObject* parent = nullptr) {}
        ActionForEmpty(const QString& text, QObject* parent = nullptr) {}
        ActionForEmpty(const IconPrivate& icon, const QString& text, QObject* parent = nullptr) {}
        ~ActionForEmpty() {}

    public:
        void setStatusTip(const QString& statusTip) {}
    };

    class WindowForEmpty : public QObject
    {
        Q_OBJECT

    public:
        WindowForEmpty(WindowForEmpty* parent = nullptr) {}
        ~WindowForEmpty() {}

    public Q_SLOTS:
        void alert(int msec) {}
        bool close() {return true;}
        void hide() {}
        void lower() {}
        void raise() {}
        void requestActivate() {}
        void requestUpdate() {}
        void setGeometry(int posx, int posy, int w, int h) {}
        void setGeometry(const QRect& rect) {}
        void setHeight(int arg) {}
        void setMaximumHeight(int h) {}
        void setMaximumWidth(int w) {}
        void setMinimumHeight(int h) {}
        void setMinimumWidth(int w) {}
        void setTitle(const QString&) {}
        void setVisible(bool visible) {}
        void setWidth(int arg) {}
        void setX(int arg) {}
        void setY(int arg) {}
        void show() {}
        void showFullScreen() {}
        void showMaximized() {}
        void showMinimized() {}
        void showNormal() {}
    };

    class WidgetForEmpty : public PaintDeviceForEmpty
    {
        Q_OBJECT

    public:
        WidgetForEmpty(WidgetForEmpty* parent = nullptr) {}
        ~WidgetForEmpty() {}

    public:
        void activateWindow() {}
        void move(int x, int y) {}
        void resize(const QSize&) {}
        void resize(int w, int h) {}
        bool isHidden() const {return false;}
        void setAttribute(Qt::WidgetAttribute attribute, bool on = true) {}
        void setAutoFillBackground(bool enabled) {}
        void setFocusPolicy(Qt::FocusPolicy policy) {}
        void setLayout(WidgetForEmpty*) {}
        void setMouseTracking(bool enable) {}
        void setMask(const BitmapPrivate&) {}
        void clearMask() {}
        void setMinimumSize(int minw, int minh) {}
        void setMaximumSize(int maxw, int maxh) {}
        void addAction(ActionPrivate* action) {}
        void setWindowFlags(Qt::WindowFlags type) {}
        Qt::WindowFlags windowFlags() const {return Qt::WindowFlags();}
        void setWindowModality(Qt::WindowModality windowModality) {}
        void setWindowOpacity(qreal level) {}
        const PalettePrivate& palette() const {return mPalette;}
        void setPalette(const PalettePrivate&) {}
        QPoint mapToGlobal(const QPoint& pos) const {return QPoint();}
        int	minimumWidth() const {return 0;}
        int	minimumHeight() const {return 0;}
        int	maximumWidth() const {return 0;}
        int	maximumHeight() const {return 0;}
        QPoint pos() const {return QPoint();}
        QSize size() const {return QSize();}
        WindowPrivate* windowHandle() const {return nullptr;}
        Qt::WindowType windowType() const {return Qt::Widget;}
        WIdPrivate winId() const {return WIdPrivate();}
        int	x() const {return 0;}
        int	y() const {return 0;}
        int	width() const {return 0;}
        int	height() const {return 0;}

    public Q_SLOTS:
        bool close() {return true;}
        void hide() {}
        void lower() {}
        void raise() {}
        void repaint() {}
        void setDisabled(bool disable) {}
        void setEnabled(bool) {}
        void setFocus() {}
        void setHidden(bool hidden) {}
        void setStyleSheet(const QString& styleSheet) {}
        virtual void setVisible(bool visible) {}
        void setWindowModified(bool) {}
        void setWindowTitle(const QString& name)
        {
            BOSS_TRACE("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
            BOSS_TRACE("┃ %s", name.toUtf8().constData());
            BOSS_TRACE("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        }
        void setWindowIcon(const IconPrivate& icon) {}
        void show() {}
        void showFullScreen() {}
        void showMaximized() {}
        void showMinimized() {}
        void showNormal() {}
        void update() {}

    protected:
        virtual void changeEvent(QEvent* event) {}
        virtual void closeEvent(CloseEventPrivate* event) {}
                void create(WIdPrivate window = 0, bool initializeWindow = true, bool destroyOldWindow = true) {}
                void destroy(bool destroyWindow = true, bool destroySubWindows = true) {}
        virtual void enterEvent(QEvent* event) {}
        virtual void focusInEvent(FocusEventPrivate* event) {}
                bool focusNextChild() {return false;}
        virtual bool focusNextPrevChild(bool next) {return false;}
        virtual void focusOutEvent(FocusEventPrivate* event) {}
                bool focusPreviousChild() {return false;}
        virtual void keyPressEvent(KeyEventPrivate* event) {}
        virtual void keyReleaseEvent(KeyEventPrivate* event) {}
        virtual void leaveEvent(QEvent* event) {}
        virtual void mouseDoubleClickEvent(MouseEventPrivate* event) {}
        virtual void mouseMoveEvent(MouseEventPrivate* event) {}
        virtual void mousePressEvent(MouseEventPrivate* event) {}
        virtual void mouseReleaseEvent(MouseEventPrivate* event) {}
        virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) {return false;}
        virtual void paintEvent(PaintEventPrivate* event) {}
        virtual void resizeEvent(ResizeEventPrivate* event) {}
        virtual void wheelEvent(WheelEventPrivate* event) {}

    protected:
        PalettePrivate mPalette;
    };

    class DesktopWidgetForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        DesktopWidgetForEmpty(WidgetPrivate* parent = nullptr) {}
        ~DesktopWidgetForEmpty() {}

    public:
        const QRect availableGeometry(int screen = -1) const {return QRect();}
        const QRect availableGeometry(const WidgetPrivate* widget) const {return QRect();}
        const QRect screenGeometry(int screen = -1) const {return QRect();}
        const QRect screenGeometry(const WidgetPrivate* widget) const {return QRect();}
        const QRect screenGeometry(const QPoint& point) const {return QRect();}
        int numScreens() const {return 1;}
        int screenNumber(const WidgetPrivate* widget = nullptr) const {return 0;}
    };

    class GuiApplicationForEmpty : public QCoreApplication
    {
        Q_OBJECT

    public:
        GuiApplicationForEmpty(int& argc, char** argv) : QCoreApplication(argc, argv) {}
        ~GuiApplicationForEmpty() {}

    public:
        static ScreenForPrivate* primaryScreen() {return nullptr;}
    };

    class ApplicationForEmpty : public GuiApplicationForEmpty
    {
        Q_OBJECT

    public:
        ApplicationForEmpty(int& argc, char** argv) : GuiApplicationForEmpty(argc, argv) {}
        ~ApplicationForEmpty() {}

    public:
        static WidgetPrivate* activeModalWidget() {return nullptr;}
        static WidgetListPrivate topLevelWidgets() {return WidgetListPrivate();}
        static DesktopWidgetPrivate* desktop() {return nullptr;}
        static StylePrivate* style() {return nullptr;}
    };

    class FrameForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        FrameForEmpty(WidgetPrivate* parent = nullptr) {}
        ~FrameForEmpty() {}
    };

    class MenuForEmpty : public WidgetForEmpty
    {
    public:
        MenuForEmpty() {}
        ~MenuForEmpty() {}

    public:
        ActionPrivate* addSeparator() {return nullptr;}
    };

    class MenuBarForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        MenuBarForEmpty(WidgetPrivate* parent = nullptr) {}
        ~MenuBarForEmpty() {}

    public:
        MenuPrivate* addMenu(const QString& title) {return nullptr;}
    };

    class StatusBarForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        StatusBarForEmpty(WidgetPrivate* parent = nullptr) {}
        ~StatusBarForEmpty() {}

    public:
        void showMessage(const QString& text, int timeout = 0) {}
    };

    class ToolBarForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        ToolBarForEmpty(WidgetPrivate* parent = nullptr) {}
        ~ToolBarForEmpty() {}

    public:
        ActionPrivate* addSeparator() {return nullptr;}
    };

    class DockWidgetForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        DockWidgetForEmpty(WidgetPrivate* parent = nullptr) {}
        DockWidgetForEmpty(const QString& title, WidgetPrivate* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags()) {}
        ~DockWidgetForEmpty() {}

    public:
        void setWidget(WidgetPrivate* widget) {}
        void setAllowedAreas(Qt::DockWidgetAreas areas) {}
        ActionPrivate* toggleViewAction() const {return nullptr;}
    };

    class MainWindowForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        MainWindowForEmpty(WidgetPrivate* parent = nullptr) {}
        ~MainWindowForEmpty() {}

    public:
        MenuBarPrivate* menuBar() const {return nullptr;}
        StatusBarPrivate* statusBar() const {return nullptr;}
        ToolBarPrivate* addToolBar(const QString& title) {return nullptr;}
        void addDockWidget(Qt::DockWidgetArea area, DockWidgetPrivate* dockwidget) {}
        void setUnifiedTitleAndToolBarOnMac(bool set) {}
        void setCentralWidget(WidgetPrivate* widget) {}
        int logicalDpiX() const {return 0;}
        int logicalDpiY() const {return 0;}
        int devicePixelRatio() const {return 0;}
        int depth() const {return 0;}
    };

    class AbstractScrollAreaForEmpty : public FrameForEmpty
    {
        Q_OBJECT

    public:
        AbstractScrollAreaForEmpty(WidgetPrivate* parent = nullptr) {}
        ~AbstractScrollAreaForEmpty() {}

    public:
        WidgetPrivate* viewport() const {return nullptr;}
    };

    class MdiAreaForEmpty : public AbstractScrollAreaForEmpty
    {
        Q_OBJECT

    public:
        MdiAreaForEmpty(WidgetPrivate* parent = nullptr) {}
        ~MdiAreaForEmpty() {}

    public:
        WidgetPrivate* currentSubWindow() const {return nullptr;}
        WidgetPrivate* addSubWindow(WidgetPrivate* widget) {return nullptr;}

    public Q_SLOTS:
        void closeAllSubWindows() {}
    };

    class ValidatorForEmpty
    {
    public:
        ValidatorForEmpty() {}
        virtual ~ValidatorForEmpty() {}
    };

    class IntValidatorForEmpty : public ValidatorForEmpty
    {
    public:
        IntValidatorForEmpty(QObject*) {}
        ~IntValidatorForEmpty() {}
    };

    class DoubleValidatorForEmpty : public ValidatorForEmpty
    {
    public:
        DoubleValidatorForEmpty(QObject*) {}
        ~DoubleValidatorForEmpty() {}
    };

    class SystemTrayIconForEmpty : public QObject
    {
        Q_OBJECT

    public:
        enum ActivationReason
        {Unknown, Context, DoubleClick, Trigger, MiddleClick};

    public:
        SystemTrayIconForEmpty(QObject* parent = nullptr) {}
        ~SystemTrayIconForEmpty() {}

    public:
        void setIcon(const IconPrivate& icon) {}

    public Q_SLOTS:
        void show() {}
    };

    class HBoxLayoutForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        HBoxLayoutForEmpty(WidgetPrivate* parent = nullptr) {}
        ~HBoxLayoutForEmpty() {}

    public:
        void setMargin(int) {}
        void addWidget(WidgetPrivate*) {}
    };

    class LineEditForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        enum EchoMode {Normal, NoEcho, Password, PasswordEchoOnEdit};
        Q_ENUM(EchoMode)

    public:
        LineEditForEmpty(const QString& contents, WidgetPrivate* parent = nullptr) {}
        ~LineEditForEmpty() {}

    public:
        void setFrame(bool) {}
        void setValidator(const ValidatorPrivate*) {}
        QString text() const {return QString();}

    public Q_SLOTS:
        void clear() {}
        void copy() const {}
        void cut() {}
        void paste() {}
        void redo() {}
        void selectAll() {}
        void setText(const QString&) {}
        void undo() {}
    };

    class ListWidgetItemForEmpty : public QObject
    {
        Q_OBJECT

    public:
        enum ItemType {Type = 0, UserType = 1000};

    public:
        ListWidgetItemForEmpty(const QString& text, WidgetPrivate* parent = nullptr, int type = Type) {}
        ~ListWidgetItemForEmpty() {}

    public:
        void setTextAlignment(int alignment) {}
    };

    class ListWidgetForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        ListWidgetForEmpty(WidgetPrivate* parent = nullptr) {}
        ~ListWidgetForEmpty() {}

    public:
        void addItem(ListWidgetItemPrivate* item) {}
        int row(const ListWidgetItemPrivate* item) const {return 0;}
    };

    class InputDialogForEmpty : public QObject
    {
        Q_OBJECT

    public:
        InputDialogForEmpty(WidgetPrivate* parent = nullptr) {}
        ~InputDialogForEmpty() {}

    public:
        static QString getText(WidgetPrivate* parent, const QString& title, const QString& label,
            LineEditPrivate::EchoMode echo = LineEditPrivate::Normal,
            const QString& text = QString(), bool* ok = nullptr,
            Qt::WindowFlags flags = Qt::WindowFlags(),
            Qt::InputMethodHints inputMethodHints = Qt::ImhNone) {return QString();}
    };

    class ColorDialogForEmpty : public QObject
    {
        Q_OBJECT

    public:
        enum ColorDialogOption {
            ShowAlphaChannel    = 0x00000001,
            NoButtons           = 0x00000002,
            DontUseNativeDialog = 0x00000004
        };
        Q_ENUM(ColorDialogOption)

        Q_DECLARE_FLAGS(ColorDialogOptions, ColorDialogOption)

    public:
        ColorDialogForEmpty(WidgetPrivate* parent = nullptr) {}
        ~ColorDialogForEmpty() {}

    public:
        static ColorPrivate getColor(const ColorPrivate& initial = Qt::white,
            WidgetPrivate* parent = nullptr,
            const QString& title = QString(),
            ColorDialogOptions options = ColorDialogOptions()) {return initial;}
    };

    class GLWidgetForEmpty : public WidgetForEmpty
    {
        Q_OBJECT

    public:
        GLWidgetForEmpty(WidgetForEmpty* parent = nullptr) {}
        ~GLWidgetForEmpty() {}

    public:
        void makeCurrent() {}
        void swapBuffers() {}

    protected:
        virtual void initializeGL() {}
        virtual void paintGL() {}
        virtual void resizeGL(int width, int height) {}

    protected:
        void setAutoBufferSwap(bool on) {}
    };

    #define USER_FRAMECOUNT (60)

    class MainData;
    extern MainData* g_data;
    extern MainWindowPrivate* g_window;
    extern WidgetPrivate* g_view;
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
        CanvasClass(PaintDevicePrivate* device);
        ~CanvasClass();
    public:
        void Bind(PaintDevicePrivate* device);
        void Unbind();
    private:
        void BindCore(PaintDevicePrivate* device);
        void UnbindCore();
    public:
        // Getter
        static inline CanvasClass* get() {return ST();}
        inline PainterPrivate& painter() {return mPainter;}
        inline bool is_font_ft() const {return mUseFontFT;}
        inline chars font_ft_nickname() const {return mFontFT.mNickName;}
        inline sint32 font_ft_height() const {return mFontFT.mHeight;}
        inline const QRect& scissor() const {return mScissor;}
        inline const ColorPrivate& color() const {return mColor;}
        // Setter
        inline void SetFont(chars name, sint32 size)
        {mUseFontFT = false; painter().setFont(FontPrivate(name, size));}
        inline void SetFontFT(chars nickname, sint32 height)
        {mUseFontFT = true; mFontFT.mNickName = nickname; mFontFT.mHeight = height;}
        inline void SetScissor(sint32 l, sint32 t, sint32 r, sint32 b)
        {
            mScissor = QRect(l, t, r - l, b - t);
            mPainter.setClipRect(mScissor);
        }
        inline void SetColor(uint08 r, uint08 g, uint08 b, uint08 a)
        {mColor.setRgb(r, g, b, a);}
        inline const PainterPrivate::CompositionMode& mask() const {return mMask;}
        inline void SetMask(PainterPrivate::CompositionMode mask)
        {mMask = mask;}
    private:
        static inline CanvasClass*& ST() {static CanvasClass* _ = nullptr; return _;}
    private:
        const bool mIsTypeSurface;
        bool mIsSurfaceBinded;
        CanvasClass* mSavedCanvas;
        float mSavedZoom;
        bool mUseFontFT;
        FTFontClass mFontFT;
        FontPrivate mSavedFont;
        PainterPrivate mPainter;
        QRect mScissor;
        ColorPrivate mColor;
        PainterPrivate::CompositionMode mMask;
    };

    class ViewAPI : public QObject
    {
        Q_OBJECT

    public:
        enum ParentType {PT_Null, PT_GenericView, PT_MainViewGL, PT_MainViewMDI};
        enum WidgetRequest {WR_Null, WR_NeedExit = -1, WR_NeedHide = -2};

    public:
        ViewAPI(ParentType type, buffer buf, View* manager, View::UpdaterCB cb, WidgetPrivate* data, WidgetPrivate* device)
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

            m_event_blocked = false;
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
            update(1);
        }

        inline void changeViewData(WidgetPrivate* data)
        {
            m_view_data = data;
            if(m_view_manager)
                m_view_manager->SetCallback(m_view_cb, m_view_data);
        }

        inline ViewClass* getClass() const
        {
            return (ViewClass*) m_view_manager->GetClass();
        }

        inline WidgetPrivate* getWidget() const
        {
            if(m_view_manager)
            {
                if(m_view_data)
                    return m_view_data;
                if(m_view_manager->IsNative())
                    return (WidgetPrivate*) m_view_manager->GetClass();
            }
            return (WidgetPrivate*) getParent();
        }

        inline WidgetPrivate* getWidgetForPaint() const
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
            m_view_manager->SendNotify(type, topic, in, out);
        }

        inline bool setEventBlocked(bool block)
        {
            const bool HasBlocked = m_event_blocked;
            m_event_blocked = block;
            return HasBlocked;
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
            if(CanvasClass::get()) return;
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
            update(1);
        }

        void update(sint32 count)
        {
            if(count == WR_NeedExit)
                m_request = WR_NeedExit;
            else if(count == WR_NeedHide)
                m_request = WR_NeedHide;
            else if(m_paintcount < count)
            {
                if(m_paintcount == 0)
                    m_update_timer.start(1000 / USER_FRAMECOUNT);
                m_paintcount = count;
            }
        }

        bool closeEvent(CloseEventPrivate* event)
        {
            if(canQuit())
            {
                event->accept();
                return true;
            }
            event->ignore();
            return false;
        }

        void mousePressEvent(MouseEventPrivate* event)
        {
            if(m_event_blocked)
            {
                event->ignore();
                return;
            }

            if(event->button() == Qt::LeftButton)
            {
                touch(TT_Press, 0, event->x(), event->y());
                m_longpress_timer.start(3000);
                m_longpress_x = event->x();
                m_longpress_y = event->y();
            }
            else if(event->button() == Qt::RightButton)
                touch(TT_ExtendPress, 0, event->x(), event->y());
            else if(event->button() == Qt::MidButton)
                touch(TT_WheelPress, 0, event->x(), event->y());

            m_tooltip_timer.stop();
        }

        void mouseMoveEvent(MouseEventPrivate* event)
        {
            if(m_event_blocked)
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

        void mouseReleaseEvent(MouseEventPrivate* event)
        {
            if(m_event_blocked)
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

        void wheelEvent(WheelEventPrivate* event)
        {
            if(m_event_blocked)
            {
                event->ignore();
                return;
            }

            sint32 WheelValue = event->delta() / 120;
            while(0 < WheelValue)
            {
                WheelValue--;
                touch(TT_WheelUp, 0, event->x(), event->y());
            }
            while(WheelValue < 0)
            {
                WheelValue++;
                touch(TT_WheelDown, 0, event->x(), event->y());
            }
        }

        void keyPressEvent(KeyEventPrivate* event)
        {
            if(m_event_blocked)
            {
                event->ignore();
                return;
            }
            if(!event->isAutoRepeat())
                key(event->key(), event->text().toUtf8().constData(), true);
        }

        void keyReleaseEvent(KeyEventPrivate* event)
        {
            if(m_event_blocked)
            {
                event->ignore();
                return;
            }
            if(!event->isAutoRepeat())
                key(event->key(), event->text().toUtf8().constData(), false);
        }

    private:
        void tick_timeout()
        {
            if(m_event_blocked)
                return;
            if(m_request == WR_Null)
                sendTick();
            else
            {
                if(m_request == WR_NeedExit)
                    getWidget()->close();
                else if(m_request == WR_NeedHide)
                    getWidget()->hide();
                m_request = WR_Null;
            }
        }

        void update_timeout()
        {
            if(m_event_blocked)
                return;
            if(m_paintcount == 0)
                m_update_timer.stop();
            getWidgetForPaint()->update();
        }

        void tooltip_timeout()
        {
            if(m_event_blocked)
                return;
            m_tooltip_timer.stop();
            point64 CursorPos;
            if(Platform::Utility::GetCursorPosInWindow(CursorPos))
                touch(TT_ToolTip, 0, CursorPos.x, CursorPos.y);
        }

        void longpress_timeout()
        {
            if(m_event_blocked)
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
        WidgetPrivate* m_view_data;
        WidgetPrivate* m_paint_device;

    private:
        bool m_event_blocked;
        sint32 m_width;
        sint32 m_height;
        WidgetRequest m_request;
        sint32 m_paintcount;
        QTimer m_tick_timer;
        QTimer m_update_timer;
        QTimer m_tooltip_timer;
        QTimer m_longpress_timer;
        sint32 m_longpress_x;
        sint32 m_longpress_y;
    };

    class GenericView : public FramePrivate
    {
        Q_OBJECT

    public:
        GenericView(WidgetPrivate* parent = nullptr) : FramePrivate(parent)
        {
            m_api = nullptr;

            setMouseTracking(true);
            setFocusPolicy(Qt::ClickFocus);
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
            setFocusPolicy(Qt::ClickFocus);
            setAutoFillBackground(false);
        }

        GenericView(h_view view)
        {
            takeView(view);

            setMouseTracking(true);
            setFocusPolicy(Qt::ClickFocus);
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
        void resizeEvent(ResizeEventPrivate* event) Q_DECL_OVERRIDE
        {
            m_api->resize(event->size().width(), event->size().height());
        }

        void paintEvent(PaintEventPrivate* event) Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
                m_api->paint();
            else FramePrivate::paintEvent(event);
            m_api->nextPaint();
        }

    protected:
        void closeEvent(CloseEventPrivate* event) Q_DECL_OVERRIDE
        {
            if(m_api->closeEvent(event))
            {
                m_closing = true;
                m_window.set_buf(nullptr);
                setAttribute(Qt::WA_DeleteOnClose);
            }
        }
        void mousePressEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(WheelEventPrivate* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count)
        {((GenericView*) data)->m_api->update(count);}

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
            WindowListPrivate processedWindows;
            while(auto w = ApplicationPrivate::activeModalWidget())
            {
                if(WindowPrivate* window = w->windowHandle())
                {
                    if(!window->close()) return false;
                    processedWindows.append(window);
                }
            }
            for(auto w : ApplicationPrivate::topLevelWidgets())
            {
                if(w->windowType() == Qt::Desktop)
                    continue;
                if(WindowPrivate* window = w->windowHandle())
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

    #ifndef BOSS_SILENT_NIGHT_IS_ENABLED
        class MainViewGL : public GLWidgetPrivate
        {
            Q_OBJECT

        public:
            MainViewGL(WidgetPrivate* parent) : GLWidgetPrivate(parent)
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

                setAttribute(Qt::WA_PaintOnScreen);
                setAttribute(Qt::WA_NoSystemBackground);
                setAttribute(Qt::WA_AcceptTouchEvents);
                setMouseTracking(true);
                setFocusPolicy(Qt::ClickFocus);
                setAutoBufferSwap(false);
                setAutoFillBackground(false);
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
            void onCloseEvent(CloseEventPrivate* event)
            {
                if(m_api->closeEvent(event))
                    GenericView::CloseAllWindows();
            }

        protected:
            void initializeGL() Q_DECL_OVERRIDE
            {
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

                            f->glGenFramebuffers(1, &m_fbo);
                            f->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
                            f->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_fbo_render);
                            GLenum status = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);
                            BOSS_ASSERT("프레임버퍼의 생성에 실패하였습니다", status == GL_FRAMEBUFFER_COMPLETE);
                            f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
                        }
                    }
                    m_api->paint();
                    swapBuffers();
                }
                else GLWidgetPrivate::paintGL();
                m_api->nextPaint();
            }

        protected:
            void closeEvent(CloseEventPrivate* event) Q_DECL_OVERRIDE {m_api->closeEvent(event);}
            void mousePressEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
            void mouseDoubleClickEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
            void mouseMoveEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
            void mouseReleaseEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
            void wheelEvent(WheelEventPrivate* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
            void keyPressEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
            void keyReleaseEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
            static void updater(void* data, sint32 count)
            {((MainViewGL*) data)->m_api->update(count);}

        public:
            ViewAPI* m_api;

        private:
            bool m_fbo_update;
            GLuint m_fbo;
            GLuint m_fbo_render;
            sint32 m_fbo_width;
            sint32 m_fbo_height;
        };
    #else
        class MainViewGL : public GLWidgetPrivate
        {
            Q_OBJECT

        public:
            MainViewGL(WidgetPrivate* parent) : GLWidgetPrivate(parent)
            {
                BOSS_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr, nullptr);
                buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BOSS_DBG 1);
                BOSS_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_MainViewGL, (buffer) this, nullptr, updater, this, nullptr);
                m_api = (ViewAPI*) NewAPI;
            }
            virtual ~MainViewGL()
            {
                if(m_api->parentIsPtr())
                    Buffer::Free((buffer) m_api);
            }
            MainViewGL(const MainViewGL* rhs) {BOSS_ASSERT("사용금지", false);}
            MainViewGL& operator=(const MainViewGL& rhs) {BOSS_ASSERT("사용금지", false); return *this;}

        public:
            void onCloseEvent(CloseEventPrivate* event)
            {
                if(m_api->closeEvent(event))
                    GenericView::CloseAllWindows();
            }

        protected:
            void initializeGL() Q_DECL_OVERRIDE
            {
            }

            void resizeGL(int width, int height) Q_DECL_OVERRIDE
            {
                m_api->resize(width, height);
            }

            void paintGL() Q_DECL_OVERRIDE
            {
                if(m_api->hasViewManager())
                {
                    m_api->paint();
                    swapBuffers();
                }
                else GLWidgetPrivate::paintGL();
                m_api->nextPaint();
            }

        protected:
            void closeEvent(CloseEventPrivate* event) Q_DECL_OVERRIDE {m_api->closeEvent(event);}
            void mousePressEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
            void mouseDoubleClickEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
            void mouseMoveEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
            void mouseReleaseEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
            void wheelEvent(WheelEventPrivate* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
            void keyPressEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
            void keyReleaseEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
            static void updater(void* data, sint32 count)
            {((MainViewGL*) data)->m_api->update(count);}

        public:
            ViewAPI* m_api;
        };
    #endif

    class MainViewMDI : public MdiAreaPrivate
    {
        Q_OBJECT

    public:
        MainViewMDI(WidgetPrivate* parent) : MdiAreaPrivate(parent)
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
            setFocusPolicy(Qt::ClickFocus);
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
        void onCloseEvent(CloseEventPrivate* event)
        {
            if(m_api->closeEvent(event))
                GenericView::CloseAllWindows();
        }

    protected:
        void resizeEvent(ResizeEventPrivate* event) Q_DECL_OVERRIDE
        {
            m_width = event->size().width();
            m_height = event->size().height();
            m_api->resize(m_width, m_height);
        }

        void paintEvent(PaintEventPrivate* event) Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
                m_api->paint();
            else MdiAreaPrivate::paintEvent(event);
            m_api->nextPaint();
        }

    protected:
        void closeEvent(CloseEventPrivate* event) Q_DECL_OVERRIDE {m_api->closeEvent(event);}
        void mousePressEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(MouseEventPrivate* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(WheelEventPrivate* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count)
        {((MainViewMDI*) data)->m_api->update(count);}

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

        void setWidget(GenericView* view, WidgetPrivate* widget)
        {
            m_view = view;
            m_widget = widget;
        }

    private:
        GenericView* m_view;
        WidgetPrivate* m_widget;
    };

    class TrayIcon : public SystemTrayIconPrivate
    {
        Q_OBJECT

    public:
        TrayIcon(WidgetPrivate* menu)
        {
            m_ref_menu = menu;
            Qt::WindowFlags TypeCollector = Qt::Dialog;
            TypeCollector |= Qt::FramelessWindowHint;
            TypeCollector |= Qt::WindowStaysOnTopHint;
            m_ref_menu->setWindowFlags(TypeCollector);
            m_ref_menu->installEventFilter(this);
            connect(this, SIGNAL(activated(SystemTrayIconPrivate::ActivationReason)),
                this, SLOT(iconActivated(SystemTrayIconPrivate::ActivationReason)));
        }
        ~TrayIcon()
        {
        }

    private slots:
        void iconActivated(SystemTrayIconPrivate::ActivationReason reason)
        {
            switch(reason)
            {
            case SystemTrayIconPrivate::Context: // R버튼 클릭
            case SystemTrayIconPrivate::Trigger: // L버튼 클릭
            case SystemTrayIconPrivate::MiddleClick: // 휠버튼 클릭
                {
                    const QPoint ClickPos = CursorPrivate::pos();
                    const QSize MenuSize = m_ref_menu->size();
                    const QRect GeometryRect = ApplicationPrivate::desktop()->screenGeometry(ClickPos);
                    const float PosX = (GeometryRect.right() < ClickPos.x() + MenuSize.width())?
                        ClickPos.x() - MenuSize.width() : ClickPos.x();
                    const float PosY = (GeometryRect.bottom() < ClickPos.y() + MenuSize.height())?
                        ClickPos.y() - MenuSize.height() : ClickPos.y();
                    m_ref_menu->move(PosX, PosY);
                    m_ref_menu->show();
                    m_ref_menu->activateWindow(); // setFocus()는 작동하지 않는다!
                    m_ref_menu->raise(); // 부모기준 첫번째 자식으로 올림
                }
                break;
            case SystemTrayIconPrivate::DoubleClick: // L버튼 더블클릭된 순간
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
                return true;
            }
            return QObject::eventFilter(watched, event);
        }

    private:
        WidgetPrivate* m_ref_menu;
    };

    class TrayBox
    {
    public:
        TrayBox()
        {
            m_ref_view = nullptr;
            m_tray = nullptr;
        }
        ~TrayBox()
        {
            delete m_tray;
        }

        void setWidget(GenericView* view, IconPrivate* icon)
        {
            m_ref_view = view;
            WidgetPrivate* OldWidget = m_ref_view->m_api->getWidget();
            OldWidget->resize(m_ref_view->getFirstSize());
            m_tray = new TrayIcon(OldWidget);
            if(icon) m_tray->setIcon(*icon);
            m_tray->show();
        }

    private:
        GenericView* m_ref_view;
        TrayIcon* m_tray;
    };

    class MainData
    {
    public:
        enum class WindowType {Normal, Minimize, Maximize};
        WindowType m_lastWindowType;
        rect128 m_lastWindowNormalRect;

    public:
        MainData(MainWindowPrivate* parent) :
            m_lastWindowType(WindowType::Normal), m_lastWindowNormalRect({0, 0, 0, 0})
        {
            m_parent = parent;
            m_viewGL = nullptr;
            m_viewMDI = nullptr;
        }
        ~MainData() {}

    public:
        void onCloseEvent(CloseEventPrivate* event)
        {
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
        void initForGL(bool frameless, bool topmost)
        {
            m_viewGL = new MainViewGL(m_parent);
            m_viewGL->m_api->renewParent(m_viewGL);
            // Qt5.9.1부터는 m_parent->setCentralWidget(m_viewGL)식의 접합은
            // 윈도우즈OS에서 다중 모니터상황에서 레이아웃의 정렬불량 문제가 발생
            WidgetPrivate* MainWidget = new WidgetPrivate();
            HBoxLayoutPrivate* MainLayout = new HBoxLayoutPrivate();
            MainLayout->setMargin(0);
            MainLayout->addWidget(m_viewGL);
            MainWidget->setLayout(MainLayout);
            m_parent->setCentralWidget(MainWidget);

            Qt::WindowFlags TypeCollector = Qt::Widget;
            if(frameless)
            {
                #if BOSS_MAC_OSX
                    TypeCollector |= Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint;
                #elif BOSS_WINDOWS | BOSS_LINUX
                    TypeCollector |= Qt::FramelessWindowHint;
                #endif
                // Frameless에서의 그림자기능 연구중!
                //MainWidget->setAttribute(Qt::WA_TranslucentBackground);
                //MainWidget->layout()->setMargin(9);
                //QGraphicsDropShadowEffect* NewEffect = new QGraphicsDropShadowEffect();
                //NewEffect->setBlurRadius(9);
                //NewEffect->setOffset(4);
                //MainWidget->setGraphicsEffect(NewEffect);
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
            }
            if(topmost)
                TypeCollector |= Qt::WindowStaysOnTopHint;
            if(TypeCollector != Qt::Widget)
                m_parent->setWindowFlags(TypeCollector);
        }

        ViewAPI* getMainAPI()
        {
            BOSS_ASSERT("GL/MDI의 초기화가 되어있지 않습니다", m_viewGL || m_viewMDI);
            if(m_viewGL) return m_viewGL->m_api;
            return m_viewMDI->m_api;
        }

        WidgetPrivate* getMainWindow()
        {
            return m_parent;
        }

        GLWidgetPrivate* getGLWidget()
        {
            return m_viewGL;
        }

        WidgetPrivate* bindWidget(GenericView* view)
        {
            BOSS_ASSERT("GL/MDI의 초기화가 되어있지 않습니다", m_viewGL || m_viewMDI);
            if(m_viewGL) return view->m_api->getWidget();
            return m_viewMDI->addSubWindow(view->m_api->getWidget());
        }

    public:
        MenuPrivate* getMenu(chars name)
        {
            MenuPrivate** Result = m_menuMap.Access(name);
            if(!Result)
            {
                m_menuMap(name) = g_window->menuBar()->addMenu(QString::fromUtf8(name));
                Result = m_menuMap.Access(name);
            }
            return *Result;
        }

        ToolBarPrivate* getToolbar(chars name)
        {
            ToolBarPrivate** Result = m_toolbarMap.Access(name);
            if(!Result)
            {
                m_toolbarMap(name) = g_window->addToolBar(QString::fromUtf8(name));
                Result = m_toolbarMap.Access(name);
            }
            return *Result;
        }

    private:
        MainWindowPrivate* m_parent;
        MainViewGL* m_viewGL;
        MainViewMDI* m_viewMDI;
        Map<MenuPrivate*> m_menuMap;
        Map<ToolBarPrivate*> m_toolbarMap;
    };

    class MainWindow : public MainWindowPrivate
    {
        Q_OBJECT

    public:
        MainWindow() : m_window_pos({0, 0}), m_window_pos_old({0, 0}), m_window_size({0, 0}), m_window_size_old({0, 0})
        {
            setUnifiedTitleAndToolBarOnMac(true);
            g_data = new MainData(this);
            connect(&m_tick_timer, &QTimer::timeout, this, &MainWindow::tick_timeout);
            m_tick_timer.start(1000 / USER_FRAMECOUNT);
            m_inited_platform = false;
            m_first_visible = true;
        }

        ~MainWindow()
        {
            m_tick_timer.stop();
            delete g_data;
            g_data = nullptr;
        }

    public:
        void SetInitedPlatform() {m_inited_platform = true;}
        bool initedPlatform() const {return m_inited_platform;}
        void SetFirstVisible(bool visible) {m_first_visible = visible;}
        bool firstVisible() const {return m_first_visible;}

    protected:
        void moveEvent(MoveEventPrivate* event) Q_DECL_OVERRIDE
        {
            m_window_pos_old = m_window_pos;
            auto CurPos = event->pos();
            m_window_pos.x = CurPos.x();
            m_window_pos.y = CurPos.y();
        }

        void resizeEvent(ResizeEventPrivate* event) Q_DECL_OVERRIDE
        {
            m_window_size_old = m_window_size;
            auto CurSize = event->size();
            m_window_size.w = CurSize.width();
            m_window_size.h = CurSize.height();
            WidgetPrivate::resizeEvent(event);
        }

        void changeEvent(QEvent* event) Q_DECL_OVERRIDE
        {
            if(event->type() == QEvent::WindowStateChange)
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
            WidgetPrivate::changeEvent(event);
        }

        void closeEvent(CloseEventPrivate* event) Q_DECL_OVERRIDE
        {
            g_data->onCloseEvent(event);
        }

    private slots:
        void OnSlot()
        {
        }

    private:
        void tick_timeout()
        {
            PlatformImpl::Core::LockProcedure();
            for(sint32 i = 0, iend = PlatformImpl::Core::GetProcedureCount(); i < iend; ++i)
                PlatformImpl::Core::GetProcedureCB(i)(PlatformImpl::Core::GetProcedureData(i));
            PlatformImpl::Core::UnlockProcedure();
        }

    private:
        point64 m_window_pos;
        point64 m_window_pos_old;
        size64 m_window_size;
        size64 m_window_size_old;
        QTimer m_tick_timer;
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
        Tracker(WidgetPrivate* parent, ExitCB exit, void* data)
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

        static void CloseAll(WidgetPrivate* parent)
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

        static bool HasAnyone(WidgetPrivate* parent)
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
        WidgetPrivate* m_parent;
        QEventLoop m_loop;
        ExitCB m_exit;
        void* m_data;
        Tracker* m_next;
    };

    class EditTracker : public LineEditPrivate
    {
        Q_OBJECT

    public:
        enum TrackerClosingType {TCT_Null, TCT_Enter, TCT_Escape, TCT_FocusOut, TCT_ForcedExit};

    public:
        EditTracker(UIEditType type, const QString& contents, WidgetPrivate* parent)
            : LineEditPrivate(contents, nullptr), m_tracker(parent, OnExit, this)
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
            case UIET_Int: setValidator(new IntValidatorPrivate(this)); break;
            case UIET_Float: setValidator(new DoubleValidatorPrivate(this)); break;
            default: break;
            }

            PalettePrivate Palette = palette();
            Palette.setColor(PalettePrivate::Base, ColorPrivate(255, 255, 255));
            Palette.setColor(PalettePrivate::Text, ColorPrivate(0, 0, 0));
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
        void focusOutEvent(FocusEventPrivate* event) Q_DECL_OVERRIDE
        {
            if(m_closing == TCT_Null)
                m_closing = TCT_FocusOut;
            close();
            m_tracker.Unlock();
        }

        void keyPressEvent(KeyEventPrivate* event) Q_DECL_OVERRIDE
        {
            if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
            {
                if(m_closing == TCT_Null)
                    m_closing = TCT_Enter;
                close();
                m_tracker.Unlock();
            }
            else if(event->key() == Qt::Key_Escape)
            {
                if(m_closing == TCT_Null)
                    m_closing = TCT_Escape;
                close();
                m_tracker.Unlock();
            }
            else LineEditPrivate::keyPressEvent(event);
        }

    private:
        Tracker m_tracker;
        TrackerClosingType m_closing;
        QPoint m_parentpos;
    };

    class ListTracker : public ListWidgetPrivate
    {
        Q_OBJECT

    public:
        ListTracker(const Strings& strings, WidgetPrivate* parent)
            : ListWidgetPrivate(nullptr), m_tracker(parent, OnExit, this)
        {
            for(sint32 i = 0, iend = strings.Count(); i < iend; ++i)
            {
                ListWidgetItemPrivate* NewItem = new ListWidgetItemPrivate(QString::fromUtf8(strings[i]), this);
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
        void focusOutEvent(FocusEventPrivate* event) Q_DECL_OVERRIDE
        {
            close();
            m_tracker.Unlock();
        }

    private slots:
        void onItemPressed(ListWidgetItemPrivate* item)
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

    #ifndef BOSS_SILENT_NIGHT_IS_ENABLED
        class OpenGLPrivate
        {
        private:
            OpenGLPrivate()
            {
                mVShader[0] = mVShader[1] = 0;
                mFShader[0] = mFShader[1] = 0;
                mProgram[0] = mProgram[1] = 0;
                switch(GetVersionGLES() & 0xF0)
                {
                case 0x20:
                    InitShaderGLES20();
                    break;
                case 0x30:
                    InitShaderGLES30();
                    break;
                default:
                    InitShaderGL();
                    break;
                }
            }
            ~OpenGLPrivate()
            {
                TermShader();
            }

        public:
            static OpenGLPrivate& ST()
            {static OpenGLPrivate _; return _;}

        public:
            void FillRect(uint32 fbo, const BOSS::Rect& rect, const BOSS::Color& color)
            {
                QOpenGLContext* ctx = QOpenGLContext::currentContext();
                QOpenGLFunctions* f = ctx->functions();

                f->glBindFramebuffer(GL_FRAMEBUFFER, fbo); TestGL(BOSS_DBG 0);
                GLint ViewPortValues[4] = {0};
                f->glGetIntegerv(GL_VIEWPORT, ViewPortValues);
                const GLint Width = ViewPortValues[2];
                const GLint Height = ViewPortValues[3];
                BOSS::Rect NewRect;
                NewRect.l = 2 * rect.l / Width - 1;
                NewRect.t = 1 - 2 * rect.t / Height;
                NewRect.r = 2 * rect.r / Width - 1;
                NewRect.b = 1 - 2 * rect.b / Height;

                f->glUseProgram(mProgram[0]); TestGL(BOSS_DBG 0);
                f->glBindBuffer(GL_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
                f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(VerticeID); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(ColorID); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(TexCoordsID); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(VerticeID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].vertices[0]); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(ColorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &mAttrib[0].colors[0]); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(TexCoordsID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].texcoords[0]); TestGL(BOSS_DBG 0);
                f->glUniformMatrix4fv(mMatrix[0], 1, GL_FALSE, (const GLfloat*) &mM[0][0]); TestGL(BOSS_DBG 0);

                f->glDisable(GL_CULL_FACE); TestGL(BOSS_DBG 0);
                f->glDisable(GL_DEPTH_TEST); TestGL(BOSS_DBG 0);
                f->glEnable(GL_BLEND); TestGL(BOSS_DBG 0);
                f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); TestGL(BOSS_DBG 0);
                f->glEnable(GL_SCISSOR_TEST); TestGL(BOSS_DBG 0);
                const QRect& CurScissor = CanvasClass::get()->scissor();
                const int ScreenHeight = CanvasClass::get()->painter().window().height();
                f->glScissor(CurScissor.x(), ScreenHeight - (CurScissor.y() + CurScissor.height()),
                    CurScissor.width(), CurScissor.height());

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
            }
            void DrawPixmap(uint32 fbo, float x, float y, const BOSS::Point p[3], const PixmapPrivate& pixmap, const BOSS::Point ip[3], const BOSS::Color& color)
            {
                QOpenGLContext* ctx = QOpenGLContext::currentContext();
                QOpenGLFunctions* f = ctx->functions();
                const float DeviceRatio = Platform::Utility::GetPixelRatio();

                f->glBindFramebuffer(GL_FRAMEBUFFER, fbo); TestGL(BOSS_DBG 0);
                GLint ViewPortValues[4] = {0};
                f->glGetIntegerv(GL_VIEWPORT, ViewPortValues);
                #if BOSS_ANDROID
                    const GLint DstWidth = ViewPortValues[2] / 4;
                    const GLint DstHeight = ViewPortValues[3] / 4;
                #else
                    const GLint DstWidth = ViewPortValues[2] / DeviceRatio;
                    const GLint DstHeight = ViewPortValues[3] / DeviceRatio;
                #endif

                auto CurContext = QGLContext::fromOpenGLContext(ctx);
                GLuint CurTexture = CurContext->bindTexture(pixmap);
                f->glActiveTexture(GL_TEXTURE0);
                f->glBindTexture(GL_TEXTURE_2D, CurTexture);

                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                f->glUseProgram(mProgram[0]); TestGL(BOSS_DBG 0);
                f->glBindBuffer(GL_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
                f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(VerticeID); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(ColorID); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(TexCoordsID); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(VerticeID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].vertices[0]); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(ColorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &mAttrib[0].colors[0]); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(TexCoordsID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].texcoords[0]); TestGL(BOSS_DBG 0);
                f->glUniformMatrix4fv(mMatrix[0], 1, GL_FALSE, (const GLfloat*) &mM[0][0]); TestGL(BOSS_DBG 0);

                f->glDisable(GL_CULL_FACE); TestGL(BOSS_DBG 0);
                f->glDisable(GL_DEPTH_TEST); TestGL(BOSS_DBG 0);
                f->glEnable(GL_BLEND); TestGL(BOSS_DBG 0);
                f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); TestGL(BOSS_DBG 0);
                f->glEnable(GL_SCISSOR_TEST); TestGL(BOSS_DBG 0);
                const QRect& CurScissor = CanvasClass::get()->scissor();
                const int ScreenHeight = CanvasClass::get()->painter().window().height();
                f->glScissor(CurScissor.x() * DeviceRatio,
                    (ScreenHeight - (CurScissor.y() + CurScissor.height())) * DeviceRatio,
                    CurScissor.width() * DeviceRatio,
                    CurScissor.height() * DeviceRatio);

                mAttrib[0].vertices[0] = 2 * (x + p[0].x) / DstWidth - 1;
                mAttrib[0].vertices[1] = 1 - 2 * (y + p[0].y) / DstHeight;
                mAttrib[0].color32 = color.ToABGR();
                mAttrib[0].texcoords[0] = ip[0].x;
                mAttrib[0].texcoords[1] = ip[0].y;

                mAttrib[1].vertices[0] = 2 * (x + p[1].x) / DstWidth - 1;
                mAttrib[1].vertices[1] = 1 - 2 * (y + p[1].y) / DstHeight;
                mAttrib[1].color32 = color.ToABGR();
                mAttrib[1].texcoords[0] = ip[1].x;
                mAttrib[1].texcoords[1] = ip[1].y;

                mAttrib[2].vertices[0] = 2 * (x + p[2].x) / DstWidth - 1;
                mAttrib[2].vertices[1] = 1 - 2 * (y + p[2].y) / DstHeight;
                mAttrib[2].color32 = color.ToABGR();
                mAttrib[2].texcoords[0] = ip[2].x;
                mAttrib[2].texcoords[1] = ip[2].y;
                f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 3); TestGL(BOSS_DBG 0);
            }
            void DrawTexture(uint32 fbo, const BOSS::Rect& rect, id_texture_read tex, const BOSS::Rect& texrect, const BOSS::Color& color, orientationtype ori, bool antialiasing)
            {
                QOpenGLContext* ctx = QOpenGLContext::currentContext();
                QOpenGLFunctions* f = ctx->functions();
                const float DeviceRatio = Platform::Utility::GetPixelRatio();

                f->glBindFramebuffer(GL_FRAMEBUFFER, fbo); TestGL(BOSS_DBG 0);
                GLint ViewPortValues[4] = {0};
                f->glGetIntegerv(GL_VIEWPORT, ViewPortValues);
                #if BOSS_ANDROID
                    const GLint DstWidth = ViewPortValues[2] / 4;
                    const GLint DstHeight = ViewPortValues[3] / 4;
                #else
                    const GLint DstWidth = ViewPortValues[2] / DeviceRatio;
                    const GLint DstHeight = ViewPortValues[3] / DeviceRatio;
                #endif
                BOSS::Rect NewRect;
                NewRect.l = 2 * rect.l / DstWidth - 1;
                NewRect.t = 1 - 2 * rect.t / DstHeight;
                NewRect.r = 2 * rect.r / DstWidth - 1;
                NewRect.b = 1 - 2 * rect.b / DstHeight;

                const bool IsNV21 = (mProgram[1] && Platform::Graphics::IsTextureNV21(tex));
                if(IsNV21)
                {
                    f->glActiveTexture(GL_TEXTURE0);
                    f->glBindTexture(GL_TEXTURE_2D, Platform::Graphics::GetTextureID(tex, 0));
                    f->glUniform1i(mTextureY, 0);
                    f->glActiveTexture(GL_TEXTURE1);
                    f->glBindTexture(GL_TEXTURE_2D, Platform::Graphics::GetTextureID(tex, 1));
                    f->glUniform1i(mTextureUV, 1);
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

                const GLint SrcWidth = Platform::Graphics::GetTextureWidth(tex);
                const GLint SrcHeight = Platform::Graphics::GetTextureHeight(tex);
                const sint32 SelectedProgram = (IsNV21)? 1 : 0;
                f->glUseProgram(mProgram[SelectedProgram]); TestGL(BOSS_DBG 0);
                f->glBindBuffer(GL_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
                f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(VerticeID); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(ColorID); TestGL(BOSS_DBG 0);
                f->glEnableVertexAttribArray(TexCoordsID); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(VerticeID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].vertices[0]); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(ColorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &mAttrib[0].colors[0]); TestGL(BOSS_DBG 0);
                f->glVertexAttribPointer(TexCoordsID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].texcoords[0]); TestGL(BOSS_DBG 0);
                f->glUniformMatrix4fv(mMatrix[SelectedProgram], 1, GL_FALSE, (const GLfloat*) &mM[0][0]); TestGL(BOSS_DBG 0);

                f->glDisable(GL_CULL_FACE); TestGL(BOSS_DBG 0);
                f->glDisable(GL_DEPTH_TEST); TestGL(BOSS_DBG 0);
                f->glEnable(GL_BLEND); TestGL(BOSS_DBG 0);
                f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); TestGL(BOSS_DBG 0);
                f->glEnable(GL_SCISSOR_TEST); TestGL(BOSS_DBG 0);
                const QRect& CurScissor = CanvasClass::get()->scissor();
                const int ScreenHeight = CanvasClass::get()->painter().window().height();
                f->glScissor(CurScissor.x() * DeviceRatio,
                    (ScreenHeight - (CurScissor.y() + CurScissor.height())) * DeviceRatio,
                    CurScissor.width() * DeviceRatio,
                    CurScissor.height() * DeviceRatio);

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
            }

        private:
            void InitShader(chars vsource_rgb, chars fsource_rgb, chars vsource_nv21 = nullptr, chars fsource_nv21 = nullptr)
            {
                QOpenGLContext* ctx = QOpenGLContext::currentContext();
                QOpenGLFunctions* f = ctx->functions();

                chars VSources[2] = {vsource_rgb, vsource_nv21};
                chars FSources[2] = {fsource_rgb, fsource_nv21};
                for(sint32 i = 0, iend = (vsource_nv21 && fsource_nv21)? 2 : 1; i < iend; ++i)
                {
                    mVShader[i] = f->glCreateShader(GL_VERTEX_SHADER); TestGL(BOSS_DBG 0);
                    f->glShaderSource(mVShader[i], 1, &VSources[i], NULL); TestGL(BOSS_DBG 0);
                    f->glCompileShader(mVShader[i]); TestShader(BOSS_DBG mVShader[i]);

                    mFShader[i] = f->glCreateShader(GL_FRAGMENT_SHADER); TestGL(BOSS_DBG 0);
                    f->glShaderSource(mFShader[i], 1, &FSources[i], NULL); TestGL(BOSS_DBG 0);
                    f->glCompileShader(mFShader[i]); TestShader(BOSS_DBG mFShader[i]);

                    mProgram[i] = f->glCreateProgram(); TestGL(BOSS_DBG 0);
                    f->glAttachShader(mProgram[i], mVShader[i]); TestShader(BOSS_DBG mVShader[i]);
                    f->glAttachShader(mProgram[i], mFShader[i]); TestShader(BOSS_DBG mFShader[i]);

                    f->glEnableVertexAttribArray(VerticeID); TestGL(BOSS_DBG 0);
                    f->glEnableVertexAttribArray(ColorID); TestGL(BOSS_DBG 0);
                    f->glBindAttribLocation(mProgram[i], VerticeID, "a_position"); TestGL(BOSS_DBG 0);
                    f->glBindAttribLocation(mProgram[i], ColorID, "a_color"); TestGL(BOSS_DBG 0);
                    f->glVertexAttribPointer(VerticeID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &mAttrib[0].vertices[0]); TestGL(BOSS_DBG 0);
                    f->glVertexAttribPointer(ColorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &mAttrib[0].colors[0]); TestGL(BOSS_DBG 0);

                    f->glLinkProgram(mProgram[i]); TestProgram(BOSS_DBG mProgram[i]);
                    f->glValidateProgram(mProgram[i]); TestProgram(BOSS_DBG mProgram[i]);

                    LoadIdentity();
                    f->glUseProgram(mProgram[i]); TestProgram(BOSS_DBG mProgram[i]);
                    mMatrix[i] = f->glGetUniformLocation(mProgram[i], "u_matrix"); TestGL(BOSS_DBG 0);
                    f->glUniformMatrix4fv(mMatrix[i], 1, GL_FALSE, (const GLfloat*) &mM[0][0]); TestGL(BOSS_DBG 0);
                    if(i == 1)
                    {
                        mTextureY = f->glGetUniformLocation(mProgram[i], "u_texture_y"); TestGL(BOSS_DBG 0);
                        mTextureUV = f->glGetUniformLocation(mProgram[i], "u_texture_uv"); TestGL(BOSS_DBG 0);
                    }
                }
            }
            void InitShaderGLES20()
            {
                InitShader(
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
            void InitShaderGLES30()
            {
                InitShader(
                    ////////////////////////////////////////////////////////////
                    // RGB
                    ////////////////////////////////////////////////////////////
                    String::Format(
                        "#version 300 es\n"
                        "layout (location = %d) in highp vec2 a_position;\n"
                        "layout (location = %d) in highp vec4 a_color;\n"
                        "layout (location = %d) in highp vec2 a_texcoord;\n"
                        "uniform highp mat4 u_matrix;\n"
                        "out mediump vec4 v_fragmentColor;\n"
                        "out mediump vec2 v_texCoord;\n"
                        "\n"
                        "void main()\n"
                        "{\n"
                        "    gl_Position = u_matrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
                        "    v_fragmentColor = a_color;\n"
                        "    v_texCoord = a_texcoord;\n"
                        "}", VerticeID, ColorID, TexCoordsID),
                    "#version 300 es\n"
                    "layout (location = 0) out highp vec4 oColour;\n"
                    "uniform highp mat4 u_matrix;\n"
                    "uniform highp sampler2D u_texture;\n"
                    "in mediump vec4 v_fragmentColor;\n"
                    "in mediump vec2 v_texCoord;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    oColour = v_fragmentColor * texture(u_texture, v_texCoord);\n"
                    "}",
                    ////////////////////////////////////////////////////////////
                    // NV21
                    ////////////////////////////////////////////////////////////
                    String::Format(
                        "#version 300 es\n"
                        "layout (location = %d) in highp vec2 a_position;\n"
                        "layout (location = %d) in highp vec4 a_color;\n"
                        "layout (location = %d) in highp vec2 a_texcoord;\n"
                        "uniform highp mat4 u_matrix;\n"
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
                        "}", VerticeID, ColorID, TexCoordsID),
                    "#version 300 es\n"
                    "layout (location = 0) out highp vec4 oColour;\n"
                    "uniform highp mat4 u_matrix;\n"
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
                    "}");
            }
            void InitShaderGL()
            {
                InitShader(
                    String::Format(
                        "#version 330 core\n"
                        "layout (location = %d) in highp vec2 a_position;\n"
                        "layout (location = %d) in highp vec4 a_color;\n"
                        "layout (location = %d) in highp vec2 a_texcoord;\n"
                        "uniform highp mat4 u_matrix;\n"
                        "out mediump vec4 v_fragmentColor;\n"
                        "out mediump vec2 v_texCoord;\n"
                        "\n"
                        "void main()\n"
                        "{\n"
                        "    gl_Position = u_matrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
                        "    v_fragmentColor = a_color;\n"
                        "    v_texCoord = a_texcoord;\n"
                        "}", VerticeID, ColorID, TexCoordsID),
                    "#version 330 core\n"
                    "layout (location = 0) out highp vec4 oColour;\n"
                    "uniform highp mat4 u_matrix;\n"
                    "uniform highp sampler2D u_texture;\n"
                    "in mediump vec4 v_fragmentColor;\n"
                    "in mediump vec2 v_texCoord;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    oColour = v_fragmentColor * texture2D(u_texture, v_texCoord);\n"
                    "}");
            }
            void TermShader()
            {
                if(QOpenGLContext* ctx = QOpenGLContext::currentContext())
                {
                    QOpenGLFunctions* f = ctx->functions();
                    for(sint32 i = 0; i < 2; ++i)
                    {
                        if(mProgram[i]) {f->glDeleteProgram(mProgram[i]); TestGL(BOSS_DBG 0); mProgram[i] = 0;}
                        if(mVShader[i]) {f->glDeleteShader(mVShader[i]); TestGL(BOSS_DBG 0); mVShader[i] = 0;}
                        if(mFShader[i]) {f->glDeleteShader(mFShader[i]); TestGL(BOSS_DBG 0); mFShader[i] = 0;}
                    }
                }
            }

        public:
            static uint08 GetVersionGLES()
            {
                static uint08 Version = 0xFF;
                if(Version != 0xFF) return Version;

                QOpenGLContext* ctx = QOpenGLContext::currentContext();
                QOpenGLFunctions* f = ctx->functions();
                chars VendorString = (chars) f->glGetString(GL_VENDOR);
                chars RendererString = (chars) f->glGetString(GL_RENDERER);
                chars VersionString = (chars) f->glGetString(GL_VERSION);
                // 예시1: OpenGL ES 2.0 IMGSGX543-124.1
                // 예시2: OpenGL ES 3.0 APPLE-12.0.38
                // 예시3: 2.1 ATI-1.51.8
                // 예시4: 4.0.0 - Build 10.18.10.4303
                BOSS_TRACE("========================================");
                BOSS_TRACE("OpenGLPrivate GetVersion()");
                BOSS_TRACE("----------------------------------------");
                BOSS_TRACE("Vendor: %s", VendorString);
                BOSS_TRACE("Renderer: %s", RendererString);
                BOSS_TRACE("Version: %s", VersionString);
                BOSS_TRACE("========================================");

                if(!boss_strncmp(VersionString, "OpenGL ES ", 10))
                {
                    const uint32 VersionMajor = VersionString[10] - '0';
                    const uint32 VersionMinor = VersionString[12] - '0';
                    Version = ((VersionMajor & 0xF) << 4) | (VersionMinor & 0xF);
                }
                else if(!boss_strcmp(VersionString, "3.0 Mesa 18.0.5"))
                    Version = 0x30;
                else Version = 0x00; // 데스크탑용 OpenGL
                return Version;
            }
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
                mM[0][0] = 1; mM[0][1] = 0; mM[0][2] = 0; mM[0][3] = 0;
                mM[1][0] = 0; mM[1][1] = 1; mM[1][2] = 0; mM[1][3] = 0;
                mM[2][0] = 0; mM[2][1] = 0; mM[2][2] = 1; mM[2][3] = 0;
                mM[3][0] = 0; mM[3][1] = 0; mM[3][2] = 0; mM[3][3] = 1;
            }
            void Multiply(
                const float m00, const float m01, const float m02, const float m03,
                const float m10, const float m11, const float m12, const float m13,
                const float m20, const float m21, const float m22, const float m23,
                const float m30, const float m31, const float m32, const float m33)
            {
                GLfloat m[4][4];
                Memory::Copy(&m[0][0], &mM[0][0], sizeof(GLfloat) * 16);
                mM[0][0] = m[0][0] * m00 + m[0][1] * m10 + m[0][2] * m20 + m[0][3] * m30;
                mM[0][1] = m[0][0] * m01 + m[0][1] * m11 + m[0][2] * m21 + m[0][3] * m31;
                mM[0][2] = m[0][0] * m02 + m[0][1] * m12 + m[0][2] * m22 + m[0][3] * m32;
                mM[0][3] = m[0][0] * m03 + m[0][1] * m13 + m[0][2] * m23 + m[0][3] * m33;
                mM[1][0] = m[1][0] * m00 + m[1][1] * m10 + m[1][2] * m20 + m[1][3] * m30;
                mM[1][1] = m[1][0] * m01 + m[1][1] * m11 + m[1][2] * m21 + m[1][3] * m31;
                mM[1][2] = m[1][0] * m02 + m[1][1] * m12 + m[1][2] * m22 + m[1][3] * m32;
                mM[1][3] = m[1][0] * m03 + m[1][1] * m13 + m[1][2] * m23 + m[1][3] * m33;
                mM[2][0] = m[2][0] * m00 + m[2][1] * m10 + m[2][2] * m20 + m[2][3] * m30;
                mM[2][1] = m[2][0] * m01 + m[2][1] * m11 + m[2][2] * m21 + m[2][3] * m31;
                mM[2][2] = m[2][0] * m02 + m[2][1] * m12 + m[2][2] * m22 + m[2][3] * m32;
                mM[2][3] = m[2][0] * m03 + m[2][1] * m13 + m[2][2] * m23 + m[2][3] * m33;
                mM[3][0] = m[3][0] * m00 + m[3][1] * m10 + m[3][2] * m20 + m[3][3] * m30;
                mM[3][1] = m[3][0] * m01 + m[3][1] * m11 + m[3][2] * m21 + m[3][3] * m31;
                mM[3][2] = m[3][0] * m02 + m[3][1] * m12 + m[3][2] * m22 + m[3][3] * m32;
                mM[3][3] = m[3][0] * m03 + m[3][1] * m13 + m[3][2] * m23 + m[3][3] * m33;
            }

        private:
            GLuint mVShader[2];
            GLuint mFShader[2];
            GLuint mProgram[2];

        private:
            enum {VerticeID = 0, ColorID = 1, TexCoordsID = 2};
            struct Attrib
            {
                GLfloat vertices[2];
                union
                {
                    GLubyte colors[4];
                    GLuint color32;
                };
                GLfloat texcoords[2];
            };
            Attrib mAttrib[4];
            GLint mMatrix[2];
            GLfloat mM[4][4];
            GLint mTextureY;
            GLint mTextureUV;
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
            inline PainterPrivate* painter() {return &mCanvas.painter();}

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
            const ImagePrivate& GetLastImage() const
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
            mutable ImagePrivate mLastImage;
            mutable TextureClass mLastTexture;
        };
    #endif

    class ThreadClass : public QThread
    {
        Q_OBJECT

    public:
        static void Begin(ThreadCB cb, payload data)
        {
            new ThreadClass(cb, nullptr, data);
        }
        static void* BeginEx(ThreadExCB cb, payload data)
        {
            return new ThreadClass(nullptr, cb, data);
        }

    private:
        ThreadClass(ThreadCB cb1, ThreadExCB cb2, payload data)
        {
            BOSS_ASSERT("cb1와 cb2가 모두 nullptr일 순 없습니다", cb1 || cb2);
            m_cb1 = cb1;
            m_cb2 = cb2;
            m_data = data;
            connect(this, SIGNAL(finished()), SLOT(OnFinished()));
            start();
        }
        virtual ~ThreadClass() {}

    private:
        void run() override
        {
            if(m_cb1) m_cb1(m_data);
            else if(m_cb2) m_cb2(m_data);
        }

    private slots:
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
            SoundClass(chars filename, bool loop)
            {
                m_volume = 0;
                m_player = new QMediaPlayer();
                m_playlist = nullptr;
                m_output = nullptr;
                m_outputdevice = nullptr;
                m_outputmutex = nullptr;

                if(loop)
                {
                    m_playlist = new QMediaPlaylist();
                    m_playlist->addMedia(QUrl::fromLocalFile(filename));
                    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
                    m_player->setPlaylist(m_playlist);
                }
                else m_player->setMedia(QUrl::fromLocalFile(filename));
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

                m_volume = 0;
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
                Mutex::Close(m_outputmutex);
            }

        public:
            void Play(float volume_rate)
            {
                m_volume = Math::Max(0, 256 * volume_rate);
                branch;
                jump(m_player)
                {
                    m_player->setVolume(100 * volume_rate);
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
                        Platform::Utility::Sleep(1, false, false);
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
                            return m_volume;
                    }
                    return -1; // Timeout
                }
                return m_volume;
            }

        private:
            sint32 m_volume;
            QMediaPlayer* m_player;
            QMediaPlaylist* m_playlist;
            QAudioOutput* m_output;
            QIODevice* m_outputdevice;
            id_mutex m_outputmutex;
        };
    #endif

    class TCPPeerData : public QObjectUserData
    {
    public:
        const sint32 ID;
        sint32 PacketNeeds;

    public:
        TCPPeerData() : ID(MakeID()) {PacketNeeds = 0;}
        virtual ~TCPPeerData() {}

    public:
        static uint ClassID()
        {static uint _ = QObject::registerUserData(); return _;}
        static int MakeID() {static int _ = -1; return ++_;}
    };

    class TCPPacket
    {
    public:
        const packettype Type;
        const sint32 PeerID;
        buffer Buffer;

    public:
        TCPPacket(packettype type, sint32 peerid, sint32 buffersize)
            : Type(type), PeerID(peerid) {Buffer = Buffer::Alloc(BOSS_DBG buffersize);}
        ~TCPPacket() {Buffer::Free(Buffer);}
    public:
        void DestroyMe() {delete this;}
    };

    class TCPAgent : public QTcpServer
    {
        Q_OBJECT

    private:
        bool UsingSizeField;
        Map<QTcpSocket*> Peers;
        Queue<TCPPacket*> PacketQueue;
        TCPPacket* FocusedPacket;

    private slots:
        void acceptPeer()
        {
            QTcpSocket* Peer = nextPendingConnection();
            TCPPeerData* NewData = new TCPPeerData();
            Peers[NewData->ID] = Peer;
            Peer->setUserData(TCPPeerData::ClassID(), NewData);
            PacketQueue.Enqueue(new TCPPacket(packettype_entrance, NewData->ID, 0));

            if(!UsingSizeField) connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeer()));
            else connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
            connect(Peer, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(errorPeer(QAbstractSocket::SocketError)));
            Platform::BroadcastNotify("entrance", nullptr, NT_SocketReceive);
        }

        void readyPeer()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
            sint64 PacketSize = Peer->bytesAvailable();

            if(0 < PacketSize)
            {
                TCPPacket* NewPacket = new TCPPacket(packettype_message, Data->ID, PacketSize);
                Peer->read((char*) NewPacket->Buffer, PacketSize);
                PacketQueue.Enqueue(NewPacket);
            }
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive);
        }

        void readyPeerWithSizeField()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
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
                        TCPPacket* NewPacket = new TCPPacket(packettype_message, Data->ID, Data->PacketNeeds);
                        Peer->read((char*) NewPacket->Buffer, Data->PacketNeeds);
                        PacketQueue.Enqueue(NewPacket);
                        Data->PacketNeeds = 0;
                    }
                    else break;
                }
            }
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive);
        }

        void errorPeer(QAbstractSocket::SocketError error)
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
            Peers.Remove(Data->ID);

            if(error == QAbstractSocket::RemoteHostClosedError)
            {
                PacketQueue.Enqueue(new TCPPacket(packettype_leaved, Data->ID, 0));
                Platform::BroadcastNotify("leaved", nullptr, NT_SocketReceive);
            }
            else
            {
                PacketQueue.Enqueue(new TCPPacket(packettype_kicked, Data->ID, 0));
                Platform::BroadcastNotify("kicked", nullptr, NT_SocketReceive);
            }
        }

    public:
        TCPAgent(bool sizefield = false)
        {
            UsingSizeField = sizefield;
            FocusedPacket = new TCPPacket(packettype_null, -1, 0);
            connect(this, SIGNAL(newConnection()), this, SLOT(acceptPeer()));
        }

        virtual ~TCPAgent()
        {
            for(TCPPacket* UnusedPacket = nullptr; UnusedPacket = PacketQueue.Dequeue();)
                UnusedPacket->DestroyMe();
            delete FocusedPacket;
        }

        TCPAgent& operator=(const TCPAgent&)
        {
            BOSS_ASSERT("호출불가", false);
            return *this;
        }

    public:
        bool TryPacket()
        {
            TCPPacket* PopPacket = PacketQueue.Dequeue();
            if(!PopPacket) return false;
            delete FocusedPacket;
            FocusedPacket = PopPacket;
            return true;
        }

        TCPPacket* GetFocusedPacket()
        {
            return FocusedPacket;
        }

        bool SendPacket(int peerid, const void* buffer, sint32 buffersize)
        {
            if(QTcpSocket** Peer = Peers.Access(peerid))
                return ((*Peer)->write((chars) buffer, buffersize) == buffersize);
            Peers.Remove(peerid);
            return false;
        }

        bool KickPeer(int peerid)
        {
            if(QTcpSocket** Peer = Peers.Access(peerid))
            {
                (*Peer)->disconnectFromHost();
                Peers.Remove(peerid);
                PacketQueue.Enqueue(new TCPPacket(packettype_kicked, peerid, 0));
                Platform::BroadcastNotify("kicked", nullptr, NT_SocketReceive);
                return true;
            }
            return false;
        }

        bool GetPeerAddress(int peerid, ip4address* ip4, ip6address* ip6, uint16* port)
        {
            if(QTcpSocket** Peer = Peers.Access(peerid))
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
            m_socket = nullptr;
            m_udp = false;
            m_udpip.clear();
            m_udpport = 0;
        }
        virtual ~SocketBox()
        {
            delete m_socket;
        }

    public:
        void Init(bool udp)
        {
            m_socket = (udp)? (QAbstractSocket*) new QUdpSocket() : (QAbstractSocket*) new QTcpSocket();
            connect(m_socket, &QAbstractSocket::connected, this, &SocketBox::OnConnected);
            connect(m_socket, &QAbstractSocket::disconnected, this, &SocketBox::OnDisconnected);
            connect(m_socket, &QAbstractSocket::readyRead, this, &SocketBox::OnReadyRead);
            m_udp = udp;
        }
        bool CheckState(chars name)
        {
            if(!m_udp)
            {
                if(!m_socket->isValid())
                    return false;
                if(m_socket->state() == QAbstractSocket::UnconnectedState)
                    return false;
            }
            return true;
        }

    private slots:
        void OnConnected()
        {
            Platform::BroadcastNotify("connected", nullptr, NT_SocketReceive);
        }
        void OnDisconnected()
        {
            Platform::BroadcastNotify("disconnected", nullptr, NT_SocketReceive);
        }
        void OnReadyRead()
        {
            Platform::BroadcastNotify("message", nullptr, NT_SocketReceive);
        }

    public:
        QAbstractSocket* m_socket;
        bool m_udp;
        QHostAddress m_udpip;
        quint16 m_udpport;

    private:
        class STClass
        {
        public:
            STClass()
            {
                static uint64 LastThreadID = 0;
                m_lastId = (++LastThreadID) << 32;
            }
            ~STClass() {}
        public:
            Map<SocketBox> m_map;
            ublock m_lastId;
        };
        static inline STClass& ST() {return *BOSS_STORAGE_SYS(STClass);}

    public:
        static void Create(id_socket& result, bool udp)
        {
            STClass& CurClass = ST();
            CurClass.m_map[++CurClass.m_lastId].Init(udp);
            result = (id_socket) AnyTypeToPtr(CurClass.m_lastId);
        }
        static SocketBox* Access(id_socket id)
        {
            if(id == nullptr) return nullptr;
            BOSS_ASSERT("타 스레드에서 생성된 소켓입니다", (ST().m_lastId >> 32) == (((ublock) id) >> 32));
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

    class PipePrivate : public QObject
    {
        Q_OBJECT

    public:
        PipePrivate(QSharedMemory* semaphore)
        {
            mStatus = CS_Connecting;
            mTempContext = nullptr;
            mSemaphore = semaphore;
        }
        virtual ~PipePrivate()
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
        QSharedMemory* mSemaphore;
    };

    class PipeServerPrivate : public PipePrivate
    {
        Q_OBJECT

    public:
        PipeServerPrivate(QLocalServer* server, QSharedMemory* semaphore) : PipePrivate(semaphore)
        {
            mServer = server;
            mLastClient = nullptr;
            connect(server, &QLocalServer::newConnection, this, &PipeServerPrivate::OnNewConnection);
        }
        ~PipeServerPrivate() override
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
                connect(mLastClient, &QLocalSocket::readyRead, this, &PipeServerPrivate::OnReadyRead);
                connect(mLastClient, &QLocalSocket::disconnected, this, &PipeServerPrivate::OnDisconnected);
            }
            else NewClient->disconnectFromServer();
        }
        void OnReadyRead()
        {
            mLastClient = (QLocalSocket*) sender();
            if(sint64 PacketSize = mLastClient->bytesAvailable())
                mLastClient->read((char*) mData.AtDumpingAdded(PacketSize), PacketSize);
        }
        void OnDisconnected()
        {
            mStatus = CS_Connecting;
            mLastClient = nullptr;
        }

    private:
        QLocalServer* mServer;
        QLocalSocket* mLastClient;
    };

    class PipeClientPrivate : public PipePrivate
    {
        Q_OBJECT

    public:
        PipeClientPrivate(chars name, QSharedMemory* semaphore) : PipePrivate(semaphore)
        {
            mClient = new QLocalSocket();
            connect(mClient, &QLocalSocket::readyRead, this, &PipeClientPrivate::OnReadyRead);
            connect(mClient, &QLocalSocket::connected, this, &PipeClientPrivate::OnConnected);
            connect(mClient, &QLocalSocket::disconnected, this, &PipeClientPrivate::OnDisconnected);

            mClient->abort();
            mClient->connectToServer(name);
        }
        ~PipeClientPrivate() override
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
        }
        void OnConnected()
        {
            mStatus = CS_Connected;
        }
        void OnDisconnected()
        {
            mStatus = CS_Disconnected;
        }

    private:
        QLocalSocket* mClient;
    };

    class WebEnginePageForExtraDesktop : public QObject
    {
        Q_OBJECT

    public:
        WebEnginePageForExtraDesktop(QObject* parent = nullptr) {}
        virtual ~WebEnginePageForExtraDesktop() {}

    public:
        enum JavaScriptConsoleMessageLevel {InfoMessageLevel, WarningMessageLevel, ErrorMessageLevel};
        typedef std::function<void(const QVariant&)> WebEngineCallbackForExtraDesktop;

    public:
        void runJavaScript(const QString& scriptSource, const WebEngineCallbackForExtraDesktop& resultCallback) {}
        virtual void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID) {}
    };

    class WebEngineViewForExtraDesktop : public QObject
    {
        Q_OBJECT

    public:
        WebEngineViewForExtraDesktop(WidgetPrivate* parent = nullptr)
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
        virtual void closeEvent(CloseEventPrivate* event) {}

    public:
        WebEnginePageForExtraDesktop* mPage;
    };

    #if defined(QT_HAVE_WEBENGINEWIDGETS) & !defined(BOSS_SILENT_NIGHT_IS_ENABLED)
        typedef QWebEnginePage WebEnginePageClass;
        typedef QWebEngineView WebEngineViewClass;
    #else
        typedef WebEnginePageForExtraDesktop WebEnginePageClass;
        typedef WebEngineViewForExtraDesktop WebEngineViewClass;
    #endif

    class WebPagePrivate : public WebEnginePageClass
    {
        Q_OBJECT

    public:
        WebPagePrivate(QObject* parent = nullptr) : WebEnginePageClass(parent)
        {
            mCb = nullptr;
            mData = nullptr;
        }
        ~WebPagePrivate() override {}

    public:
        void SetCallback(Platform::Web::EventCB cb, payload data)
        {
            mCb = cb;
            mData = data;
        }

    private:
        void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID) override
        {
            if(mCb)
                mCb(mData, String::Format("JSConsole:%d", level), message.toUtf8().constData());
        }

    private:
        Platform::Web::EventCB mCb;
        payload mData;
    };

    class WebViewPrivate : public WebEngineViewClass
    {
        Q_OBJECT

    public:
        WebViewPrivate(WidgetPrivate* parent = nullptr) : WebEngineViewClass(parent), mHandle(h_web::null())
        {
            mNowLoading = false;
            mLoadingProgress = 100;
            mLoadingRate = 1;
            mCb = nullptr;
            mData = nullptr;

            setPage(new WebPagePrivate(this));
            setMouseTracking(true);
            connect(this, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));
            connect(this, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
            connect(this, SIGNAL(loadStarted()), SLOT(onLoadStarted()));
            connect(this, SIGNAL(loadProgress(int)), SLOT(onLoadProgress(int)));
            connect(this, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished(bool)));
        }
        virtual ~WebViewPrivate()
        {
            mHandle.set_buf(nullptr);
        }

    public:
        WebViewPrivate(const WebViewPrivate&) {BOSS_ASSERT("사용금지", false);}
        WebViewPrivate& operator=(const WebViewPrivate&) {BOSS_ASSERT("사용금지", false); return *this;}

    public:
        void SetCallback(Platform::Web::EventCB cb, payload data)
        {
            mCb = cb;
            mData = data;
            ((WebPagePrivate*) page())->SetCallback(cb, data);
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
        void closeEvent(CloseEventPrivate* event) Q_DECL_OVERRIDE
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

    #if defined(QT_HAVE_WEBENGINEWIDGETS) & !defined(BOSS_SILENT_NIGHT_IS_ENABLED)
        class WebPrivateForDesktop
        {
        public:
            WebPrivateForDesktop()
            {
                mProxy = mScene.addWidget(&mView);
                mLastTextureGL = nullptr;
            }
            ~WebPrivateForDesktop()
            {
                mScene.removeItem(mProxy);
                mLastTexture.ClearDirectly();
                delete mLastTextureGL;
            }

        public:
            WebPrivateForDesktop(const WebPrivateForDesktop&) {BOSS_ASSERT("사용금지", false);}
            WebPrivateForDesktop& operator=(const WebPrivateForDesktop&) {BOSS_ASSERT("사용금지", false); return *this;}

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
                mView.load(QUrl(QString(url)));
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
                    mLastImage = ImagePrivate(width, height, ImagePrivate::Format_ARGB32);
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
                MouseEventPrivate::Type CurType = MouseEventPrivate::None;
                switch(type)
                {
                case TT_Moving: CurType = MouseEventPrivate::MouseMove; break;
                case TT_Press: CurType = MouseEventPrivate::MouseButtonPress; break;
                case TT_Dragging: CurType = MouseEventPrivate::MouseMove; break;
                case TT_Release: CurType = MouseEventPrivate::MouseButtonRelease; break;
                default: BOSS_ASSERT("해당 case가 준비되지 않았습니다", false);
                }
                MouseEventPrivate NewEvent(CurType, QPoint(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                Q_FOREACH(QObject* obj, mView.page()->view()->children())
                {
                    if(qobject_cast<WidgetPrivate*>(obj))
                        ApplicationPrivate::sendEvent(obj, &NewEvent);
                }
            }
            void SendKeyEvent(sint32 code, chars text, bool pressed)
            {
                if(auto CurWidget = mView.focusProxy())
                {
                    KeyEventPrivate NewEvent((pressed)? KeyEventPrivate::KeyPress : KeyEventPrivate::KeyRelease, code, Qt::NoModifier, text);
                    ApplicationPrivate::sendEvent(CurWidget, &NewEvent);
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
            const PixmapPrivate GetPixmap()
            {
                return PixmapPrivate::fromImage(GetImage());
            }
            const ImagePrivate& GetImage()
            {
                CanvasClass CurCanvas(&mLastImage);
                mView.update();
                mView.render(&CurCanvas.painter());
                return mLastImage;
            }

        private:
            WebViewPrivate mView;
            QGraphicsProxyWidget* mProxy;
            QGraphicsScene mScene;
            ImagePrivate mLastImage;
            TextureClass mLastTexture;
            QOpenGLTexture* mLastTextureGL;
        };
        typedef WebPrivateForDesktop WebPrivate;
    #else
        class WebPrivateForExtraDesktop
        {
        public:
            WebPrivateForExtraDesktop()
            {
            }
            ~WebPrivateForExtraDesktop()
            {
            }

        public:
            WebPrivateForExtraDesktop(const WebPrivateForExtraDesktop&) {BOSS_ASSERT("사용금지", false);}
            WebPrivateForExtraDesktop& operator=(const WebPrivateForExtraDesktop&) {BOSS_ASSERT("사용금지", false); return *this;}

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
                    mLastImage = ImagePrivate(width, height, ImagePrivate::Format_ARGB32);
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
            const PixmapPrivate GetPixmap()
            {
                return PixmapPrivate::fromImage(GetImage());
            }
            const ImagePrivate& GetImage()
            {
                CanvasClass CurCanvas(&mLastImage);
                const QRect CurRect(0, 0, mLastImage.width(), mLastImage.height());
                return mLastImage;
            }

        private:
            ImagePrivate mLastImage;
        };
        typedef WebPrivateForExtraDesktop WebPrivate;
    #endif

    #ifdef QT_HAVE_PURCHASING
        class PurchasePrivate : public QInAppStore
        {
            Q_OBJECT

        public:
            PurchasePrivate(WidgetPrivate* parent = nullptr) : QInAppStore(parent)
            {
                mProduct = nullptr;
                connect(this, SIGNAL(productRegistered(QInAppProduct*)), SLOT(onProductRegistered(QInAppProduct*)));
                connect(this, SIGNAL(productUnknown(QInAppProduct::ProductType, const QString&)), SLOT(onProductUnknown(QInAppProduct::ProductType, const QString&)));
                connect(this, SIGNAL(transactionReady(QInAppTransaction*)), SLOT(onTransactionReady(QInAppTransaction*)));
            }
            ~PurchasePrivate()
            {
            }

        public:
            PurchasePrivate(const PurchasePrivate&) {BOSS_ASSERT("사용금지", false);}
            PurchasePrivate& operator=(const PurchasePrivate&) {BOSS_ASSERT("사용금지", false); return *this;}

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
        class PurchasePrivate : public QObject {Q_OBJECT};
    #endif

    #if BOSS_ANDROID & defined(QT_HAVE_SERIALPORT)
        typedef QSerialPortInfo SerialPortInfoClass;
        class SerialPortForAndroid
        {
        public:
            enum BaudRate {
                Baud1200 = QSerialPort::Baud1200,
                Baud2400 = QSerialPort::Baud2400,
                Baud4800 = QSerialPort::Baud4800,
                Baud9600 = QSerialPort::Baud9600,
                Baud19200 = QSerialPort::Baud19200,
                Baud38400 = QSerialPort::Baud38400,
                Baud57600 = QSerialPort::Baud57600,
                Baud115200 = QSerialPort::Baud115200,
                UnknownBaud = QSerialPort::UnknownBaud};
            enum DataBits {
                Data5 = QSerialPort::Data5,
                Data6 = QSerialPort::Data6,
                Data7 = QSerialPort::Data7,
                Data8 = QSerialPort::Data8,
                UnknownDataBits = QSerialPort::UnknownDataBits};
            enum Parity {
                NoParity = QSerialPort::NoParity,
                EvenParity = QSerialPort::EvenParity,
                OddParity = QSerialPort::OddParity,
                SpaceParity = QSerialPort::SpaceParity,
                MarkParity = QSerialPort::MarkParity,
                UnknownParity = QSerialPort::UnknownParity};
            enum StopBits {
                OneStop = QSerialPort::OneStop,
                OneAndHalfStop = QSerialPort::OneAndHalfStop,
                TwoStop = QSerialPort::TwoStop,
                UnknownStopBits = QSerialPort::UnknownStopBits};
            enum FlowControl {
                NoFlowControl = QSerialPort::NoFlowControl,
                HardwareControl = QSerialPort::HardwareControl,
                SoftwareControl = QSerialPort::SoftwareControl,
                UnknownFlowControl = QSerialPort::UnknownFlowControl};
            enum SerialPortError {
                NoError = QSerialPort::NoError,
                DeviceNotFoundError = QSerialPort::DeviceNotFoundError,
                PermissionError = QSerialPort::PermissionError,
                OpenError = QSerialPort::OpenError,
                ParityError = QSerialPort::ParityError,
                FramingError = QSerialPort::FramingError,
                BreakConditionError = QSerialPort::BreakConditionError,
                WriteError = QSerialPort::WriteError,
                ReadError = QSerialPort::ReadError,
                ResourceError = QSerialPort::ResourceError,
                UnsupportedOperationError = QSerialPort::UnsupportedOperationError,
                UnknownError = QSerialPort::UnknownError,
                TimeoutError = QSerialPort::TimeoutError,
                NotOpenError = QSerialPort::NotOpenError};
        public:
            SerialPortForAndroid(const SerialPortInfoClass& info)
            {
                mPortName = info.portName().toUtf8().constData();
                mLocation = info.systemLocation().toUtf8().constData();
                mBaudRate = UnknownBaud;
                mDataBits = UnknownDataBits;
                mParity = UnknownParity;
                mStopBits = UnknownStopBits;
                mFlowControl = UnknownFlowControl;
                mFd = -1;
            }
            ~SerialPortForAndroid()
            {
                close();
            }

        public:
            bool open(QIODevice::OpenModeFlag flag)
            {
                close();
                int mode = 0;
                switch(flag)
                {
                case QIODevice::ReadOnly: mode = O_RDONLY | O_NOCTTY | O_NONBLOCK; break;
                case QIODevice::WriteOnly: mode = O_WRONLY | O_NOCTTY | O_NONBLOCK; break;
                case QIODevice::ReadWrite: mode = O_RDWR | O_NOCTTY | O_NONBLOCK; break;
                default: BOSS_ASSERT("알 수 없는 flag값입니다", false); return false;
                }
                const int fd = ::open(mLocation, mode);
                BOSS_TRACE("SerialPortForAndroid::open(%s, %d) fd = %d", (chars) mLocation, mode, fd);
                if(fd == -1)
                {
                    if(errno == EINTR) // Recurse because this is a recoverable error
                        return open(flag);
                    return false;
                }
                mFd = reconfigurePort(fd);
                return true;
            }
            void close()
            {
                if(mFd != -1)
                {
                    ::close(mFd);
                    mFd = -1;
                }
            }
            bool waitForReadyRead(int msecs)
            {
                if(mFd != -1)
                {
                    fd_set readfds;
                    FD_ZERO(&readfds);
                    FD_SET(mFd, &readfds);
                    timespec timeout_ts(timespec_from_ms(msecs));
                    if(0 < pselect(mFd + 1, &readfds, nullptr, nullptr, &timeout_ts, nullptr))
                        return (FD_ISSET(mFd, &readfds) != 0);
                }
                return false;
            }
            QByteArray readAll()
            {
                QByteArray Result;
                if(mFd != -1)
                {
                    char buf[1024];
                    ssize_t bytes_read = ::read(mFd, buf, sizeof(buf));
                    if(0 < bytes_read)
                        Result.append(buf, bytes_read);
                }
                return Result;
            }
            qint64 write(const char* data, qint64 len)
            {
                if(mFd == -1) return 0;
                size_t data_focus = 0;
                while(data_focus < len)
                {
                    fd_set writefds;
                    FD_ZERO(&writefds);
                    FD_SET(mFd, &writefds);
                    timespec timeout_ts(timespec_from_ms(10));
                    int ret = pselect(mFd + 1, nullptr, &writefds, nullptr, &timeout_ts, nullptr);
                    if(ret < 0)
                    {
                        if(errno == EINTR) continue;
                        return false;
                    }
                    else if(0 < ret)
                    {
                        if(FD_ISSET(mFd, &writefds) != 0)
                        {
                            const ssize_t bytes_written_now = ::write(mFd, data + data_focus, len - data_focus);
                            if(bytes_written_now < 1) break;
                            data_focus += bytes_written_now;
                        }
                    }
                }
                return data_focus;
            }
            bool flush()
            {
                return (mFd != -1);
            }

        public: // setter
            void setBaudRate(BaudRate baudrate) {mBaudRate = baudrate;}
            void setDataBits(DataBits databits) {mDataBits = databits;}
            void setParity(Parity parity) {mParity = parity;}
            void setStopBits(StopBits stopbits) {mStopBits = stopbits;}
            void setFlowControl(FlowControl flowcontrol) {mFlowControl = flowcontrol;}

        public: // getter
            QString portName() {return (chars) mPortName;}
            QString errorString() {return "NoError";}
            SerialPortError error() {return NoError;}

        private:
            String mPortName;
            String mLocation;
            BaudRate mBaudRate;
            DataBits mDataBits;
            Parity mParity;
            StopBits mStopBits;
            FlowControl mFlowControl;
            int mFd;

        private:
            int reconfigurePort(const int fd)
            {
                struct termios options;
                if(tcgetattr(fd, &options) == -1)
                {
                    ::close(fd);
                    return -1;
                }

                // set up raw mode / no echo / binary
                options.c_cflag |= (tcflag_t)  (CLOCAL | CREAD);
                options.c_lflag &= (tcflag_t) ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN);
                options.c_oflag &= (tcflag_t) ~(OPOST);
                options.c_iflag &= (tcflag_t) ~(INLCR | IGNCR | ICRNL | IGNBRK);
                #ifdef IUCLC
                    options.c_iflag &= (tcflag_t) ~IUCLC;
                #endif
                #ifdef PARMRK
                    options.c_iflag &= (tcflag_t) ~PARMRK;
                #endif

                // setup baud rate
                speed_t baud;
                switch(mBaudRate)
                {
                case Baud1200: baud = B1200; break;
                case Baud2400: baud = B2400; break;
                case Baud4800: baud = B4800; break;
                default: BOSS_ASSERT("알 수 없는 mBaudRate값입니다", false);
                case Baud9600: baud = B9600; break;
                case Baud19200: baud = B19200; break;
                case Baud38400: baud = B38400; break;
                case Baud57600: baud = B57600; break;
                case Baud115200: baud = B115200; break;
                }
                #ifdef _BSD_SOURCE
                    ::cfsetspeed(&options, baud);
                #else
                    ::cfsetispeed(&options, baud);
                    ::cfsetospeed(&options, baud);
                #endif

                // setup char len
                options.c_cflag &= (tcflag_t) ~CSIZE;
                switch(mDataBits)
                {
                case Data5: options.c_cflag |= CS5; break;
                case Data6: options.c_cflag |= CS6; break;
                case Data7: options.c_cflag |= CS7; break;
                default: BOSS_ASSERT("알 수 없는 mDataBits값입니다", false);
                case Data8: options.c_cflag |= CS8; break;
                }

                // setup stopbits
                switch(mStopBits)
                {
                default: BOSS_ASSERT("알 수 없는 mStopBits값입니다", false);
                case OneStop: options.c_cflag &= (tcflag_t) ~(CSTOPB); break;
                case OneAndHalfStop: options.c_cflag |= (CSTOPB); break;
                case TwoStop: options.c_cflag |= (CSTOPB); break;
                }

                // setup parity
                options.c_iflag &= (tcflag_t) ~(INPCK | ISTRIP);
                switch(mParity)
                {
                default: BOSS_ASSERT("알 수 없는 mParity값입니다", false);
                case NoParity: options.c_cflag &= (tcflag_t) ~(PARENB | PARODD); break;
                case EvenParity: options.c_cflag &= (tcflag_t) ~(PARODD); options.c_cflag |= (PARENB); break;
                case OddParity: options.c_cflag |= (PARENB | PARODD); break;
                case SpaceParity: options.c_cflag |= (PARENB | CMSPAR); options.c_cflag &= (tcflag_t) ~(PARODD); break;
                case MarkParity: options.c_cflag |= (PARENB | CMSPAR | PARODD); break;
                }
                #ifndef CMSPAR
                    if(mParity == SpaceParity || mParity == MarkParity)
                        BOSS_ASSERT("OS에서 지원하지 않는 mParity값입니다", false);
                #endif

                // setup flow control
                bool xonxoff, rtscts;
                switch(mFlowControl)
                {
                default: BOSS_ASSERT("알 수 없는 mFlowControl값입니다", false);
                case NoFlowControl: xonxoff = false; rtscts = false; break;
                case HardwareControl: xonxoff = false; rtscts = true; break;
                case SoftwareControl: xonxoff = true; rtscts = false; break;
                }
                // xonxoff
                #ifdef IXANY
                    if(xonxoff) options.c_iflag |= (IXON | IXOFF);
                    else options.c_iflag &= (tcflag_t) ~(IXON | IXOFF | IXANY);
                #else
                    if(xonxoff) options.c_iflag |= (IXON | IXOFF);
                    else options.c_iflag &= (tcflag_t) ~(IXON | IXOFF);
                #endif
                // rtscts
                #ifdef CRTSCTS
                    if(rtscts) options.c_cflag |= (CRTSCTS);
                    else options.c_cflag &= (unsigned long) ~(CRTSCTS);
                #elif defined CNEW_RTSCTS
                    if(rtscts) options.c_cflag |= (CNEW_RTSCTS);
                    else options.c_cflag &= (unsigned long) ~(CNEW_RTSCTS);
                #else
                    BOSS_ASSERT("OS에서 지원하지 않는 rtscts값입니다", false);
                #endif

                options.c_cc[VMIN] = 0;
                options.c_cc[VTIME] = 0;
                if(tcsetattr(fd, TCSANOW, &options) == -1)
                {
                    ::close(fd);
                    return -1;
                }
                return fd;
            }
            timespec timespec_from_ms(const uint32_t millis)
            {
                timespec time;
                time.tv_sec = millis / 1e3;
                time.tv_nsec = (millis - (time.tv_sec * 1e3)) * 1e6;
                return time;
            }
        };
        typedef SerialPortForAndroid SerialPortClass;
    #elif defined(QT_HAVE_SERIALPORT)
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
        class SerialPortForBlank
        {
        public:
            enum BaudRate {Baud115200};
            enum DataBits {Data8};
            enum Parity {NoParity};
            enum StopBits {OneStop};
            enum FlowControl {NoFlowControl};
            enum SerialPortError {NoError};
        public:
            SerialPortForBlank(const SerialPortInfoClass& info)
            {
            }
            ~SerialPortForBlank()
            {
            }
        public:
            bool open(QIODevice::OpenModeFlag flag) {return false;}
            void close() {}
            bool waitForReadyRead(int msecs) {return false;}
            QByteArray readAll() {return QByteArray();}
            qint64 write(const char* data, qint64 len) {return 0;}
            bool flush() {return false;}
        public: // setter
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

    class SerialClass
    {
    private:
        SerialPortClass* mSerial;
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
        SerialClass(chars name, SerialDecodeCB dec, SerialEncodeCB enc)
        {
            mSerial = nullptr;
            mDec = (dec)? dec : DefaultSerialDecode;
            mEnc = (enc)? enc : DefaultSerialEncode;
            mLastDecodedDataFocus = 0;
            const QList<SerialPortInfoClass>& AllPorts = SerialPortInfoClass::availablePorts();
            foreach(const auto& CurPort, AllPorts)
            {
                if(*name == '\0' || CurPort.portName() == name || CurPort.description() == name)
                {
                    mSerial = new SerialPortClass(CurPort);
                    mSerial->setBaudRate(SerialPortClass::Baud115200);
                    mSerial->setDataBits(SerialPortClass::Data8);
                    mSerial->setParity(SerialPortClass::NoParity);
                    mSerial->setStopBits(SerialPortClass::OneStop);
                    mSerial->setFlowControl(SerialPortClass::NoFlowControl);
                    if(!mSerial->open(QIODevice::ReadWrite))
                    {
                        BOSS_TRACE("QSerialPort error: port=%s, error=%s(%d)",
                            mSerial->portName().toUtf8().constData(),
                            mSerial->errorString().toUtf8().constData(),
                            (sint32) mSerial->error());
                        delete mSerial;
                        mSerial = nullptr;
                        if(*name == '\0') continue;
                    }
                    break;
                }
            }
        }

        ~SerialClass()
        {
            delete mSerial;
        }

    public:
        bool IsValid()
        {
            return (mSerial != nullptr);
        }

        bool Connected()
        {
            return (mSerial->error() == SerialPortClass::NoError);
        }

        bool ReadReady(sint32* gettype)
        {
            // 데이터수신시 읽기스트림에 추가연결
            if(mSerial->waitForReadyRead(1))
            {
                QByteArray NewArray = mSerial->readAll();
                Memory::Copy(mReadStream.AtDumpingAdded(NewArray.length()), NewArray.constData(), NewArray.length());
            }

            // 읽기스트림의 처리
            if(0 < mReadStream.Count())
            {
                // 디코딩과정
                mLastDecodedData.SubtractionAll();
                mLastDecodedDataFocus = 0;
                const sint32 UsedLength = mDec(&mReadStream[0], mReadStream.Count(), mLastDecodedData, gettype);
                if(0 < UsedLength)
                {
                    BOSS_ASSERT("SerialDecodeCB의 리턴값이 허용범위를 초과합니다", UsedLength <= mReadStream.Count());
                    const sint32 CopyLength = mReadStream.Count() - UsedLength;
                    if(0 < CopyLength)
                    {
                        uint08s NewReadStream;
                        Memory::Copy(NewReadStream.AtDumping(0, CopyLength), &mReadStream[UsedLength], CopyLength);
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
                    mLastDecodedDataFocus += LastLength;
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
                    mSerial->write((const char*) &NewEncodedData[0], NewEncodedData.Count());
                    mSerial->flush();
                }
            }
        }

    private:
        static sint32 DefaultSerialDecode(bytes data, sint32 length, uint08s& outdata, sint32* outtype)
        {
            Memory::Copy(outdata.AtDumping(0, length), data, length);
            return length;
        }

        static void DefaultSerialEncode(bytes data, sint32 length, uint08s& outdata, sint32 type)
        {
            Memory::Copy(outdata.AtDumping(0, length), data, length);
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
                    return Platform::Graphics::CreateTexture(false, bitmapcache, mLastImageWidth, mLastImageHeight, &mLastImage[0]);
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
                    }
                    //단편화방지차원: mLastImage.Clear();
                }
                Mutex::Unlock(mMutex);
            }

        private:
            virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
            {
                Q_UNUSED(handleType);
                return QList<QVideoFrame::PixelFormat>()
                    << QVideoFrame::Format_RGB32
                    << QVideoFrame::Format_BGR32;
            }

        private slots:
            virtual bool present(const QVideoFrame& frame)
            {
                AddPictureShotCount();
                bool Result = false;
                if(frame.isValid() && CaptureEnabled())
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
                                Memory::Copy(mLastImage.AtDumpingAdded(4 * mLastImageWidth * mLastImageHeight),
                                    ClonedFrame.bits(), 4 * mLastImageWidth * mLastImageHeight);
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
                for(sint32 y = 0, yend = height; y < yend; ++y)
                {
                    Bmp::bitmappixel* CurDst = (Bmp::bitmappixel*) &dstbits[rowbytes * y];
                    bytes CurSrc = (FLIP)? &srcbits[rowbytes * (yend - 1 - y)] : &srcbits[rowbytes * y];
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
            void GetCapturedImage(PixmapPrivate& pixmap, sint32 maxwidth, sint32 maxheight, sint32 rotate)
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
                        ImagePrivate NewImage(&mDecodedBits[0], mDecodedWidth, mDecodedHeight, ImagePrivate::Format_ARGB32);

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
                            MatrixPrivate NewMatrix;
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
            PixmapPrivate mLastPixmap;
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
        class MicrophoneClass : public QAudioProbe
        {
            Q_OBJECT

        private:
            class Data
            {
            public:
                Data() {}
                ~Data() {}
            public:
                Data(const Data& rhs) {operator=(rhs);}
                Data& operator=(const Data& rhs)
                {
                    mPcm = rhs.mPcm;
                    mTimeMsec = rhs.mTimeMsec;
                    return *this;
                }
                operator void*() const {return nullptr;}
                bool operator!() const {return (mPcm.Count() == 0);}
            public:
                uint08s mPcm;
                uint64 mTimeMsec;
            };

        private:
            QAudioRecorder* mRecorder;
            QAudioEncoderSettings mAudioSettings;
            const sint32 mMaxQueueCount;
            Queue<Data> mDataQueue;
            Data mLastData;

        public:
            static Strings GetList(String* spec)
            {
                Strings Result;
                Context SpecCollector;
                const QList<QAudioDeviceInfo>& AllMicrophones = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
                foreach(const auto& CurMicrophone, AllMicrophones)
                {
                    String CurName = CurMicrophone.deviceName().toUtf8().constData();
                    Result.AtAdding() = CurName;
                    if(spec)
                    {
                        Context& NewChild = SpecCollector.At(SpecCollector.LengthOfIndexable());
                        NewChild.At("devicename").Set(CurName);
                        const auto& AllByteOrders = CurMicrophone.supportedByteOrders();
                        foreach(const auto& CurByteOrder, AllByteOrders)
                        {
                            Context& NewChild2 = NewChild.At("byteorders").At(NewChild.At("byteorders").LengthOfIndexable());
                            if(CurByteOrder == QAudioFormat::BigEndian) NewChild2.Set("BigEndian");
                            else if(CurByteOrder == QAudioFormat::LittleEndian) NewChild2.Set("LittleEndian");
                        }
                        const auto& AllChannelCounts = CurMicrophone.supportedChannelCounts();
                        foreach(const auto& CurChannelCount, AllChannelCounts)
                        {
                            Context& NewChild2 = NewChild.At("channelcounts").At(NewChild.At("channelcounts").LengthOfIndexable());
                            NewChild2.Set(String::FromInteger(CurChannelCount));
                        }
                        const auto& AllCodecs = CurMicrophone.supportedCodecs();
                        foreach(const auto& CurCodec, AllCodecs)
                        {
                            Context& NewChild2 = NewChild.At("codecs").At(NewChild.At("codecs").LengthOfIndexable());
                            NewChild2.Set(CurCodec.toUtf8().constData());
                        }
                        const auto& AllSampleRates = CurMicrophone.supportedSampleRates();
                        foreach(const auto& CurSampleRate, AllSampleRates)
                        {
                            Context& NewChild2 = NewChild.At("samplerates").At(NewChild.At("samplerates").LengthOfIndexable());
                            NewChild2.Set(String::FromInteger(CurSampleRate));
                        }
                        const auto& AllSampleSizes = CurMicrophone.supportedSampleSizes();
                        foreach(const auto& CurSampleSize, AllSampleSizes)
                        {
                            Context& NewChild2 = NewChild.At("samplesizes").At(NewChild.At("samplesizes").LengthOfIndexable());
                            NewChild2.Set(String::FromInteger(CurSampleSize));
                        }
                        const auto& AllSampleTypes = CurMicrophone.supportedSampleTypes();
                        foreach(const auto& CurSampleType, AllSampleTypes)
                        {
                            Context& NewChild2 = NewChild.At("sampletypes").At(NewChild.At("sampletypes").LengthOfIndexable());
                            if(CurSampleType == QAudioFormat::Unknown) NewChild2.Set("Unknown");
                            else if(CurSampleType == QAudioFormat::SignedInt) NewChild2.Set("SignedInt");
                            else if(CurSampleType == QAudioFormat::UnSignedInt) NewChild2.Set("UnSignedInt");
                            else if(CurSampleType == QAudioFormat::Float) NewChild2.Set("Float");
                        }
                    }
                }
                if(spec)
                    *spec = SpecCollector.SaveJson(*spec);
                return Result;
            }

        public:
            MicrophoneClass(chars name, sint32 maxcount) : mMaxQueueCount(maxcount)
            {
                mRecorder = new QAudioRecorder();
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
                    delete mRecorder;
                    mRecorder = nullptr;
                    return;
                }
                mRecorder->setAudioInput(SelectedInput);

                mAudioSettings = mRecorder->audioSettings();
                mAudioSettings.setCodec("audio/pcm");
                mAudioSettings.setBitRate(128000);
                mAudioSettings.setChannelCount(2);
                mAudioSettings.setSampleRate(44100);
                mAudioSettings.setQuality(QMultimedia::HighQuality);
                mAudioSettings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
                mRecorder->setAudioSettings(mAudioSettings);

                connect(this, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));
                setSource(mRecorder);
                mRecorder->record();
            }
            ~MicrophoneClass()
            {
                setSource((QMediaRecorder*) nullptr);
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
            const uint08s& GetLastData(uint64* timems) const
            {
                if(timems) *timems = mLastData.mTimeMsec;
                return mLastData.mPcm;
            }
            const QAudioEncoderSettings& GetAudioSettings() const
            {return mAudioSettings;}

        private slots:
            virtual void processBuffer(const QAudioBuffer& buffer)
            {
                // 한계처리
                while(mMaxQueueCount < mDataQueue.Count())
                    mDataQueue.Dequeue();
                // 데이터적재
                if(buffer.isValid())
                {
                    Data NewData;
                    Memory::Copy(NewData.mPcm.AtDumpingAdded(buffer.byteCount()), buffer.constData(), buffer.byteCount());
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
    class MainViewGL : public QGLWidget {Q_OBJECT};
    class MainViewMDI : public QMdiArea {Q_OBJECT};
    class TrayIcon : public QSystemTrayIcon {Q_OBJECT};
    class MainWindow : public QMainWindow {Q_OBJECT};
    class EditTracker : public QLineEdit {Q_OBJECT};
    class ListTracker : public QListWidget {Q_OBJECT};
    class ThreadClass : public QThread {Q_OBJECT};
    class TCPAgent : public QTcpServer {Q_OBJECT};
    class PipePrivate : public QObject {Q_OBJECT};
    class PipeServerPrivate : public PipePrivate {Q_OBJECT};
    class PipeClientPrivate : public PipePrivate {Q_OBJECT};
    class WebEngineViewForExtraDesktop : public QObject {Q_OBJECT};
    class WebViewPrivate : public WebEngineViewClass {Q_OBJECT};
    class PurchasePrivate : public QInAppStore {Q_OBJECT};
    class CameraSurface : public QAbstractVideoSurface {Q_OBJECT};
    class MicrophoneClass : public QAudioProbe {Q_OBJECT};

#endif
