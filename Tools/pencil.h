#ifndef PENCIL_H
#define PENCIL_H

#include <QWidget>
#include <QLayout>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPolygonF>
#include <QPen>
#include <QPointF>

#include "serializers.h"
#include "tool.h"


class PencilItem : public QGraphicsPathItem, public Serializable
{
public:
    PencilItem();
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
    QPen defaultPen();
    void verticesToPath();

    QPolygonF vertices;
};


namespace Ui {
class PencilProps;
}

class PencilProps : public QWidget
{
    Q_OBJECT

public:
    explicit PencilProps(QWidget *parent = nullptr);
    ~PencilProps();

    qreal getWidth() const;

private slots:
    void onWidthSliderChanged(int value);

private:
    Ui::PencilProps* ui;

    qreal width;
};


class Pencil : public Tool
{
    Q_OBJECT

public:
    Pencil(Canvas* canvas, QLayout* props_container, QObject* parent = nullptr);

protected:
    void toolDown(const QPointF& pos) override;
    void toolDragged(const QPointF& pos) override;
    void toolUp(const QPointF& pos) override;

private:
    PencilItem* cur_item = nullptr;

    PencilProps* pencil_props = nullptr;
};

#endif // PENCIL_H
