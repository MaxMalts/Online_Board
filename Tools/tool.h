#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QColor>
#include <QVector2D>

class Canvas;

class Tool : public QObject
{
    Q_OBJECT
    friend class Canvas;

public:
    explicit Tool(Canvas* canvas, QObject* parent = nullptr);

    void activate();
    void inactivate();

    virtual ~Tool();

private slots:
    virtual void toolDown(const QVector2D& pos) = 0;
    virtual void toolMoved(const QVector2D& pos) = 0;
    virtual void toolUp(const QVector2D& pos) = 0;

private:
    virtual void toolActivated() = 0;
    virtual void toolInactivated() = 0;

    Canvas* canvas = nullptr;
};

#endif // TOOL_H
