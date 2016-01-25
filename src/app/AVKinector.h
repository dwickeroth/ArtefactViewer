#ifndef KINECTOR_H
#define KINECTOR_H

class Kinector
{
public:
    Kinector();
};

//#include "Kinect.h"
//#include <QMutex>
//#include <QPoint>
//#include <QPointF>
//#include "avglwidget.h"
//#include "avpointframe.h"

//class Kinector : public QObject
//{
//    Q_OBJECT

//public:

//    static Kinector* instance()
//    {
//        static QMutex mutex;
//        if(!m_instance)
//        {
//            mutex.lock();
//            if(!m_instance) m_instance = new Kinector;
//            mutex.unlock();
//        }
//        return m_instance;
//    }

//    static void destroy()
//    {
//        static QMutex mutex;
//        mutex.lock();
//        delete m_instance;
//        m_instance = 0;
//        mutex.unlock();
//        DisconnectServer();
//    }

//    int Init();

//signals:
//    void throwKP (AVPointFrame pFrame);

//private:
//    explicit Kinector(QObject* parent = 0);
//    // we leave just the declarations, so the compiler will warn us
//    // if we try to use those two functions by accident
//    Kinector(const Kinector &); //hide copy constructor
//    Kinector& operator=(const Kinector &); //hide assign op
//    ~Kinector();
//    AVGLWidget*         m_glWidget;
//    static Kinector* m_instance;
//};




#endif // KINECTOR_H
