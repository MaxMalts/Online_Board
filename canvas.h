#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr, QSize size = QSize(500, 500));


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void drawLine(const QPoint& first, const QPoint& second);

    QImage canv_image;
    QPoint prev_point;
    bool drawing = false;

};

#endif // CANVAS_H