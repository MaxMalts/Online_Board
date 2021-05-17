#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <QWidget>
#include <QLayout>
#include <QGraphicsEllipseItem>
#include <QRectF>
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
        setBrush(QBrush(Qt::SolidPattern));
    }

#ifdef JSON_SERIALIZER
    virtual bool deserialize(const QJsonObject& json) override;
    virtual bool serialize(QJsonObject& json) const override;
#else
    static_assert(false, "No serializer defined.");
#endif
};


namespace Ui {
class EllipseProps;
}

class EllipseProps : public QWidget
{
    Q_OBJECT

public:
    explicit EllipseProps(QWidget *parent = nullptr);
    ~EllipseProps();

    qreal strokeWidth() const;
    QColor strokeColor() const;
    QColor fillColor() const;

private slots:
    void onWidthSliderChanged(int value);
    void onStrokeColorClicked();
    void onFillColorClicked();

private:
    void updatePreviewColorCss(QWidget* color_preview, const QColor& color);

    Ui::EllipseProps* ui;

    qreal stroke_width;
    QColor stroke_color = Qt::black;
    QColor fill_color = Qt::transparent;
};


class Ellipse : public Tool
{
    Q_OBJECT

public:
    Ellipse(Canvas* canvas, QLayout* props_container, QObject* parent = nullptr);

protected:
    void toolDown(const QPointF& pos) override;
    void toolDragged(const QPointF& pos) override;
    void toolUp(const QPointF& pos) override;

private:
    QRectF cur_rect;
    EllipseItem* cur_item = nullptr;

    EllipseProps* ellipse_props = nullptr;
};

#endif // ELLIPSE_H
