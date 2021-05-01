#include <QLayout>
#include <QString>
#include <QDebug>

#include "onlineboard.h"
#include "ui_onlineboard.h"
#include "canvas.h"

OnlineBoard::OnlineBoard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OnlineBoard)
{
    ui->setupUi(this);

    if (ServerApi::connectToServer()) {
        statusBar()->setStyleSheet("");
        statusBar()->showMessage("Connected to server.", 3000);

        QSize canvas_size(size().width(),
                          size().height() - statusBar()->height() - menuBar()->height());
        canvas = new Canvas(canvas_size, centralWidget());

    } else {
        statusBar()->setStyleSheet("color: red;");
        statusBar()->showMessage("Error while connecting to server: " +
                                 ServerApi::lastErrorStr());
        qDebug() << "Error while connecting to server: " << ServerApi::lastError();
        qDebug() << size() << frameSize();
    }
}

OnlineBoard::~OnlineBoard()
{
    delete ui;
}
