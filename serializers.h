#ifndef SERIALIZERS_H
#define SERIALIZERS_H

#include <QJsonDocument>
#include <QByteArray>


class Serializable
{
public:
#ifdef JSON_SERIALIZER
    virtual bool serialize(QJsonObject& json) const = 0;
    virtual bool deserialize(const QJsonObject& json) = 0;
#else
    static_assert(false, "No serializer defined.");
#endif

    virtual ~Serializable() = default;
};


// Represents object in serialized format
class Serializer
{
public:
    virtual bool serialize(const Serializable& object) = 0;
    virtual bool deserialize(Serializable& object) const = 0;
    virtual QByteArray getData() const = 0;
    virtual bool set(const QByteArray& data) = 0;

    virtual bool isNull() const = 0;

    virtual ~Serializer() = default;
};


#ifdef JSON_SERIALIZER
class JsonSerializer : public Serializer
{
public:
    JsonSerializer() = default;
    JsonSerializer(const Serializable& object);
    JsonSerializer(const QByteArray& data);
    JsonSerializer(const QJsonObject& json_doc);

    bool serialize(const Serializable& object) override;
    bool deserialize(Serializable& object) const override;
    QByteArray getData() const override;
    bool set(const QByteArray& data) override;

    QJsonDocument getJson() const;
    bool set(const QJsonObject& json_doc);

    bool isNull() const override;

    QJsonParseError lastError() const;

private:
    QJsonDocument json_doc;
    QJsonParseError last_error;
};
#else
    static_assert(false, "No serializer defined.");
#endif

#endif // SERIALIZERS_H
