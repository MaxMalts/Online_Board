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

    static ServerApi* getInstance();

    static bool connectToServer();
    static bool sendData(const QByteArray& data);
    static bool sendData(const char* data);
    static bool sendData(const char* data, qint64 maxSize);
    static QByteArray readData();

    static QTcpSocket::SocketError lastError();
    static QString lastErrorStr();

signals:
    void dataReceived();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    static QTcpSocket* socket;
    static ServerApi* instance;
};

#endif // SERVERAPI_H
