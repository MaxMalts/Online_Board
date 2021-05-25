#include "serverapi.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QEventLoop>
#include <QString>
#include <QFile>
#include <QDebug>
#include <stdint.h>

#include "common.h"
#include "serializers.h"


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


/* ServerConfig */

void ServerApi::ServerConfig::initFromConfigFile()
{
    const QString config_file_name = "server_config.json";

    QFile config_file(config_file_name);
    if (!config_file.open(QIODevice::ReadOnly)) {
        qDebug() << "Can not open" << config_file_name << "file:" <<
                    config_file.errorString();
        return;
    }

    QJsonObject config_json = QJsonDocument::fromJson(config_file.readAll()).object();

    config_file.close();

    QJsonValue cur_value = config_json.value("server_ip");
    if (cur_value.isString()) {
        server_ip = cur_value.toString();
    }

    cur_value = config_json.value("server_port");
    if (cur_value.isDouble()) {
        server_port = cur_value.toInt();
    }

    cur_value = config_json.value("connect_timeout_ms");
    if (cur_value.isDouble()) {
        connect_timeout_ms = cur_value.toInt();
    }

    cur_value = config_json.value("reconnect_interval_ms");
    if (cur_value.isDouble()) {
        reconnect_interval_ms = cur_value.toInt();
    }
}


/* ServerApi */

ServerApi* ServerApi::instance = nullptr;

ServerApi::ServerApi(QObject* parent) : QObject(parent)
{
    if (instance == nullptr) {
        Q_ASSERT(socket == nullptr);

        config.initFromConfigFile();

        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

        read_manager.addSocket(socket);
        connect(&read_manager,
                SIGNAL(packageReceived(QTcpSocket*, const QJsonObject&,
                                       const QByteArray&)),
                this,
                SLOT(onPackageReceived(QTcpSocket*, const QJsonObject&,
                                       const QByteArray&)));

        connect(this, SIGNAL(cInitClient(const Serializer&)),
                this, SLOT(onInitClient(const Serializer&)));

        reconnectTimer.setInterval(config.reconnect_interval_ms);
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
    ServerConfig& config = instance->config;
    QTcpSocket* socket = instance->socket;

    socket->connectToHost(config.server_ip, config.server_port);
    bool connected = waitForSignal(static_cast<QAbstractSocket*>(socket),
                                   &QAbstractSocket::connected,
                                   config.connect_timeout_ms);
    if (!connected ||
        !waitForSignal(instance, &ServerApi::cInitClient, config.connect_timeout_ms)) {

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

void ServerApi::sDeleteLayer(const Serializer& argument)
{
    instance->sendMethod("s_delete_layer", argument);
}

void ServerApi::sUndo()
{
    instance->sendMethod("s_undo");
}

void ServerApi::sClearBoard()
{
    instance->sendMethod("s_clear_board");
}

QTcpSocket::SocketError ServerApi::lastError()
{
    return instance->socket->error();
}

QString ServerApi::lastErrorStr()
{
    return instance->socket->errorString();
}

ServerApi::~ServerApi()
{
    read_manager.removeSocket(socket);
    socket->close();
}

void ServerApi::onInitClient(const Serializer& argument)
{
    argument.deserialize(props);
    Q_ASSERT(props.client_id > 0);
}

void ServerApi::onPackageReceived(QTcpSocket* socket,
                                  const QJsonObject& header,
                                  const QByteArray& argument)
{
    Q_ASSERT(socket != nullptr);
    Q_ASSERT(!header.isEmpty());
    Q_ASSERT(argument.size() > 0);

#ifdef JSON_SERIALIZER
    JsonSerializer serial_arg(argument);
#else
    static_assert(false, "No serializer defined.");
#endif

    Q_ASSERT(str_to_signal.contains(header.value("method").toString()));
    emit (this->*str_to_signal[header.value("method").toString()])(serial_arg);
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
    package.reserve(sizeof(header_size) + header_size + arg_data_size);
    package += QByteArray(reinterpret_cast<char*>(&header_size), sizeof(header_size)) +
                          header_data + arg_data;

    if (socket->write(package) == package.size()) {
        qDebug() << "Data sent to server:";
        qDebug().noquote() << package.data() + sizeof(header_size);
        return true;
    }
    return false;
}
