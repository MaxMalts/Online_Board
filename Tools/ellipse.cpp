#include <QGraphicsEllipseItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRectF>
#include <QPointF>
#include <QDebug>

#include "serverapi.h"
#include "serializers.h"
#include "canvas.h"
#include "ellipse.h"


#ifdef JSON_SERIALIZER
bool EllipseItem::deserialize(const QJsonObject& json)
{
    Q_ASSERT(!json.isEmpty());

    QJsonValue size = json.value("size");
    if (!size.isArray()) {
        return false;
    }
    QJsonValue width = size.toArray().at(0);
    QJsonValue height = size.toArray().at(1);
    if (!width.isDouble() || !height.isDouble()) {
        return false;
    }
    setRect(0, 0, width.toDouble(), height.toDouble());

    return true;
}

bool EllipseItem::serialize(QJsonObject& json) const
{
    json = QJsonObject();

    QRectF cur_rect = rect();
    if (cur_rect.isValid()) {
        return false;
    }

    json.insert("size", QJsonArray{ cur_rect.width(), cur_rect.height() });

    return true;
}
#else
static_assert(false, "No serializer defined.");
#endif


void Ellipse::toolDown(const QPointF& pos)
{
    cur_rect.setTopLeft(pos);
    cur_rect.setBottomRight(pos);
}

void Ellipse::toolDragged(const QPointF& pos)
{
    // to do
}

void Ellipse::toolUp(const QPointF& pos)
{
    cur_rect.setBottomRight(pos);
    if (cur_rect.isNull()) {
        return;
    }
    cur_rect = cur_rect.normalized();

    EllipseItem* rect_item = new EllipseItem(cur_rect);

    // Normalizing position to bounding rect
    QPointF item_pos = rect_item->boundingRect().topLeft();
    rect_item->setPos(item_pos);
    cur_rect.translate(-item_pos);
    rect_item->setRect(cur_rect);

    setItem(rect_item);
    sendItem(AddLayerArgs::LayerType::rectangle, rect_item);

    cur_rect = QRectF();
}
