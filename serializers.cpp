#include <QJsonDocument>
#include <QJsonObject>

#include "serializers.h"


JsonSerializer::JsonSerializer(const Serializable& object)
{
    serialize(object);
}

JsonSerializer::JsonSerializer(const QByteArray& json)
{
    set(json);
}

JsonSerializer::JsonSerializer(const QJsonObject& json)
{
    set(json);
}

void JsonSerializer::serialize(const Serializable& object)
{
    QJsonObject jsonObj;
    object.serialize(jsonObj);
    json_doc.setObject(jsonObj);
}

void JsonSerializer::deserialize(Serializable& object) const
{
    object.deserialize(json_doc.object());
}

QByteArray JsonSerializer::getData() const
{
    return json_doc.toJson();
}

QJsonParseError JsonSerializer::lastError() const
{
    return last_error;
}

void JsonSerializer::set(const QByteArray& json)
{
    json_doc = QJsonDocument::fromJson(json, &last_error);
}

void JsonSerializer::set(const QJsonObject& json)
{
    json_doc.setObject(json);
}
