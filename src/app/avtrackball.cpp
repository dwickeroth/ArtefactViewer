/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "avtrackball.h"
#include <iostream>
#define PI 3.14159265358979             

//============================================================================//
//                                  TrackBall                                 //
//============================================================================//

AVTrackBall::AVTrackBall(TrackMode mode)
    : m_pressed(false)
    , m_mode(mode)
{

}

void AVTrackBall::push(const QPointF& p, const QQuaternion &)
{
    m_pressed = true;
    m_lastPos = p;
}

QQuaternion AVTrackBall::move(const QPointF& p, const QMatrix4x4 &transformation)
{
    if (!m_pressed)
        return QQuaternion();

    QQuaternion rotation;
    QVector3D axis;

    switch (m_mode) {
    case Plane:
        {
            QLineF delta(m_lastPos, p);
            axis = QVector3D(-delta.dy(), delta.dx(), 0.0f).normalized();
            axis = transformation.inverted().mapVector(axis);
            axis.normalize();
            rotation = QQuaternion::fromAxisAndAngle(axis, 180 / PI * delta.length());
        }
        break;
    case Sphere:
        {
            QVector3D lastPos3D = QVector3D(m_lastPos.x(), m_lastPos.y(), 0.0f);
            float sqrZ = 1 - QVector3D::dotProduct(lastPos3D, lastPos3D);
            if (sqrZ > 0)
                lastPos3D.setZ(sqrt(sqrZ));
            else
                lastPos3D.normalize();

            QVector3D currentPos3D = QVector3D(p.x(), p.y(), 0.0f);
            sqrZ = 1 - QVector3D::dotProduct(currentPos3D, currentPos3D);
            if (sqrZ > 0)
                currentPos3D.setZ(sqrt(sqrZ));
            else
                currentPos3D.normalize();

            axis = QVector3D::crossProduct(lastPos3D, currentPos3D);
            float angle = 180 / PI * asin(sqrt(QVector3D::dotProduct(axis, axis)));

            axis = transformation.inverted().mapVector(axis);
            axis.normalize();
            rotation = QQuaternion::fromAxisAndAngle(axis, angle);
        }
        break;
    }

    m_lastPos = p;
    return rotation;
}

QQuaternion AVTrackBall::release(const QPointF& p, const QMatrix4x4 &transformation)
{
    // Calling move() caused the rotation to stop if the framerate was too low.
    QQuaternion rotation = move(p, transformation);
    m_pressed = false;
    return rotation;
}
