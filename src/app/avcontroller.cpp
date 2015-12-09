#include "avcontroller.h"

#include "avglwidget.h"
#include "avmainwindow.h"
#include "avmodel.h"
#include "avplugininterfaces.h"
#include "avpluginmanager.h"
#include "avpqreader.h"
#include <QMessageBox>
#include <QDomDocument>
#include <QCoreApplication>
#include "avtouchevent.h"

#include <iostream>

AVController* AVController::m_instance = 0;

AVController::AVController()
{
    m_mainWindow = AVMainWindow::instance();
    m_glWidget = new AVGLWidget(m_mainWindow);
    m_pqReader= AVPQReader::instance();

    int err_code=m_pqReader->Init();
        std::cout<<"We started the PQReader, the initialization code is "<<err_code<<std::endl;
    m_pqReader->setGLWidget(m_glWidget);
    m_glWidget->setFocusPolicy(Qt::StrongFocus);
    m_mainWindow->setGLWidget(m_glWidget);
    m_mainWindow->showMaximized();

    //eager initialization of Singletons
    m_model = AVModel::instance();
    m_pluginManager = AVPluginManager::instance();

    //set plugins dir in plugin manager
    QDir pluginsDir = QCoreApplication::applicationDirPath();

    //TODO: Versioning for mac and linux, insert here

    pluginsDir.cd("plugins");
    m_pluginManager->setPluginsDir(pluginsDir.canonicalPath());
    m_pluginManager->loadPlugins();
    m_currentlyOpenFile = QString("");
    m_xmlFileAlreadyExists = false;


//SignalSlotApproach
    QObject::connect(m_pqReader,SIGNAL(throwEvent(AVTouchEvent*)),
                     m_glWidget,SLOT(catchEvent(AVTouchEvent*)));
//    if(QApplication::sendEvent(m_glWidget,&m_pqReader->e))
//        cout<<"sent"<<endl;
}


AVController::~AVController()
{
    m_pluginManager->destroy();
    m_model->destroy();
    m_mainWindow->destroy();
    m_pqReader->destroy();
}


int AVController::readFile(QString filename)
{

    QFileInfo fileInfo(filename);
    ReaderInterface* reader = AVPluginManager::instance()->getReaderForFileType(fileInfo.suffix().toLower());

    if(!reader)
    {
        std::cout << "Could not get reader for filetype: " << fileInfo.suffix().toLower().toLocal8Bit().constData() << std::endl;
        return 1;
    }

    reader->setFilename(filename);
    reader->readFile(m_model->m_vertices, m_model->m_colors, m_model->m_triangles);

    m_model->calculateNormals();
    m_model->calculateNeighborhood();
    m_model->calculateShadowMap(0, 0, 0, 0);
    m_model->calculateCenterPoint();

    m_mainWindow->initialize();
    m_glWidget->initialize();
    QString filePath = fileInfo.path();
    QString fileBaseName = fileInfo.baseName();
    m_currentlyOpenFile = filePath.append("/" +  fileBaseName);

    QFile file(m_currentlyOpenFile + ".xml");
    if (file.exists())
    {
        m_xmlFileAlreadyExists = true;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::information(m_mainWindow, QStringLiteral("Error"), QStringLiteral("Unable to open XML-file!"));
            return 1;
        }
        QDomDocument settings;
        if(!settings.setContent(&file))
        {
            QMessageBox::information(m_mainWindow, QStringLiteral("Error"), QStringLiteral("Unable to read XML-file!"));
            return 2;
        }
        file.close();
        QDomElement root = settings.firstChildElement();

        QDomElement misc = root.elementsByTagName("misc").at(0).toElement();
        bool use_lighting(misc.attribute("lighting") == "1");
        bool use_vertexColors(misc.attribute("vertexColors") == "1");
        bool paintAnnotations(misc.attribute("paintAnnotations") == "1");
        m_glWidget->setLighting(use_lighting);
        m_glWidget->setVertexColors(use_vertexColors);
        m_glWidget->setPaintAnnotations(paintAnnotations);
        m_mainWindow->setCheckBoxes(use_lighting, use_vertexColors, paintAnnotations);

        QDomElement background1 = misc.elementsByTagName("background").at(0).toElement();
        float red = background1.attribute("red").toFloat();
        float green = background1.attribute("green").toFloat();
        float blue = background1.attribute("blue").toFloat();
        m_glWidget->setBackgroundColor1(QVector3D(red, green, blue));

        QDomElement background2 = misc.elementsByTagName("background").at(1).toElement();
        red = background2.attribute("red").toFloat();
        green = background2.attribute("green").toFloat();
        blue = background2.attribute("blue").toFloat();
        m_glWidget->setBackgroundColor2(QVector3D(red, green, blue));

        QDomElement flatColor = misc.elementsByTagName("flatColor").at(0).toElement();
        m_model->m_flatColor.setX(flatColor.attribute("red").toFloat());
        m_model->m_flatColor.setY(flatColor.attribute("green").toFloat());
        m_model->m_flatColor.setZ(flatColor.attribute("blue").toFloat());

        QDomElement view = root.elementsByTagName("view").at(0).toElement();
        m_glWidget->setCamDistanceToOrigin(view.attribute("camDistanceToOrigin").toFloat());
        m_glWidget->setMatrixArtefact(QStringToQMatrix4x4(view.attribute("mMatrixArtefact")));        
        m_glWidget->setCamOrigin(QVector3D(QStringToQVector4D(view.attribute("camOrigin"))));

        for (int i=0; i < 4; i++)
        {
            QDomElement light = root.elementsByTagName("light"+QString::number(i)).at(0).toElement();
            m_glWidget->m_lights[i].setIsOn(light.attribute("isOn") == "1");
            m_glWidget->m_lights[i].setIntensity(light.attribute("intensity").toFloat());
            m_glWidget->m_lights[i].setVRotation(light.attribute("vRotation").toFloat());
            m_glWidget->m_lights[i].setHRotation(light.attribute("hRotation").toFloat());
            m_glWidget->m_lights[i].setDistanceToOrigin(light.attribute("distanceToOrigin").toFloat());
        }
        m_mainWindow->updateLightComboBox();
        m_model->m_listOfPointClouds.clear();
        QDomElement domPointClouds = root.elementsByTagName("domPointClouds").at(0).toElement();
        for (int i=0; i < domPointClouds.elementsByTagName("domPointCloud").size(); i++)
        {
            QDomElement domPointCloud = domPointClouds.elementsByTagName("domPointCloud").at(i).toElement();
            m_model->m_listOfPointClouds.append(pointCloud());
            m_model->m_listOfPointClouds.last().text = domPointCloud.attribute("text");
            m_model->m_listOfPointClouds.last().typeId = domPointCloud.attribute("typeId").toInt();
            QDomElement color = domPointCloud.elementsByTagName("color").at(0).toElement();
                m_model->m_listOfPointClouds.last().color.setRed(color.attribute("red").toInt());
                m_model->m_listOfPointClouds.last().color.setGreen(color.attribute("green").toInt());
                m_model->m_listOfPointClouds.last().color.setBlue(color.attribute("blue").toInt());
            QDomElement points = domPointCloud.elementsByTagName("points").at(0).toElement();
            for (int j=0; j < points.elementsByTagName("point").size(); j++)
            {
                QDomElement point = points.elementsByTagName("point").at(j).toElement();
                m_model->m_listOfPointClouds.last().points.append(QVector3D(0,0,0));
                m_model->m_listOfPointClouds.last().points.last().setX(point.attribute("x").toFloat());
                m_model->m_listOfPointClouds.last().points.last().setY(point.attribute("y").toFloat());
                m_model->m_listOfPointClouds.last().points.last().setZ(point.attribute("z").toFloat());
            }
        }
        m_model->m_listOfPointClouds.append(pointCloud());
        m_model->m_listOfPointClouds.last().color = QColor(Qt::blue);
        m_model->m_listOfPointClouds.last().typeId = 0;
        m_mainWindow->updatePushButtons();
    }
    else // Things to do in case no XML file has been found
    {
        m_xmlFileAlreadyExists = false;

        QMatrix4x4 modelMatrix = m_glWidget->getMatrixArtefact();
        modelMatrix.setToIdentity();
        modelMatrix.translate(-m_model->m_centerPoint);
        m_glWidget->setMatrixArtefact(modelMatrix);
    }

    m_mainWindow->updateInfoGroup();
    m_glWidget->fillBuffers();
    m_glWidget->updateGL();
    m_glWidget->updateGL();

    return 0;
}


void AVController::saveXmlFile()
{
    QMessageBox::StandardButton safetyQuestion = QMessageBox::Yes;

    if (m_xmlFileAlreadyExists) safetyQuestion = QMessageBox::question(m_mainWindow,
                         QStringLiteral("Overwrite settings?"),
                         QStringLiteral("While opening the current file additional informations stored in \
                                        XML concerning rotation and light settings have been read. \
                                        Do you want to overwrite them now with the current values?"),
                         QMessageBox::Yes | QMessageBox::No);
    if (safetyQuestion == QMessageBox::Yes)
    {
        QDomDocument document;

        QDomElement settings = document.createElement("settings");
        document.appendChild(settings);

        QDomElement misc = document.createElement("misc");
        misc.setAttribute("lighting", m_glWidget->getUseLighting());
        misc.setAttribute("vertexColors", m_glWidget->getUseVertexColors());
        misc.setAttribute("paintAnnotations", m_glWidget->getPaintAnnotations());
            QDomElement background1 = document.createElement("background");
            background1.setAttribute("red", m_glWidget->getBackgroundColor1().x());
            background1.setAttribute("green", m_glWidget->getBackgroundColor1().y());
            background1.setAttribute("blue", m_glWidget->getBackgroundColor1().z());
            misc.appendChild(background1);
            QDomElement background2 = document.createElement("background");
            background2.setAttribute("red", m_glWidget->getBackgroundColor2().x());
            background2.setAttribute("green", m_glWidget->getBackgroundColor2().y());
            background2.setAttribute("blue", m_glWidget->getBackgroundColor2().z());
            misc.appendChild(background2);
            QDomElement flatColor = document.createElement("flatColor");
            flatColor.setAttribute("red", m_model->m_flatColor.x());
            flatColor.setAttribute("green", m_model->m_flatColor.y());
            flatColor.setAttribute("blue", m_model->m_flatColor.z());
            misc.appendChild(flatColor);
        settings.appendChild(misc);

        QDomElement view = document.createElement("view");

        view.setAttribute("camDistanceToOrigin", m_glWidget->getCamDistanceToOrigin());
        view.setAttribute("mMatrixArtefact", QMatrix4x4ToQString(m_glWidget->getMatrixArtefact()));
        view.setAttribute("camOrigin", QVector4DToQString(m_glWidget->getCamOrigin()));
        settings.appendChild(view);

        QDomElement domLights[4];
        for (int i=0; i<4; i++)
        {
            domLights[i] = document.createElement("light"+QString::number(i));
            domLights[i].setAttribute("isOn", m_glWidget->m_lights[i].getIsOn());
            domLights[i].setAttribute("intensity", m_glWidget->m_lights[i].getIntensity());
            domLights[i].setAttribute("vRotation", m_glWidget->m_lights[i].getVRotation());
            domLights[i].setAttribute("hRotation", m_glWidget->m_lights[i].getHRotation());
            domLights[i].setAttribute("distanceToOrigin", m_glWidget->m_lights[i].getDistanceToOrigin());
            settings.appendChild(domLights[i]);
        }

        QDomElement domPointClouds = document.createElement("domPointClouds");
        for (int i=0; i < m_model->m_listOfPointClouds.size()-1; i++)
        {
            QDomElement domPointCloud = document.createElement("domPointCloud");
            domPointCloud.setAttribute("text", m_model->m_listOfPointClouds[i].text);
            domPointCloud.setAttribute("typeId", QString::number(m_model->m_listOfPointClouds[i].typeId));
                QDomElement color = document.createElement("color");
                color.setAttribute("red", m_model->m_listOfPointClouds[i].color.red());
                color.setAttribute("green", m_model->m_listOfPointClouds[i].color.green());
                color.setAttribute("blue", m_model->m_listOfPointClouds[i].color.blue());
                domPointCloud.appendChild(color);
                QDomElement points = document.createElement("points");
                for (int j=0; j < m_model->m_listOfPointClouds[i].points.size(); j++)
                {
                    QDomElement point = document.createElement("point");
                    point.setAttribute("x", QString::number(m_model->m_listOfPointClouds[i].points[j].x()));
                    point.setAttribute("y", QString::number(m_model->m_listOfPointClouds[i].points[j].y()));
                    point.setAttribute("z", QString::number(m_model->m_listOfPointClouds[i].points[j].z()));
                    points.appendChild(point);
                }
                domPointCloud.appendChild(points);
            domPointClouds.appendChild(domPointCloud);
        }
        settings.appendChild(domPointClouds);

        QFile file(m_currentlyOpenFile + ".xml");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::information(m_mainWindow, QStringLiteral("Error"), QStringLiteral("Unable to save file!"));
            return;
        }
        QTextStream stream(&file);
        stream << document.toString();
        file.close();
    }

}



// ************* Helper Functions *************

//! Converts a color stored in a QVector3D (as floats between 0 and 1) to a QColor with 3 times 256 Values
QColor AVController::QVector3DToQColor(QVector3D vector)
{
    return QColor(vector.x()*255, vector.y()*255, vector.z()*255);
}


//! Serializes a QVector4D into a QString for storage in an XML file
QString AVController::QVector4DToQString(QVector4D vector)
{
    QString result;
    result.append(QString::number(vector.x())); result.append(",");
    result.append(QString::number(vector.y())); result.append(",");
    result.append(QString::number(vector.z())); result.append(",");
    result.append(QString::number(vector.w()));
    return result;
}


//! Calls QVector4DToQString with the four rows of a QMatrix4D to store the whole matrix in a QString for storage in an XML file
QString AVController::QMatrix4x4ToQString(QMatrix4x4 matrix)
{
    QString result;
    result.append(QVector4DToQString(matrix.row(0)));
    result.append(",");
    result.append(QVector4DToQString(matrix.row(1)));
    result.append(",");
    result.append(QVector4DToQString(matrix.row(2)));
    result.append(",");
    result.append(QVector4DToQString(matrix.row(3)));
    return result;
}


//! Corrensponds to QVector4DToQString and creates a QVector4D from four comma separated values of a given QString
QVector4D AVController::QStringToQVector4D(QString string)
{
    QStringList list = string.split(",", QString::SkipEmptyParts);
    if(list.size() < 4)
    {
        qDebug() << "AVController::QStringToQVector4D : error: Stringlist too short. : " << string;
        return QVector4D(0,0,0,0);
    }
    return QVector4D(list[0].toFloat(), list[1].toFloat(), list[2].toFloat(), list[3].toFloat());
}


//! Corresponds to QMatrix4x4ToQString and creates a QMatrix4x4 from sixteen comma separated values in a given QString
QMatrix4x4 AVController::QStringToQMatrix4x4(QString string)
{
    QStringList list = string.split(",", QString::SkipEmptyParts);
    if(list.size() < 4)
    {
        qDebug() << "AVController::QStringToQMatrix4x4 : error: Stringlist too short. : " << string;
        return QMatrix4x4();
    }
    return QMatrix4x4(list[0].toFloat(), list[1].toFloat(), list[2].toFloat(), list[3].toFloat(), list[4].toFloat(), list[5].toFloat(), list[6].toFloat(), list[7].toFloat(), list[8].toFloat(), list[9].toFloat(), list[10].toFloat(), list[11].toFloat(), list[12].toFloat(), list[13].toFloat(), list[14].toFloat(), list[15].toFloat());
}


