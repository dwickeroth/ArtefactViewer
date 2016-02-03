#include "AVKinector.h"
#include "Kinect.h"
#include <iostream>
#include "stdafx.h"
#include "avhand.h"
using namespace std;

//AVKinector* AVKinector::m_instance = 0;
int kinerror=0;
int counters=0;
AVKinector::AVKinector(QObject *parent)
{
    m_activated=true;
//    InitializeDefaultSensor();
    cout<<"AVKinector on, setting priority to lowest"<<endl;
//    QThread::currentThread()->setPriority(QThread::LowestPriority);
//    run();
//    cout<<"requested first run from initialization"<<endl;
}
AVKinector::~AVKinector()
{
    cout<<"AVKinector off"<<endl;
}

int AVKinector::Init(){
    InitializeDefaultSensor();
    moveToThread(this);
    return kinerror;
}

HRESULT AVKinector::InitializeDefaultSensor()
{
    HRESULT hr;
    INT32 count;
    BOOLEAN active;

    hr = GetDefaultKinectSensor(&m_pKinectSensor);
    if (FAILED(hr))
    {
        return hr;
    }

    if (m_pKinectSensor)
    {
        // Initialize the Kinect and get coordinate mapper and the body reader
        IBodyFrameSource* pBodyFrameSource = NULL;

        hr = m_pKinectSensor->Open();

        if (SUCCEEDED(hr))
        {
            cout<<"kinect sensor is open"<<endl;
            hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
        }

        if (SUCCEEDED(hr))
        {
            cout<<"got coordinate mapper"<<endl;

            hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
            if(SUCCEEDED(pBodyFrameSource->get_IsActive(&active)))
                cout<<"Frame source is active"<<endl;
            if(SUCCEEDED(pBodyFrameSource->get_BodyCount(&count)))
                cout<<"body count is "<<(int)count<<endl;
        }

        if (SUCCEEDED(hr))
        {
            hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
            if(SUCCEEDED(m_pBodyFrameReader->put_IsPaused(false)))
            cout<<"Body Frame reader is open and unpaused"<<endl;
        }

    }

    if (!m_pKinectSensor || FAILED(hr))
    {
        cout<<"No ready Kinect found!"<<endl;
        return E_FAIL;
    }
    return hr;
}

void AVKinector::run(){

    while (m_pBodyFrameReader)
    {
    AVKinector::Update();

//    cout<<"update over, call exec"<<exec()<<endl;
//    return;
    }
    exec();
}

void AVKinector::Update()
{
//    cout<<"kinector running"<<endl;
    if (!m_pBodyFrameReader)
    {
    cout<<"no body frame reader"<<endl;
    return;
    }

    IBodyFrame* pBodyFrame = NULL;

    HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
//    cout<<"body frame requested"<<endl;
    if (SUCCEEDED(hr))
    {
//        cout<<"body frame acquired"<<endl;
        INT64 nTime = 0;

        hr = pBodyFrame->get_RelativeTime(&nTime);

        IBody* ppBodies[BODY_COUNT] = {0};
        if (SUCCEEDED(hr))
        {
//            cout<<"body created, get and refreshed body data"<<endl;
            hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
        }

        if (SUCCEEDED(hr))
        {
//            cout<<"please process body"<<endl;
            AVKinector::ProcessBody(nTime, BODY_COUNT, ppBodies);
        }

        for (int i = 0; i < _countof(ppBodies); ++i)
        {
            SafeRelease(ppBodies[i]);
        }
    }
//else{cout<<"body frame NOT acquired"<<endl;}
    SafeRelease(pBodyFrame);

}



void AVKinector::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
//    cout<<"body processing"<<endl;


            for (int i = 0; i < nBodyCount; ++i)
            {
                IBody* pBody = ppBodies[i];
                if (pBody)
                {
//                        cout<<"we have a body"<<endl;

                    BOOLEAN bTracked = false;
                    hr = pBody->get_IsTracked(&bTracked);

                    if (SUCCEEDED(hr) && bTracked)
                    {
//                        cout<<"body is tracked"<<endl;

                        Joint joints[JointType_Count];
//                        D2D1_POINT_2F jointPoints[JointType_Count];
                        HandState leftHandState = HandState_Unknown;
                        HandState rightHandState = HandState_Unknown;

                        pBody->get_HandLeftState(&leftHandState);
//                        cout<<leftHandState<<endl;
                        pBody->get_HandRightState(&rightHandState);

                        hr = pBody->GetJoints(_countof(joints), joints);
                        if(SUCCEEDED(hr)){
                            counters++;
                            AVHand izq;
                            izq.isLeft=true;
                            izq.hState=leftHandState;
                            izq.x=joints[JointType_HandLeft].Position.X;
                            izq.y=joints[JointType_HandLeft].Position.Y;
                            izq.z=joints[JointType_HandLeft].Position.Z;
                            if(counters%3==0)
                            emit throwKP(izq);

                            AVHand der;
                            der.isLeft=false;
                            der.hState=rightHandState;
                            der.x=joints[JointType_HandRight].Position.X;
                            der.y=joints[JointType_HandRight].Position.Y;
                            der.z=joints[JointType_HandRight].Position.Z;
                            if(counters%3==0)
                            emit throwKP(der);

//                           if(leftHandState==2)
//                               cout<<"left hand is open at"
//                                   <<joints[JointType_HandLeft].Position.X
//                                   <<joints[JointType_HandLeft].Position.Y
//                                   <<joints[JointType_HandLeft].Position.Z
//                                    <<endl;
//                           if(leftHandState==3)
//                               cout<<"left hand is closed at"
//                                   <<joints[JointType_HandLeft].Position.X
//                                   <<joints[JointType_HandLeft].Position.Y
//                                   <<joints[JointType_HandLeft].Position.Z
//                                    <<endl;

//                            if(rightHandState==2)
//                               cout<<"right hand is open at"
//                                   <<joints[JointType_HandRight].Position.X
//                                   <<joints[JointType_HandRight].Position.Y
//                                   <<joints[JointType_HandRight].Position.Z
//                                    <<endl;

//                            if(rightHandState==3)
//                               cout<<"right hand is closed at"
//                                   <<joints[JointType_HandRight].Position.X
//                                   <<joints[JointType_HandRight].Position.Y
//                                   <<joints[JointType_HandRight].Position.Z
//                                    <<endl;
                        }
//                        if (SUCCEEDED(hr))
//                        {
//                            for (int j = 0; j < _countof(joints); ++j)
//                            {
//                                jointPoints[j] = BodyToScreen(joints[j].Position, width, height);
//                            }

//                            DrawBody(joints, jointPoints);

//                            DrawHand(leftHandState, jointPoints[JointType_HandLeft]);
//                            DrawHand(rightHandState, jointPoints[JointType_HandRight]);
//                        }
                    }
                }
            }


//            hr = m_pRenderTarget->EndDraw();

            // Device lost, need to recreate the render target
            // We'll dispose it now and retry drawing
            if (D2DERR_RECREATE_TARGET == hr)
            {
                hr = S_OK;
//                DiscardDirect2DResources();
            }


        if (!m_nStartTime)
        {
            m_nStartTime = nTime;
        }

        double fps = 0.0;

        LARGE_INTEGER qpcNow = {0};
        if (m_fFreq)
        {
            if (QueryPerformanceCounter(&qpcNow))
            {
                if (m_nLastCounter)
                {
                    m_nFramesSinceUpdate++;
                    fps = m_fFreq * m_nFramesSinceUpdate / double(qpcNow.QuadPart - m_nLastCounter);
                }
            }
        }

        WCHAR szStatusMessage[64];
//        StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_nStartTime));

//        if (SetStatusMessage(szStatusMessage, 1000, false))
//        {
            m_nLastCounter = qpcNow.QuadPart;
            m_nFramesSinceUpdate = 0;
//        }
            return;

}
