#include "onlineboard.h"
#include "ui_mainwindow.h"
#include "canvas.h"

#include <QLayout>

OnlineBoard::OnlineBoard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OnlineBoard)
{
    ui->setupUi(this);
    canvas = new Canvas(centralWidget(), size());

//    socket = new QTcpSocket(this);
//    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReadyToRead()));
//    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisconnected()));

//    socket->connectToHost("127.0.0.1", 5555);
//    socket->waitForConnected(30000);
}


OnlineBoard::~OnlineBoard()
{
    delete ui;
}


void OnlineBoard::sockReadyToRead()
{
    QByteArray data = socket->readAll();
    qDebug() << data;
}


void OnlineBoard::sockDisconnected()
{
    socket->deleteLater();
}
