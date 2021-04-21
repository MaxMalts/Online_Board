#include <QDebug>

#include <QGraphicsLineItem>
#include <QJsonDocument>
#include <QLineF>
#include <QPoint>
#include "serverapi.h"
#include "serializers.h"
#include "line.h"
#include "canvas.h"


#ifdef JSON_SERIALIZER
bool LineItem::deserialize(const QJsonObject& json)
{
    return false;
}

bool LineItem::serialize(QJsonObject& json) const
{
    return false;
}
#else
static_assert(false, "No serializer defined.");
#endif



void Line::toolDown(const QVector2D& pos)
{
    cur_line.setP1(pos.toPointF());
    qDebug() << "Line down.";
}

void Line::toolDragged(const QVector2D& pos)
{
    // to do
    qDebug() << "Line dragged.";
}

void Line::toolUp(const QVector2D& pos)
{
    cur_line.setP2(pos.toPointF());

    LineItem* line_item = new LineItem(cur_line);

    // Normalizing position to bounding rect
    QPointF item_pos = line_item->boundingRect().topLeft();
    line_item->setPos(item_pos);
    cur_line.translate(-item_pos);
    line_item->setLine(cur_line);

    setItem(line_item);
    sendItem("line", line_item);

    cur_line = QLineF();
    qDebug() << "Line up.";
}
