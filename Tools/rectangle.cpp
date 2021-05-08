#include <QGraphicsRectItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRectF>
#include <QPointF>
#include <QDebug>

#include "ServerApi/serverapi.h"
#include "serializers.h"
#include "canvas.h"
#include "rectangle.h"


#ifdef JSON_SERIALIZER
bool RectangleItem::deserialize(const QJsonObject& json)
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

bool RectangleItem::serialize(QJsonObject& json) const
{
    json = QJsonObject();

    QRectF cur_rect = rect();
    if (!cur_rect.isValid()) {
        return false;
    }

    json.insert("size", QJsonArray{ cur_rect.width(), cur_rect.height() });

    return true;
}
#else
static_assert(false, "No serializer defined.");
#endif


void Rectangle::toolDown(const QPointF& pos)
{
    Q_ASSERT(cur_item == nullptr);

    cur_rect.setTopLeft(pos);
    cur_rect.setBottomRight(pos);

    cur_item = new RectangleItem(cur_rect);
    canvas->addPreviewItem(Canvas::ItemType::rectangle, cur_item);
}

void Rectangle::toolDragged(const QPointF& pos)
{
    cur_rect.setBottomRight(pos);
    cur_item->setRect(cur_rect.normalized());
}

void Rectangle::toolUp(const QPointF& pos)
{
    cur_rect.setBottomRight(pos);
    if (cur_rect.isNull()) {
        bool ret = canvas->deletePreviewItem(cur_item);
        Q_ASSERT(ret);

        cur_item = nullptr;
        return;
    }
    cur_rect = cur_rect.normalized();

    cur_item->setRect(cur_rect);

    // Normalizing position to bounding rect
    QPointF item_pos = cur_item->boundingRect().topLeft();
    cur_item->setPos(item_pos);
    cur_rect.translate(-item_pos);
    cur_item->setRect(cur_rect);

    bool ret = canvas->fixPreviewItem(cur_item);
    Q_ASSERT(ret);

    cur_rect = QRectF();
    cur_item = nullptr;
}
