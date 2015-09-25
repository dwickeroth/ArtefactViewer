#ifndef READSTL_H
#define READSTL_H

#include "avplugininterfaces.h"

#include <QObject>
#include <QtPlugin>
#include <QStringList>
#include <QVector>
#include <QVector3D>

class ReadStl : public QObject, public ReaderInterface
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "av.reader.0.1" FILE "readStl.json")
    Q_INTERFACES(ReaderInterface)

public:

    bool init();

    QStringList fileTypes();

    void setFilename(const QString& filename);
    QString getFilename();

    int readFile(QVector<QVector3D>& vertices, QVector<QVector3D>& colors, QVector<unsigned int>& triangles);

private:
    QString m_fileName;
};

#endif // READSTL_H
