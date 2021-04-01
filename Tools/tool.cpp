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
            this, SLOT(onMouseDown(const QVector2D&)));
    connect(canvas, SIGNAL(mouseMoved(const QVector2D&)),
            this, SLOT(onMouseMoved(const QVector2D&)));
    connect(canvas, SIGNAL(mouseUp(const QVector2D&)),
            this, SLOT(onMouseUp(const QVector2D&)));

    toolActivated();
}

void Tool::inactivate()
{
    disconnect(canvas, SIGNAL(mouseDown(const QVector2D&)),
               this, SLOT(onMouseDown(const QVector2D&)));
    disconnect(canvas, SIGNAL(mouseMoved(const QVector2D&)),
               this, SLOT(nMouseMoved(const QVector2D&)));
    disconnect(canvas, SIGNAL(mouseUp(const QVector2D&)),
               this, SLOT(onMouseUp(const QVector2D&)));

    toolInactivated();
}

void Tool::onMouseDown(const QVector2D& pos)
{
    mouse_pressed = true;
    toolDown(pos);
}

void Tool::onMouseMoved(const QVector2D& pos)
{
    mouse_pressed ? toolDragged(pos) : toolMovedIdle(pos);
}

void Tool::onMouseUp(const QVector2D& pos)
{
    mouse_pressed = false;
    toolUp(pos);
}

void Tool::toolActivated() {}
void Tool::toolDown(const QVector2D&) {}
void Tool::toolDragged(const QVector2D&) {}
void Tool::toolMovedIdle(const QVector2D&) {}
void Tool::toolUp(const QVector2D&) {}
void Tool::toolInactivated() {}
