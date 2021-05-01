#include "serverapi.h"

#include <stdint.h>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <QSizeF>
#include <QTimer>
#include <QEventLoop>
#include <QString>
#include <QDebug>

#include "common.h"


/* AddLayerArgs */

const BiMap<QString, AddLayerArgs::LayerType> AddLayerArgs::str_layertype_map {
    { "line", AddLayerArgs::LayerType::line }
};

#ifdef JSON_SERIALIZER
bool AddLayerArgs::serialize(QJsonObject& json) const
{
    if (layer_data.isNull()) {
        return false;
    }

    json = QJsonObject();
    json.insert("position", QJsonArray{ position.x(), position.y() });
    json.insert("size", QJsonArray{ size.width(), size.height() });
    json.insert("layer_type", str_layertype_map.leftByRight(layer_type));

    QJsonObject layer_data_json = layer_data.getJson().object();
    Q_ASSERT(!layer_data_json.isEmpty());
    json.insert("layer_data", layer_data_json);

    return true;
}

bool AddLayerArgs::deserialize(const QJsonObject& json)
{
    // position
    QJsonValue cur_value = json.value("position");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue x = cur_value.toArray().at(0);
    QJsonValue y = cur_value.toArray().at(1);
    if (!x.isDouble() || !y.isDouble()) {
        return false;
    }
    position = QPointF(x.toDouble(), y.toDouble());

    // size
    cur_value = json.value("size");
    if (!cur_value.isArray()) {
        return false;
    }
    QJsonValue width = cur_value.toArray().at(0);
    QJsonValue height = cur_value.toArray().at(1);
    if (!width.isDouble() || !height.isDouble()) {
        return false;
    }
    size = QSizeF(width.toDouble(), height.toDouble());

    // layer_type
    cur_value = json.value("layer_type");
    if (!cur_value.isString()) {
        return false;
    }
    layer_type = str_layertype_map.rightByLeft(cur_value.toString());

    //layer_data
    cur_value = json.value("layer_data");
    if (!cur_value.isObject()) {
        return false;
    }
    layer_data.set(cur_value.toObject());
    Q_ASSERT(!layer_data.isNull());

    return true;
}
#else
static_assert(false, "No serializer defined.");
#endif


/* ClientProps */

bool ServerApi::ClientProps::serialize(QJsonObject& json) const
{
    json["client_id"] = client_id;
    return true;
}

bool ServerApi::ClientProps::deserialize(const QJsonObject& json)
{
    if (!json.contains("client_id")) {
        return false;
    }

    client_id = json["client_id"].toInt();
    return true;;
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
    if (!instance->socket->waitForConnected(config.connect_timeout)) {
        return false;
    }

    return waitForSignal(instance, &ServerApi::cInitClient, config.connect_timeout);
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

        QJsonObject header = QJsonDocument::fromJson(data).object();
        int arg_size = header["argument_size"].toInt();
        Q_ASSERT(arg_size > 0);

        data = socket->read(arg_size);
        Q_ASSERT(data.size() == arg_size);

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
