#include "canvas.h"

#include <QMouseEvent>
#include <QString>
#include <QVariant>
#include <QDebug>

#include "ServerApi/serverapi.h"
#include "ServerApi/addlayerargs.h"
#include "ServerApi/confirmaddlayerargs.h"
#include "serializers.h"
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
    connect(ServerApi::getInstance(), SIGNAL(cConfirmAddLayer(const Serializer&)),
            this, SLOT(onLayerConfirmed(const Serializer&)));

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
    switch (layer_info.item_type) {
    case ItemType::line: {
        LineItem* line_item = new LineItem;
        layer_info.layer_data.deserialize(*line_item);
        item = line_item;
        break;
    }
    case ItemType::pencil: {
        PencilItem* pencil_item = new PencilItem;
        layer_info.layer_data.deserialize(*pencil_item);
        item = pencil_item;
        break;
    }
    case ItemType::rectangle: {
        RectangleItem* rectangle_item = new RectangleItem;
        layer_info.layer_data.deserialize(*rectangle_item);
        item = rectangle_item;
        break;
    }
    case ItemType::ellipse: {
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

    item->setData(ItemDataInd::id, layer_info.layer_id);
    item->setData(ItemDataInd::item_type, QVariant::fromValue(layer_info.item_type));
    gscene.addItem(item);
}

void Canvas::onLayerConfirmed(const Serializer& argument)
{
    Q_ASSERT(!pending_add_confirm.isEmpty());

    ConfirmAddLayerArgs layer_id;
    argument.deserialize(layer_id);

    QGraphicsItem* item = pending_add_confirm.dequeue();
    item->setData(ItemDataInd::id, layer_id.layer_id);
}

void Canvas::addItem(ItemType type, QGraphicsItem* item)
{
    Q_ASSERT(item != nullptr);

    item->setData(ItemDataInd::item_type, QVariant::fromValue(type));
    gscene.addItem(item);

    sendItem(item);
}

bool Canvas::deleteItem(QGraphicsItem* item)
{
    Q_ASSERT(item != nullptr);

    if (!deleteFromScene(item)) {
        return false;
    }

    // to do sending info to server
    return true;
}

void Canvas::addPreviewItem(Canvas::ItemType type, QGraphicsItem* item)
{
    Q_ASSERT(item != nullptr);

    item->setData(ItemDataInd::is_preview, true);
    item->setData(ItemDataInd::item_type, QVariant::fromValue(type));
    gscene.addItem(item);
}

bool Canvas::fixPreviewItem(QGraphicsItem* item)
{
    Q_ASSERT(item != nullptr);

    if (!item->data(ItemDataInd::is_preview).toBool()) {
        return false;
    }

    item->setData(ItemDataInd::is_preview, false);
    sendItem(item);
    return true;
}

bool Canvas::deletePreviewItem(QGraphicsItem* item)
{
    Q_ASSERT(item != nullptr);

    if (!item->data(ItemDataInd::is_preview).toBool()) {
        return false;
    }

    deleteFromScene(item);
    return true;
}

void Canvas::redrawRect(const QRectF& rect)
{
    gscene.invalidate(rect);
}

void Canvas::sendItem(QGraphicsItem* item)
{
    Q_ASSERT(item != nullptr);

    AddLayerArgs args;
    args.layer_id = 0;
    args.position = item->scenePos();
    args.scale = item->scale();
    args.item_type = item->data(ItemDataInd::item_type).value<ItemType>();
    args.layer_data.serialize(dynamic_cast<Serializable&>(*item));

#ifdef JSON_SERIALIZER
    JsonSerializer argument(args);
#else
static_assert(false, "No serializer defined.");
#endif

    pending_add_confirm.enqueue(item);
    ServerApi::sAddLayer(argument);
}

bool Canvas::deleteFromScene(QGraphicsItem* item)
{
    Q_ASSERT(item != nullptr);

    if (!gscene.items().contains(item)) {
        return false;
    }

    gscene.removeItem(item);
    delete item;

    return true;
}
