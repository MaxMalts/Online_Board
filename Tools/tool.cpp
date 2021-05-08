#include <QDebug>

#include "canvas.h"
#include "tool.h"
#include "ServerApi/serverapi.h"


Tool::Tool(Canvas* canvas, QObject *parent)
      : QObject(parent),
        canvas(canvas)
{
    Q_ASSERT(canvas != nullptr);
}

void Tool::activate()
{
    connect(canvas, SIGNAL(mouseDown(const QPointF&)),
            this, SLOT(toolDown(const QPointF&)));
    connect(canvas, SIGNAL(mouseDragged(const QPointF&)),
            this, SLOT(toolDragged(const QPointF&)));
    connect(canvas, SIGNAL(mouseUp(const QPointF&)),
            this, SLOT(toolUp(const QPointF&)));

    toolActivated();
}

void Tool::inactivate()
{
    disconnect(canvas, SIGNAL(mouseDown(const QPointF&)),
               this, SLOT(toolDown(const QPointF&)));
    disconnect(canvas, SIGNAL(mouseDragged(const QPointF&)),
               this, SLOT(toolDragged(const QPointF&)));
    disconnect(canvas, SIGNAL(mouseUp(const QPointF&)),
               this, SLOT(toolUp(const QPointF&)));

    toolInactivated();
}

void Tool::toolActivated() {}
void Tool::toolDown(const QPointF&) {}
void Tool::toolDragged(const QPointF&) {}
void Tool::toolUp(const QPointF&) {}
void Tool::toolInactivated() {}
