#include "avtouchevent.h"
#include "qevent.h"
#include "iostream"

AVTouchEvent::AVTouchEvent(QEvent *parent/*,
                            QEvent::Type eventType,
                            QTouchDevice *device,
                            Qt::KeyboardModifiers modifiers,
                            Qt::TouchPointStates touchPointStates,
                            const QList<QTouchEvent::TouchPoint> &touchPoints*/) :
                            QInputEvent(parent->type(), Qt::NoModifier)
{
    std::cout<<"open"<<std::endl;

}

AVTouchEvent::~AVTouchEvent(){
std::cout<<"close"<<std::endl;
}

//AVGLWidget::AVGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
//{
//    m_model = AVModel::instance();
//    m_pMatrix.setToIdentity();
//    m_model->setupLightGeometry();
//    QGLFormat stereoFormat;
//    stereoFormat.setSampleBuffers(true);
//    stereoFormat.setStereo(true);
//    this->setFormat(stereoFormat);
////    accept touch events from Windows native
//    this->setAttribute(Qt::WA_AcceptTouchEvents,true);
////    Hide the cursor
//    this->setCursor(Qt::BlankCursor);
//    initialize();
//    setAutoFillBackground(false);
//    setAutoBufferSwap(false);

//    m_trackball = new AVTrackBall(AVTrackBall::Sphere);

//}

//AVGLWidget::~AVGLWidget()
//{
//    delete m_trackball;
//}

//AVTouchEvent::AVTouchEvent(AVTouchEvent *parent) :
//    QTouchEvent(parent)
//{
////   if( parent->touchPoints().isEmpty())
////       std::cout<<"list empty"<<std::endl;
//}
