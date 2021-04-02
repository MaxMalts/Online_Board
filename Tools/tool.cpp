#include <QDebug>
#include "canvas.h"
#include "tool.h"
#include <QGuiApplication>

Tool::Tool(Canvas* canvas, QObject *parent)
      : QObject(parent),
        canvas(canvas)
{
    Q_ASSERT(canvas != nullptr);
}

void Tool::activate()
{
    connect(canvas, SIGNAL(mouseDown(const QVector2D&)),
            this, SLOT(toolDown(const QVector2D&)));
    connect(canvas, SIGNAL(mouseDragged(const QVector2D&)),
            this, SLOT(toolDragged(const QVector2D&)));
    connect(canvas, SIGNAL(mouseUp(const QVector2D&)),
            this, SLOT(toolUp(const QVector2D&)));

    toolActivated();
}

void Tool::inactivate()
{
    disconnect(canvas, SIGNAL(mouseDown(const QVector2D&)),
               this, SLOT(toolDown(const QVector2D&)));
    disconnect(canvas, SIGNAL(mouseDragged(const QVector2D&)),
               this, SLOT(toolDragged(const QVector2D&)));
    disconnect(canvas, SIGNAL(mouseUp(const QVector2D&)),
               this, SLOT(toolUp(const QVector2D&)));

    toolInactivated();
}

void Tool::toolActivated() {}
void Tool::toolDown(const QVector2D&) {}
void Tool::toolDragged(const QVector2D&) {}
void Tool::toolUp(const QVector2D&) {}
void Tool::toolInactivated() {}
