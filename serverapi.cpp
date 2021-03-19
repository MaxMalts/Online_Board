#include "serverapi.h"

#include <stdint.h>
#include <QByteArray>
#include <QJsonDocument>
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

void ServerApi::sAddLayer(const Serializer& argument)
{
    sendMethod("s_add_layer", argument);
}

//QByteArray ServerApi::readData()
//{
//    QByteArray package = socket->readAll();
//    int data_size = package.split('\n').at(0).toInt();
//    return socket->readAll();
//}

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
    uint64_t header_size = 0;
    while (socket->read(reinterpret_cast<char*>(&header_size), sizeof(header_size)) ==
           sizeof(header_size)) {
        Q_ASSERT(header_size > 0);

        QByteArray data = socket->read(header_size);
        Q_ASSERT(data.size() == header_size);

        QJsonObject header = QJsonDocument::fromJson(data).object();
        int arg_size = header["argument_size"].toInt();
        Q_ASSERT(arg_size > 0);

        data = socket->read(arg_size);
        Q_ASSERT(data.size() == arg_size);

#ifdef JSON_SERIALIZER
        JsonSerializer serial_arg(data);
#endif

        Q_ASSERT(str_to_signal.contains(header["method"].toString()));
        void (ServerApi::*test)(const Serializer&) = &ServerApi::cAddLayer;
        emit (this->*str_to_signal[header["mehtod"].toString()])(serial_arg);
    }
}

void ServerApi::onDisconnected()
{
    qDebug() << "Socket disconnected.";
}

bool ServerApi::sendMethod(const QString& method, const Serializer& argument)
{
    QByteArray arg_data = argument.getData();
    int arg_data_size = arg_data.size();

    QJsonObject header;
    header["client_id"] = 1;  // To be implemented
    header["method"] = method;
    header["argument_size"] = arg_data_size;

    QByteArray header_data = QJsonDocument(header).toJson();
    uint64_t header_size = header_data.size();

    QByteArray package;
    package.resize(sizeof(header_size) + header_size + 1 + arg_data_size);
    package += QByteArray(reinterpret_cast<char*>(&header_size), sizeof(header_size)) +
            header_data + '\n' + arg_data;

    return socket->write(package) == package.size();
}
