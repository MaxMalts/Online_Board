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
    Tool(Canvas* canvas, QObject* parent = nullptr);

    void activate();
    void inactivate();

    virtual ~Tool() = default;

private slots:
    void onMouseDown(const QVector2D& pos);
    void onMouseMoved(const QVector2D& pos);
    void onMouseUp(const QVector2D& pos);

private:
    virtual void toolActivated();
    virtual void toolDown(const QVector2D& pos);
    virtual void toolDragged(const QVector2D& pos);
    virtual void toolMovedIdle(const QVector2D& pos);
    virtual void toolUp(const QVector2D& pos);
    virtual void toolInactivated();

    Canvas* canvas = nullptr;

    bool mouse_pressed = false;
};

#endif // TOOL_H
