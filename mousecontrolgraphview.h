#ifndef MOUSECONTROLGRAPHVIEW_H
#define MOUSECONTROLGRAPHVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QScrollBar>

class MouseControlGraphView : public QGraphicsView
{
public:
    MouseControlGraphView(QGraphicsScene *scene) : QGraphicsView(scene) {}

protected:

    virtual void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            // 记录鼠标按下位置
            _lastPanPoint = event->pos();
            setCursor(Qt::ClosedHandCursor);
        }
        QGraphicsView::mousePressEvent(event);
    }

    virtual void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            _lastPanPoint = QPoint();
            setCursor(Qt::ArrowCursor);
        }
        QGraphicsView::mouseReleaseEvent(event);
    }

    virtual void mouseMoveEvent(QMouseEvent *event) override {
        if (!_lastPanPoint.isNull()) {
            // 计算移动距离
            QPointF delta = mapToScene(event->pos()) - mapToScene(_lastPanPoint);
            translate(delta.x(), delta.y());
            _lastPanPoint = event->pos();
        }
        QGraphicsView::mouseMoveEvent(event);
    }


    virtual void wheelEvent(QWheelEvent *event) override {
        const double scaleFactor = 1.15; // 缩放因子
        if (event->angleDelta().y() > 0 ){//&& this->size()) {
            // 向上滚动，放大
            scale(scaleFactor, scaleFactor);
        } else {
            // 向下滚动，缩小
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }

private:
    QPoint _lastPanPoint;
};

#endif // CUSTOMGRAPHICSVIEW_H
