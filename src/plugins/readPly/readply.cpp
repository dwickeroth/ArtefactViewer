#include "readply.h"
#include "rply.h"
#include "iostream"

#include <float.h>

#include <QColor>


//! Callback function that stores one "line" of information as the three coordinates and color values of a Vertex
int storeVertex_cb(p_ply_argument argument)
{
    long axis;
    void* pointer;
    ply_get_argument_user_data(argument, &pointer, &axis);
    QVector<QVector3D>* vertices = static_cast<QVector<QVector3D>*>(pointer);

    if (axis == 0)
    {
        vertices->append(QVector3D(0,0,0));
    }
    switch (axis)
    {
    case 0: vertices->last().setX(ply_get_argument_value(argument)); break;
    case 1: vertices->last().setY(ply_get_argument_value(argument)); break;
    case 2: vertices->last().setZ(ply_get_argument_value(argument)); break;
    }
    return 1;
}

int storeColor_cb(p_ply_argument argument)
{
    long axis;
    void* pointer;
    ply_get_argument_user_data(argument, &pointer, &axis);
    QVector<QVector3D>* colors = static_cast<QVector<QVector3D>*>(pointer);

    if (axis == 0)
    {
        colors->append(QVector3D(0,0,0));
    }
    switch (axis)
    {
    case 0: colors->last().setX(ply_get_argument_value(argument)/255.0f); break;
    case 1: colors->last().setY(ply_get_argument_value(argument)/255.0f); break;
    case 2: colors->last().setZ(ply_get_argument_value(argument)/255.0f); break;
    }

    return 1;
}

//! Callback function that stores a triangle in the geometry
int storeTriangle_cb(p_ply_argument argument)
{
    long length, value_index;
    ply_get_argument_property(argument, NULL, &length, &value_index);

    void* pointer;
    ply_get_argument_user_data(argument, &pointer, 0);
    QVector<unsigned int>* triangles = static_cast<QVector<unsigned int>*>(pointer);

    switch (value_index)
    {
    case -1:
        return 1;
    case 0:
    case 1:
    case 2:
        triangles->append((quint32)ply_get_argument_value(argument));
        break;
    default:
        break;
    }
    return 1;
}

bool ReadPly::init()
{
    return true;
}

QStringList ReadPly::fileTypes()
{
    return QStringList("ply");
}

void ReadPly::setFilename(const QString &filename)
{
    m_fileName = filename;
}

QString ReadPly::getFilename()
{
    return m_fileName;
}

int ReadPly::readFile(QVector<QVector3D> &vertices, QVector<QVector3D> &colors, QVector<unsigned int> &triangles)
{
    std::cout << "readFile: " << m_fileName.toLocal8Bit().constData() << std::endl;

    p_ply ply = ply_open(m_fileName.toLocal8Bit().constData(), NULL, 0, NULL);
    if (!ply) return 1;
    if (!ply_read_header(ply)) return 1;


    long nvertices = ply_set_read_cb(ply, "vertex", "x", &storeVertex_cb, &vertices, 0);
    ply_set_read_cb(ply, "vertex", "y", &storeVertex_cb, &vertices, 1);
    ply_set_read_cb(ply, "vertex", "z", &storeVertex_cb, &vertices, 2);

    long ncolors = ply_set_read_cb(ply, "vertex", "red", &storeColor_cb, &colors, 0);
    ply_set_read_cb(ply, "vertex", "green", &storeColor_cb, &colors, 1);
    ply_set_read_cb(ply, "vertex", "blue", &storeColor_cb, &colors, 2);

    long ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", &storeTriangle_cb, &triangles, 0);

    vertices.clear();
    colors.clear();
    triangles.clear();

    if (!ply_read(ply)) return 1;
    ply_close(ply);

    return 0;
}

