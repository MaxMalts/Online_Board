#include "serverapi.h"

#include <stdint.h>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QEventLoop>
#include <QString>
#include <QDebug>

#include "common.h"


/* ClientProps */

bool ServerApi::ClientProps::serialize(QJsonObject& json) const
{
    json.insert("client_id", client_id);
    return true;
}

bool ServerApi::ClientProps::deserialize(const QJsonObject& json)
{
    Q_ASSERT(!json.isEmpty());

    if (!json.contains("client_id")) {
        return false;
    }

    QJsonValue cur_value = json.value("client_id");
    if (!cur_value.isDouble()) {
        return false;
    }

    client_id = cur_value.toInt();
    return true;
}


/* ServerApi */

ServerApi::ServerConfig ServerApi::config;
ServerApi* ServerApi::instance = nullptr;

ServerApi::ServerApi(QObject* parent) : QObject(parent)
{
    if (instance == nullptr) {
        Q_ASSERT(socket == nullptr);
        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

        connect(this, SIGNAL(cInitClient(const Serializer&)),
                this, SLOT(onInitClient(const Serializer&)));

        reconnectTimer.setInterval(config.reconnect_interval);
        connect(&reconnectTimer, SIGNAL(timeout()), this, SLOT(onReconnectTimer()));

        instance = this;
    }
}

ServerApi* ServerApi::getInstance()
{
    return instance;
}

bool ServerApi::connectToServer()
{
    instance->socket->connectToHost(config.ip_address, config.port);
    if (!instance->socket->waitForConnected(config.connect_timeout) ||
        !waitForSignal(instance, &ServerApi::cInitClient, config.connect_timeout)) {

        qDebug() << "Connection failed:" << lastError();

        instance->reconnectTimer.start();
        return false;
    }

    qDebug() << "Socket connected.";

    emit instance->connected();
    return true;
}

void ServerApi::sAddLayer(const Serializer& argument)
{
    instance->sendMethod("s_add_layer", argument);
}

QTcpSocket::SocketError ServerApi::lastError()
{
    return instance->socket->error();
}

QString ServerApi::lastErrorStr()
{
    return instance->socket->errorString();
}

void ServerApi::onInitClient(const Serializer& argument)
{
    argument.deserialize(props);
    Q_ASSERT(props.client_id > 0);
}


void ServerApi::onReadyRead()
{
    uint64_t header_size = 0;
    while (socket->read(reinterpret_cast<char*>(&header_size), sizeof(header_size)) ==
           sizeof(header_size)) {
        Q_ASSERT(header_size > 0);

        QByteArray data = socket->read(header_size);
        Q_ASSERT(static_cast<uint64_t>(data.size()) == header_size);
        qDebug() << "Data received from server:";
        qDebug().noquote() << QString::fromUtf8(data);

        QJsonObject header = QJsonDocument::fromJson(data).object();
        int arg_size = header["argument_size"].toInt();
        Q_ASSERT(arg_size > 0);

        data = socket->read(arg_size);
        Q_ASSERT(data.size() == arg_size);
        qDebug().noquote().nospace() << QString::fromUtf8(data);

#ifdef JSON_SERIALIZER
        JsonSerializer serial_arg(data);
#else
        static_assert(false, "No serializer defined.");
#endif

        Q_ASSERT(str_to_signal.contains(header["method"].toString()));
        emit (this->*str_to_signal[header["method"].toString()])(serial_arg);
    }
}

void ServerApi::onDisconnected()
{
    qDebug() << "Socket disconnected.";
    emit disconnected();

    reconnectTimer.start();
}

void ServerApi::onReconnectTimer()
{
    reconnectTimer.stop();

    qDebug() << "Reconnecting...";

    if (!connectToServer()) {
        reconnectTimer.start();

    } else {
        emit connected();
    }
}

bool ServerApi::sendMethod(const QString& method, const Serializer& argument)
{
    QByteArray arg_data = argument.getData();
    int arg_data_size = arg_data.size();

    QJsonObject header;
    header.insert("client_id", props.client_id);
    header.insert("method", method);
    header.insert("argument_size", arg_data_size);

    QByteArray header_data = QJsonDocument(header).toJson();
    uint64_t header_size = header_data.size();

    QByteArray package;
    package.reserve(sizeof(header_size) + header_size + 1 + arg_data_size);
    package += QByteArray(reinterpret_cast<char*>(&header_size), sizeof(header_size)) +
            '\n' + header_data + arg_data;

    if (socket->write(package) == package.size()) {
        qDebug() << "Data sent to server:";
        qDebug().noquote() << package.data() + sizeof(header_size);
        return true;
    }
    return false;
}
