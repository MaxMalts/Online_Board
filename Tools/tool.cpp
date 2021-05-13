#include <QDebug>

#include "canvas.h"
#include "tool.h"
#include "ServerApi/serverapi.h"


Tool::Tool(Canvas* canvas, QLayout* props_container, QObject *parent)
      : QObject(parent),
        canvas(canvas),
        props_container(props_container)
{
    Q_ASSERT(canvas != nullptr);
    Q_ASSERT(props_container != nullptr);
}

void Tool::activate()
{
    connect(canvas, SIGNAL(mouseDown(const QPointF&)),
            this, SLOT(toolDown(const QPointF&)));
    connect(canvas, SIGNAL(mouseDragged(const QPointF&)),
            this, SLOT(toolDragged(const QPointF&)));
    connect(canvas, SIGNAL(mouseUp(const QPointF&)),
            this, SLOT(toolUp(const QPointF&)));

    if (props_widget != nullptr) {
        props_container->addWidget(props_widget);
        //props_widget->show();
        qDebug() << props_widget->parent();
    }

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

    if (props_widget != nullptr) {
        props_container->removeWidget(props_widget);
        qDebug() << props_widget->parent();
    }

    toolInactivated();
}

Tool::~Tool()
{
    delete props_widget;
}

void Tool::toolActivated() {}
void Tool::toolDown(const QPointF&) {}
void Tool::toolDragged(const QPointF&) {}
void Tool::toolUp(const QPointF&) {}
void Tool::toolInactivated() {}

void Tool::setPropsWidget(QWidget* props_widget)
{
    Q_ASSERT(props_widget != nullptr);
    this->props_widget = props_widget;
}
