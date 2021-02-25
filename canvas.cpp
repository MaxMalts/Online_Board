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
        prev_point = event->pos();
        drawing = true;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (drawing) {
        drawLine(prev_point, event->pos());
        prev_point = event->pos();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        drawing = false;
    }
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(event->rect(), background, event->rect());
    painter.drawImage(event->rect(), canv_image, event->rect());
}

void Canvas::drawLine(const QPoint& first, const QPoint& second)
{
    QPainter painter(&canv_image);
    painter.drawLine(first, second);
    update();
}
