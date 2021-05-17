#include <QtMath>

#include "hand.h"
#include "canvas.h"


Hand::Hand(Canvas* canvas, QLayout* props_container, QObject* parent)
    : Tool(canvas, props_container, parent) {}

void Hand::toolActivated()
{
    canvas->setDragable(true);
}

void Hand::toolInactivated()
{
    canvas->setDragable(false);
}

void Hand::toolScrolled(QWheelEvent* event)
{
    const qreal factor = 1.1;

    int delta = event->angleDelta().y();
    canvas->scale(delta > 0 ? factor : 1 / factor);
}
