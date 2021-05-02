#include <QDebug>

#include "canvas.h"
#include "tool.h"
#include "serverapi.h"


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

void Tool::setItem(QGraphicsItem* item)
{
    canvas->addItem(item);
}

void Tool::sendItem(AddLayerArgs::LayerType layer_type, QGraphicsItem* item)
{
    AddLayerArgs args;
    args.position = item->scenePos();
    args.scale = item->scale();
    args.layer_type = layer_type;
    args.layer_data.serialize(dynamic_cast<Serializable&>(*item));

#ifdef JSON_SERIALIZER
    JsonSerializer argument(args);
#else
static_assert(false, "No serializer defined.");
#endif

    ServerApi::sAddLayer(argument);
}
