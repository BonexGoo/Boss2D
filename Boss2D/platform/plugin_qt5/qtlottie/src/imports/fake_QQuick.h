#pragma once

#include <QPainter>
#include <QUrl>

class fake_QQuickItem : public QObject
{
    Q_OBJECT

public:
    fake_QQuickItem(QObject* parent);
    virtual ~fake_QQuickItem();

public:
    void setAntialiasing(bool enabled);
    void setSmooth(bool enabled);
    void setWidth(qreal size);
    void setHeight(qreal size);
    bool isComponentComplete() const;
    void update();

public:
    virtual void paint(QPainter* painter);
    virtual void componentComplete();
};

class fake_QQuickPaintedItem : public fake_QQuickItem
{
    Q_OBJECT

public:
    fake_QQuickPaintedItem(QObject* parent);
    virtual ~fake_QQuickPaintedItem();

public:
    enum RenderTarget {Image, FramebufferObject, InvertedYFramebufferObject};

public:
    void setRenderTarget(RenderTarget target);
};

class fake_QQmlFile : public QObject
{
    Q_OBJECT

public:
    fake_QQmlFile(QUrl url);
    fake_QQmlFile(const fake_QQmlFile& rhs);
    virtual ~fake_QQmlFile();

public:
    bool isLoading() const;
    bool isReady() const;
    bool isError() const;
    const QByteArray dataByteArray() const;
    void connectFinished(QObject* sender, const char* signal);
};
