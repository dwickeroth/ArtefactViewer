/*Copyright (c) 2014, Dominic Michael Laurentius


All rights reserved.


Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMatrix4x4>
#include <QGLShaderProgram>
#include <QGLBuffer>
#include <QGLFunctions>

#include "avhand.h"
#include "AVKinector.h"
#include "avlight.h"
#include "avtrackball.h"
#include "avtouchpoint.h"
#include "avpointframe.h"

class AVModel;
class AVTrackBall;
class AVKinector;


class AVGLWidget: public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

public:
    AVGLWidget(QWidget *parent = 0);
    ~AVGLWidget();
    AVTrackBall*  m_trackball;
    AVModel*    m_model;
    AVKinector* m_kinect;

    void    initialize();

    QVector<AVLight> m_lights;

    void fillBuffers();

    QMatrix4x4 getMvpMatrix();
    QMatrix4x4 getLeftMvpMatrix();
    QMatrix4x4 getRightMvpMatrix();

    int getSelectedPoint() const;
    void setSelectedPoint(int selectedPoint);

    int getDraggedPoint() const;
    void setDraggedPoint(int draggedPoint);

    int getCurrentAnnotation() const;
    void setCurrentAnnotation(int currentAnnotation);

    int getEnhancement() const;
    void setEnhancement(int enhancement);

    int getEnhancementParam(int i, int j) const;
    void setEnhancementParam(int i, int j, int value);

    bool isShiftDown() const;
    void setShiftDown(bool shiftDown);

    bool getLightsAreVisible() const;
    void setLightsAreVisible(bool lightsAreVisible);

    bool getUseLighting() const;
    void setLighting(bool lighting);

    bool getUseVertexColors() const;
    void setVertexColors(bool vertexColors);

    bool getPaintAnnotations() const;
    void setPaintAnnotations(bool paintAnnotations);

    double getCamDistanceToOrigin() const;
    void setCamDistanceToOrigin(double getCamDistanceToOrigin);

    QPoint getLastMousePosition() const;
    void setLastMousePosition(QPoint lastMousePosition);

    QVector3D getCamOrigin() const;
    void setCamOrigin(const QVector3D &camOrigin);

    QVector3D getBackgroundColor1() const;
    void setBackgroundColor1(const QVector3D &backgroundColor1);

    QVector3D getBackgroundColor2() const;
    void setBackgroundColor2(const QVector3D &backgroundColor2);

    QMatrix4x4 getMatrixArtefact() const;
    void setMatrixArtefact(const QMatrix4x4 &matrixArtefact);

    QImage renderToOffscreenBuffer(int width, int height);


public slots:
    void catchKP(AVHand mano);
    void catchPF(AVPointFrame pFrame);


private:
    void setZoomLevel(double level, bool relative);
    void kinectCount(AVHand mano);
    void resetLeftKinectCounts();
    void resetRightKinectCounts();


signals:
    void annotationClicked();
    void annotationDoubleClicked();
    void pointSelected();
    void glWidgetInitialised();
    void pointAdded();


protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    //to spit out all event types
    //    bool event(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    QMatrix4x4  m_MatrixArtefact;


private:
    void catchEvent(AVTouchPoint &tPoint);

    void drawBackground();
    void setupCamera(boolean sizeIsLeft);
    void drawModelShaded(QMatrix4x4 l_vMatrix);
    void drawModelNoShading(QMatrix4x4 l_vMatrix);
    void drawLights(QMatrix4x4 l_vMatrix);
    void drawLightCircles(QMatrix4x4 l_vMatrix);
    void drawOverlays(QPaintDevice* device, bool offscreen = false, int fboWidth = 0);

private:
    //private members with getters/setters
    int         m_selectedPoint;
    int         m_draggedPoint;
    int         m_currentAnnotation;
    int         m_enhancement;
    int         m_enhance_param[4][3];

    bool        m_shiftDown;
    bool        m_lightsAreVisible;
    bool        m_lighting;
    bool        m_vertexColors;
    bool        m_paintAnnotations;

    bool        m_kinectIsWatching;
    bool        m_kRTr; //!Kinect right Traslation on
    bool        m_kLTr; //!Kinect right Traslation on
    bool        m_kRot; //! Kinect Rotation on


    double      m_camDistanceToOrigin;


    QPoint      m_lastMousePosition;

    QVector3D   m_camOrigin;
    QVector3D   m_camPosition;
    QVector3D   m_LeftCamPosition;
    QVector3D   m_RightCamPosition;
    QVector3D   m_backgroundColor1;
    QVector3D   m_backgroundColor2;

    QVector4D   m_kInitialTr;//! Kinect Translation start point
    QVector4D   m_kNewTr;//! Kinect Translation update point
    QVector4D   m_kResTr;//! Kinect Translation result

    QVector4D   m_kNewLeftRot;  //!Kinect Rotation update point
    QVector4D   m_kOldLeftRot;  //!Kinect Rotation start point
    QVector4D   m_kNewRightRot;  //!Kinect Rotation update point
    QVector4D   m_kOldRightRot;  //!Kinect Rotation start point


    //private members without getters/setters
    double      m_eyeSeparation;
    bool        zooming;
    unsigned int         m_kLUC; //! kinect unknown count
    unsigned int         m_kLNTC; //! kinect not tracked count
    unsigned int         m_kLOC; //! kinect open count
    unsigned int         m_kLCC; //! kinect closed count
    unsigned int         m_kLLC; //! kinect lasso count
    unsigned int         m_kRUC; //! kinect unknown count
    unsigned int         m_kRNTC; //! kinect not tracked count
    unsigned int         m_kROC; //! kinect open count
    unsigned int         m_kRCC; //! kinect closed count
    unsigned int         m_kRLC; //! kinect lasso count
    unsigned int         m_slowDownTrackball; //!accumulates Touchscreen events when 3+fingers move
    QPoint      m_iFP; //!initial finger position
    QPointF      m_iZS; //!initial zoom separation
    QPointF      m_nZS; //!new zoom separation
    QVector<QPointF>      m_lastTouchPosition; //!list of last positions where the fingers were

    static const
    int  m_jumpSize=40;
    static const
    int      m_zoomTolerance=10;

    QMatrix4x4  m_pMatrix;
    QMatrix4x4  m_vMatrix;
    QMatrix4x4  m_LeftVMatrix;
    QMatrix4x4  m_RightVMatrix;
    QMatrix4x4  m_MatrixLights;
    QMatrix4x4  m_MatrixLightCircle;

    QGLBuffer   m_vertexBuffer;
    QGLBuffer   m_colorBuffer;
    QGLBuffer   m_normalBuffer;
    QGLBuffer   m_indexBuffer;
    QGLBuffer   m_lightVertexBuffer;
    QGLBuffer   m_lightOnColorBuffer;
    QGLBuffer   m_lightOffColorBuffer;
    QGLBuffer   m_lightCircleVertexBuffer;
    QGLBuffer   m_lightCircleColorBuffer;

    QGLShaderProgram m_lightingShaderP;
    QGLShaderProgram m_coloringShaderP;
    QGLShaderProgram m_singleColorShaderP;

    double normalizeAngle180(double angle);
    double normalizeAngle360(double angle);

    bool getIntersectionPoint(QPoint point, QVector3D *desiredPoint,  QMatrix4x4 l_vMatrix);
    bool getClickedLight(QPoint point, int* light,  QMatrix4x4 l_vMatrix);

    QPoint QVector3DUnnormalizedToQPoint(QVector3D vector);
    QVector3D QPointNormalizedToVector3D(QPoint point);
    QPointF pixelPosToViewPos(const QPointF &p);

    QSize   minimumSizeHint() const;
    QSize   sizeHint() const;

};


#endif
