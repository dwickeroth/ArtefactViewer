#ifndef AVCONTROLLER_H
#define AVCONTROLLER_H

#include <QMutex>
#include <QString>
#include <QColor>


class AVMainWindow;
class AVPluginManager;
class AVModel;
class AVGLWidget;
class AVPQReader;

class AVController
{
public:

    static AVController* instance()
    {
        static QMutex mutex;
        if(!m_instance)
        {
            mutex.lock();
            if(!m_instance) m_instance = new AVController;
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

    int readFile(QString filename);
    void saveXmlFile();

private:

    AVController();

    // we leave just the declarations, so the compiler will warn us
    // if we try to use those two functions by accident
    AVController(const AVController &); //hide copy constructor
    AVController& operator=(const AVController &); //hide assign op

    ~AVController();

    static AVController*    m_instance;

    AVMainWindow*           m_mainWindow;
    AVPluginManager*        m_pluginManager;
    AVModel*                m_model;
    AVGLWidget*             m_glWidget;
    AVPQReader*             m_pqReader;

    QString                 m_currentlyOpenFile;
    bool                    m_xmlFileAlreadyExists;

public:

    static QColor      QVector3DToQColor(QVector3D vector);
    static QString     QVector4DToQString(QVector4D vector);
    static QString     QMatrix4x4ToQString(QMatrix4x4 matrix);
    static QVector4D   QStringToQVector4D(QString string);
    static QMatrix4x4  QStringToQMatrix4x4(QString string);
};

#endif // AVCONTROLLER_H
