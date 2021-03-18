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
    int data_size = data.size();
    QByteArray package = QString::number(data_size).append('\n\r').toUtf8().append(data);

    return socket->write(package) == package.size();
}

bool ServerApi::sendData(const char* data)
{
    int data_size = qstrlen(data);
    QByteArray package = QString::number(data_size).toUtf8().append(QString(data).toUtf8());

    return socket->write(package) == package.size();
}

QByteArray ServerApi::readData()
{
    QByteArray package = socket->readAll();
    int data_size = package.split('\n').at(0).toInt();
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
}

void ServerApi::onDisconnected()
{
    qDebug() << "Socket disconnected.";
}
