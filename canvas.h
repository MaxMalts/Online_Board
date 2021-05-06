#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QDebug>

#include "Tools/tool.h"
#include "Tools/pencil.h"
#include "Tools/line.h"
#include "Tools/rectangle.h"
#include "Tools/ellipse.h"
#include "serializers.h"
#include "common.h"


class Canvas : public QGraphicsView
{
    Q_OBJECT

public:
    enum ToolType {
        undefined,
        pencil,
        line,
        rectangle,
        ellipse,
    };

    Canvas(QSize size = QSize(500, 500), QWidget* parent = nullptr);

    void resize(QSize size);

    bool setActiveTool(ToolType tool);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    // pos is position on the scene
    void mouseDown(const QPointF& pos);
    void mouseDragged(const QPointF& pos);
    void mouseUp(const QPointF& pos);

private slots:
    void onLayerReceived(const Serializer& argument);

private:
    void addItem(QGraphicsItem* item);

    QGraphicsScene gscene;

    Tool* active_tool = nullptr;
    const QList<Tool*> tools {
        new Pencil(this, this),
        new Line(this, this),
        new Rectangle(this, this),
        new Ellipse(this, this)
    };

    friend class Tool;
};

#endif // CANVAS_H
