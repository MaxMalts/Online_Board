#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QGraphicsItem>
#include <QString>

#include "serverapi.h"


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
    virtual void toolDown(const QPointF& pos);
    virtual void toolDragged(const QPointF& pos);
    virtual void toolUp(const QPointF& pos);

protected:
    virtual void toolActivated();
    virtual void toolInactivated();

    void setItem(QGraphicsItem* item);
    void sendItem(AddLayerArgs::LayerType layer_type, QGraphicsItem* item);

    Canvas* canvas = nullptr;

private:
    bool mouse_pressed = false;
};

#endif // TOOL_H
