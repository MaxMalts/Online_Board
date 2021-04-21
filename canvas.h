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


struct AddLayerArgs : public Serializable {
public:
    enum LayerType {
        undefined,
        line
    };

#ifdef JSON_SERIALIZER
    bool serialize(QJsonObject& json) const override;
    bool deserialize(const QJsonObject& json) override;

    static const BiMap<QString, LayerType> str_layertype_map;
    JsonSerializer layer_data;
#else
    static_assert(false, "No serializer defined.");
#endif

    QPointF position;
    QSizeF size;
    LayerType layer_type = undefined;
};


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
