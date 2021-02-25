#include "serverapi.h"

#include <QDebug>

QTcpSocket* ServerApi::socket = nullptr;
ServerApi::ServerConfig ServerApi::config;

ServerApi::ServerApi(QObject* parent) : QObject(parent)
{
    if (socket == nullptr) {
        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    }
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

QByteArray ServerApi::receiveData()
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
    qDebug() << "Socket received data: " << socket->readAll();
}

void ServerApi::onDisconnected()
{
    qDebug() << "Socket disconnected.";
}
