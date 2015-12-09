#ifndef AVTOUCHEVENT_H
#define AVTOUCHEVENT_H

#include <QObject>
#include <QMutex>
#include "qevent.h"
#include "iostream"
using namespace std;

class AVTouchEvent : public QEvent {
    public:
        static const QEvent::Type AVTouchEventType=static_cast<QEvent::Type>(13000);
        AVTouchEvent():QEvent(AVTouchEventType){
        }
        QString	m_action;
        QStringList	m_args;
        bool isTouch (QEvent::Type tipo) const
        {
            if(tipo==AVTouchEventType){
                cout<<"touch type"<<endl;
            }
            return true;
        }
        unsigned short	point_event;//Indicates current action or event of the touch point.
        unsigned short	id;         //use id to distinguish different points on the screen.
        int				x;			//the x-coordinate of the center position of the point.In pixels.
        int				y;			//the y-coordinate of the center position of the point.In pixels.
        unsigned short	dx;		    //the x-width of the touch point.In pixels.
        unsigned short	dy;			//the y-width of the touch point.In pixels.

//SignalSlotApproach 2

//    signals:
//        void throwEvent(AVTouchEvent event);
};

#endif // AVTOUCHEVENT_H


