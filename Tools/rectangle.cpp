#include <QGraphicsRectItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRectF>
#include <QPointF>
#include <QPen>
#include <QColor>
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

bool RectangleItem::serialize(QJsonObject& json) const
{
    json = QJsonObject();

    QRectF cur_rect = rect();
    if (!cur_rect.isValid()) {
        return false;
    }

    json.insert("size", QJsonArray{ cur_rect.width(), cur_rect.height() });

    QColor color = pen().color();
    json.insert("color", QJsonArray{
            color.alpha(),
            color.red(),
            color.green(),
            color.blue()
        });

    json.insert("width", pen().widthF());

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
    width = ui->widthSlider->value();
}

RectangleProps::~RectangleProps()
{
    delete ui;
}

qreal RectangleProps::getWidth() const
{
    return width;
}

void RectangleProps::onWidthSliderChanged(int value)
{
    width = value;
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
    pen.setColor(canvas->activeColor());
    pen.setWidthF(rectangle_props->getWidth());
    cur_item->setPen(pen);

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
