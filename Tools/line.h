#ifndef LINE_H
#define LINE_H

#include <QWidget>
#include <QLayout>
#include <QGraphicsLineItem>
#include <QLineF>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <utility>

#include "serializers.h"
#include "tool.h"


class LineItem : public QGraphicsLineItem, public Serializable
{
public:
    template<typename ...Args>
    LineItem(Args... args)
        : QGraphicsLineItem(std::forward<Args>(args)...)
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
class LineProps;
}

class LineProps : public QWidget
{
    Q_OBJECT

public:
    explicit LineProps(QWidget *parent = nullptr);
    ~LineProps();

    qreal getWidth() const;

private slots:
    void onWidthSliderChanged(int value);

private:
    Ui::LineProps *ui;

    qreal width;
};


class Line : public Tool
{
    Q_OBJECT

public:
    Line(Canvas* canvas, QLayout* props_container, QObject* parent = nullptr);

protected:
    void toolDown(const QPointF& pos) override;
    void toolDragged(const QPointF& pos) override;
    void toolUp(const QPointF& pos) override;

private:
    QLineF cur_line;
    LineItem* cur_item = nullptr;

    LineProps* line_props = nullptr;
};

#endif // LINE_H
