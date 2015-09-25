#ifndef AVMODEL_H
#define AVMODEL_H

#include <QMutex>
#include <QVector>
#include <QString>
#include <QVector3D>
#include <QColor>

struct pointCloud
{
    QString text;
    QColor color;
    int typeId; // 0 = Note, 1 = Distance, 2 = Angle, 3 = Area
    QVector<QVector3D> points;
};


class AVModel
{
public:
    static AVModel* instance()
    {
        static QMutex mutex;
        if(!m_instance)
        {
            mutex.lock();
            if(!m_instance) m_instance = new AVModel;
            mutex.unlock();
        }
        return m_instance;
    }

    static void destroy()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_instance;
        m_instance = 0;
        mutex.unlock();
    }

private:
    AVModel();

    // we leave just the declarations, so the compiler will warn us
    // if we try to use those two functions by accident
    AVModel(const AVModel&); //hide copy constructor
    AVModel& operator=(const AVModel &); //hide assign op
    ~AVModel();

    static AVModel* m_instance;

public:
    QVector<QVector3D>  m_vertices;
    QVector<quint32>     m_triangles;
    QVector<QVector3D>  m_normals;
    QVector<QVector3D>  m_colors;
    QVector<float>      m_shadowMap;

    QVector3D           m_flatColor;
    QVector<QVector3D>  m_lightVertices;
    QVector<QVector3D>  m_lightOnColors;
    QVector<QVector3D>  m_lightOffColors;
    QVector<QVector3D>  m_lightCircleVertices;
    QVector<QVector3D>  m_lightCircleColors;
    QVector3D           m_centerPoint;
    QList<pointCloud>   m_listOfPointClouds;

    QVector< QVector<quint32> > m_neighborhood;

    void addNeighbor(quint32 vertex, quint32 neighbor);

    void setupLightGeometry();
    void calculateCenterPoint();
    void calculateNormals();
    void calculateShadowMap(int mode, int param1, int param2, int param3);
    void calculateNeighborhood();
    bool isInTriangle(QVector3D a, QVector3D b, QVector3D c, QVector3D normal, QVector3D intersection);

    float getArtefactDepth(QMatrix4x4 mMatrixArtefact);
    float getArtefactWidth(QMatrix4x4 mMatrixArtefact);
    float getArtefactHeight(QMatrix4x4 mMatrixArtefact);


private:

    void setupLightCircle(QVector<QVector3D>& vertexArray, QVector<QVector3D>& colorArray);
    QVector<quint32> getKNeighborhood(int index, int k, bool firstRun = true);

};

#endif // AVMODEL_H
