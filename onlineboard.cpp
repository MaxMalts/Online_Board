#include <QLayout>
#include <QString>
#include <QSizePolicy>
#include <QDebug>

#include "onlineboard.h"
#include "ui_onlineboard.h"
#include "canvas.h"

OnlineBoard::OnlineBoard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OnlineBoard)
{
    ui->setupUi(this);

    while (!ServerApi::connectToServer()) {
        statusBar()->setStyleSheet("color: red;");
        statusBar()->showMessage("Error while connecting to server: " +
                                 ServerApi::lastErrorStr());

        qDebug() << "Error while connecting to server: " << ServerApi::lastError();
    }

    statusBar()->setStyleSheet("");
    statusBar()->showMessage("Connected to server.", 3000);

    canvas = new Canvas(QSize(0, 0), centralWidget());
    canvas->lower();
    canvas->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
}

void OnlineBoard::resizeEvent(QResizeEvent*)
{
    QSize canvas_size(size().width(),
                      size().height() - statusBar()->height() -
                      menuBar()->height() + 1);

    canvas->resize(canvas_size);
}

OnlineBoard::~OnlineBoard()
{
    delete ui;
}
