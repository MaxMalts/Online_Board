#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

#include "canvas.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    const QSize init_size = QSize(500, 500);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void sockReadyToRead();
    void sockDisconnected();


private:
    Ui::MainWindow *ui;

    QTcpSocket* socket;
    Canvas* canvas;
};
#endif // MAINWINDOW_H
