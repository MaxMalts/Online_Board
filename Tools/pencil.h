#ifndef PENCIL_H
#define PENCIL_H

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPolygonF>
#include <QPointF>

#include "serializers.h"
#include "tool.h"


class PencilItem : public QGraphicsPathItem, public Serializable
{
    using QGraphicsPathItem::QGraphicsPathItem;

public:
    PencilItem(const QPolygonF& vertices);
    PencilItem(const QPolygonF&& vertices);

    void setVertices(const QPolygonF& vertices);
    void setVertices(const QPolygonF&& vertices);
    void addVertex(const QPointF& vertex);
    void translateVertices(const QPointF& offset);

#ifdef JSON_SERIALIZER
    virtual bool deserialize(const QJsonObject& json) override;
    virtual bool serialize(QJsonObject& json) const override;
#else
    static_assert(false, "No serializer defined.");
#endif

private:
    void verticesToPath();

    QPolygonF vertices;
};


class Pencil : public Tool
{
    Q_OBJECT
    using Tool::Tool;

public:
    Pencil() = delete;

protected:
    void toolDown(const QPointF& pos) override;
    void toolDragged(const QPointF& pos) override;
    void toolUp(const QPointF& pos) override;

private:
    PencilItem* cur_item = nullptr;
};

#endif // PENCIL_H
