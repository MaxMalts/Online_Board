#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QGraphicsScene>
#include <QDebug>

class Canvas : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr, QSize size = QSize(500, 500));

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //void paintEvent(QPaintEvent* event) override;

private:
    void startDrawing(const QPoint& pos);
    void drawLineToPoint(const QPoint& point);
    void stopDrawing();
    void drawLine(const QPoint& first, const QPoint& second);

    QImage background;
    QImage canv_image;
    QPoint prev_point;
    bool drawing = false;

private slots:
    void onDataReceived();
};

#endif // CANVAS_H
