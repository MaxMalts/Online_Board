#include "onlineboard.h"
#include "ServerApi/serverapi.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerApi server_api;
    OnlineBoard w;
    w.show();
    return a.exec();
}
