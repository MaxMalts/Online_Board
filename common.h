#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QEventLoop>
#include <QMap>
#include <QDebug>
#include <initializer_list>
#include <utility>


template<typename QObject_T, typename... Args>
bool waitForSignal(QObject_T* object, void (QObject_T::*signal)(Args... args), int ms_timeout)
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


// Bidirectinoal map
template<typename LeftVal, typename RightVal>
class BiMap {
public:
    BiMap() = default;
    BiMap(const BiMap<LeftVal, RightVal>& other) = default;
    BiMap(BiMap<LeftVal, RightVal>&& other) = default;

    BiMap(std::initializer_list<std::pair<LeftVal, RightVal>> list) {
        for (auto& pair : list) {
            insert(pair.first, pair.second);
        }
    }

    BiMap<LeftVal, RightVal>& operator=(const BiMap<LeftVal, RightVal>& other) = default;

    bool insert(const LeftVal& left_val, const RightVal& right_val) {
        typename QMap<LeftVal, RightVal>::iterator foundLeft =
                left_to_right.find(left_val);
        if (foundLeft != left_to_right.end()) {
            return false;
        }

        typename QMap<RightVal, LeftVal>::iterator foundRight =
                right_to_left.find(right_val);
        if  (foundRight != right_to_left.end()) {
            return false;
        }

        left_to_right.insert(left_val, right_val);
        right_to_left.insert(right_val, left_val);

        return true;
    }

    const RightVal rightByLeft(const LeftVal& left,
                               const RightVal& defaultValue = RightVal()) const {

        return left_to_right.value(left, defaultValue);
    }

    const LeftVal leftByRight(const RightVal& right,
                              const LeftVal& defaultValue = LeftVal()) const {

        return right_to_left.value(right, defaultValue);
    }

private:
    QMap<LeftVal, RightVal> left_to_right;
    QMap<RightVal, LeftVal> right_to_left;
};


/*
 * This class generates signals for useful events as the
 * QWidget has very scarce set of signals implemented! >:(
*/
class EventSignalAdapter : public QObject {
    Q_OBJECT

public:
    EventSignalAdapter(QWidget* widget)
        : QObject(widget),
          widget(widget) {
        widget->installEventFilter(this);
    }

    QWidget* getWidget() {
        return widget;
    }

    virtual ~EventSignalAdapter() = default;

signals:
    void rClick();
    void enter();
    void leave();
    void focusIn();
    void focusOut();

protected:
    bool eventFilter(QObject* /*watched*/, QEvent* event) override {
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

private:
    QWidget* widget;

    bool right_press_no_leave = false;
};

#endif // COMMON_H
