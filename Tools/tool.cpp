#include <QDebug>
#include "canvas.h"
#include "tool.h"

Tool::Tool(Canvas* canvas, QObject *parent)
      : QObject(parent),
        canvas(canvas)
{
    Q_ASSERT(canvas != nullptr);
}

void Tool::activate()
{
    connect(canvas, SIGNAL(mouseDown(const QVector2D&)), this, SLOT(toolDown(const QVector2D&)));
    connect(canvas, SIGNAL(mouseMoved(const QVector2D&)), this, SLOT(toolMoved(const QVector2D&)));
    connect(canvas, SIGNAL(mouseUp(const QVector2D&)), this, SLOT(toolUp(const QVector2D&)));

    toolActivated();
}

void Tool::inactivate()
{
    disconnect(canvas, SIGNAL(mouseDown(const QVector2D&)), this, SLOT(toolDown(const QVector2D&)));
    disconnect(canvas, SIGNAL(mouseMoved(const QVector2D&)), this, SLOT(toolMoved(const QVector2D&)));
    disconnect(canvas, SIGNAL(mouseUp(const QVector2D&)), this, SLOT(toolUp(const QVector2D&)));

    toolInactivated();
}
