#include "AVKinector.h"
#include "Kinect.h"
#include <iostream>
#include "stdafx.h"
#include "avhand.h"
using namespace std;

AVKinector* AVKinector::m_instance = 0;

AVKinector::AVKinector(QObject *parent)
{
    m_activated=true;
    InitializeDefaultSensor();
    cout<<"AVKinector on"<<endl;
}
AVKinector::~AVKinector()
{

    cout<<"AVKinector off"<<endl;
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
            hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
            cout<<"got coordinate mapper"<<endl;

        }

        if (SUCCEEDED(hr))
        {
            hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
            cout<<"got Frame source"<<endl;
            if(SUCCEEDED(pBodyFrameSource->get_IsActive(&active)))
                cout<<"is active"<<endl;
            if(SUCCEEDED(pBodyFrameSource->get_BodyCount(&count)))
                    cout<<"body count is "<<(int)count<<endl;
        }

        if (SUCCEEDED(hr))
        {
            hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
            cout<<"reader is open"<<endl;
            if(SUCCEEDED(m_pBodyFrameReader->put_IsPaused(false)))
            cout<<"is not paused"<<endl;
        }

    }

    if (!m_pKinectSensor || FAILED(hr))
    {
        cout<<"No ready Kinect found!"<<endl;
        return E_FAIL;
    }
    if (m_activated)
    {
        Update();
        threader();
        cout<<"updating from while"<<endl;
    }
    return hr;


}

void AVKinector::threader(){
    while (m_pBodyFrameReader)
    {
    AVKinector::Update();
    }

}

void AVKinector::Update()
{
    if (!m_pBodyFrameReader)
    {
    cout<<"no body frame reader"<<endl;
    }

    IBodyFrame* pBodyFrame = NULL;
    IBody* bodies;
    HandState status;

    HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

    if (SUCCEEDED(hr))
    {
        INT64 nTime = 0;

        hr = pBodyFrame->get_RelativeTime(&nTime);

        IBody* ppBodies[BODY_COUNT] = {0};
//        cout<<"body created"<<endl;
        if (SUCCEEDED(hr))
        {
            hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
//            cout<<"got and refreshed body data"<<endl;
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
//                    cout<<"body is there"<<endl;
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
                            AVHand izq;
                            izq.isLeft=true;
                            izq.hState=leftHandState;
                            izq.x=joints[JointType_HandLeft].Position.X;
                            izq.y=joints[JointType_HandLeft].Position.Y;
                            izq.z=joints[JointType_HandLeft].Position.Z;
                            emit throwKP(izq);

                            AVHand der;
                            der.isLeft=false;
                            der.hState=rightHandState;
                            der.x=joints[JointType_HandRight].Position.X;
                            der.y=joints[JointType_HandRight].Position.Y;
                            der.z=joints[JointType_HandRight].Position.Z;
                            emit throwKP(der);
                           if(leftHandState==2)
                               cout<<"left hand is open at"
                                   <<joints[JointType_HandLeft].Position.X
                                   <<joints[JointType_HandLeft].Position.Y
                                   <<joints[JointType_HandLeft].Position.Z
                                    <<endl;
                           if(leftHandState==3)
                               cout<<"left hand is closed at"
                                   <<joints[JointType_HandLeft].Position.X
                                   <<joints[JointType_HandLeft].Position.Y
                                   <<joints[JointType_HandLeft].Position.Z
                                    <<endl;

                            if(rightHandState==2)
                               cout<<"right hand is open at"
                                   <<joints[JointType_HandRight].Position.X
                                   <<joints[JointType_HandRight].Position.Y
                                   <<joints[JointType_HandRight].Position.Z
                                    <<endl;

                            if(rightHandState==3)
                               cout<<"right hand is closed at"
                                   <<joints[JointType_HandRight].Position.X
                                   <<joints[JointType_HandRight].Position.Y
                                   <<joints[JointType_HandRight].Position.Z
                                    <<endl;
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

}
