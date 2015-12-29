#include "avpqreader.h"
#include "avglwidget.h"
#include "PQMTClient.h"
#include "avpointframe.h"
#include <iostream>
#include <set>
#include <map>
#include <cassert>
#include <functional>
using namespace PQ_SDK_MultiTouch;
using namespace std;

int moveThreshold;
const int moveResolution=15;
int counter =0;
AVPQReader* AVPQReader::m_instance = 0;

AVPQReader::AVPQReader(QObject *parent)
{
    memset(m_pf_on_tges,0, sizeof(m_pf_on_tges));
    cout<<"AVPQReader on"<<endl;
}

AVPQReader::~AVPQReader()
{
    DisconnectServer();
    cout<<"AVPQReader off"<<endl;
}


/////////////////////////// functions ///////////////////////////////////
int AVPQReader::Init()
{
    QThread::currentThread()->setPriority(QThread::LowestPriority);
    int err_code = PQMTE_SUCCESS;
    // initialize the handle functions of gestures;
    //    InitFuncOnTG();
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
    tcq.type = RQST_RAWDATA_ALL /*| RQST_GESTURE_ALL*/;
    if((err_code = SendRequest(tcq)) != PQMTE_SUCCESS){
        cout << " send request fail, error code:" << err_code << endl;
        return err_code;
    }
    ////////////you can set the move_threshold when the tcq.type is RQST_RAWDATA_INSIDE;
    ////send threshold
    int move_threshold = moveThreshold;// 0 pixel, receive all the touch points that are touching in the windows area of this client;
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

void AVPQReader::SetFuncsOnReceiveProc()
{
    cout<<"function settings are being called"<<endl;
    PFuncOnReceivePointFrame old_rf_func = SetOnReceivePointFrame(&AVPQReader::OnReceivePointFrame,this);
    PFuncOnReceiveGesture old_rg_func = SetOnReceiveGesture(&AVPQReader::OnReceiveGesture,this);
    PFuncOnServerBreak old_svr_break = SetOnServerBreak(&AVPQReader::OnServerBreak,NULL);
    PFuncOnReceiveError old_rcv_err_func = SetOnReceiveError(&AVPQReader::OnReceiveError,NULL);
    PFuncOnGetDeviceInfo old_gdi_func = SetOnGetDeviceInfo(&AVPQReader::OnGetDeviceInfo,NULL);
}

void AVPQReader:: OnReceivePointFrame(int frame_id, int time_stamp, int moving_point_count, const TouchPoint * moving_point_array, void * call_back_object)
{

    AVPQReader * sample = static_cast<AVPQReader*>(call_back_object);
    assert(sample != NULL);
    AVPointFrame pf;
    pf.pf_frame_id=frame_id;
    pf.pf_time_stamp=time_stamp;
    pf.pf_moving_point_count=moving_point_count;
    pf.pf_moving_point_array=moving_point_array;
    if(counter%moveResolution==0||pf.pf_moving_point_array[0].point_event==TP_DOWN||pf.pf_moving_point_array[0].point_event==TP_UP){
        sample->OnTouchPoint(pf);
        for(int i = 0; i < moving_point_count; ++ i){
            switch(moving_point_array[i].point_event){
            case TP_DOWN:
                std::cout<< "type "<<(int) moving_point_array[i].point_event<<" Touch"<<std::endl;
                break;
            case TP_MOVE:
                std::cout<< "type "<<(int) moving_point_array[i].point_event<<" Move"<<std::endl;
                break;
            case TP_UP:
                std::cout<< "type "<<(int) moving_point_array[i].point_event<<" End Touch"<<std::endl;
                break;
            }
            //        std::cout<<(int) moving_point_array[i].point_event<<std::endl;
        }
    }
    counter++;
}

void AVPQReader:: OnReceiveGesture(const TouchGesture & ges, void * call_back_object)
{
    cout<<"gesture received"<<endl;
    AVPQReader * sample = static_cast<AVPQReader*>(call_back_object);
    assert(sample != NULL);
    //throw exception("test exception here");
}

void AVPQReader:: OnServerBreak(void * param, void * call_back_object)
{
    // when the server breaks, disconenct server;
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
void AVPQReader:: OnTouchPoint(const AVPointFrame & pf
                               )
{
    AVPointFrame pFrame;
    pFrame.pf_frame_id=pf.pf_frame_id;
    pFrame.pf_time_stamp=pf.pf_time_stamp;
    pFrame.pf_moving_point_count=pf.pf_moving_point_count;
    pFrame.pf_moving_point_array=pf.pf_moving_point_array;
    emit throwPF(pFrame);
}
