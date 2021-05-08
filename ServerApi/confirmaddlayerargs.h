#ifndef CONFIRMADDLAYERARGS_H
#define CONFIRMADDLAYERARGS_H

#include <QJsonObject>

#include "serializers.h"


struct ConfirmAddLayerArgs : public Serializable {
public:
#ifdef JSON_SERIALIZER
    bool serialize(QJsonObject& json) const override;
    bool deserialize(const QJsonObject& json) override;
#else
    static_assert(false, "No serializer defined.");
#endif

    int layer_id = 0;
};

#endif // CONFIRMADDLAYERARGS_H
