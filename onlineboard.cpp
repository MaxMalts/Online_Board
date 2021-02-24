#include "onlineboard.h"
#include "ui_mainwindow.h"
#include "canvas.h"

#include <QLayout>
#include <QString>

OnlineBoard::OnlineBoard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OnlineBoard)
{
    ui->setupUi(this);
    canvas = new Canvas(centralWidget(), size());

    server_api = new ServerApi(this);
    if (!server_api->connectToServer()) {
        statusBar()->setStyleSheet("color: red;");
        statusBar()->showMessage("Error while connecting to server.");
        qDebug() << "Error while connecting to server: " << server_api->lastError();
    }
}

OnlineBoard::~OnlineBoard()
{
    delete ui;
}
