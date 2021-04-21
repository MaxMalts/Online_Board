#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QTimer>
#include <QEventLoop>
#include <QMap>
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

#endif // COMMON_H
