#include "addlayerargs.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <QString>
#include <cmath>

#include "serializers.h"
#include "common.h"
#include "canvas.h"


const BiMap<QString, Canvas::ItemType> AddLayerArgs::str_itemtype_map {
    { "pencil", Canvas::ItemType::pencil },
    { "line", Canvas::ItemType::line },
    { "rectangle", Canvas::ItemType::rectangle },
    { "ellipse", Canvas::ItemType::ellipse }
};

#ifdef JSON_SERIALIZER
bool AddLayerArgs::serialize(QJsonObject& json) const
{
    if (layer_data.isNull()) {
        Q_ASSERT(false);
        return false;
    }

    json = QJsonObject();
    json.insert("layer_id", layer_id);
    json.insert("position", QJsonArray{ position.x(), position.y() });
    json.insert("scale", scale);
    json.insert("layer_type", str_itemtype_map.atR(item_type));

    QJsonObject layer_data_json = layer_data.getJson().object();
    Q_ASSERT(!layer_data_json.isEmpty());
    json.insert("layer_data", layer_data_json);

    return true;
}

bool AddLayerArgs::deserialize(const QJsonObject& json)
{
    Q_ASSERT(!json.isEmpty());

    // layer_id
    QJsonValue cur_value = json.value("layer_id");
    if (!cur_value.isDouble()) {
        return false;
    }
    layer_id = std::lround(cur_value.toDouble());

    // position
    cur_value = json.value("position");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue x = cur_value.toArray().at(0);
    QJsonValue y = cur_value.toArray().at(1);
    if (!x.isDouble() || !y.isDouble()) {
        return false;
    }
    position = QPointF(x.toDouble(), y.toDouble());

    // size
    cur_value = json.value("scale");
    if (!cur_value.isDouble()) {
        return false;
    }
    scale = cur_value.toDouble();

    // layer_type
    cur_value = json.value("layer_type");
    if (!cur_value.isString()) {
        return false;
    }
    item_type = str_itemtype_map.atL(cur_value.toString());

    //layer_data
    cur_value = json.value("layer_data");
    if (!cur_value.isObject()) {
        return false;
    }
    layer_data.set(cur_value.toObject());
    Q_ASSERT(!layer_data.isNull());

    return true;
}
#else
static_assert(false, "No serializer defined.");
#endif
