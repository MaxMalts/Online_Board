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
    void serialize(const Serializable& object) override;
    void deserialize(Serializable& object) override;
    QByteArray getData() override;
    void set(const QByteArray& data) override;

    QJsonDocument getJson();
    void set(const QJsonObject& json);

    QJsonParseError lastError();

private:
    QJsonDocument json;
    QJsonParseError last_error;
};

#endif // SERIALIZER_H
