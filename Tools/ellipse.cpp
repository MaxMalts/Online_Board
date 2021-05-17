#include <QGraphicsEllipseItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRectF>
#include <QMarginsF>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QColorDialog>
#include <QDebug>

#include "ServerApi/serverapi.h"
#include "serializers.h"
#include "canvas.h"
#include "ui_ellipseprops.h"
#include "ellipse.h"


/* EllipseItem */

#ifdef JSON_SERIALIZER
bool EllipseItem::deserialize(const QJsonObject& json)
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

    QPen new_pen(QBrush(), 1, Qt::SolidLine, Qt::RoundCap);

    // color
    cur_value = json.value("color");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue r = cur_value.toArray().at(0);
    QJsonValue g = cur_value.toArray().at(1);
    QJsonValue b = cur_value.toArray().at(2);
    QJsonValue a = cur_value.toArray().at(3);
    if (!a.isDouble() || !r.isDouble() ||
        !g.isDouble() || !b.isDouble()) {
        return false;
    }
    new_pen.setColor(QColor(r.toInt(), g.toInt(),
                            b.toInt(), a.toInt()));

    // width
    cur_value = json.value("width");
    if (!cur_value.isDouble()) {
        return false;
    }
    new_pen.setWidthF(cur_value.toDouble());

    setPen(new_pen);
    setRect(0, 0, width.toDouble(), height.toDouble());

    return true;
}

bool EllipseItem::serialize(QJsonObject& json) const
{
    json = QJsonObject();

    QRectF cur_rect = rect();
    if (!cur_rect.isValid()) {
        return false;
    }

    json.insert("size", QJsonArray{ cur_rect.width(), cur_rect.height() });

    QColor color = pen().color();
    json.insert("color", QJsonArray{
            color.red(),
            color.green(),
            color.blue(),
            color.alpha()
        });

    json.insert("width", pen().widthF());

    return true;
}
#else
static_assert(false, "No serializer defined.");
#endif


/* EllipseProps */

EllipseProps::EllipseProps(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EllipseProps)
{
    ui->setupUi(this);
    stroke_width = ui->widthSlider->value();

    updatePreviewColorCss(ui->strokeColorButton, stroke_color);
    updatePreviewColorCss(ui->fillColorButton, fill_color);
}

EllipseProps::~EllipseProps()
{
    delete ui;
}

qreal EllipseProps::strokeWidth() const
{
    return stroke_width;
}

QColor EllipseProps::strokeColor() const
{
    return stroke_color;
}

QColor EllipseProps::fillColor() const
{
    return fill_color;
}

void EllipseProps::onWidthSliderChanged(int value)
{
    stroke_width = value;
}

void EllipseProps::onStrokeColorClicked()
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

void EllipseProps::onFillColorClicked()
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

void EllipseProps::updatePreviewColorCss(QWidget* color_preview, const QColor& color)
{
    QString new_css = color_preview->styleSheet();
    bool ret = ChangeCssProp(new_css, "background",
                             color.name(QColor::HexArgb));
    Q_ASSERT(ret);

    color_preview->setStyleSheet(new_css);
}


/* Ellipse */

Ellipse::Ellipse(Canvas* canvas, QLayout* props_container, QObject* parent)
    : Tool(canvas, props_container, parent),
      ellipse_props(new EllipseProps)
{
    setPropsWidget(ellipse_props);
}

void Ellipse::toolDown(const QPointF& pos)
{
    Q_ASSERT(cur_item == nullptr);

    cur_rect.setTopLeft(pos);
    cur_rect.setBottomRight(pos);

    cur_item = new EllipseItem(cur_rect);

    QPen pen = cur_item->pen();
    pen.setWidthF(ellipse_props->strokeWidth());
    pen.setColor(ellipse_props->strokeColor());
    cur_item->setPen(pen);

    QBrush brush = cur_item->brush();
    brush.setColor(ellipse_props->fillColor());
    cur_item->setBrush(brush);

    canvas->addPreviewItem(Canvas::ItemType::ellipse, cur_item);
}

void Ellipse::toolDragged(const QPointF& pos)
{
    QRectF prev_rect = cur_rect;
    cur_rect.setBottomRight(pos);
    cur_item->setRect(cur_rect.normalized());

    // QGraphicsRectItem updates a smaller rect than needed and
    // leaves dirt on canvas! >:(
    canvas->redrawRect(prev_rect.normalized() + QMarginsF(1000, 1000, 1000, 1000));
}

void Ellipse::toolUp(const QPointF& pos)
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
