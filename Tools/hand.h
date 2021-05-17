#ifndef HAND_H
#define HAND_H

#include <QPointF>

#include "tool.h"


class Hand : public Tool
{
    Q_OBJECT

public:
    Hand(Canvas* canvas, QLayout* props_container, QObject* parent = nullptr);

protected:
    void toolActivated() override;
    void toolInactivated() override;
};

#endif // HAND_H
