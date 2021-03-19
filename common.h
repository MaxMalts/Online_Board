#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QTimer>
#include <QEventLoop>

template<typename QObject_T>
bool waitForSignal(QObject_T* object, void (QObject_T::*signal), int ms_timeout)
{
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;

    QObject_T::connect(object, signal, &loop, &QEventLoop::quit);
    QObject_T::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(ms_timeout);
    loop.exec();

    return timer.isActive();
}

#endif // COMMON_H
