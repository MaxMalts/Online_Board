#include "onlineboard.h"
#include "ui_mainwindow.h"
#include "canvas.h"

#include <QLayout>
#include <QString>
#include <QDebug>

OnlineBoard::OnlineBoard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OnlineBoard)
{
    ui->setupUi(this);
    canvas = new Canvas(centralWidget(), size());

    if (!ServerApi::connectToServer()) {
        statusBar()->setStyleSheet("color: red;");
        statusBar()->showMessage("Error while connecting to server: " +
                                 ServerApi::lastErrorStr());
        qDebug() << "Error while connecting to server: " << ServerApi::lastError();
    }
}

OnlineBoard::~OnlineBoard()
{
    delete ui;
}
