#ifndef SERVERAPI_H
#define SERVERAPI_H

#include <QObject>
#include <QTcpSocket>

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

    static bool connectToServer();
    static bool sendData(const QByteArray& data);
    static QByteArray receiveData();

    static QTcpSocket::SocketError lastError();
    static QString lastErrorStr();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    static QTcpSocket* socket;

};

#endif // SERVERAPI_H
