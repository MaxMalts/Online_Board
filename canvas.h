#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QSizeF>
#include <QQueue>
#include <QDebug>

#include "Tools/tool.h"
#include "Tools/pencil.h"
#include "Tools/line.h"
#include "Tools/rectangle.h"
#include "Tools/ellipse.h"
#include "serializers.h"
#include "common.h"


class Canvas : public QGraphicsView
{
    Q_OBJECT

public:
    enum class ToolType {
        undefined,
        pencil,
        line,
        rectangle,
        ellipse,
    };

    enum class ItemType {
        undefined,
        pencil,
        line,
        rectangle,
        ellipse
    };

    Canvas(QSize size = QSize(500, 500), QWidget* parent = nullptr);

    void resize(QSize size);

    void setActiveTool(ToolType tool);

    void addItem(ItemType type, QGraphicsItem* item);  // takes ownership
    bool deleteItem(QGraphicsItem* item);  // deletes pointer

    void addPreviewItem(ItemType type, QGraphicsItem* item);  // takes ownership
    bool fixPreviewItem(QGraphicsItem* item);
    bool deletePreviewItem(QGraphicsItem* item);  // deletes pointer

    void redrawRect(const QRectF& rect);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    // pos is position on the scene
    void mouseDown(const QPointF& pos);
    void mouseDragged(const QPointF& pos);
    void mouseUp(const QPointF& pos);

private slots:
    void onLayerReceived(const Serializer& argument);
    void onLayerConfirmed(const Serializer& argument);

private:
    enum ItemDataInd {
        id,
        item_type,
        is_preview
    };

    void sendItem(QGraphicsItem* item);
    bool deleteFromScene(QGraphicsItem* item);


    QGraphicsScene gscene;

    QQueue<QGraphicsItem*> pending_add_confirm;

    Tool* active_tool = nullptr;
    const BiMap<ToolType, Tool*> tools {
        { ToolType::pencil, new Pencil(this, this) },
        { ToolType::line, new Line(this, this) },
        { ToolType::rectangle, new Rectangle(this, this) },
        { ToolType::ellipse, new Ellipse(this, this) }
    };
};

Q_DECLARE_METATYPE(Canvas::ItemType)  // to use in QVariant

#endif // CANVAS_H
