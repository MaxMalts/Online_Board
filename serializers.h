#ifndef SERIALIZERS_H
#define SERIALIZERS_H

#include <QJsonDocument>
#include <QByteArray>


class Serializable {
public:
    virtual bool serialize(QJsonObject& json) const = 0;
    virtual bool deserialize(const QJsonObject& json) = 0;
};


// Represents object in serialized format
class Serializer {
public:
    virtual bool serialize(const Serializable& object) = 0;
    virtual bool deserialize(Serializable& object) const = 0;
    virtual QByteArray getData() const = 0;
    virtual bool set(const QByteArray& data) = 0;

    virtual bool isNull() const = 0;
};


class JsonSerializer : public Serializer{
public:
    JsonSerializer(const Serializable& object);
    JsonSerializer(const QByteArray& data);
    JsonSerializer(const QJsonObject& json_doc);

    virtual bool serialize(const Serializable& object) override;
    virtual bool deserialize(Serializable& object) const override;
    virtual QByteArray getData() const override;
    virtual bool set(const QByteArray& data) override;

    QJsonDocument getJson() const;
    bool set(const QJsonObject& json_doc);

    virtual bool isNull() override;

    QJsonParseError lastError() const;

private:
    QJsonDocument json_doc;
    QJsonParseError last_error;
};

#endif // SERIALIZERS_H
