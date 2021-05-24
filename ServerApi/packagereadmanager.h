#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QTcpSocket>
#include <QMap>
#include <stdint.h>


class PackageReadManager : public QObject
{
    Q_OBJECT

public:
    void addSocket(QTcpSocket* socket);
    void removeSocket(QTcpSocket* socket);

signals:
    void packageReceived(QTcpSocket* socket, const QJsonObject& header,
                         const QByteArray& argument);

private slots:
    void onReadyRead();

private:
    enum ReadStatus {
        nothing_read,
        header_size_read,
        header_read,
        argument_read
    };

    struct ReadState {
        ReadState() = default;
        ReadState(QTcpSocket* socket) : socket(socket) {}

        QTcpSocket* socket = nullptr;
        ReadStatus status = nothing_read;

        uint64_t header_size = 0;
        QJsonObject header;

        int arg_size = 0;
        QByteArray argument;
    };

    void handleNothingRead(ReadState& state);
    void handleHeaderSizeRead(ReadState& state);
    void handleHeaderRead(ReadState& state);

    QMap<QTcpSocket*, ReadState> sockets_states;
};

#endif // COMMUNICATIONMANAGER_H
