#include "confirmaddlayerargs.h"

#include <QJsonObject>
#include <cmath>

#include "serializers.h"


#ifdef JSON_SERIALIZER
bool ConfirmAddLayerArgs::serialize(QJsonObject& json) const
{
    json = QJsonObject();
    json.insert("layer_id", layer_id);

    return true;
}

bool ConfirmAddLayerArgs::deserialize(const QJsonObject& json)
{
    Q_ASSERT(!json.isEmpty());

    QJsonValue cur_value = json.value("layer_id");
    if (!cur_value.isDouble()) {
        return false;
    }

    layer_id = std::lround(cur_value.toDouble());
}
#else
static_assert(false, "No serializer defined.");
#endif
