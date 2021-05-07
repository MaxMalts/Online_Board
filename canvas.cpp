#include "canvas.h"

#include <QMouseEvent>
#include <QString>
#include <QDebug>

#include "serverapi.h"
#include "common.h"


/* Canvas */

Canvas::Canvas(QSize size, QWidget* parent)
    : QGraphicsView(parent),
      gscene(this)
{
    setScene(&gscene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resize(size);

    connect(ServerApi::getInstance(), SIGNAL(cAddLayer(const Serializer&)),
            this, SLOT(onLayerReceived(const Serializer&)));

    active_tool = tools.atL(ToolType::pencil);
    active_tool->activate();
}

void Canvas::resize(QSize size)
{
    setSceneRect(-size.width() / 2, -size.height() / 2, size.width(), size.height());
    QGraphicsView::resize(size);
}

void Canvas::setActiveTool(Canvas::ToolType tool)
{
    Tool* new_tool = tools.atL(tool);
    if (active_tool != new_tool) {
        active_tool->inactivate();
        active_tool = new_tool;
        active_tool->activate();
    }
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    emit mouseDown(mapToScene(event->pos()));
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    emit mouseDragged(mapToScene(event->pos()));
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    emit mouseUp(mapToScene(event->pos()));
}

void Canvas::onLayerReceived(const Serializer& argument)
{
    AddLayerArgs layer_info;
    argument.deserialize(layer_info);

    QGraphicsItem* item = nullptr;
    switch (layer_info.layer_type) {
    case AddLayerArgs::LayerType::line: {
        LineItem* line_item = new LineItem;
        layer_info.layer_data.deserialize(*line_item);
        item = line_item;
        break;
    }
    case AddLayerArgs::LayerType::pencil: {
        PencilItem* pencil_item = new PencilItem;
        layer_info.layer_data.deserialize(*pencil_item);
        item = pencil_item;
        break;
    }
    case AddLayerArgs::LayerType::rectangle: {
        RectangleItem* rectangle_item = new RectangleItem;
        layer_info.layer_data.deserialize(*rectangle_item);
        item = rectangle_item;
        break;
    }
    case AddLayerArgs::LayerType::ellipse: {
        EllipseItem* ellipse_item = new EllipseItem;
        layer_info.layer_data.deserialize(*ellipse_item);
        item = ellipse_item;
        break;
    }
    default:
        Q_ASSERT(false);
    }

    item->setPos(layer_info.position);
    item->setScale(layer_info.scale);

    addItem(item);
}

void Canvas::addItem(QGraphicsItem* item)
{
    gscene.addItem(item);  // Takes ownership
}
