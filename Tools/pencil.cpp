#include <QGraphicsPathItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainterPath>
#include <QPointF>
#include <QDebug>

#include "ServerApi/serverapi.h"
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

void PencilItem::addVertex(const QPointF& vertex)
{
    vertices.append(vertex);
    verticesToPath();
}

void PencilItem::translateVertices(const QPointF& offset)
{
    vertices.translate(offset);
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
    Q_ASSERT(cur_item == nullptr);

    cur_item = new PencilItem();
    cur_item->addVertex(pos);
    canvas->addPreviewItem(Canvas::ItemType::pencil, cur_item);
}

void Pencil::toolDragged(const QPointF& pos)
{
    cur_item->addVertex(pos);
}

void Pencil::toolUp(const QPointF& pos)
{
    cur_item->addVertex(pos);

    // Normalizing position to bounding rect
    QPointF item_pos = cur_item->boundingRect().topLeft();
    cur_item->setPos(item_pos);
    cur_item->translateVertices(-item_pos);

    bool ret = canvas->fixPreviewItem(cur_item);
    Q_ASSERT(ret);

    cur_item = nullptr;
}
