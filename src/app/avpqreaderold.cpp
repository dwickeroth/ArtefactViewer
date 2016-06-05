#include "avpqreader.h"
#include "avglwidget.h"
#include "PQMTClient.h"
#include "avtrackball.h"
#include "avmodel.h"

#include <QTouchEvent>

#include <QPoint>
#include <QPointF>
#include <QVector3D>
#include <iostream>
#include <set>
#include <map>
#include <cassert>
#include <functional>
//using namespace PQ_SDK_MultiTouch;
using namespace std;

AVPQReader* AVPQReader::m_instance = 0;

AVPQReader::AVPQReader()
{
    memset(m_pf_on_tges,0, sizeof(m_pf_on_tges));
    cout<<"AVPQReader on"<<endl;
}

AVPQReader::~AVPQReader()
{
    PQ_SDK_MultiTouch::DisconnectServer();
    cout<<"AVPQReader off"<<endl;
}
/////////////////////////// functions ///////////////////////////////////
int AVPQReader::Init()
{
    int err_code = PQMTE_SUCCESS;

    // initialize the handle functions of gestures;
    InitFuncOnTG();
    // set the functions on server callback
    SetFuncsOnReceiveProc();
    // connect server
    cout << " connect to server..." << endl;
    if((err_code = ConnectServer()) != PQMTE_SUCCESS){
        cout << " connect server fail, socket error code:" << err_code << endl;
        return err_code;
    }
    // send request to server
    cout << " connect success, send request." << endl;
    TouchClientRequest tcq = {0};
    tcq.type = RQST_RAWDATA_ALL | RQST_GESTURE_ALL;
    if((err_code = SendRequest(tcq)) != PQMTE_SUCCESS){
        cout << " send request fail, error code:" << err_code << endl;
        return err_code;
    }
    ////////////you can set the move_threshold when the tcq.type is RQST_RAWDATA_INSIDE;
    ////send threshold
    int move_threshold = 0;// 0 pixel, receive all the touch points that are touching in the windows area of this client;
    if((err_code = SendThreshold(move_threshold)) != PQMTE_SUCCESS){
        cout << " send threadhold fail, error code:" << err_code << endl;
        return err_code;
    }

    //////// you can set the resolution of the touch point(raw data) here;
    //// setrawdata_resolution
    //int maxX = 32768, maxY = 32768;
    //if((err_code= SetRawDataResolution(maxX, maxY)) != PQMTE_SUCCESS){
    //	cout << " set raw data resolution fail, error code:" << err_code << endl;
    //}
    ////////////////////////
    //get server resolution
    if((err_code = GetServerResolution(OnGetServerResolution, NULL)) != PQMTE_SUCCESS){
        cout << " get server resolution fail,error code:" << err_code << endl;
        return err_code;
    }
    //
    // start receiving
    cout << " send request success, start recv." << endl;
    return err_code;
}

void AVPQReader:: InitFuncOnTG()
{
    // initialize the call back functions of toucha gestures;
    m_pf_on_tges[TG_TOUCH_START] = &AVPQReader::OnTG_TouchStart;
    m_pf_on_tges[TG_DOWN] = &AVPQReader::OnTG_Down;
    m_pf_on_tges[TG_MOVE] = &AVPQReader::OnTG_Move;
    m_pf_on_tges[TG_UP] = &AVPQReader::OnTG_Up;

    m_pf_on_tges[TG_SECOND_DOWN] = &AVPQReader::OnTG_SecondDown;
    m_pf_on_tges[TG_SECOND_UP] = &AVPQReader::OnTG_SecondUp;

    m_pf_on_tges[TG_SPLIT_START] = &AVPQReader::OnTG_SplitStart;
    m_pf_on_tges[TG_SPLIT_APART] = &AVPQReader::OnTG_SplitApart;
    m_pf_on_tges[TG_SPLIT_CLOSE] = &AVPQReader::OnTG_SplitClose;
    m_pf_on_tges[TG_SPLIT_END] = &AVPQReader::OnTG_SplitEnd;

    m_pf_on_tges[TG_TOUCH_END] = &AVPQReader::OnTG_TouchEnd;
}
void AVPQReader::SetFuncsOnReceiveProc()
{
    cout<<"function settings are being called"<<endl;
    PFuncOnReceivePointFrame old_rf_func = SetOnReceivePointFrame(&AVPQReader::OnReceivePointFrame,this);
    PFuncOnReceiveGesture old_rg_func = SetOnReceiveGesture(&AVPQReader::OnReceiveGesture,this);
    PFuncOnServerBreak old_svr_break = SetOnServerBreak(&AVPQReader::OnServerBreak,NULL);
    PFuncOnReceiveError old_rcv_err_func = SetOnReceiveError(&AVPQReader::OnReceiveError,NULL);
    PFuncOnGetDeviceInfo old_gdi_func = SetOnGetDeviceInfo(&AVPQReader::OnGetDeviceInfo,NULL);
}


void AVPQReader::setGLWidget(AVGLWidget *glWidget)
{
    m_glWidget=glWidget;
//    m_glWidget->initialize();
}



void AVPQReader:: OnReceivePointFrame(int frame_id, int time_stamp, int moving_point_count, const TouchPoint * moving_point_array, void * call_back_object)
{
    AVPQReader * sample = static_cast<AVPQReader*>(call_back_object);
    assert(sample != NULL);
    const char * tp_event[] =
    {
        "down",
        "move",
        "up",
    };

//    cout << " frame_id:" << frame_id << " time:"  << time_stamp << " ms" << " moving point count:" << moving_point_count << endl;
    for(int i = 0; i < moving_point_count; ++ i){
        TouchPoint tp = moving_point_array[i];
        sample->OnTouchPoint(tp);
    }
    //throw exception("test exception here");
}

void AVPQReader:: OnReceiveGesture(const TouchGesture & ges, void * call_back_object)
{
    AVPQReader * sample = static_cast<AVPQReader*>(call_back_object);
    assert(sample != NULL);
    sample->OnTouchGesture(ges);
    //throw exception("test exception here");
}
void AVPQReader:: OnServerBreak(void * param, void * call_back_object)
{
    // when the server break, disconenct server;
    cout << "server break, disconnect here" << endl;
    DisconnectServer();
}
void AVPQReader::OnReceiveError(int err_code, void * call_back_object)
{
    switch(err_code)
    {
    case PQMTE_RCV_INVALIDATE_DATA:
        cout << " error: receive invalidate data." << endl;
        break;
    case PQMTE_SERVER_VERSION_OLD:
        cout << " error: the multi-touch server is old for this client, please update the multi-touch server." << endl;
        break;
    case PQMTE_EXCEPTION_FROM_CALLBACKFUNCTION:
        cout << "**** some exceptions thrown from the call back functions." << endl;
        assert(0); //need to add try/catch in the callback functions to fix the bug;
        break;
    default:
        cout << " socket error, socket error code:" << err_code << endl;
    }
}
void AVPQReader:: OnGetServerResolution(int x, int y, void * call_back_object)
{
    cout << " server resolution:" << x << "," << y << endl;
}
void AVPQReader::OnGetDeviceInfo(const TouchDeviceInfo & deviceinfo,void *call_back_object)
{
    cout << " touch screen, SerialNumber: " << deviceinfo.serial_number <<",(" << deviceinfo.screen_width << "," << deviceinfo.screen_height << ")."<<  endl;
}
// here, just record the position of point,
//	you can do mouse map like "OnTG_Down" etc;
void AVPQReader:: OnTouchPoint(const TouchPoint & tp)
{
//    punto=new QPoint(tp.x,tp.y);
//      punto=new QPointF(tp.x,tp.y);
//      AVTrackBall m_trackball=m_glWidget->m_trackball;
//    punto.x=tp.x;
//    punto.y=tp.y;
//    punto.dx=tp.dx;
//    punto.dy=tp.dy;

    switch(tp.point_event)
    {
    case TP_DOWN:
        cout << "  point " << tp.id << " come at (" << tp.x << "," << tp.y
             << ") width:" << tp.dx << " height:" << tp.dy << endl;
//        tap=new QTouchEvent(194,0,Qt::NoModifier,0,tp);
//        if(!m_glWidget->isShiftDown()) m_glWidget->m_trackball->push(pixelPosToViewPos(punto), QQuaternion());
//            m_glWidget->setLastMousePosition(punto->toPoint());
//            std::cout << "That was a touch down, punto x: "<<punto->x()<<"touchpoint x: "<<tp.x<< std::endl;

        break;
    case TP_MOVE:
//        if(!m_glWidget->isShiftDown()) m_glWidget->m_trackball->push(pixelPosToViewPos(punto), QQuaternion());
//        m_glWidget->setLastMousePosition(punto->toPoint());
//        if(!m_glWidget->isShiftDown()){
//            QQuaternion rotation = m_glWidget->m_trackball->move(pixelPosToViewPos(punto), m_glWidget->getMatrixArtefact());

//            m_glWidget->getMatrixArtefact().translate(m_glWidget->m_model->m_centerPoint);
//            m_glWidget->getMatrixArtefact().translate(m_glWidget->getCamOrigin());
//            m_glWidget->getMatrixArtefact().rotate(rotation);
//            m_glWidget->getMatrixArtefact().translate(-m_glWidget->getCamOrigin());
//            m_glWidget->getMatrixArtefact().translate(-m_glWidget->m_model->m_centerPoint);
//            std::cout << "That was a touch move, punto x: "<<punto->x()<<"touchpoint x: "<<tp.x<< std::endl;

//        }

//        cout << "  point " << tp.id << " move at (" << tp.x << "," << tp.y
//             << ") width:" << tp.dx << " height:" << tp.dy << endl;

//                  cout<<"I have a Widget! whose Cam Dist2Origin is: "<<m_glWidget->getCamDistanceToOrigin()<<endl;
//                QVector3D intersectionPoint;
//                m_glWidget->getIntersectionPoint(QPoint(tp.x, tp.y), &intersectionPoint,m_glWidget->m_vMatrix);
//                if (!intersectionPoint.isNull())
//                {
//                    m_glWidget->m_model->m_listOfPointClouds.last().points[m_draggedPoint] = (m_glWidget->m_MatrixArtefact.inverted() * intersectionPoint);
//                    m_glWidget->updateGL();
//                }


        break;
    case TP_UP:
        cout << "  point " << tp.id << " leave at (" << tp.x << "," << tp.y
             << ") width:" << tp.dx << " height:" << tp.dy << endl;
        break;
    }
}
void AVPQReader:: OnTouchGesture(const TouchGesture & tg)
{
    if(TG_NO_ACTION == tg.type)
        return ;

    assert(tg.type <= TG_TOUCH_END);
    DefaultOnTG(tg,this);
    PFuncOnTouchGesture pf = m_pf_on_tges[tg.type];
    if(NULL != pf){
        pf(tg,this);
    }
}
void AVPQReader:: OnTG_TouchStart(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_TOUCH_START);
//    cout << "  here, the touch start, initialize something." << endl;
}
void AVPQReader:: DefaultOnTG(const TouchGesture & tg,void * call_object) // just show the gesture
{
//    cout <<"ges,name:"<< GetGestureName(tg) << " type:" << tg.type << ",param size:" << tg.param_size << " ";
//    for(int i = 0; i < tg.param_size; ++ i)
//        cout << tg.params[i] << " ";
//    cout << endl;
}
void AVPQReader:: OnTG_Down(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_DOWN && tg.param_size >= 2);
//    cout << "  the single finger touching at :( "
//         << tg.params[0] << "," << tg.params[1] << " )" << endl;
}
void AVPQReader:: OnTG_Move(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_MOVE && tg.param_size >= 2);
//    cout << "  the single finger moving on the screen at :( "
//         << tg.params[0] << "," << tg.params[1] << " )" << endl;
}
void AVPQReader:: OnTG_Up(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_UP && tg.param_size >= 2);
    cout << " the single finger is leaving the screen at :( "
         << tg.params[0] << "," << tg.params[1] << " )" << endl;
}
//
void AVPQReader:: OnTG_SecondDown(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_SECOND_DOWN && tg.param_size >= 4);
//    cout << "  the second finger touching at :( "
//         << tg.params[0] << "," << tg.params[1] << " ),"
//         << " after the first finger touched at :( "
//         << tg.params[2] << "," << tg.params[3] << " )" << endl;
}
void AVPQReader:: OnTG_SecondUp(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_SECOND_UP && tg.param_size >= 4);
    cout << "  the second finger is leaving at :( "
         << tg.params[0] << "," << tg.params[1] << " ),"
         << " while the first finger still anchored around :( "
         << tg.params[2] << "," << tg.params[3] << " )" << endl;
}
//
void AVPQReader:: OnTG_SplitStart(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_SPLIT_START && tg.param_size >= 4);
    cout << "  the two fingers is splitting with one finger at: ( "
         << tg.params[0] << "," << tg.params[1] << " ),"
         << " , the other at :( "
         << tg.params[2] << "," << tg.params[3] << " )" << endl;
}

void AVPQReader:: OnTG_SplitApart(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_SPLIT_APART && tg.param_size >= 1);
    cout << "  the two fingers is splitting apart with there distance incresed by "
         << tg.params[0]
         << " with a ratio :" << tg.params[1]
         << endl;
}
void AVPQReader:: OnTG_SplitClose(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_SPLIT_CLOSE && tg.param_size >= 1);
    cout << "  the two fingers is splitting close with there distance decresed by "
         << tg.params[0]
         << " with a ratio :" << tg.params[1]
         << endl;
}
void AVPQReader:: OnTG_SplitEnd(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_SPLIT_END);
    cout << "  the two splitting fingers with one finger at: ( "
         << tg.params[0] << "," << tg.params[1] << " ),"
         << " , the other at :( "
         << tg.params[2] << "," << tg.params[3] << " )"
         << " will end" << endl;
}

//AVWidget Routine to convert a point in widget coordinates to openGL coordinates and return a QPointF

QPointF AVPQReader::pixelPosToViewPos(QPointF *p)
{
    return QPointF(2.0 * float(p->x()) / m_glWidget->width() - 1.0,
                   1.0 - 2.0 * float(p->y()) / m_glWidget->height());
}

// OnTG_TouchEnd: to clear what need to clear
void AVPQReader:: OnTG_TouchEnd(const TouchGesture & tg,void * call_object)
{
    assert(tg.type == TG_TOUCH_END);
    cout << "  all the fingers are leaving and there are no fingers on the screen." << endl;
}
/////////////////////////// functions ///////////////////////////////////
