#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QColor>
#include <QVector2D>

#include "canvas.h"

class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(QObject* parent = nullptr);

    void setActive(Canvas* canvas, size_t size = 3, QColor color = QColor());

    void setInactive();

signals:

private:
    virtual void toolActivated() = 0;
    virtual void toolDown(const QVector2D& pos) = 0;
    virtual void toolMoved(const QVector2D& pos) = 0;
    virtual void toolUp(const QVector2D& pos) = 0;
    virtual void toolInactivated() = 0;

    Canvas* canvas = nullptr;
    size_t size = 3;
    QColor color;
};

#endif // TOOL_H
