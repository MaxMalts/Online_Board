#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QColor>
#include <QPointF>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QString>

#include "ServerApi/serverapi.h"


class Canvas;

class Tool : public QObject
{
    Q_OBJECT

public:
    Tool(Canvas* canvas, QLayout* props_container, QObject* parent = nullptr);

    void activate();
    void inactivate();

    virtual ~Tool();

protected slots:
    virtual void toolDown(const QPointF& pos);
    virtual void toolDragged(const QPointF& pos);
    virtual void toolUp(const QPointF& pos);
    virtual void toolScrolled(QWheelEvent* event);

protected:
    virtual void toolActivated();
    virtual void toolInactivated();

    void setPropsWidget(QWidget* props_widget);

    Canvas* canvas = nullptr;

private:
    QLayout* props_container = nullptr;
    QWidget* props_widget = nullptr;

    bool mouse_pressed = false;
};

#endif // TOOL_H
