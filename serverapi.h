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
    //static bool sendData(const QByteArray& data);
    //static bool sendData(const char* data);
    //static QByteArray readData();
    static void sAddLayer(const LayerData& argument);

    static QTcpSocket::SocketError lastError();
    static QString lastErrorStr();

signals:
    //void dataReceived();
    void cAddLayer(const LayerData& arg);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    static QTcpSocket* socket;
    static ServerApi* instance;
};

#endif // SERVERAPI_H
