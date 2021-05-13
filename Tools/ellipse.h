#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <QGraphicsEllipseItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <utility>

#include "serializers.h"
#include "tool.h"


class EllipseItem : public QGraphicsEllipseItem, public Serializable
{
public:
    template<typename ...Args>
    EllipseItem(Args... args)
        : QGraphicsEllipseItem(std::forward<Args>(args)...)
    {
        setPen(QPen(QBrush(), 1, Qt::SolidLine, Qt::RoundCap));
    }

#ifdef JSON_SERIALIZER
    virtual bool deserialize(const QJsonObject& json) override;
    virtual bool serialize(QJsonObject& json) const override;
#else
    static_assert(false, "No serializer defined.");
#endif
};


class Ellipse : public Tool
{
    Q_OBJECT
    using Tool::Tool;

public:
    Ellipse() = delete;

protected:
    void toolDown(const QPointF& pos) override;
    void toolDragged(const QPointF& pos) override;
    void toolUp(const QPointF& pos) override;

private:
    QRectF cur_rect;
    EllipseItem* cur_item = nullptr;
};

#endif // ELLIPSE_H
