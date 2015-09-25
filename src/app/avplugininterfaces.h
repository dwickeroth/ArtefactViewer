#ifndef PLUGINFILEREADERINTERFACE_H
#define PLUGINFILEREADERINTERFACE_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
//class QVector;
//class QVector3D;
QT_END_NAMESPACE

class AVModel;

class ReaderInterface
{
public:
    virtual ~ReaderInterface() {}

    virtual bool init() = 0;

    virtual QStringList fileTypes() = 0;
    virtual void setFilename(const QString& filename) = 0;
    virtual QString getFilename() = 0;

    virtual int readFile(QVector<QVector3D>& vertices, QVector<QVector3D>& colors, QVector<unsigned int>& triangles) = 0;
};


QT_BEGIN_NAMESPACE
#define ReaderInterface_iid "av.reader.0.1"
Q_DECLARE_INTERFACE(ReaderInterface, ReaderInterface_iid)
QT_END_NAMESPACE

#endif // PLUGINFILEREADERINTERFACE_H
