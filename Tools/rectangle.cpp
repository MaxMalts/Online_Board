#include <QGraphicsRectItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRectF>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QColorDialog>
#include <QDebug>

#include "ServerApi/serverapi.h"
#include "serializers.h"
#include "canvas.h"
#include "ui_rectangleprops.h"
#include "rectangle.h"


/* RectangleItem */

#ifdef JSON_SERIALIZER
bool RectangleItem::deserialize(const QJsonObject& json)
{
    Q_ASSERT(!json.isEmpty());

    // size
    QJsonValue cur_value = json.value("size");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue width = cur_value.toArray().at(0);
    QJsonValue height = cur_value.toArray().at(1);
    if (!width.isDouble() || !height.isDouble()) {
        return false;
    }

    QPen new_pen(QBrush(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    // stroke-width
    cur_value = json.value("stroke-width");
    if (!cur_value.isDouble()) {
        return false;
    }
    new_pen.setWidthF(cur_value.toDouble());

    // stroke-color
    cur_value = json.value("stroke-color");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue r = cur_value.toArray().at(0);
    QJsonValue g = cur_value.toArray().at(1);
    QJsonValue b = cur_value.toArray().at(2);
    QJsonValue a = cur_value.toArray().at(3);
    if (!r.isDouble() || !g.isDouble() ||
        !b.isDouble() || !a.isDouble()) {
        return false;
    }
    new_pen.setColor(QColor(r.toInt(), g.toInt(),
                            b.toInt(), a.toInt()));

    QBrush new_brush(Qt::SolidPattern);

    // fill-color
    cur_value = json.value("fill-color");
    if (!cur_value.isArray()) {
        return false;
    }
    r = cur_value.toArray().at(0);
    g = cur_value.toArray().at(1);
    b = cur_value.toArray().at(2);
    a = cur_value.toArray().at(3);
    if (!r.isDouble() || !g.isDouble() ||
        !b.isDouble() || !a.isDouble()) {
        return false;
    }
    new_brush.setColor(QColor(r.toInt(), g.toInt(),
                              b.toInt(), a.toInt()));

    setPen(new_pen);
    setBrush(new_brush);
    setRect(0, 0, width.toDouble(), height.toDouble());

    return true;
}

bool RectangleItem::serialize(QJsonObject& json) const
{
    json = QJsonObject();

    QRectF cur_rect = rect();
    if (!cur_rect.isValid()) {
        return false;
    }

    json.insert("size", QJsonArray{ cur_rect.width(), cur_rect.height() });

    json.insert("stroke-width", pen().widthF());

    QColor stroke_color = pen().color();
    json.insert("stroke-color", QJsonArray{
            stroke_color.red(),
            stroke_color.green(),
            stroke_color.blue(),
            stroke_color.alpha()
        });

    Q_ASSERT(brush().style() == Qt::SolidPattern);
    QColor fill_color = brush().color();
    json.insert("fill-color", QJsonArray{
            fill_color.red(),
            fill_color.green(),
            fill_color.blue(),
            fill_color.alpha()
        });

    return true;
}
#else
static_assert(false, "No serializer defined.");
#endif


/* RectangleProps */

RectangleProps::RectangleProps(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RectangleProps)
{
    ui->setupUi(this);
    stroke_width = ui->widthSlider->value();

    updatePreviewColorCss(ui->strokeColorButton, stroke_color);
    updatePreviewColorCss(ui->fillColorButton, fill_color);
}

RectangleProps::~RectangleProps()
{
    delete ui;
}

qreal RectangleProps::strokeWidth() const
{
    return stroke_width;
}

QColor RectangleProps::strokeColor() const
{
    return stroke_color;
}

QColor RectangleProps::fillColor() const
{
    return fill_color;
}

void RectangleProps::onWidthSliderChanged(int value)
{
    stroke_width = value;
}

void RectangleProps::onStrokeColorClicked()
{
    Q_ASSERT(sender() == ui->strokeColorButton);

    QColor new_color = QColorDialog::getColor(stroke_color, this,
                                              "Color Picker",
                                              QColorDialog::ShowAlphaChannel);
    if (!new_color.isValid()) {
        return;
    }

    updatePreviewColorCss(ui->strokeColorButton, new_color);

    stroke_color = new_color;
}

void RectangleProps::onFillColorClicked()
{
    Q_ASSERT(sender() == ui->fillColorButton);

    QColor new_color = QColorDialog::getColor(fill_color, this,
                                              "Color Picker",
                                              QColorDialog::ShowAlphaChannel);
    if (!new_color.isValid()) {
        return;
    }

    updatePreviewColorCss(ui->fillColorButton, new_color);

    fill_color = new_color;
}

void RectangleProps::updatePreviewColorCss(QWidget* color_preview, const QColor& color)
{
    QString new_css = color_preview->styleSheet();
    bool ret = ChangeCssProp(new_css, "background",
                             color.name(QColor::HexArgb));
    Q_ASSERT(ret);

    color_preview->setStyleSheet(new_css);
}


/* Rectangle */

Rectangle::Rectangle(Canvas* canvas, QLayout* props_container, QObject* parent)
    : Tool(canvas, props_container, parent),
      rectangle_props(new RectangleProps)
{
    setPropsWidget(rectangle_props);
}

void Rectangle::toolDown(const QPointF& pos)
{
    Q_ASSERT(cur_item == nullptr);

    cur_rect.setTopLeft(pos);
    cur_rect.setBottomRight(pos);

    cur_item = new RectangleItem(cur_rect);

    QPen pen = cur_item->pen();
    pen.setWidthF(rectangle_props->strokeWidth());
    pen.setColor(rectangle_props->strokeColor());
    cur_item->setPen(pen);

    QBrush brush = cur_item->brush();
    brush.setColor(rectangle_props->fillColor());
    cur_item->setBrush(brush);

    canvas->addPreviewItem(Canvas::ItemType::rectangle, cur_item);
}

void Rectangle::toolDragged(const QPointF& pos)
{
    cur_rect.setBottomRight(pos);
    cur_item->setRect(cur_rect.normalized());
}

void Rectangle::toolUp(const QPointF& pos)
{
    cur_rect.setBottomRight(pos);
    if (cur_rect.isNull()) {
        bool ret = canvas->deletePreviewItem(cur_item);
        Q_ASSERT(ret);

        cur_item = nullptr;
        return;
    }
    cur_rect = cur_rect.normalized();

    cur_item->setRect(cur_rect);

    // Normalizing position to bounding rect
    QPointF item_pos = cur_item->boundingRect().topLeft();
    cur_item->setPos(item_pos);
    cur_rect.translate(-item_pos);
    cur_item->setRect(cur_rect);

    bool ret = canvas->fixPreviewItem(cur_item);
    Q_ASSERT(ret);

    cur_rect = QRectF();
    cur_item = nullptr;
}
