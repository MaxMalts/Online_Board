#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QRectF>
#include <QSizeF>
#include <QQueue>
#include <QColor>
#include <QDebug>

#include "Tools/tool.h"
#include "Tools/hand.h"
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
        hand,
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
    void setupTools(QLayout* props_container);

    void setActiveTool(ToolType tool);
    void setActiveColor(const QColor& color);
    QColor activeColor() const;

//    void addItem(ItemType type, QGraphicsItem* item);  // takes ownership
    bool deleteItem(QGraphicsItem* item);  // deletes pointer

    void addPreviewItem(ItemType type, QGraphicsItem* item);  // takes ownership
    bool fixPreviewItem(QGraphicsItem* item);
    bool deletePreviewItem(QGraphicsItem* item);  // deletes pointer

    void redrawRect(const QRectF& rect);

    void setDragable(bool dragable);

    void scale(qreal factor);

    void undo();
    void clear();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

signals:
    // pos is position on the scene
    void mouseDown(const QPointF& pos);
    void mouseDragged(const QPointF& pos);
    void mouseUp(const QPointF& pos);
    void mouseScrolled(QWheelEvent* event);

private slots:
    void onLayerReceived(const Serializer& argument);
    void onLayerConfirmed(const Serializer& argument);
    void onDeleteLayer(const Serializer& argument);
    void onClearBoard();

private:
    enum ItemDataInd {
        id,
        item_type,
        is_preview
    };

    void sendItem(QGraphicsItem* item);
    bool deleteFromScene(QGraphicsItem* item);


    QGraphicsScene gscene;
    QMap<int, QGraphicsItem*> id_to_item;

    QQueue<QGraphicsItem*> pending_add_confirm;

    Tool* active_tool = nullptr;
    BiMap<ToolType, Tool*> tools;

    QColor active_color;
};

Q_DECLARE_METATYPE(Canvas::ItemType)  // to use in QVariant

#endif // CANVAS_H
