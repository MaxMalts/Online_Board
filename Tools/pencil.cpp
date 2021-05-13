#include <QGraphicsPathItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainterPath>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QDebug>

#include "ServerApi/serverapi.h"
#include "serializers.h"
#include "canvas.h"
#include "ui_pencilprops.h"
#include "pencil.h"


/* PencilItem */

PencilItem::PencilItem()
{
    setPen(defaultPen());
}

PencilItem::PencilItem(const QPolygonF& vertices) : vertices(vertices)
{
    setPen(defaultPen());
    verticesToPath();
}

PencilItem::PencilItem(const QPolygonF&& vertices) : vertices(vertices)
{
    setPen(defaultPen());
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

    // vertices
    QJsonValue cur_value = json.value("coordinates");
    if (!cur_value.isArray()) {
        return false;
    }
    QPolygonF new_vertices;
    new_vertices.reserve(cur_value.toArray().size());

    for (const QJsonValue& vertex : cur_value.toArray()) {
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

    QPen new_pen = defaultPen();

    // color
    cur_value = json.value("color");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue r = cur_value.toArray().at(0);
    QJsonValue g = cur_value.toArray().at(1);
    QJsonValue b = cur_value.toArray().at(2);
    QJsonValue a = cur_value.toArray().at(3);
    if (!a.isDouble() || !r.isDouble() ||
        !g.isDouble() || !b.isDouble()) {
        return false;
    }
    new_pen.setColor(QColor(r.toInt(), g.toInt(),
                            b.toInt(), a.toInt()));

    // width
    cur_value = json.value("width");
    if (!cur_value.isDouble()) {
        return false;
    }
    new_pen.setWidthF(cur_value.toDouble());

    setPen(new_pen);
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

QPen PencilItem::defaultPen()
{
    return QPen(QBrush(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
}

void PencilItem::verticesToPath()
{
    QPainterPath path;
    path.addPolygon(vertices);
    setPath(path);
}


/* PencilProps */

PencilProps::PencilProps(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PencilProps)
{
    ui->setupUi(this);
    width = ui->widthSlider->value();
}

PencilProps::~PencilProps()
{
    delete ui;
}

qreal PencilProps::getWidth() const
{
    return width;
}

void PencilProps::onWidthSliderChanged(int value)
{
    width = value;
}


/* Pencil */

Pencil::Pencil(Canvas* canvas, QLayout* props_container, QObject* parent)
    : Tool(canvas, props_container, parent),
      pencil_props(new PencilProps)
{
    setPropsWidget(pencil_props);
}

void Pencil::toolDown(const QPointF& pos)
{
    Q_ASSERT(cur_item == nullptr);

    cur_item = new PencilItem();
    QPen pen = cur_item->pen();
    pen.setColor(canvas->activeColor());
    pen.setWidthF(pencil_props->getWidth());
    cur_item->setPen(pen);

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
