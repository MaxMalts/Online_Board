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
