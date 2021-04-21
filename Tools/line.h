#ifndef LINE_H
#define LINE_H

#include <QGraphicsLineItem>
#include <QLineF>
#include <QJsonDocument>
#include "serializers.h"
#include "tool.h"


class LineItem : public QGraphicsLineItem, public Serializable
{
    using QGraphicsLineItem::QGraphicsLineItem;

public:
#ifdef JSON_SERIALIZER
    virtual bool deserialize(const QJsonObject& json) override;
    virtual bool serialize(QJsonObject& json) const override;
#else
    static_assert(false, "No serializer defined.");
#endif
};


class Line : public Tool
{
    Q_OBJECT
    using Tool::Tool;

public:
    Line() = delete;

protected:
    void toolDown(const QVector2D& pos) override;
    void toolDragged(const QVector2D& pos) override;
    void toolUp(const QVector2D& pos) override;

private:
    QLineF cur_line;
};

#endif // LINE_H
