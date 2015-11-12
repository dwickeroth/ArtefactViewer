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

#include "avlight.h"
#include "avtrackball.h"

class AVModel;
class AVTrackBall;


class AVGLWidget: public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

public:
    AVGLWidget(QWidget *parent = 0);
    ~AVGLWidget();
    AVTrackBall*  m_trackball;


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

//    AVTrackBall getTrackBall() const;
//    void setTrackBall(AVTrackBall trackBall);

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

private:
    void setZoomLevel(double level, bool relative);

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
    //bool AVGLWidget::event(QEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);


private:

    void drawBackground();
    void setupCamera(boolean sizeIsLeft);
    void drawModelShaded(QMatrix4x4 l_vMatrix);
    void drawModelNoShading(QMatrix4x4 l_vMatrix);
    void drawLights(QMatrix4x4 l_vMatrix);
    void drawLightCircles(QMatrix4x4 l_vMatrix);
    void drawOverlays(QPaintDevice* device, bool offscreen = false, int fboWidth = 0);

private:
    //private members with getters/setters
//    AVTrackBall*  m_trackball;

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

    double      m_camDistanceToOrigin;

    QPoint      m_lastMousePosition;
    QVector3D   m_camOrigin;
    QVector3D   m_camPosition;
    QVector3D   m_LeftCamPosition;
    QVector3D   m_RightCamPosition;
    QVector3D   m_backgroundColor1;
    QVector3D   m_backgroundColor2;

    QMatrix4x4  m_MatrixArtefact;


    //private members without getters/setters
    AVModel*    m_model;

    double      m_eyeSeparation;

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
