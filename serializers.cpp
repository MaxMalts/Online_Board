#include <QJsonDocument>

#include "serializers.h"


JsonSerializer::JsonSerializer(const Serializable& object)
{
    serialize(object);
}

JsonSerializer::JsonSerializer(const QByteArray& data)
{
    set(data);
}

JsonSerializer::JsonSerializer(const QJsonObject& json)
{
    set(json);
}

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
