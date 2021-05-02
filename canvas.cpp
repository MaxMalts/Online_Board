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

    active_tool = tools.at(0);
    active_tool->activate();
}

void Canvas::resize(QSize size)
{
    setSceneRect(-size.width() / 2, -size.height() / 2, size.width(), size.height());
    QGraphicsView::resize(size);
}

bool Canvas::setActiveTool(Canvas::ToolType tool)
{
    int tool_ind = 0;
    switch (tool) {
    case pencil:
        return false;
    case line:
        tool_ind = 0;
        break;
    case rectangle:
        tool_ind = 1;
        break;
    default:
        Q_ASSERT(false);
        return false;
    }

    Tool* new_tool = tools.at(tool_ind);
    if (active_tool != new_tool) {
        active_tool->inactivate();
        active_tool = new_tool;
        active_tool->activate();
    }

    return true;
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
