#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QEventLoop>
#include <QMap>
#include <QSet>
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

    const RightVal atL(const LeftVal& left,
                       const RightVal& defaultValue = RightVal()) const {

        return left_to_right.value(left, defaultValue);
    }

    const LeftVal atR(const RightVal& right,
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
    EventSignalAdapter(QWidget* widget);

    QWidget* getWidget();

    virtual ~EventSignalAdapter() = default;

signals:
    void rClick();
    void enter();
    void leave();
    void focusIn();
    void focusOut();

protected:
    bool eventFilter(QObject* /*watched*/, QEvent* event) override;

private:
    QWidget* widget;
    bool right_press_no_leave = false;
};


///*
// * Used when you need to handle events and forward them to
// * other objects. Most useful for mouse propagation.
//*/
//class MouseForwarder : public QObject
//{
//    Q_OBJECT
//public:
//    MouseForwarder(QWidget* to_forward, QWidget* widget);
//    MouseForwarder(const QSet<QEvent::Type>& events, QWidget* to_forward, QWidget* widget);

//protected:
//    bool eventFilter(QObject* /*watched*/, QEvent* event) override;

//private:
//    QWidget* to_forward = nullptr;
//    QSet<QEvent::Type> events;
//};

#endif // COMMON_H
