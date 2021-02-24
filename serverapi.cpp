#include "serverapi.h"

#include <QDebug>

ServerApi::ServerApi(QObject* parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

bool ServerApi::connectToServer()
{
    socket->connectToHost(ip_address, port);
    return socket->waitForConnected(connect_timeout);
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


void ServerApi::onReadyRead()
{
    qDebug() << socket->readAll();
}

void ServerApi::onDisconnected()
{
    qDebug() << "Disconnected.";
}
