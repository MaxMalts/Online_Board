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
