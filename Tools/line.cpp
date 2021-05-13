#include <QGraphicsLineItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLineF>
#include <QPointF>
#include <QPen>
#include <QColor>
#include <QDebug>

#include "ServerApi/serverapi.h"
#include "serializers.h"
#include "line.h"
#include "ui_lineprops.h"
#include "canvas.h"


/* LineItem */

#ifdef JSON_SERIALIZER
bool LineItem::deserialize(const QJsonObject& json)
{
    Q_ASSERT(!json.isEmpty());

    QLineF new_line;

    // points
    QJsonValue cur_value = json.value("first_point");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue x = cur_value.toArray().at(0);
    QJsonValue y = cur_value.toArray().at(1);
    if (!x.isDouble() || !y.isDouble()) {
        return false;
    }
    new_line.setP1(QPointF(x.toDouble(), y.toDouble()));

    cur_value = json.value("second_point");
    if (!cur_value.isArray()) {
        return false;
    }
    x = cur_value.toArray().at(0);
    y = cur_value.toArray().at(1);
    if (!x.isDouble() || !y.isDouble()) {
        return false;
    }
    new_line.setP2(QPointF(x.toDouble(), y.toDouble()));

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
    setLine(new_line);

    return true;
}

bool LineItem::serialize(QJsonObject& json) const
{
    json = QJsonObject();

    QLineF cur_line = line();
    if (cur_line.isNull()) {
        return false;
    }

    json.insert("first_point", QJsonArray{ cur_line.p1().x(), cur_line.p1().y() });
    json.insert("second_point", QJsonArray{ cur_line.p2().x(), cur_line.p2().y() });

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


/* LineProps */

LineProps::LineProps(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LineProps)
{
    ui->setupUi(this);
    width = ui->widthSlider->value();
}

LineProps::~LineProps()
{
    delete ui;
}

qreal LineProps::getWidth() const
{
    return width;
}

void LineProps::onWidthSliderChanged(int value)
{
    width = value;
}


/* Line */

Line::Line(Canvas* canvas, QLayout* props_container, QObject* parent)
    : Tool(canvas, props_container, parent),
      line_props(new LineProps)
{
    setPropsWidget(line_props);
}

void Line::toolDown(const QPointF& pos)
{
    Q_ASSERT(cur_item == nullptr);

    cur_line.setP1(pos);
    cur_line.setP2(pos);

    cur_item = new LineItem(cur_line);
    QPen pen = cur_item->pen();
    pen.setColor(canvas->activeColor());
    pen.setWidthF(line_props->getWidth());
    cur_item->setPen(pen);

    canvas->addPreviewItem(Canvas::ItemType::line, cur_item);
}

void Line::toolDragged(const QPointF& pos)
{
    cur_line.setP2(pos);
    cur_item->setLine(cur_line);
}

void Line::toolUp(const QPointF& pos)
{
    cur_line.setP2(pos);
    if (cur_line.isNull()) {
        bool ret = canvas->deletePreviewItem(cur_item);
        Q_ASSERT(ret);

        cur_item = nullptr;
        return;
    }

    cur_item->setLine(cur_line);

    // Normalizing position to bounding rect
    QPointF item_pos = cur_item->boundingRect().topLeft();
    cur_item->setPos(item_pos);
    cur_line.translate(-item_pos);
    cur_item->setLine(cur_line);

    bool ret = canvas->fixPreviewItem(cur_item);
    Q_ASSERT(ret);

    cur_line = QLineF();
    cur_item = nullptr;
}
