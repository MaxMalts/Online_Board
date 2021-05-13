#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QWidget>
#include <QLayout>
#include <QGraphicsRectItem>
#include <QRectF>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <utility>

#include "serializers.h"
#include "tool.h"


class RectangleItem : public QGraphicsRectItem, public Serializable
{
public:
    template<typename ...Args>
    RectangleItem(Args... args)
        : QGraphicsRectItem(std::forward<Args>(args)...)
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


namespace Ui {
class RectangleProps;
}

class RectangleProps : public QWidget
{
    Q_OBJECT

public:
    explicit RectangleProps(QWidget *parent = nullptr);
    ~RectangleProps();

    qreal getWidth() const;

private slots:
    void onWidthSliderChanged(int value);

private:
    Ui::RectangleProps* ui;

    qreal width;
};


class Rectangle : public Tool
{
    Q_OBJECT

public:
    Rectangle(Canvas* canvas, QLayout* props_container, QObject* parent = nullptr);

protected:
    void toolDown(const QPointF& pos) override;
    void toolDragged(const QPointF& pos) override;
    void toolUp(const QPointF& pos) override;

private:
    QRectF cur_rect;
    RectangleItem* cur_item = nullptr;

    RectangleProps* rectangle_props = nullptr;
};

#endif // RECTANGLE_H
