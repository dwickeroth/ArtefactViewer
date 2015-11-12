#include "avpluginmanager.h"
#include "avmainwindow.h"
#include "avplugininterfaces.h"
#include "avcontroller.h"
#include "avpqreader.h"

#include <iostream>

#include <QPluginLoader>


AVPluginManager* AVPluginManager::m_instance = 0;

AVPluginManager::AVPluginManager(QObject *parent) :
    QObject(parent)
{
}

AVPluginManager::~AVPluginManager()
{

}


QStringList AVPluginManager::getReadableFileTypes()
{
    return m_readerPluginMap.uniqueKeys();
}


ReaderInterface *AVPluginManager::getReaderForFileType(QString filetype)
{
    return m_readerPluginMap.value(filetype.toLower());
}


void AVPluginManager::setPluginsDir(const QString &dirName)
{
    std::cout << "pluginsDir: " << dirName.toStdString().c_str() << std::endl;
    m_pluginsDir = QDir(dirName);
}


void AVPluginManager::loadPlugins()
{
    foreach(QString fileName, m_pluginsDir.entryList(QDir::Files))
    {
     //if clauses to shorten standard output
        if(fileName.toStdString()=="readStld.dll")
            std::cout << "checking file: " << fileName.toStdString().c_str() << std::endl;
        if(fileName.toStdString()=="readPlyd.dll")
            std::cout << "checking file: " << fileName.toStdString().c_str() << std::endl;

        QPluginLoader loader(m_pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if(!plugin)
        {
          //if clauses to shorten standard output
            if(fileName.toStdString()=="readStld.dll")
            std::cout << "not a plugin: " << fileName.toStdString().c_str() << std::endl;
            if(fileName.toStdString()=="readPlyd.dll")
            std::cout << "not a plugin: " << fileName.toStdString().c_str() << std::endl;
            continue;
        }

        std::cout << "loaded plugin: " << fileName.toStdString().c_str() << " successfully" << std::endl;


        ReaderInterface* iReader = qobject_cast<ReaderInterface*>(plugin);
        if(iReader)
        {
            std::cout << "casted plugin to reader" << std::endl;

            if(!iReader->init())
            {
                std::cout << "Plugin init unsucessful" << std::endl;
                continue;
            }

            QStringList fileTypes = iReader->fileTypes();
            for(int i = 0; i < fileTypes.size(); i++)
            {
                m_readerPluginMap.insertMulti(fileTypes.at(i), iReader);
            }
        }
    }

}

