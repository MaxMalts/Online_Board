#ifndef LINE_H
#define LINE_H

#include "tool.h"

class Line : public Tool
{
    Q_OBJECT
    using Tool::Tool;

public:
    Line() = delete;

private:
    void toolDown(const QVector2D& pos) override;
    void toolDragged(const QVector2D& pos) override;
    void toolUp(const QVector2D& pos) override;
};

#endif // LINE_H
