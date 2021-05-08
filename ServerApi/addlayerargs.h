#ifndef ADDLAYERARGS_H
#define ADDLAYERARGS_H

#include <QJsonObject>
#include <QPointF>
#include <QString>

#include "serializers.h"
#include "common.h"
#include "canvas.h"


struct AddLayerArgs : public Serializable {
public:
#ifdef JSON_SERIALIZER
    bool serialize(QJsonObject& json) const override;
    bool deserialize(const QJsonObject& json) override;

    static const BiMap<QString, Canvas::ItemType> str_itemtype_map;
    JsonSerializer layer_data;
#else
    static_assert(false, "No serializer defined.");
#endif

    int layer_id = 0;
    QPointF position;
    qreal scale = 1;
    Canvas::ItemType item_type;
};

#endif // ADDLAYERARGS_H
