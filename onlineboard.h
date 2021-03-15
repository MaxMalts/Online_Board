#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "canvas.h"
#include "serverapi.h"


QT_BEGIN_NAMESPACE
namespace Ui { class OnlineBoard; }
QT_END_NAMESPACE


class OnlineBoard : public QMainWindow
{
    Q_OBJECT

private:
    const QSize init_size = QSize(500, 500);

public:
    OnlineBoard(QWidget* parent = nullptr);
    ~OnlineBoard();

private:
    Ui::OnlineBoard* ui;

    Canvas* canvas;
};
#endif // MAINWINDOW_H
