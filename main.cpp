#include "onlineboard.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OnlineBoard w;
    w.show();
    return a.exec();
}
