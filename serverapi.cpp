#include "serverapi.h"

#include <QDebug>

ServerApi::ServerConfig ServerApi::config;
QTcpSocket* ServerApi::socket = nullptr;
ServerApi* ServerApi::instance = nullptr;

ServerApi::ServerApi(QObject* parent) : QObject(parent)
{
    if (instance == nullptr) {
        Q_ASSERT(socket == nullptr);
        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

        instance = this;

    } else {
        Q_ASSERT(socket != nullptr);
    }
}

ServerApi* ServerApi::getInstance()
{
    return instance;
}

bool ServerApi::connectToServer()
{
    socket->connectToHost(config.ip_address, config.port);
    return socket->waitForConnected(config.connect_timeout);
}

bool ServerApi::sendData(const QByteArray& data)
{
    return socket->write(data) == data.size();
}

bool ServerApi::sendData(const char* data)
{
    return socket->write(data) != -1;
}

bool ServerApi::sendData(const char* data, qint64 maxSize)
{
    return socket->write(data, maxSize) != -1;
}

QByteArray ServerApi::readData()
{
    return socket->readAll();
}

QTcpSocket::SocketError ServerApi::lastError()
{
    return socket->error();
}

QString ServerApi::lastErrorStr()
{
    return socket->errorString();
}


void ServerApi::onReadyRead()
{
    emit dataReceived();
    //qDebug() << "Socket received data: " << socket->readAll();
}

void ServerApi::onDisconnected()
{
    qDebug() << "Socket disconnected.";
}
