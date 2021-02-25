#include "canvas.h"
#include "serverapi.h"

#include <QMouseEvent>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QDebug>

Canvas::Canvas(QWidget* parent, QSize size)
    : QWidget(parent),
      background(size, QImage::Format_ARGB32),
      canv_image(size, QImage::Format_ARGB32)
{
    resize(size);
    background.fill(Qt::white);
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        startDrawing(event->pos());
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (drawing) {
        drawLineToPoint(event->pos());
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        stopDrawing();
    }
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(event->rect(), background, event->rect());
    painter.drawImage(event->rect(), canv_image, event->rect());
}

void Canvas::startDrawing(const QPoint& pos)
{
    prev_point = pos;
    drawing = true;

    QString sendStr = QString("%1 %2").arg(pos.x()).arg(pos.y());
    ServerApi::sendData(sendStr.toUtf8());
}

void Canvas::drawLineToPoint(const QPoint& point)
{
    QPainter painter(&canv_image);
    painter.drawLine(prev_point, point);
    update();
    prev_point = point;

    QString sendStr = QString("%1 %2").arg(point.x()).arg(point.y());
    ServerApi::sendData(sendStr.toUtf8());
}

void Canvas::stopDrawing()
{
    drawing = false;
}
