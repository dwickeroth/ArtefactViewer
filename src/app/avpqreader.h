#ifndef AVPQREADER_H
#define AVPQREADER_H
#include "PQMTClient.h"
#include <QMutex>
#include <QPoint>
#include <QPointF>
#include "avglwidget.h"
#include "avpointframe.h"

using namespace PQ_SDK_MultiTouch;

class AVPQReader : public QObject
{
    Q_OBJECT

public:

    static AVPQReader* instance()
    {
        static QMutex mutex;
        if(!m_instance)
        {
            mutex.lock();
            if(!m_instance) m_instance = new AVPQReader;
            mutex.unlock();
        }
        return m_instance;
    }

    static void destroy()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_instance;
        m_instance = 0;
        mutex.unlock();
        DisconnectServer();
    }

    int Init();
//    void setGLWidget(AVGLWidget* glWidget);
    AVTouchPoint e;
    AVPointFrame pf;
    static const int moveResolution=24;

signals:
//    void throwEvent(AVTouchPoint event);
    void throwPF(AVPointFrame pFrame);
private:
    explicit AVPQReader(QObject* parent = 0);
    // we leave just the declarations, so the compiler will warn us
    // if we try to use those two functions by accident
    AVPQReader(const AVPQReader &); //hide copy constructor
    AVPQReader& operator=(const AVPQReader &); //hide assign op
    ~AVPQReader();
    AVGLWidget*         m_glWidget;
    static AVPQReader* m_instance;

    //////////////////////call back functions///////////////////////
    // OnReceivePointFrame: function to handle when recieve touch point frame
    //	the unmoving touch point won't be sent from server. The new touch point with its pointevent is TP_DOWN
    //	and the leaving touch point with its pointevent will be always sent from server;
    static void OnReceivePointFrame(int frame_id,int time_stamp,int moving_point_count,const TouchPoint * moving_point_array, void * call_back_object);

//    // OnReceiveGesture: function to handle when recieve a gesture
    static void OnReceiveGesture(const TouchGesture & ges, void * call_back_object);

    // OnServerBreak: function to handle when server break(disconnect or network error)
    static void OnServerBreak(void * param, void * call_back_object);

    // OnReceiveError: function to handle when some errors occur on the process of receiving touch datas.
    static void OnReceiveError(int err_code,void * call_back_object);

    // OnGetServerResolution: function to get the resolution of the server system.attention: not the resolution of touch screen.
    static void OnGetServerResolution(int x, int y, void * call_back_object);

    // OnGetDeviceInfo: function to get the information of the touch device.
    static void OnGetDeviceInfo(const TouchDeviceInfo & device_info, void * call_back_object);

//////////////////////call back functions end ///////////////////////

    // functions to handle TouchGestures, attention the means of the params
    void InitFuncOnTG();

    // set the call back functions while reciving touch data;
    void SetFuncsOnReceiveProc();

    // OnTouchPoint: function to handle TouchPoint
    void OnTouchPoint(const AVPointFrame & pf);

    // OnTouchGesture: function to handle TouchGesture
//    void OnTouchGesture(const TouchGesture & tg);
    //

    //here use function pointer table to handle the different gesture types;
    typedef void (*PFuncOnTouchGesture)(const TouchGesture & tg,void * call_object);

    //    static void DefaultOnTG(const TouchGesture & tg,void * call_object); // just show the gesture

    //    static void OnTG_TouchStart(const TouchGesture & tg,void * call_object);
    //    static void OnTG_Down(const TouchGesture & tg,void * call_object);
    //    static void OnTG_Move(const TouchGesture & tg,void * call_object);
    //    static void OnTG_Up(const TouchGesture & tg,void * call_object);

//    //
//    static void OnTG_SecondDown(const TouchGesture & tg,void * call_object);
//    static void OnTG_SecondUp(const TouchGesture & tg,void * call_object);

//    //
//    static void OnTG_SplitStart(const TouchGesture & tg,void * call_object);
//    static void OnTG_SplitApart(const TouchGesture & tg,void * call_object);
//    static void OnTG_SplitClose(const TouchGesture & tg,void * call_object);
//    static void OnTG_SplitEnd(const TouchGesture & tg,void * call_object);

// OnTG_TouchEnd: to clear what need to clear;
//    static void OnTG_TouchEnd(const TouchGesture & tg,void * call_object);

private:
    PFuncOnTouchGesture m_pf_on_tges[TG_TOUCH_END + 1];
};

#endif // AVPQREADER_H
