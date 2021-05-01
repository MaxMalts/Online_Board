#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QVector2D>
#include <QPointF>
#include <QSizeF>
#include <QDebug>

#include "Tools/tool.h"
#include "Tools/line.h"
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
    };

    Canvas(QSize size = QSize(500, 500), QWidget *parent = nullptr);

    bool setActiveTool(ToolType tool);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    // pos is position on the scene
    void mouseDown(const QVector2D& pos);
    void mouseDragged(const QVector2D& pos);
    void mouseUp(const QVector2D& pos);

private:
    void addItem(QGraphicsItem* item);

    QGraphicsScene gscene;

    Tool* active_tool = nullptr;
    const QList<Tool*> tools {
        new Line(this, this)
    };

    friend class Tool;
};

#endif // CANVAS_H
