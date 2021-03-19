#ifndef SERVERAPI_H
#define SERVERAPI_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

#include "serializers.h"


class ServerApi : public QObject
{
    Q_OBJECT

private:
    static struct ServerConfig
    {
        const QString ip_address = "localhost";
        const quint16 port = 5555;
        const int connect_timeout = 10000;
    } config;


public:
    explicit ServerApi(QObject* parent = nullptr);

    static ServerApi* getInstance();

    static bool connectToServer();
    //static QByteArray readData();
    static void sAddLayer(const Serializer& argument);

    static QTcpSocket::SocketError lastError();
    static QString lastErrorStr();

signals:
    //void dataReceived();
    void cAddLayer(const Serializer& argument);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    static bool sendMethod(const QString& method, const Serializer& argument);

    static QTcpSocket* socket;
    static ServerApi* instance;

    const QMap<QString, void (ServerApi::*)(const Serializer&)> str_to_signal {
        { "c_add_layer", &ServerApi::cAddLayer }
    };
};

#endif // SERVERAPI_H
