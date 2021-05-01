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
    setSceneRect(-size.width() / 2, -size.height() / 2, size.width(), size.height());

    connect(ServerApi::getInstance(), SIGNAL(cAddLayer(const Serializer&)),
            this, SLOT(onLayerReceived(const Serializer&)));

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
