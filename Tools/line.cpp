#include <QDebug>

#include <QGraphicsLineItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLineF>
#include <QPoint>
#include "serverapi.h"
#include "serializers.h"
#include "line.h"
#include "canvas.h"


#ifdef JSON_SERIALIZER
bool LineItem::deserialize(const QJsonObject& json)
{
    Q_ASSERT(!json.isEmpty());

    QLineF new_line;

    QJsonValue cur_value = json.value("first_point");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue x = cur_value.toArray().at(0);
    QJsonValue y = cur_value.toArray().at(1);
    if (!x.isDouble() || !y.isDouble()) {
        return false;
    }
    new_line.setP1(QPointF(x.toDouble(), y.toDouble()));

    cur_value = json.value("second_point");
    if (!cur_value.isArray()) {
        return false;
    }
    x = cur_value.toArray().at(0);
    y = cur_value.toArray().at(1);
    if (!x.isDouble() || !y.isDouble()) {
        return false;
    }
    new_line.setP2(QPointF(x.toDouble(), y.toDouble()));

    setLine(new_line);

    return true;
}

bool LineItem::serialize(QJsonObject& json) const
{
    json = QJsonObject();

    QLineF cur_line = line();
    if (cur_line.isNull()) {
        return false;
    }

    json.insert("first_point", QJsonArray{ cur_line.p1().x(), cur_line.p1().y() });
    json.insert("second_point", QJsonArray{ cur_line.p2().x(), cur_line.p2().y() });

    return true;
}
#else
static_assert(false, "No serializer defined.");
#endif



void Line::toolDown(const QVector2D& pos)
{
    cur_line.setP1(pos.toPointF());
}

void Line::toolDragged(const QVector2D& pos)
{
    // to do
}

void Line::toolUp(const QVector2D& pos)
{
    cur_line.setP2(pos.toPointF());
    if (cur_line.p1() == cur_line.p2()) {
        return;
    }

    LineItem* line_item = new LineItem(cur_line);

    // Normalizing position to bounding rect
    QPointF item_pos = line_item->boundingRect().topLeft();
    line_item->setPos(item_pos);
    cur_line.translate(-item_pos);
    line_item->setLine(cur_line);

    setItem(line_item);
    sendItem(AddLayerArgs::LayerType::line, line_item);

    cur_line = QLineF();
}
