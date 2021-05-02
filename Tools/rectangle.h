#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QGraphicsRectItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointF>

#include "serializers.h"
#include "tool.h"


class RectangleItem : public QGraphicsRectItem, public Serializable
{
    using QGraphicsRectItem::QGraphicsRectItem;

public:
#ifdef JSON_SERIALIZER
    virtual bool deserialize(const QJsonObject& json) override;
    virtual bool serialize(QJsonObject& json) const override;
#else
    static_assert(false, "No serializer defined.");
#endif
};


class Rectangle : public Tool
{
    Q_OBJECT
    using Tool::Tool;

public:
    Rectangle() = delete;

protected:
    void toolDown(const QPointF& pos) override;
    void toolDragged(const QPointF& pos) override;
    void toolUp(const QPointF& pos) override;

private:
    QRectF cur_rect;
};

#endif // RECTANGLE_H
