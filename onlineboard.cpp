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

    canvas = new Canvas(QSize(0, 0), centralWidget());
    canvas->lower();
    canvas->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);

    connect(ServerApi::getInstance(), SIGNAL(disconnected()),
            this, SLOT(onServerDisconnected()));
    connect(ServerApi::getInstance(), SIGNAL(connected()),
            this, SLOT(onServerConnected()));

    if (!ServerApi::connectToServer()) {
        statusBar()->setStyleSheet("color: red;");
        statusBar()->showMessage("Disconnected from server: " +
                                 ServerApi::lastErrorStr() + ". Reconnecting...");
        setEnabled(false);

        qDebug() << "Error while connecting to server: " << ServerApi::lastError();

    } else {
        statusBar()->setStyleSheet("");
        statusBar()->showMessage("Connected to server.", 3000);
    }
}

void OnlineBoard::resizeEvent(QResizeEvent*)
{
    QSize canvas_size(size().width(),
                      size().height() - statusBar()->height() -
                      menuBar()->height() + 1);

    canvas->resize(canvas_size);
}

void OnlineBoard::onServerDisconnected()
{
    statusBar()->setStyleSheet("color: red;");
    statusBar()->showMessage("Disconnected from server: " +
                             ServerApi::lastErrorStr() + " Reconnecting...");
    setEnabled(false);

    qDebug() << "Disconnected from server: " << ServerApi::lastError();
}


void OnlineBoard::onServerConnected()
{
    setEnabled(true);

    statusBar()->setStyleSheet("");
    statusBar()->showMessage("Connected to server.", 3000);
}

OnlineBoard::~OnlineBoard()
{
    delete ui;
}
