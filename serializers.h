#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QJsonDocument>


class Serializable {
public:
    virtual void serialize(QJsonObject& json) const = 0;
    virtual void deserialize(const QJsonObject& json) = 0;
};


class Serializer {
public:
    virtual void serialize(const Serializable& object) = 0;
    virtual void deserialize(Serializable& object) = 0;
    virtual QByteArray getData() = 0;
    virtual void set(const QByteArray& data) = 0;
};


class JsonSerializer : public Serializer{
public:
    JsonSerializer(const Serializable& object);
    JsonSerializer(const QByteArray& data);
    JsonSerializer(const QJsonObject& json);

    virtual void serialize(const Serializable& object) override;
    virtual void deserialize(Serializable& object) override;
    virtual QByteArray getData() override;
    virtual void set(const QByteArray& data) override;

    QJsonDocument getJson();
    void set(const QJsonObject& json);

    QJsonParseError lastError();

private:
    QJsonDocument json;
    QJsonParseError last_error;
};

#endif // SERIALIZER_H
