#include "canvas.h"

#include <QMouseEvent>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "serverapi.h"
#include "common.h"

/* AddLayerArgs */

const BiMap<QString, AddLayerArgs::LayerType> AddLayerArgs::str_layertype_map {
    { "line", AddLayerArgs::LayerType::line }
};

#ifdef JSON_SERIALIZER
bool AddLayerArgs::serialize(QJsonObject& json) const
{
    if (layer_data.isNull()) {
        return false;
    }

    json = QJsonObject();
    json.insert("position", QJsonArray{ position.x(), position.y() });
    json.insert("size", QJsonArray{ size.width(), size.height() });
    json.insert("layer_type", str_layertype_map.leftByRight(layer_type));

    QJsonObject layer_data_json = layer_data.getJson().object();
    Q_ASSERT(!layer_data_json.isEmpty());
    json.insert("layer_data", layer_data_json);

    return true;
}

bool AddLayerArgs::deserialize(const QJsonObject& json)
{
    // position
    QJsonValue cur_value = json.value("position");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue x = cur_value.toArray().at(0);
    QJsonValue y = cur_value.toArray().at(1);
    if (!x.isDouble() || !y.isDouble()) {
        return false;
    }
    position = QPointF(x.toDouble(), y.toDouble());

    // size
    cur_value = json.value("size");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue width = cur_value.toArray().at(0);
    QJsonValue height = cur_value.toArray().at(1);
    if (!width.isDouble() || !height.isDouble()) {
        return false;
    }
    size = QSizeF(width.toDouble(), height.toDouble());

    // layer_type
    cur_value = json.value("layer_type");
    if (!cur_value.isString()) {
        return false;
    }
    layer_type = str_layertype_map.rightByLeft(cur_value.toString());

    //layer_data
    cur_value = json.value("layer_data");
    if (!cur_value.isObject()) {
        return false;
    }
    layer_data.set(cur_value.toObject());
    Q_ASSERT(!layer_data.isNull());

    return true;
}
#else
static_assert(false, "No serializer defined.");
#endif


/* Canvas */

Canvas::Canvas(QSize size, QWidget* parent)
    : QGraphicsView(parent),
      gscene(this)
{
    setScene(&gscene);
    setSceneRect(-size.width() / 2, -size.height() / 2, size.width(), size.height());

    active_tool = tools.at(0);
    active_tool->activate();
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
    emit mouseDown(QVector2D(mapToScene(event->pos())));
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    emit mouseDragged(QVector2D(mapToScene(event->pos())));
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    emit mouseUp(QVector2D(mapToScene(event->pos())));
}

void Canvas::addItem(QGraphicsItem* item)
{
    gscene.addItem(item);
}

//void Canvas::mousePressEvent(QMouseEvent* event)
//{
//    if (event->button() == Qt::LeftButton) {
//        startDrawing(event->pos());
//    }
//}

//void Canvas::mouseMoveEvent(QMouseEvent* event)
//{
//    if (drawing) {
//        drawLineToPoint(event->pos());
//    }
//}

//void Canvas::mouseReleaseEvent(QMouseEvent* event)
//{
//    if (event->button() == Qt::LeftButton) {
//        stopDrawing();
//    }
//}

//void Canvas::paintEvent(QPaintEvent* event)
//{
//    QPainter painter(this);
//    painter.drawImage(event->rect(), background, event->rect());
//    painter.drawImage(event->rect(), canv_image, event->rect());
//}

//void Canvas::startDrawing(const QPoint& pos)
//{
//    prev_point = pos;
//    drawing = true;
//}

//void Canvas::drawLineToPoint(const QPoint& point)
//{
//    drawLine(prev_point, point);

//    QString sendStr = QString("%1 %2 %3 %4").arg(prev_point.x()).
//            arg(prev_point.y()).arg(point.x()).arg(point.y());
//    //ServerApi::sendData(sendStr.toUtf8());

//    prev_point = point;
//}

//void Canvas::drawLine(const QPoint& first, const QPoint& second)
//{
//    QPainter painter(&canv_image);
//    painter.drawLine(first, second);
//    update();
//}

//void Canvas::stopDrawing()
//{
//    drawing = false;
//}


//void Canvas::onDataReceived()
//{
////    QString data = QString::fromUtf8(ServerApi::readData());
////    qDebug() << "Data received: " << data;

////    QStringList data_split = data.split(' ');
////    Q_ASSERT(data_split.size() == 4);

////    QPoint first(data_split.at(0).toInt(), data_split.at(1).toInt());
////    QPoint second(data_split.at(2).toInt(), data_split.at(3).toInt());

////    drawLine(first, second);
//}
