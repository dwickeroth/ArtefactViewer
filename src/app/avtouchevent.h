#ifndef AVTOUCHEVENT_H
#define AVTOUCHEVENT_H

#include <QObject>
#include "qevent.h"

//class AVTouchEvent : public QTouchEvent
//{
//    Q_GUI_EXPORT
//public:
//    explicit AVTouchEvent(QTouchEvent *parent = 0);

//signals:

//public slots:

//};

class AVTouchEventTouchPointPrivate;
class Q_GUI_EXPORT AVTouchEvent : public QInputEvent
{
public:
    class Q_GUI_EXPORT TouchPoint
    {
    public:
        enum InfoFlag {
            Pen  = 0x0001
        };
        Q_DECLARE_FLAGS(InfoFlags, InfoFlag)

        explicit TouchPoint(int id = -1);
        TouchPoint(const TouchPoint &other);
#ifdef Q_COMPILER_RVALUE_REFS
        TouchPoint(TouchPoint &&other) : d(other.d) { other.d = 0; }
        TouchPoint &operator=(TouchPoint &&other)
        { qSwap(d, other.d); return *this; }
#endif
        ~TouchPoint();

        TouchPoint &operator=(const TouchPoint &other)
        { if ( d != other.d ) { TouchPoint copy(other); swap(copy); } return *this; }

        void swap(TouchPoint &other) { qSwap(d, other.d); }

        int id() const;

        Qt::TouchPointState state() const;

        QPointF pos() const;
        QPointF startPos() const;
        QPointF lastPos() const;

        QPointF scenePos() const;
        QPointF startScenePos() const;
        QPointF lastScenePos() const;

        QPointF screenPos() const;
        QPointF startScreenPos() const;
        QPointF lastScreenPos() const;

        QPointF normalizedPos() const;
        QPointF startNormalizedPos() const;
        QPointF lastNormalizedPos() const;

        QRectF rect() const;
        QRectF sceneRect() const;
        QRectF screenRect() const;

        qreal pressure() const;
        QVector2D velocity() const;
        InfoFlags flags() const;
        QVector<QPointF> rawScreenPositions() const;

        // internal
        void setId(int id);
        void setState(Qt::TouchPointStates state);
        void setPos(const QPointF &pos);
        void setScenePos(const QPointF &scenePos);
        void setScreenPos(const QPointF &screenPos);
        void setNormalizedPos(const QPointF &normalizedPos);
        void setStartPos(const QPointF &startPos);
        void setStartScenePos(const QPointF &startScenePos);
        void setStartScreenPos(const QPointF &startScreenPos);
        void setStartNormalizedPos(const QPointF &startNormalizedPos);
        void setLastPos(const QPointF &lastPos);
        void setLastScenePos(const QPointF &lastScenePos);
        void setLastScreenPos(const QPointF &lastScreenPos);
        void setLastNormalizedPos(const QPointF &lastNormalizedPos);
        void setRect(const QRectF &rect);
        void setSceneRect(const QRectF &sceneRect);
        void setScreenRect(const QRectF &screenRect);
        void setPressure(qreal pressure);
        void setVelocity(const QVector2D &v);
        void setFlags(InfoFlags flags);
        void setRawScreenPositions(const QVector<QPointF> &positions);

    private:
        AVTouchEventTouchPointPrivate *d;
        friend class QGuiApplication;
        friend class QGuiApplicationPrivate;
        friend class QApplication;
        friend class QApplicationPrivate;
    };

#if QT_DEPRECATED_SINCE(5, 0)
    enum DeviceType {
        TouchScreen,
        TouchPad
    };
#endif

    explicit AVTouchEvent(QEvent *parent=0/*,
                         QEvent::Type eventType,
                         QTouchDevice *device = 0,
                         Qt::KeyboardModifiers modifiers = Qt::NoModifier,
                         Qt::TouchPointStates touchPointStates = 0,
                         const QList<QTouchEvent::TouchPoint> &touchPoints = QList<QTouchEvent::TouchPoint>()*/);
    ~AVTouchEvent();

    inline QWindow *window() const { return _window; }
    inline QObject *target() const { return _target; }
#if QT_DEPRECATED_SINCE(5, 0)
    QT_DEPRECATED inline AVTouchEvent::DeviceType deviceType() const { return static_cast<DeviceType>(int(_device->type())); }
#endif
    inline Qt::TouchPointStates touchPointStates() const { return _touchPointStates; }
    inline const QList<AVTouchEvent::TouchPoint> &touchPoints() const { return _touchPoints; }
    inline QTouchDevice *device() const { return _device; }

    // internal
    inline void setWindow(QWindow *awindow) { _window = awindow; }
    inline void setTarget(QObject *atarget) { _target = atarget; }
    inline void setTouchPointStates(Qt::TouchPointStates aTouchPointStates) { _touchPointStates = aTouchPointStates; }
    inline void setTouchPoints(const QList<AVTouchEvent::TouchPoint> &atouchPoints) { _touchPoints = atouchPoints; }
    inline void setDevice(QTouchDevice *adevice) { _device = adevice; }

protected:
    QWindow *_window;
    QObject *_target;
    QTouchDevice *_device;
    Qt::TouchPointStates _touchPointStates;
    QList<AVTouchEvent::TouchPoint> _touchPoints;

    friend class QGuiApplication;
    friend class QGuiApplicationPrivate;
    friend class QApplication;
    friend class QApplicationPrivate;
};
Q_DECLARE_TYPEINFO(AVTouchEvent::TouchPoint, Q_MOVABLE_TYPE);
Q_DECLARE_OPERATORS_FOR_FLAGS(AVTouchEvent::TouchPoint::InfoFlags)


#endif // AVTOUCHEVENT_H
