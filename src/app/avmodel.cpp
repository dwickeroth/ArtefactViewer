#include "avmodel.h"

#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QTime>

AVModel* AVModel::m_instance = 0;

AVModel::AVModel()
{
}

AVModel::~AVModel()
{

}


//! Sets up three QVectors to draw the light representations
/*! lightVertices stores the vertices of a pyramind style object
 * lightOnColors stores the vertex colors for lights that are switched on
 * lightOffColors stores the vertex colors for lights that are switched off.
 */
void AVModel::setupLightGeometry()
{
    m_lightVertices << QVector3D(0,1,0) << QVector3D(-0.5,0,-0.5) << QVector3D(-0.5,0,0.5)
                    << QVector3D(0,1,0) << QVector3D(-0.5,0,0.5) << QVector3D(0.5,0,0.5)
                    << QVector3D(0,1,0) << QVector3D(0.5,0,0.5) << QVector3D(0.5,0,-0.5)
                    << QVector3D(0,1,0) << QVector3D(0.5,0,-0.5) << QVector3D(-0.5,0,-0.5)
                    << QVector3D(-0.5,0,-0.5) << QVector3D(0.5,0,-0.5) << QVector3D(-0.5,0,0.5)
                    << QVector3D(0.5,0,0.5) << QVector3D(-0.5,0,0.5) << QVector3D(0.5,0,-0.5);

    m_lightOnColors << QVector3D(0.4f ,0.4f ,0.2f) << QVector3D(0.4f ,0.4f ,0.2f) << QVector3D(0.4f ,0.4f ,0.2f)
                    << QVector3D(0.4f ,0.4f ,0.2f) << QVector3D(0.4f ,0.4f ,0.2f) << QVector3D(0.4f ,0.4f ,0.2f)
                    << QVector3D(0.4f ,0.4f ,0.2f) << QVector3D(0.4f ,0.4f ,0.2f) << QVector3D(0.4f ,0.4f ,0.2f)
                    << QVector3D(0.4f ,0.4f ,0.2f) << QVector3D(0.4f ,0.4f ,0.2f) << QVector3D(0.4f ,0.4f ,0.2f)
                    << QVector3D(1.1f ,1.1f ,0.8f) << QVector3D(1.1f ,1.1f ,0.8f) << QVector3D(1.1f ,1.1f ,0.8f)
                    << QVector3D(1.1f ,1.1f ,0.8f) << QVector3D(1.1f ,1.1f ,0.8f) << QVector3D(1.1f ,1.1f ,0.8f);

    m_lightOffColors << QVector3D(0.1f ,0.1f ,0.1f) << QVector3D(0.1f ,0.1f ,0.1f) << QVector3D(0.1f ,0.1f ,0.1f)
                     << QVector3D(0.1f ,0.1f ,0.1f) << QVector3D(0.1f ,0.1f ,0.1f) << QVector3D(0.1f ,0.1f ,0.1f)
                     << QVector3D(0.1f ,0.1f ,0.1f) << QVector3D(0.1f ,0.1f ,0.1f) << QVector3D(0.1f ,0.1f ,0.1f)
                     << QVector3D(0.1f ,0.1f ,0.1f) << QVector3D(0.1f ,0.1f ,0.1f) << QVector3D(0.1f ,0.1f ,0.1f)
                     << QVector3D(0.3f ,0.3f ,0.3f) << QVector3D(0.3f ,0.3f ,0.3f) << QVector3D(0.3f ,0.3f ,0.3f)
                     << QVector3D(0.3f ,0.3f ,0.3f) << QVector3D(0.3f ,0.3f ,0.3f) << QVector3D(0.3f ,0.3f ,0.3f);

    setupLightCircle(m_lightCircleVertices, m_lightCircleColors);
}


//! Returns the center point of the vertices QVector
/*! Center point is calculated by separately averaging the x, y and z values
 * of all elements.
 */
void AVModel::calculateCenterPoint()
{
    double sumX = 0.0;
    double sumY = 0.0;
    double sumZ = 0.0;

    for (int i=0; i < m_vertices.size(); i++)
    {
        sumX += m_vertices[i].x();
        sumY += m_vertices[i].y();
        sumZ += m_vertices[i].z();
    }
    m_centerPoint = QVector3D(sumX / m_vertices.size(), sumY / m_vertices.size(), sumZ / m_vertices.size());
}


//! Calculate a shadow map
/*! According to the triangles surrounding a vertex a value is calculated that determines the darkness that will be added to the point later
 * To do this, the averaged normal at each vertex is taken and for each triangle the vertex is participating in
 * the vector from the top of the normalized averaged normal perpendicular to the plane of the current triangle
 * is determined. If the point where the plane is hit is outside the triangle, there will be no darkness for the dark
 * valley algorithm, because it indicates that the vertex is relatively exposed. Only in case the triangle is hit there seems
 * to be some kind of valley and a certain darkness is added. For the dark edges algorithm it does not matter whether
 * the point is inside or outside the triangle. Darkness is only determined by the distance to the vertex, because
 * that indicates, how sharp the edge is.

 */
void AVModel::calculateShadowMap(int mode, int param1, int param2, int param3)
{
    //param 1 = strength
    //param 2 = threshold
    //param 3 = smooth

    //qDebug() << "AVModel::calculateShadowMap: param1: " << param1 << "  param2: " << param2 << "  param3: " << param3;

    m_shadowMap.clear();

    switch (mode)
    {
    case 0:
        for (int i=0; i < m_vertices.size(); i++) m_shadowMap.append(1.0f);
        break;
    case 1:
    case 2:
    case 3:
        QVector<GLuint> weights;
        for (int i=0; i < m_vertices.size(); i++)
        {
            weights.append(1);
            m_shadowMap.append(1.0f);
        }
        QVector3D a = QVector3D(0,0,0);
        QVector3D b = QVector3D(0,0,0);
        QVector3D c = QVector3D(0,0,0);
        QVector3D normalAtA = QVector3D(0,0,0);
        QVector3D normalAtB = QVector3D(0,0,0);
        QVector3D normalAtC = QVector3D(0,0,0);
        QVector3D planeNormal = QVector3D(0,0,0);

        for (int i=0; i < m_triangles.size(); i+=3)
        {
            a = m_vertices[m_triangles[i]];
            b = m_vertices[m_triangles[i+1]];
            c = m_vertices[m_triangles[i+2]];
            normalAtA = m_normals[m_triangles[i]];
            normalAtB = m_normals[m_triangles[i+1]];
            normalAtC = m_normals[m_triangles[i+2]];
            planeNormal = QVector3D::normal(a,b,c);

            // Calculate the points on the triangle's plane with the shortest distance
            //to the normalized averaged normal vector of the corresponding vertex
            QVector3D point1 = (a + normalAtA) - QVector3D::dotProduct(normalAtA, planeNormal) * planeNormal;
            QVector3D point2 = (b + normalAtB) - QVector3D::dotProduct(normalAtB, planeNormal) * planeNormal;
            QVector3D point3 = (c + normalAtC) - QVector3D::dotProduct(normalAtC, planeNormal) * planeNormal;

            float val = 0;
            if (mode == 1 || mode == 2)
            {
                if ((point1 - a).length() > (float)param2 / 500)
                {
                    val = (1.0 - ((point1 - a).length()));
                    m_shadowMap[m_triangles[i]] = m_shadowMap[m_triangles[i]] + pow(val,param1);
                    weights[m_triangles[i]]++;
                }
                if ((point2 - b).length() > (float)param2 / 500)
                {
                    val = (1.0 - ((point2 - b).length()));
                    m_shadowMap[m_triangles[i+1]] = m_shadowMap[m_triangles[i+1]] + pow(val,param1);
                    weights[m_triangles[i+1]]++;
                }
                if ((point3 - c).length() > (float)param2 / 500)
                {
                    val = (1.0 - ((point3 - c).length()));
                    m_shadowMap[m_triangles[i+2]] = m_shadowMap[m_triangles[i+2]] + pow(val,param1);
                    weights[m_triangles[i+2]]++;
                }
            }
            else
            {
                if (isInTriangle(a, b, c, planeNormal, point1))
                {
                    val = (1.0 - ((point1 - a).length()));
                    m_shadowMap[m_triangles[i]] = m_shadowMap[m_triangles[i]] + pow(val,param1*10);
                    weights[m_triangles[i]]++;
                }
                if (isInTriangle(a, b, c, planeNormal, point2))
                {
                    val = (1.0 - ((point2 - b).length()));
                    m_shadowMap[m_triangles[i+1]] = m_shadowMap[m_triangles[i+1]] + pow(val,param1*10);
                    weights[m_triangles[i+1]]++;
                }
                if (isInTriangle(a, b, c, planeNormal, point3))
                {
                    val = (1.0 - ((point3 - c).length()));
                    m_shadowMap[m_triangles[i+2]] = m_shadowMap[m_triangles[i+2]] + pow(val,param1*10);
                    weights[m_triangles[i+2]]++;
                }
            }
        }

        // Calculate average
        for (int i=0; i < m_shadowMap.size(); i++) m_shadowMap[i] = m_shadowMap[i] / weights[i];

        // Smoothing
        bool useOldSmoothing = false;
        if(useOldSmoothing)
        {
            if (mode == 3) param3 *=2;
            for (int i=0; i < param3; i++)
            {
                QVector<float> darkness;
                for (int j=0; j < m_vertices.size(); j++) darkness.append(0.0);
                for (int j=0; j < darkness.size(); j++)
                {
                    float sum = 0.0;
                    for (int k=0; k < m_neighborhood[j].size(); k++)
                    {
                        sum += m_shadowMap[m_neighborhood[j][k]];
                    }
                    sum /= m_neighborhood[j].size();
                    darkness[j] += (1-sum);
                }

                for (int j=0; j < m_shadowMap.size(); j++)
                {
                    m_shadowMap[j] = 1-darkness[j];
                }
            }
        }
        else
        {
            /*
            QVector<quint32> neigh1 = getKNeighborhood(0,1);
            QVector<quint32> neigh2 = getKNeighborhood(0,2);
            QVector<quint32> neigh3 = getKNeighborhood(0,3);

            qDebug() << "neigh1 : " << neigh1;
            qDebug() << "neigh2 : " << neigh2;
            qDebug() << "neigh3 : " << neigh3;
            */

            if(param3 > 0)
            {
                QVector<float> darkness;
                for(int i = 0; i < m_vertices.size(); i++)
                {
                    if(i % 2000 == 0) {
                        float percent = (float(i) * 100.0f) / m_vertices.size();
                        qDebug() << "completed " << i << " vertices = " << percent << " percent";
                    }

                    QVector<quint32> neighborhood = getKNeighborhood(i,param3);
                    float darknessVal = 0.0;
                    for(int j = 0; j < neighborhood.size(); j++) darknessVal += m_shadowMap[neighborhood[j]];
                    darknessVal /= neighborhood.size();
                    darkness.append(darknessVal);
                }

                for(int i = 0; i < m_shadowMap.size(); i++)
                    m_shadowMap[i] = darkness[i];
            }

        }

        // Black on white or white on black?
        if (mode == 2) for (int i=0; i < m_shadowMap.size(); i++) m_shadowMap[i] = 1-m_shadowMap[i];

        break;
    }
}


//! Calculates for three given vertices and a provided normal, whether the given intersection point is inside or outside the triangle of the three points.
bool AVModel::isInTriangle(QVector3D a, QVector3D b, QVector3D c, QVector3D normal, QVector3D intersection)
{
    QVector3D x = a + normal;
    if ((QVector3D::dotProduct(intersection, QVector3D::crossProduct(b-x, a-x)) - (QVector3D::dotProduct(x, QVector3D::crossProduct(b-x, a-x)))) < 0) return false;
    if ((QVector3D::dotProduct(intersection, QVector3D::crossProduct(c-x, b-x)) - (QVector3D::dotProduct(x, QVector3D::crossProduct(c-x, b-x)))) < 0) return false;
    if ((QVector3D::dotProduct(intersection, QVector3D::crossProduct(a-x, c-x)) - (QVector3D::dotProduct(x, QVector3D::crossProduct(a-x, c-x)))) < 0) return false;
    return true;
}


//! Walks all triangles and saves for every three vertices of a triangle that they are neighbors.
void AVModel::calculateNeighborhood()
{
    m_neighborhood.clear();
    for (int i=0; i < m_vertices.size(); i++) m_neighborhood.append(QVector<GLuint>());
    for (int i=0; i < m_triangles.size(); i+=3)
    {
        addNeighbor(m_triangles[i], m_triangles[i+1]);
        addNeighbor(m_triangles[i], m_triangles[i+2]);
        addNeighbor(m_triangles[i+1], m_triangles[i]);
        addNeighbor(m_triangles[i+1], m_triangles[i+2]);
        addNeighbor(m_triangles[i+2], m_triangles[i+1]);
        addNeighbor(m_triangles[i+2], m_triangles[i]);
    }
}


//! If a vertex already is a neighbor, skip it. Otherwise, add it.
void AVModel::addNeighbor(GLuint vertex, GLuint neighbor)
{
    for (int i=0; i < m_neighborhood[vertex].size(); i++)
    {
        if (m_neighborhood[vertex][i] == neighbor) return;
    }
    m_neighborhood[vertex].append(neighbor);
    return;
}


//! Calculate normals for all vertices
/*! Walks the triangles QVector, calculates normals for 3 vertices per triangle and
 * stores them in the normals QVector. Because most vertices are part of several triangles
 * the final normal vector of a vertex is the average of all calculated normals.
 */
void AVModel::calculateNormals()
{
    m_normals.clear();
    for (int i=0; i < m_vertices.size(); i++) m_normals.append(QVector3D(0,0,0));

    for (int i=0; i < m_triangles.size(); i+=3)
    {
        QVector3D tempNormal;
        tempNormal = QVector3D::normal(m_vertices[m_triangles[i]], m_vertices[m_triangles[i+1]], m_vertices[m_triangles[i+2]]);
        m_normals[m_triangles[i]]   = m_normals[m_triangles[i]]   + tempNormal;
        m_normals[m_triangles[i+1]] = m_normals[m_triangles[i+1]] + tempNormal;
        m_normals[m_triangles[i+2]] = m_normals[m_triangles[i+2]] + tempNormal;
    }

    for(int i = 0; i < m_normals.size(); i++) m_normals[i].normalize();
}


//! Calculates the height by the difference of the highest and lowest value regarding the Y-axis
float AVModel::getArtefactHeight(QMatrix4x4 mMatrixArtefact)
{
    float highestY = 0.0f;
    float lowestY = 0.0f;

    if (m_vertices.size() > 0)
    {
        highestY = (mMatrixArtefact * m_vertices[0]).y();
        lowestY = (mMatrixArtefact * m_vertices[0]).y();
    }

    for (int i=1; i < m_vertices.size(); i++)
    {
        if ((mMatrixArtefact * m_vertices[i]).y() > highestY) highestY = (mMatrixArtefact * m_vertices[i]).y();
        if ((mMatrixArtefact * m_vertices[i]).y() < lowestY) lowestY = (mMatrixArtefact * m_vertices[i]).y();
    }
    return (highestY - lowestY);
}


//! Calculates the width by the difference of the highest and lowest value regarding the X-axis
float AVModel::getArtefactWidth(QMatrix4x4 mMatrixArtefact)
{
    float highestX = 0.0f;
    float lowestX = 0.0f;

    if (m_vertices.size() > 0)
    {
        highestX = (mMatrixArtefact * m_vertices[0]).x();
        lowestX = (mMatrixArtefact * m_vertices[0]).x();
    }

    for (int i=1; i < m_vertices.size(); i++)
    {
        if ((mMatrixArtefact * m_vertices[i]).x() > highestX) highestX = (mMatrixArtefact * m_vertices[i]).x();
        if ((mMatrixArtefact * m_vertices[i]).x() < lowestX) lowestX = (mMatrixArtefact * m_vertices[i]).x();
    }
    return (highestX - lowestX);
}


//! Calculates the depth by the difference of the highest and lowest value regarding the Z-axis
float AVModel::getArtefactDepth(QMatrix4x4 mMatrixArtefact)
{
    float highestZ = 0.0f;
    float lowestZ = 0.0f;

    if (m_vertices.size() > 0)
    {
        highestZ = (mMatrixArtefact * m_vertices[0]).z();
        lowestZ = (mMatrixArtefact * m_vertices[0]).z();
    }

    for (int i=1; i < m_vertices.size(); i++)
    {
        if ((mMatrixArtefact * m_vertices[i]).z() > highestZ) highestZ = (mMatrixArtefact * m_vertices[i]).z();
        if ((mMatrixArtefact * m_vertices[i]).z() < lowestZ) lowestZ = (mMatrixArtefact * m_vertices[i]).z();
    }
    return (highestZ - lowestZ);
}

void AVModel::setupLightCircle(QVector<QVector3D>& vertexArray, QVector<QVector3D> &colorArray)
{
    int num_segments = 64;
    float theta = 2 * 3.14159265358979 / float(num_segments);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;

    //we start at angle = 0
    float x = 1;
    float y = 0;

    for(int ii = 0; ii < num_segments; ii++)
    {
        vertexArray << QVector3D(x, 0.0f, y);//output vertex
        colorArray  << QVector3D(0,1,0);

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
}


QVector<quint32> AVModel::getKNeighborhood(int index, int k, bool firstRun)
{

    QVector<quint32> nhood;

    if(firstRun)
        nhood.append(index);

    if(k > 0)
    {
        nhood += m_neighborhood[index];
        k--;

        if(k > 0)
        {
            for(int i = 0; i < m_neighborhood[index].size(); i++)
            {
                nhood += getKNeighborhood(m_neighborhood[index][i],k,false);
            }
        }
    }

    if(firstRun)
    {
        std::sort(nhood.begin(),nhood.end());
        QVector<quint32> returnHood;
        returnHood.append(nhood[0]);
        for(int i = 1; i < nhood.size(); i++) if(nhood[i] != nhood[i-1]) returnHood.append(nhood[i]);

        return returnHood;
    }

    return nhood;
}

