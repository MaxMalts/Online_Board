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
    virtual void serialize(const Serializable& object) = 0;
    virtual void deserialize(Serializable& object) const = 0;
    virtual QByteArray getData() const = 0;
    virtual void set(const QByteArray& data) = 0;
};


class JsonSerializer : public Serializer{
public:
    JsonSerializer(const Serializable& object);
    JsonSerializer(const QByteArray& data);
    JsonSerializer(const QJsonObject& json_doc);

    virtual void serialize(const Serializable& object) override;
    virtual void deserialize(Serializable& object) const override;
    virtual QByteArray getData() const override;
    virtual void set(const QByteArray& data) override;

    QJsonDocument getJson() const;
    void set(const QJsonObject& json_doc);

    QJsonParseError lastError() const;

private:
    QJsonDocument json_doc;
    QJsonParseError last_error;
};

#endif // SERIALIZERS_H
