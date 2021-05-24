#include "packagereadmanager.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <stdint.h>


void PackageReadManager::addSocket(QTcpSocket* socket)
{
    Q_ASSERT(socket != nullptr);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    sockets_states.insert(socket, ReadState(socket));
}

void PackageReadManager::removeSocket(QTcpSocket* socket)
{
    Q_ASSERT(socket != nullptr);
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    sockets_states.remove(socket);
}

void PackageReadManager::onReadyRead()
{
    ReadState& state = sockets_states[dynamic_cast<QTcpSocket*>(sender())];
    Q_ASSERT(state.socket != nullptr);

    switch (state.status) {
    case nothing_read:
        handleNothingRead(state);
        break;
    case header_size_read:
        handleHeaderSizeRead(state);
        break;
    case header_read:
        handleHeaderRead(state);
        break;
    default:
        Q_ASSERT(false);
    }

    while (state.status == argument_read) {
        emit packageReceived(state.socket, state.header, state.argument);
        state.status = nothing_read;
        handleNothingRead(state);
    }
}

void PackageReadManager::handleNothingRead(ReadState& state)
{
    Q_ASSERT(state.status == nothing_read);

    if (state.socket->bytesAvailable() >=
        static_cast<qint64>(sizeof(state.header_size))) {

        state.socket->read(reinterpret_cast<char*>(&state.header_size),
                           sizeof(state.header_size));
        Q_ASSERT(state.header_size > 0);

        state.status = header_size_read;
        handleHeaderSizeRead(state);
    }
}

void PackageReadManager::handleHeaderSizeRead(ReadState& state)
{
    Q_ASSERT(state.status == header_size_read);

    if (state.socket->bytesAvailable() >=
        static_cast<qint64>(state.header_size)) {

        QByteArray header_raw = state.socket->read(state.header_size);
        Q_ASSERT(static_cast<uint64_t>(header_raw.size()) == state.header_size);
        qDebug() << "Data received from server:";
        qDebug().noquote() << QString::fromUtf8(header_raw);

        state.header = QJsonDocument::fromJson(header_raw).object();
        state.arg_size = state.header.value("argument_size").toInt();
        Q_ASSERT(state.arg_size > 0);

        state.status = header_read;
        handleHeaderRead(state);
    }
}

void PackageReadManager::handleHeaderRead(ReadState& state)
{
    Q_ASSERT(state.status == header_read);

    if (state.socket->bytesAvailable() >= state.arg_size) {
        state.argument = state.socket->read(state.arg_size);
        Q_ASSERT(state.argument.size() == state.arg_size);
        qDebug().noquote().nospace() << QString::fromUtf8(state.argument);

        state.status = argument_read;
    }
}
