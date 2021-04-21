#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QColor>
#include <QVector2D>
#include <QGraphicsItem>
#include <QString>


class Canvas;

class Tool : public QObject
{
    Q_OBJECT

public:
    Tool(Canvas* canvas, QObject* parent = nullptr);

    void activate();
    void inactivate();

    virtual ~Tool() = default;

protected slots:
    virtual void toolDown(const QVector2D& pos);
    virtual void toolDragged(const QVector2D& pos);
    virtual void toolUp(const QVector2D& pos);

protected:
    virtual void toolActivated();
    virtual void toolInactivated();

    void setItem(QGraphicsItem* item);
    void sendItem(QString tool_name, QGraphicsItem* item);

    Canvas* canvas = nullptr;

private:
    bool mouse_pressed = false;
};

#endif // TOOL_H
