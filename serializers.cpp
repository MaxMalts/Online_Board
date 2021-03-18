#include <QJsonDocument>

#include "serializers.h"


void JsonSerializer::serialize(const Serializable& object)
{
    QJsonObject jsonObj;
    object.serialize(jsonObj);
    json.setObject(jsonObj);
}

void JsonSerializer::deserialize(Serializable& object)
{
    object.deserialize(json.object());
}

QByteArray JsonSerializer::getData()
{
    return json.toJson();
}

QJsonParseError JsonSerializer::lastError()
{
    return last_error;
}

bool JsonSerializer::set(const QByteArray& json)
{
    json = QJsonDocument::fromJson(json, &last_error);
    return !json.isNull();
}
