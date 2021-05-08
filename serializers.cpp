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

bool JsonSerializer::serialize(const Serializable& object)
{
    QJsonObject jsonObj;
    if (!object.serialize(jsonObj)) {
        return false;
    }

    json_doc.setObject(jsonObj);
    Q_ASSERT(!json_doc.isNull());

    return true;
}

bool JsonSerializer::deserialize(Serializable& object) const
{
    return object.deserialize(json_doc.object());
}

QByteArray JsonSerializer::getData() const
{
    if (!json_doc.isNull()) {
        return json_doc.toJson();
    } else {
        return {};
    }
}

bool JsonSerializer::set(const QByteArray& json)
{
    json_doc = QJsonDocument::fromJson(json, &last_error);
    return !json_doc.isNull();
}

QJsonDocument JsonSerializer::getJson() const
{
    return json_doc;
}

bool JsonSerializer::set(const QJsonObject& json)
{
    json_doc.setObject(json);
    return !json_doc.isNull();
}

bool JsonSerializer::isNull() const
{
    return json_doc.isNull();
}

QJsonParseError JsonSerializer::lastError() const
{
    return last_error;
}
