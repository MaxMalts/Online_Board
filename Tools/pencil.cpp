#include <QGraphicsPathItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainterPath>
#include <QPointF>
#include <QDebug>

#include "serverapi.h"
#include "serializers.h"
#include "canvas.h"
#include "pencil.h"


PencilItem::PencilItem(const QPolygonF& vertices) : vertices(vertices)
{
    verticesToPath();
}

PencilItem::PencilItem(const QPolygonF&& vertices) : vertices(vertices)
{
    verticesToPath();
}

void PencilItem::setVertices(const QPolygonF& vertices) {
    this->vertices = vertices;
    verticesToPath();
}

void PencilItem::setVertices(const QPolygonF&& vertices)
{
    this->vertices = vertices;
    verticesToPath();
}

#ifdef JSON_SERIALIZER
bool PencilItem::deserialize(const QJsonObject& json)
{
    Q_ASSERT(!json.isEmpty());

    QPolygonF new_vertices;

    QJsonValue json_vertices = json.value("coordinates");
    if (!json_vertices.isArray()) {
        return false;
    }

    new_vertices.reserve(json_vertices.toArray().size());

    for (const QJsonValue& vertex : json_vertices.toArray()) {
        if (!vertex.isArray()) {
            return false;
        }

        QJsonValue x = vertex.toArray().at(0);
        QJsonValue y = vertex.toArray().at(1);
        if (!x.isDouble() || !y.isDouble()) {
            return false;
        }

        new_vertices.append(QPointF(x.toDouble(), y.toDouble()));
    }

    vertices = new_vertices;
    verticesToPath();

    return true;
}

bool PencilItem::serialize(QJsonObject& json) const
{
    json = QJsonObject();

    if (vertices.isEmpty()) {
        return false;
    }

    QJsonArray json_vertices;
    for (const QPointF& vertex : vertices) {
        json_vertices.append(QJsonArray{ vertex.x(), vertex.y() });
    }

    json.insert("coordinates", json_vertices);
    return true;
}
#else
    static_assert(false, "No serializer defined.");
#endif

void PencilItem::verticesToPath()
{
    QPainterPath path;
    path.addPolygon(vertices);
    setPath(path);
}


void Pencil::toolDown(const QPointF& pos)
{
    Q_ASSERT(cur_vertices.isEmpty());

    cur_vertices.push_back(pos);
}

void Pencil::toolDragged(const QPointF& pos)
{
    cur_vertices.push_back(pos);
}

void Pencil::toolUp(const QPointF& pos)
{
    cur_vertices.push_back(pos);

    PencilItem* pencil_item = new PencilItem(cur_vertices);

    // Normalizing position to bounding rect
    QPointF item_pos = pencil_item->boundingRect().topLeft();
    pencil_item->setPos(item_pos);
    cur_vertices.translate(-item_pos);
    pencil_item->setVertices(cur_vertices);

    setItem(pencil_item);
    sendItem(AddLayerArgs::LayerType::pencil, pencil_item);

    cur_vertices.clear();
}
