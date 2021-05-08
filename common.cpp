#include "common.h"

#include <QApplication>
#include <QMouseEvent>
#include <QWindow>
#include <QGuiApplication>


/* EventSignalAdapter */

EventSignalAdapter::EventSignalAdapter(QWidget* widget)
    : QObject(widget),
      widget(widget) {
    widget->installEventFilter(this);
}

QWidget* EventSignalAdapter::getWidget() {
    return widget;
}

bool EventSignalAdapter::eventFilter(QObject*, QEvent* event) {
    switch (event->type()) {
    case QEvent::Enter:
        emit enter();
        break;

    case QEvent::Leave:
        right_press_no_leave = false;
        emit leave();
        break;

    case QEvent::FocusIn:
        emit focusIn();
        break;

    case QEvent::FocusOut:
        emit focusOut();
        break;

    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick: {
        QMouseEvent* mouse_event =
                dynamic_cast<QMouseEvent*>(event);

        if (mouse_event->button() == Qt::RightButton) {
            right_press_no_leave = true;
        }
        break;
    }

    case QEvent::MouseButtonRelease: {
        QMouseEvent* mouse_event =
                dynamic_cast<QMouseEvent*>(event);

        if (mouse_event->button() == Qt::RightButton &&
                right_press_no_leave) {

            right_press_no_leave = false;
            emit rClick();
        }
        break;
    }

    default: {}  // To prevent warnings
    }

    return false;
}


///* EventsForwarder */

//MouseForwarder::MouseForwarder(QWidget* to_forward, QWidget* widget)
//    : QObject(widget),
//      to_forward(to_forward) {}


//MouseForwarder::MouseForwarder(const QSet<QEvent::Type>& events, QWidget* to_forward, QWidget* widget)
//    : QObject(widget),
//      to_forward(to_forward),
//      events(events) {}

//bool MouseForwarder::eventFilter(QObject* watched, QEvent* event)
//{
//    if (event->type() == QEvent::MouseButtonPress ||
//        event->type() == QEvent::MouseButtonDblClick) {

//        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
//        QMouseEvent* event_copy = new QMouseEvent(mouse_event->type(), mouse_event->pos(), mouse_event->windowPos(), mouse_event->globalPos(),
//                                                  mouse_event->button(), mouse_event->buttons(), mouse_event->modifiers(), mouse_event->source());

//        //widget->setAttribute(Qt::WA_TransparentForMouseEvents);
//        //QWidget* test = static_cast<QWidget*>(widget->children().at(0)->children().at(1));
//        //widget->releaseMouse();//setAttribute(Qt::WA_TransparentForMouseEvents);
//        //qApp->processEvents();
//        bool test = event_copy->spontaneous();
//        qApp->postEvent(dynamic_cast<Canvas*>(to_forward), event_copy);
//        //widget->setAttribute(Qt::WA_TransparentForMouseEvents, false);

//        //event->ignore();
//        //return true;
//    }

//    return false;
//}
