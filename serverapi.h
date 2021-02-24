#ifndef SERVERAPI_H
#define SERVERAPI_H

#include <QObject>
#include <QTcpSocket>

class ServerApi : public QObject
{
    Q_OBJECT

private:
    const QString ip_address = "localhost";
    const quint16 port = 5555;
    const int connect_timeout = 10000;

public:
    explicit ServerApi(QObject* parent = nullptr);

    bool connectToServer();
    bool sendData(const QByteArray& data);
    QByteArray receiveData();

    QTcpSocket::SocketError lastError();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket* socket;

};

#endif // SERVERAPI_H
