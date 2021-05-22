#include "fake_QQuick.h"

////////////////////////////////////////////////////////////
// fake_QQuickItem
////////////////////////////////////////////////////////////

fake_QQuickItem::fake_QQuickItem(QObject* parent)
{
}

fake_QQuickItem::~fake_QQuickItem()
{
}

void fake_QQuickItem::setAntialiasing(bool enabled)
{
}

void fake_QQuickItem::setSmooth(bool enabled)
{
}

void fake_QQuickItem::setWidth(qreal size)
{
}

void fake_QQuickItem::setHeight(qreal size)
{
}

bool fake_QQuickItem::isComponentComplete() const
{
    return false;
}

void fake_QQuickItem::update()
{
}

void fake_QQuickItem::paint(QPainter* painter)
{
}

void fake_QQuickItem::componentComplete()
{
}

////////////////////////////////////////////////////////////
// fake_QQuickPaintedItem
////////////////////////////////////////////////////////////

fake_QQuickPaintedItem::fake_QQuickPaintedItem(QObject* parent)
    : fake_QQuickItem(parent)
{
}

fake_QQuickPaintedItem::~fake_QQuickPaintedItem()
{
}

void fake_QQuickPaintedItem::setRenderTarget(RenderTarget target)
{
}

////////////////////////////////////////////////////////////
// fake_QQmlFile
////////////////////////////////////////////////////////////

fake_QQmlFile::fake_QQmlFile(QUrl url)
{
}

fake_QQmlFile::fake_QQmlFile(const fake_QQmlFile& rhs)
{
}

fake_QQmlFile::~fake_QQmlFile()
{
}

bool fake_QQmlFile::isLoading() const
{
    return false;
}

bool fake_QQmlFile::isReady() const
{
    return false;
}

bool fake_QQmlFile::isError() const
{
    return false;
}

const QByteArray fake_QQmlFile::dataByteArray() const
{
    return QByteArray();
}

void fake_QQmlFile::connectFinished(QObject* sender, const char* signal)
{
}

#include <boss.h>

// QTCREATOR가 아닌 경우 MOC코드 포함
#if !defined(BOSS_QTCREATOR) || BOSS_QTCREATOR != 1
    #if BOSS_NDEBUG
        #if BOSS_X64
            #include "../../../../../project/GeneratedFiles/Release64/moc_fake_QQuick.cpp"
        #else
            #include "../../../../../project/GeneratedFiles/Release32/moc_fake_QQuick.cpp"
        #endif
    #else
        #if BOSS_X64
            #include "../../../../../project/GeneratedFiles/Debug64/moc_fake_QQuick.cpp"
        #else
            #include "../../../../../project/GeneratedFiles/Debug32/moc_fake_QQuick.cpp"
        #endif
    #endif
#endif
