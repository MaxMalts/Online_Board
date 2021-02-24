#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvas.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), canvas(this, init_size)
{
    ui->setupUi(this);
    resize(init_size);
//    socket = new QTcpSocket(this);
//    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReadyToRead()));
//    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisconnected()));

//    socket->connectToHost("127.0.0.1", 5555);
//    socket->waitForConnected(30000);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::sockReadyToRead()
{
    QByteArray data = socket->readAll();
    qDebug() << data;
}


void MainWindow::sockDisconnected()
{
    socket->deleteLater();
}
