#ifndef AVPLUGINMANAGER_H
#define AVPLUGINMANAGER_H

#include <QObject>
#include <QMutex>
#include <QDir>
#include <QStringList>
#include <QVector>
#include <QMap>

class AVModel;
class ReaderInterface;


class AVPluginManager : public QObject
{
    Q_OBJECT

public:
    static AVPluginManager* instance()
    {
        static QMutex mutex;
        if(!m_instance)
        {
            mutex.lock();
            if(!m_instance) m_instance = new AVPluginManager;
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

    void setPluginsDir(const QString& dirName);
    void loadPlugins();

    QStringList         getReadableFileTypes();
    ReaderInterface*    getReaderForFileType(QString filetype);


private:

    explicit AVPluginManager(QObject *parent = 0);

    // we leave just the declarations, so the compiler will warn us
    // if we try to use those two functions by accident
    AVPluginManager(const AVPluginManager &); //hide copy constructor
    AVPluginManager& operator=(const AVPluginManager &); //hide assign op

    ~AVPluginManager();

    static AVPluginManager* m_instance;
    QDir m_pluginsDir;
    QStringList pluginFileNames;

    QMap<QString, ReaderInterface*> m_readerPluginMap;

signals:

public slots:

};

#endif // AVPLUGINMANAGER_H
