#include "canvas.h"

#include <QMouseEvent>
#include <QImage>
#include <QPainter>

Canvas::Canvas(QWidget* parent, QSize size)
    : QWidget(parent), canv_image(size, QImage::Format_ARGB32)
{

}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        prev_point = event->pos();
        drawing = true;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (drawing) {
        drawLine(prev_point, event->pos());
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        drawing = false;
    }
}

void Canvas::drawLine(const QPoint& first, const QPoint& second)
{
    QPainter painter(&canv_image);
    painter.drawLine(first, second);
}
