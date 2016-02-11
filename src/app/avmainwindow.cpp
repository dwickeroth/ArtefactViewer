#include "avmainwindow.h"
#include "ui_avmainwindow.h"

#include "avabout.h"
#include "avmodel.h"
#include "avglwidget.h"
#include "avpluginmanager.h"
#include "avplugininterfaces.h"
#include "avcontroller.h"
#include "avoffscreendialog.h"
#include "avpqreader.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QtXml>
#include <QColorDialog>
#include <QDesktopServices>
#include <QInputDialog>

#include <iostream>
//TODO P: change this declaration to .h file
bool k_Ctrl=false;
AVMainWindow* AVMainWindow::m_instance = 0;

AVMainWindow::AVMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AVMainWindow)
{
    m_currentLightIndex = 0;
    ui->setupUi(this);
    /*
     * TODO!
    readPlyFile("../mesh/input_klein.ply");
    currentlyOpenFile = "input_klein";
     */
}

AVMainWindow::~AVMainWindow()
{
    delete ui;
}


void AVMainWindow::setGLWidget(AVGLWidget *glWidget)
{
    m_glWidget = glWidget;
    ui->verticalLayout_glWidget->addWidget(glWidget);

    QGLFormat format = m_glWidget->format();
    qDebug() << "found OpenGL Version " << format.majorVersion() << "." << format.minorVersion();
    qDebug() << "found flags: " << QGLFormat::openGLVersionFlags();

    connect(m_glWidget, SIGNAL(pointAdded()), this, SLOT(updatePushButtons()));
    connect(m_glWidget, SIGNAL(pointSelected()), this, SLOT(updatePushButtons()));
    connect(m_glWidget, SIGNAL(glWidgetInitialised()), this, SLOT(updateLightComboBox()));
    connect(m_glWidget, SIGNAL(annotationClicked()), this, SLOT(currentAnnotationChanged()));
    connect(m_glWidget, SIGNAL(annotationDoubleClicked()), this, SLOT(on_pushButton_editAnnotation_clicked()));

    initialize();
    m_glWidget->initialize();

}


//! Initializes the main window and its ui elements
void AVMainWindow::initialize()
{
    m_model = AVModel::instance();

    m_model->m_listOfPointClouds.clear();
    m_model->m_listOfPointClouds.append(pointCloud());
    m_model->m_listOfPointClouds.last().color = QColor(Qt::blue);
    m_model->m_listOfPointClouds.last().typeId = 0;
    m_glWidget->setSelectedPoint(-1);
    updatePushButtons();
    updateInfoGroup();
    ui->checkBox_lightingSwitch->setChecked(true);
    ui->checkBox_vertexColors->setChecked(false);
    ui->checkBox_annotations->setChecked(false);
    ui->comboBox_enhance->setCurrentIndex(0);
}


void AVMainWindow::setCheckBoxes(bool use_lighting, bool use_vertexColors, bool paintAnnotations)
{
    ui->checkBox_lightingSwitch->setChecked(use_lighting);
    ui->checkBox_vertexColors->setChecked(use_vertexColors);
    ui->checkBox_annotations->setChecked(paintAnnotations);
}


//! Handles some key shortcuts
void AVMainWindow::keyPressEvent(QKeyEvent *e)
{
    //TODO: Set Artefact Rotation
    if(e->key()==Qt::Key_Control) k_Ctrl=true;

    if (e->key() == Qt::Key_Escape)
        close();
    else if (e->key() == Qt::Key_Shift) m_glWidget->setShiftDown(true);
    else if (e->key() == Qt::Key_2 || e->key() == Qt::Key_Down)
    {
        on_pushButton_down_clicked();
    }
    else if (e->key() == Qt::Key_4 || e->key() == Qt::Key_Left)
    {
        on_pushButton_left_clicked();
    }
    else if (e->key() == Qt::Key_5)
    {
        on_pushButton_reset_clicked();
    }
    else if (e->key() == Qt::Key_6 || e->key() == Qt::Key_Right)
    {
        on_pushButton_right_clicked();
    }
    else if (e->key() == Qt::Key_7)
    {
        on_pushButton_cclock_clicked();
    }
    else if (e->key() == Qt::Key_8 || e->key() == Qt::Key_Up)
    {
        on_pushButton_up_clicked();
    }
    else if (e->key() == Qt::Key_9)
    {
        on_pushButton_clock_clicked();
    }
//keyboard shortcuts
    if(k_Ctrl&&e->key()==Qt::Key_F)
        on_actionFullscreen_triggered();
    if(k_Ctrl&&e->key()==Qt::Key_O)
        on_actionOpen_triggered();
    if(k_Ctrl&&e->key()==Qt::Key_S)
        on_actionSave_triggered();
    if(k_Ctrl&&e->key()==Qt::Key_P)
        on_actionScreenshot_triggered();
    if(k_Ctrl&&e->key()==Qt::Key_M)
      m_glWidget->setCursor(Qt::ArrowCursor);
    if(k_Ctrl&&e->key()==Qt::Key_H)
        m_glWidget->setCursor(Qt::BlankCursor);
}


//! Switches lateral movement mode off
void AVMainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift) m_glWidget->setShiftDown(false);
    if(e->key()==Qt::Key_Control) k_Ctrl=false;
}


//! Updates the information box that shows the amout of triangles, height, width and depth of the artefact
/*! Used when a file is opened and every time the artefact is rotated in either direction
 */
void AVMainWindow::updateInfoGroup()
{
    ui->label_infoTriangle_2->setText(QString::number(m_model->m_triangles.size()/3));
    ui->label_infoHeight_2->setText(QString::number(m_model->getArtefactHeight(m_glWidget->getMatrixArtefact())) + " mm");
    ui->label_infoWidth_2->setText(QString::number(m_model->getArtefactWidth(m_glWidget->getMatrixArtefact())) + " mm");
    ui->label_infoDepth_2->setText(QString::number(m_model->getArtefactDepth(m_glWidget->getMatrixArtefact())) + " mm");
}


//! Updates the sliders of the visual enhancements with the values in m_glWidget. Used when a file is loaded
void AVMainWindow::updateParamSliders()
{
    ui->horizontalSlider_param1->setValue(m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),0));
    ui->horizontalSlider_param2->setValue(m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),1));
    ui->horizontalSlider_param3->setValue(m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),2));
}


//! Updates the text inputs of the visual enhancements with the values in m_glWidget. Used when a file is loaded
void AVMainWindow::updateParamLineEdits()
{
    ui->lineEdit_param1->setText(QString::number(m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),0)));
    ui->lineEdit_param2->setText(QString::number(m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),1)));
    ui->lineEdit_param3->setText(QString::number(m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),2)));
}

//! Updates the enabled/disabled state of the pushbuttons of the annotations. Used mainly when the amount of set points changes
void AVMainWindow::updatePushButtons()
{
    ui->pushButton_note->setEnabled(false);
    ui->pushButton_distance->setEnabled(false);
    ui->pushButton_angle->setEnabled(false);
    ui->pushButton_area->setEnabled(false);
    ui->pushButton_reset->setEnabled(false);
    ui->pushButton_removeSelected->setEnabled(false);
    ui->pushButton_rearrange->setEnabled(false);

    if (m_model->m_listOfPointClouds.last().points.size() == 1) ui->pushButton_note->setEnabled(true);
    if (m_model->m_listOfPointClouds.last().points.size() > 1) ui->pushButton_distance->setEnabled(true);
    if (m_model->m_listOfPointClouds.last().points.size() == 3) ui->pushButton_angle->setEnabled(true);
    if (m_model->m_listOfPointClouds.last().points.size() > 2) ui->pushButton_area->setEnabled(true);
    if (m_model->m_listOfPointClouds.last().points.size() > 0) ui->pushButton_reset->setEnabled(true);
    if (m_glWidget->getDraggedPoint() != -1) ui->pushButton_removeSelected->setEnabled(true);
    if (m_model->m_listOfPointClouds.size() > 2) ui->pushButton_rearrange->setEnabled(true);
}


//! Synchronizes the ui with the variables in m_glWidget concerning the lights, used after a new file is loaded or a new light is selected in the combobox
void AVMainWindow::updateLightComboBox()
{
    qDebug() << "updateLightComboBox";

    ui->comboBox->setCurrentIndex(m_currentLightIndex);
    ui->checkBox_lightSwitch->setChecked(m_glWidget->m_lights[m_currentLightIndex].getIsOn());
    ui->checkBox_lightVisible->setChecked(m_glWidget->getLightsAreVisible());
    ui->slider_lightIntensity->setValue(m_glWidget->m_lights[m_currentLightIndex].getIntensity());
    ui->slider_lightDistance->setValue(m_glWidget->m_lights[m_currentLightIndex].getDistanceToOrigin());
    ui->slider_lightHRotation->setValue(m_glWidget->m_lights[m_currentLightIndex].getHRotation());
    ui->slider_lightVRotation->setValue(-m_glWidget->m_lights[m_currentLightIndex].getVRotation());
}


//! Enables the buttons for deleting and editing an annotation as soon as one is selected
void AVMainWindow::currentAnnotationChanged()
{
    ui->pushButton_delete->setEnabled(true);
    ui->pushButton_editAnnotation->setEnabled(true);
}


//! Makes the camera rotate upwards for the selected amount of degrees
void AVMainWindow::on_pushButton_up_clicked()
{
    float angle = getRotationAngleFromUI();

    QMatrix4x4 modelMatrix = m_glWidget->getMatrixArtefact();
    modelMatrix.translate(m_model->m_centerPoint);
    modelMatrix.rotate(-angle, QVector3D(1,0,0) * modelMatrix);
    modelMatrix.translate(-m_model->m_centerPoint);
    m_glWidget->setMatrixArtefact(modelMatrix);

    m_glWidget->updateGL();
    updateInfoGroup();
}


//! Makes the camera rotate downwards for the selected amount of degrees
void AVMainWindow::on_pushButton_down_clicked()
{
    float angle = getRotationAngleFromUI();

    QMatrix4x4 modelMatrix = m_glWidget->getMatrixArtefact();
    modelMatrix.translate(m_model->m_centerPoint);
    modelMatrix.rotate(angle, QVector3D(1,0,0) * modelMatrix);
    modelMatrix.translate(-m_model->m_centerPoint);
    m_glWidget->setMatrixArtefact(modelMatrix);

    m_glWidget->updateGL();
    updateInfoGroup();
}


//! Makes the camera rotate right for the selected amount of degrees
void AVMainWindow::on_pushButton_right_clicked()
{
    float angle = getRotationAngleFromUI();

    QMatrix4x4 modelMatrix = m_glWidget->getMatrixArtefact();
    modelMatrix.translate(m_model->m_centerPoint);
    modelMatrix.rotate(angle, QVector3D(0,1,0) * modelMatrix);
    modelMatrix.translate(-m_model->m_centerPoint);
    m_glWidget->setMatrixArtefact(modelMatrix);

    m_glWidget->updateGL();
    updateInfoGroup();
}


//! Makes the camera rotate left for the selected amount of degrees
void AVMainWindow::on_pushButton_left_clicked()
{
    float angle = getRotationAngleFromUI();

    QMatrix4x4 modelMatrix = m_glWidget->getMatrixArtefact();
    modelMatrix.translate(m_model->m_centerPoint);
    modelMatrix.rotate(-angle, QVector3D(0,1,0) * modelMatrix);
    modelMatrix.translate(-m_model->m_centerPoint);
    m_glWidget->setMatrixArtefact(modelMatrix);

    m_glWidget->updateGL();
    updateInfoGroup();
}


//! Offers the user a reset the rotation of camera and artefact
void AVMainWindow::on_pushButton_center_clicked()
{
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(-m_model->m_centerPoint);
    m_glWidget->setMatrixArtefact(modelMatrix);
    m_glWidget->setCamDistanceToOrigin(150);
    m_glWidget->setCamOrigin(QVector3D(0,0,0));
    m_glWidget->updateGL();
    updateInfoGroup();
}


//! Makes the artefact rotate clockwise for the selected amount of degrees
void AVMainWindow::on_pushButton_clock_clicked()
{
    float rotation;
    if (ui->radioButton->isChecked()) rotation = 1.0;
    if (ui->radioButton_2->isChecked()) rotation = 10.0;
    if (ui->radioButton_3->isChecked()) rotation = 45.0;
    if (ui->radioButton_4->isChecked()) rotation = 90.0;
    if (ui->radioButton_5->isChecked()) rotation = 180.0;
    if (ui->radioButton_6->isChecked()) rotation = ui->lineEdit->text().toDouble();

    QMatrix4x4 modelMatrix = m_glWidget->getMatrixArtefact();
    modelMatrix.translate(m_model->m_centerPoint);
    modelMatrix.rotate(-rotation, QVector3D(0,0,1) * modelMatrix);
    modelMatrix.translate(-m_model->m_centerPoint);
    m_glWidget->setMatrixArtefact(modelMatrix);

    m_glWidget->updateGL();
    updateInfoGroup();
}


//! Makes the artefact rotate counterclockwise for the selected amount of degrees
void AVMainWindow::on_pushButton_cclock_clicked()
{
    float rotation;
    if (ui->radioButton->isChecked()) rotation = 1.0;
    if (ui->radioButton_2->isChecked()) rotation = 10.0;
    if (ui->radioButton_3->isChecked()) rotation = 45.0;
    if (ui->radioButton_4->isChecked()) rotation = 90.0;
    if (ui->radioButton_5->isChecked()) rotation = 180.0;
    if (ui->radioButton_6->isChecked()) rotation = ui->lineEdit->text().toDouble();

    QMatrix4x4 modelMatrix = m_glWidget->getMatrixArtefact();
    modelMatrix.translate(m_model->m_centerPoint);
    modelMatrix.rotate(rotation, QVector3D(0,0,1) * modelMatrix);
    modelMatrix.translate(-m_model->m_centerPoint);
    m_glWidget->setMatrixArtefact(modelMatrix);

    m_glWidget->updateGL();
    updateInfoGroup();
}


//! Selects the "free angle" radio button as soon as the user inputs a value
void AVMainWindow::on_lineEdit_editingFinished()
{
    ui->radioButton_6->setChecked(true);
}


//! Adjusts the ui to the actual values of the new light as soon as it is selected
void AVMainWindow::on_comboBox_currentIndexChanged(int index)
{
    qDebug() << "on_comboBox_currentIndexChanged";

    m_currentLightIndex = index;
    updateLightComboBox();
}


//! If the lighting checkbox is clicked, the group for lights and the visibility checkbox is enabled. The variable in m_glWidget is updated
void AVMainWindow::on_checkBox_lightingSwitch_toggled(bool checked)
{
    qDebug() << "on_checkBox_lightingSwitch_toggled";

    ui->groupBox_lights->setEnabled(checked);
    ui->checkBox_lightVisible->setEnabled(checked);
    m_glWidget->setLighting(checked);

    updateLightComboBox();

    m_glWidget->updateGL();
}


//! If the isOn checkbox is (un)checked, the variable in m_glWidget is updated and the widget is redrawn
void AVMainWindow::on_checkBox_lightSwitch_toggled(bool checked)
{
    qDebug() << "on_checkBox_lightSwitch_toggled";

    m_glWidget->m_lights[m_currentLightIndex].setIsOn(checked);
    m_glWidget->updateGL();
}


//! If the visibility checkbox is (un)checked, the variable in m_glWidget is updated and the widget is redrawn
void AVMainWindow::on_checkBox_lightVisible_toggled(bool checked)
{
    m_glWidget->setLightsAreVisible(checked);
    m_glWidget->updateGL();
}


//! If the checkbox for vertex colors is clicked, the variable in m_glWidget is updated and the graphics hardware's buffer is filled with the appropriate colors
void AVMainWindow::on_checkBox_vertexColors_toggled(bool checked)
{
    std::cout << "AVMainWindow::on_checkBox_vertexColors_toggled(bool checked) " << checked << std::endl;

    m_glWidget->setVertexColors(checked);
    m_glWidget->fillBuffers();
    m_glWidget->updateGL();
}


//! If the checkbox annotations is clicked, the variable in m_glWidget is changed, that decides, whether the whole annotation system is activated and drawn or not
void AVMainWindow::on_checkBox_annotations_toggled(bool checked)
{
    qDebug() << "on_checkBox_annotations_toggled";
    m_glWidget->setPaintAnnotations(checked);
    m_glWidget->updateGL();
}


//! If the intensity slider value is changed, the variable in m_glWidget is updated and the widget is redrawn
void AVMainWindow::on_slider_lightIntensity_valueChanged(int value)
{
    m_glWidget->m_lights[m_currentLightIndex].setIntensity(value);
    m_glWidget->updateGL();
}


//! If the distance slider value is changed, the variable in m_glWidget is updated and the widget is redrawn
void AVMainWindow::on_slider_lightDistance_valueChanged(int value)
{
    m_glWidget->m_lights[m_currentLightIndex].setDistanceToOrigin((float)value);
    m_glWidget->updateGL();
}


//! If the horizontal rotation slider value is changed, the variable in m_glWidget is updated and the widget is redrawn
void AVMainWindow::on_slider_lightHRotation_valueChanged(int value)
{
    m_glWidget->m_lights[m_currentLightIndex].setHRotation((float)value);
    m_glWidget->updateGL();
}


//! If the vertical rotation slider value is changed, the variable in m_glWidget is updated and the widget is redrawn
void AVMainWindow::on_slider_lightVRotation_valueChanged(int value)
{
    m_glWidget->m_lights[m_currentLightIndex].setVRotation(-(float)value);
    m_glWidget->updateGL();
}


// *********** Menu Actions *************

//! Opens up a standard dialog to select a file and loads the file after the user has clicked ok
/*! Valid suffixes are PLY and STL, the location of the opened file is stored to use it for later storage of the XML file
 * in case the user wishes to save his settings. The mainwindow and m_glWidget are initialized and if a XML file already
 * exists, its content is loaded
 */
void AVMainWindow::on_actionOpen_triggered()
{

    QStringList filetypes = AVPluginManager::instance()->getReadableFileTypes();
    QString filterString("Mesh Files (");
    for(int i = 0; i < filetypes.size(); i++)
    {
        filterString.append("*.").append(filetypes.at(i));
        if(i < filetypes.size() - 1) filterString.append(" ");
    }
    filterString.append(")");
    QString fileString = QFileDialog::getOpenFileName(this, tr("Open file"), QString("."), filterString);

    if (!fileString.isEmpty())
    {
        AVController::instance()->readFile(fileString);
    }
}


//! Saves the current settings in an XML file with the same name as the currently open PLY/STL file
/*! If there already is an XML file with the current open file's name, the user is asked whether to overwrite it. If ok is clicked
 * the settings are saved.
 */
void AVMainWindow::on_actionSave_triggered()
{
    AVController::instance()->saveXmlFile();
}


//! Closes the application. Can also be done by pressing the ESC key
void AVMainWindow::on_actionClose_triggered()
{
    close();
}

//! toggles Fullscreen
void AVMainWindow::on_actionFullscreen_triggered()
{
    if(!m_instance->isFullScreen())
        m_instance->showFullScreen();
    else
        m_instance->showMaximized();
}


//! Asks the user for a location to store the screenshot and generates it by grabbing the framebuffer and writing it to disk
void AVMainWindow::on_actionScreenshot_triggered()
{
    QString fileString = QFileDialog::getSaveFileName(this, "Abbild speichern", "", "Bilddateien (*.jpg *.png *.bmp)");
    if (!fileString.isEmpty())
    {
        m_glWidget->updateGL();
        QImage screenshot(m_glWidget->grabFrameBuffer());
        QPixmap pixmap(QPixmap::fromImage(screenshot));
        pixmap.save(fileString);
    }
}

void AVMainWindow::on_actionOffscreen_triggered()
{
    AVOffscreenDialog offscreenDialog(m_glWidget->width(), m_glWidget->height());

    int hSize, vSize, quality;

    if(offscreenDialog.exec())
    {
        offscreenDialog.getOptions(&hSize, &vSize, &quality);
        qDebug() << hSize << " " << vSize << " " << quality << " ";

        QImage image = m_glWidget->renderToOffscreenBuffer(hSize,vSize);
        QString saveFilename = QFileDialog::getSaveFileName(0,"Save Rendering",QDir::homePath(),"*.png");
        if(!saveFilename.endsWith(".png",Qt::CaseInsensitive)) saveFilename.append(".png");
        image.save(saveFilename,"PNG",quality);
    }

}


//! Presents two color dialogs, one set the lower color and one to set the upper color. Both values are store in the m_glWidget that handles the drawing of the background
void AVMainWindow::on_actionBackground_triggered()
{
    QColor temp = QColorDialog::getColor(AVController::QVector3DToQColor(m_glWidget->getBackgroundColor1()), this, tr("Choose the color for the lower half of the viewport"));
    if (temp.isValid()) m_glWidget->setBackgroundColor1(QVector3D(temp.redF(), temp.greenF(), temp.blueF()));
    temp = QColorDialog::getColor(AVController::QVector3DToQColor(m_glWidget->getBackgroundColor2()), this, tr("Choose the color for the upper half of the viewport"));
    if (temp.isValid()) m_glWidget->setBackgroundColor2(QVector3D(temp.redF(), temp.greenF(), temp.blueF()));
    m_glWidget->updateGL();
}


//! Presents a color dialog and sets the variable in m_glWidget that handles the buffering and drawing
void AVMainWindow::on_actionArtefact_triggered()
{
    QColor temp = QColorDialog::getColor(AVController::QVector3DToQColor(m_model->m_flatColor));
    if (temp.isValid()) m_model->m_flatColor = QVector3D(temp.redF(), temp.greenF(), temp.blueF());
    m_glWidget->fillBuffers();
    m_glWidget->updateGL();
}


//! Shows the HTML-Page containing the user manual
//!
void AVMainWindow::on_actionUser_Manual_triggered()
{
    QFileInfo manualFileinfo("manual/usermanual.html");
    QUrl url = QUrl::fromLocalFile(manualFileinfo.canonicalFilePath());
    bool success = QDesktopServices::openUrl(url);
    if(!success) qDebug() << "Could not open file " << manualFileinfo.canonicalFilePath();
}


//! Opens up a little widget, offering the world information about the genius that wrote this fine piece of software
void AVMainWindow::on_actionAbout_triggered()
{
    AVAbout about;
    about.setWindowTitle(tr("About"));
    about.exec();
}


//! Asks the user for a annotation text, lets him choose a color and generates a note from the current point cloud (and makes a new one)
void AVMainWindow::on_pushButton_note_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Annotation"), tr("Please enter annotation text"), QLineEdit::Normal, "", &ok);
    if (ok && !(text == ""))
    {
        QColor color = QColorDialog::getColor();
        if (color.isValid())
        {
            m_model->m_listOfPointClouds.last().color = color;
            m_model->m_listOfPointClouds.last().typeId = 0;
            m_model->m_listOfPointClouds.last().text = text;
            m_model->m_listOfPointClouds.append(pointCloud());
            m_model->m_listOfPointClouds.last().color = QColor(0, 0, 255);
            m_model->m_listOfPointClouds.last().typeId = 0;
            m_glWidget->setSelectedPoint(-1);
            if (m_model->m_listOfPointClouds.size() > 2) ui->pushButton_rearrange->setEnabled(true);
            updatePushButtons();
            m_glWidget->updateGL();
        }
    }
}


//! Deletes all points the user has currently defined
void AVMainWindow::on_pushButton_reset_clicked()
{
    m_model->m_listOfPointClouds.last().points.clear();
    m_glWidget->setSelectedPoint(-1);
    updatePushButtons();
    m_glWidget->updateGL();
}


//! Deletes the current annotation without further hassle
void AVMainWindow::on_pushButton_delete_clicked()
{
    if (m_glWidget->getCurrentAnnotation() != -1) m_model->m_listOfPointClouds.removeAt(m_glWidget->getCurrentAnnotation());
    m_glWidget->setCurrentAnnotation(-1);
    ui->pushButton_delete->setEnabled(false);
    ui->pushButton_editAnnotation->setEnabled(false);
    updatePushButtons();
    m_glWidget->updateGL();
}


//! Pops up a widget with the existing annotation text and lets the user edit it and choose a new color
void AVMainWindow::on_pushButton_editAnnotation_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Annotation"), tr("Please enter annotation text"), QLineEdit::Normal, m_model->m_listOfPointClouds[m_glWidget->getCurrentAnnotation()].text, &ok);
    if (ok && !(text == ""))
    {
        QColor color = QColorDialog::getColor(m_model->m_listOfPointClouds[m_glWidget->getCurrentAnnotation()].color);
        if (color.isValid()) m_model->m_listOfPointClouds[m_glWidget->getCurrentAnnotation()].color = color;
        m_model->m_listOfPointClouds[m_glWidget->getCurrentAnnotation()].text = text;
        updatePushButtons();
        m_glWidget->updateGL();
    }
    m_glWidget->updateGL();
}


//! Rearrages the order of the annotations to reduce crossing lines
void AVMainWindow::on_pushButton_rearrange_clicked()
{
    sortPointClouds();
    m_glWidget->setCurrentAnnotation(-1);
    ui->pushButton_delete->setEnabled(false);
    ui->pushButton_editAnnotation->setEnabled(false);
    m_glWidget->updateGL();
}


//! Calculates the lengths between all points and offers to add an annotation with prewritten text
void AVMainWindow::on_pushButton_distance_clicked()
{
    QVector3D vector;
    float length = 0;
    for (int i=0; i < m_model->m_listOfPointClouds.last().points.size()-1; i++)
    {
         vector = m_model->m_listOfPointClouds.last().points[i] - m_model->m_listOfPointClouds.last().points[i+1];
         length += vector.length();
    }
    QMessageBox::StandardButton infoQuestion = QMessageBox::information(this, tr("Measured length"), tr("Length: ").append(QString::number(length, 'g', 4)).append(tr(" mm. Generate an annotation for this?")), QMessageBox::Yes | QMessageBox::No);
    if (infoQuestion == QMessageBox::No) return;
    else
    {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Annotation"), tr("Please enter annotation text"), QLineEdit::Normal, tr("Length: ").append(QString::number(length, 'g', 4)).append(tr(" mm")), &ok);
        if (ok && !(text == ""))
        {
            QColor color = QColorDialog::getColor();
            if (color.isValid())
            {
                m_model->m_listOfPointClouds.last().color = color;
                m_model->m_listOfPointClouds.last().typeId = 1;
                m_model->m_listOfPointClouds.last().text = text;
                m_model->m_listOfPointClouds.append(pointCloud());
                m_model->m_listOfPointClouds.last().color = QColor(0, 0, 255);
                m_model->m_listOfPointClouds.last().typeId = 0;
                m_glWidget->setSelectedPoint(-1);
                updatePushButtons();
                m_glWidget->updateGL();
            }
        }
    }
}


//! Calculates the angle between the three points A, B and C at the Point A, generates some geometry for eye candy and offers to add an annotation
void AVMainWindow::on_pushButton_angle_clicked()
{
    QVector3D vector1 = m_model->m_listOfPointClouds.last().points[1] - m_model->m_listOfPointClouds.last().points[0];
    QVector3D vector2 = m_model->m_listOfPointClouds.last().points[2] - m_model->m_listOfPointClouds.last().points[0];
    vector1.normalize();
    vector2.normalize();
    float angle = acos(QVector3D::dotProduct(vector1,vector2))*180.0/3.14159265;


    QMessageBox::StandardButton infoQuestion = QMessageBox::information(this, tr("Measured angle"), tr("Angle: ").append(QString::number(angle, 'g', 4)).append(tr(" degrees. Generate an annotation for this?")), QMessageBox::Yes | QMessageBox::No);
    if (infoQuestion == QMessageBox::No) return;
    else
    {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Annotation"), tr("Please enter annotation text"), QLineEdit::Normal, tr("Angle: ").append(QString::number(angle, 'g', 4)).append(tr(" degrees")), &ok);
        if (ok && !(text == ""))
        {
            QColor color = QColorDialog::getColor();
            if (color.isValid())
            {
                m_model->m_listOfPointClouds.last().color = color;
                m_model->m_listOfPointClouds.last().typeId = 2;
                m_model->m_listOfPointClouds.last().text = text;

                // Add a little arc for the angle indicator
                QVector3D firstVector  = (m_model->m_listOfPointClouds.last().points[1] - m_model->m_listOfPointClouds.last().points[0]);
                QVector3D fifthVector  = (m_model->m_listOfPointClouds.last().points[2] - m_model->m_listOfPointClouds.last().points[0]);
                float length;
                if (firstVector.length() < fifthVector.length()) length = firstVector.length() / 4;
                else length = fifthVector.length() / 4;

                firstVector.normalize();
                firstVector *= length;
                fifthVector.normalize();
                fifthVector *= length;

                QVector3D thirdVector  = (firstVector + fifthVector);
                thirdVector.normalize();
                thirdVector *= length;

                QVector3D secondVector = (2 * firstVector + fifthVector);
                secondVector.normalize();
                secondVector *= length;

                QVector3D fourthVector = (firstVector + 2 * fifthVector);
                fourthVector.normalize();
                fourthVector *= length;

                m_model->m_listOfPointClouds.last().points.append(m_model->m_listOfPointClouds.last().points[0] + firstVector);
                m_model->m_listOfPointClouds.last().points.append(m_model->m_listOfPointClouds.last().points[0] + secondVector);
                m_model->m_listOfPointClouds.last().points.append(m_model->m_listOfPointClouds.last().points[0] + thirdVector);
                m_model->m_listOfPointClouds.last().points.append(m_model->m_listOfPointClouds.last().points[0] + fourthVector);
                m_model->m_listOfPointClouds.last().points.append(m_model->m_listOfPointClouds.last().points[0] + fifthVector);

                // Save everthing and generate a new empty pointCloud
                m_model->m_listOfPointClouds.append(pointCloud());
                m_model->m_listOfPointClouds.last().color = QColor(0, 0, 255);
                m_model->m_listOfPointClouds.last().typeId = 0;
                m_glWidget->setSelectedPoint(-1);
                updatePushButtons();
                m_glWidget->updateGL();
            }
        }
    }
}


//! Calculates the area between several points
/*! This is done by first calculationg a center points as an average of all user provided points then selecting
 * one of them (number 1 in this case but anyone would do) and sorting all other points regarding their angle to
 * that reference point. That way the resulting shape will be convex in most cases. Errors may occur when the
 * area to be measured is very bumpy. After sorting the points a triangle fan is defined and its area is presented
 * in a widget that offers to add an annotation.
 */
void AVMainWindow::on_pushButton_area_clicked()
{
    // Calculate middle
    QVector3D center = QVector3D(0,0,0);
    for (int i = 0; i < m_model->m_listOfPointClouds.last().points.size(); i++) center += m_model->m_listOfPointClouds.last().points[i];
    center /= m_model->m_listOfPointClouds.last().points.size();
    m_model->m_listOfPointClouds.last().points.push_front(center);

    QVector3D one = QVector3D(0,0,0);
    QVector3D two = QVector3D(0,0,0);
    QVector3D upNormal = QVector3D(0,0,0);
    QMatrix4x4 plusOneBit;
    plusOneBit.setToIdentity();

    float hypotenuse = 0;
    float g = 0;
    float angle = 0;
    float h = 0;
    float area = 0;
    float pi = 3.14159265f;

    QList<vectorAngle> vectorAngleList;
    // Set up first vector as 0 angle
    one = m_model->m_listOfPointClouds.last().points[1] - center;
    one.normalize();
    vectorAngle temp;
    temp.vector = m_model->m_listOfPointClouds.last().points[1];
    temp.angle = 0.0f;
    vectorAngleList.push_back(temp);

    // Find an approximation plane
    two = m_model->m_listOfPointClouds.last().points[2] - center;
    two.normalize();
    upNormal = QVector3D::crossProduct(one, two);
    plusOneBit.rotate(0.01f, upNormal);

    for (int i = 2; i < m_model->m_listOfPointClouds.last().points.size(); i++)
    {
        vectorAngle temp;
        temp.vector = m_model->m_listOfPointClouds.last().points[i];

        two = m_model->m_listOfPointClouds.last().points[i] - center;
        two.normalize();

        float angle = QVector3D::dotProduct(one, two);
        float referenceAngle = QVector3D::dotProduct(one, plusOneBit * two);
        if (referenceAngle > angle) temp.angle = acos(QVector3D::dotProduct(one, two));
        else temp.angle = 2*pi - acos(QVector3D::dotProduct(one, two));

        vectorAngleList.push_back(temp);
    }

    // Actual sorting
    sortVectorAngleList(vectorAngleList);

    // Writing back
    m_model->m_listOfPointClouds.last().points.clear();
    m_model->m_listOfPointClouds.last().points.push_back(center);
    for (int i = 0; i < vectorAngleList.size(); i++)
    {
        m_model->m_listOfPointClouds.last().points.push_back(vectorAngleList[i].vector);
    }
    m_model->m_listOfPointClouds.last().points.push_back(m_model->m_listOfPointClouds.last().points[1]);

    // Calculate area
        for (int i = 0; i < m_model->m_listOfPointClouds.last().points.size() - 2; i++)
        {
            one = center - m_model->m_listOfPointClouds.last().points[i+1];
            two = center - m_model->m_listOfPointClouds.last().points[i+2];
            hypotenuse = one.length();
            g = two.length();
            one.normalize();
            two.normalize();
            angle = acos(QVector3D::dotProduct(one,two));
            h = sin(angle) * hypotenuse;
            area += (h * g / 200);
        }

    QMessageBox::StandardButton infoQuestion = QMessageBox::information(this,
                                               tr("Measured area"),
                                               tr("Area: ").append(QString::number(area, 'g', 4)).append(tr(" square centimeters. Generate an annotation for this?")),
                                               QMessageBox::Yes | QMessageBox::No);

    if (infoQuestion == QMessageBox::Yes)
    {
        bool ok;
        QString text = QInputDialog::getText(this,
                                             tr("Annotation"),
                                             tr("Please enter annotation text"),
                                             QLineEdit::Normal,
                                             tr("Area: ").append(QString::number(area, 'g', 4)).append(tr(" square centimeters")),
                                             &ok);

        if (ok && !(text == ""))
        {
            QColor color = QColorDialog::getColor();
            if (color.isValid())
            {
                m_model->m_listOfPointClouds.last().color = color;
                m_model->m_listOfPointClouds.last().typeId = 3;
                m_model->m_listOfPointClouds.last().text = text;
                m_model->m_listOfPointClouds.append(pointCloud());
                m_model->m_listOfPointClouds.last().color = QColor(0, 0, 255);
                m_model->m_listOfPointClouds.last().typeId = 0;
                m_glWidget->setSelectedPoint(-1);
                updatePushButtons();
                m_glWidget->updateGL();
            }
            else m_model->m_listOfPointClouds.last().points.pop_front();
        }
        else m_model->m_listOfPointClouds.last().points.pop_front();
    }
    else m_model->m_listOfPointClouds.last().points.pop_front();
}


//! Deletes the selected (dragged) point
void AVMainWindow::on_pushButton_removeSelected_clicked()
{
    m_model->m_listOfPointClouds.last().points.remove(m_glWidget->getSelectedPoint());
    m_glWidget->setDraggedPoint(-1);
    m_glWidget->setSelectedPoint(-1);
    updatePushButtons();
    m_glWidget->updateGL();
}


//! Calculates a new shadow map as soon as the user selects a different algorithm in the enhancement list
void AVMainWindow::on_comboBox_enhance_currentIndexChanged(int index)
{
    qDebug() << "AVMainWindow::on_comboBox_enhance_currentIndexChanged. index : " << index;

    m_glWidget->setEnhancement(index);
    m_model->calculateShadowMap(m_glWidget->getEnhancement(),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),0),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),1),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),2));
    updateParamSliders();
    updateParamLineEdits();
    m_glWidget->fillBuffers();
    m_glWidget->updateGL();
}


//! If the slider for the first parameter of the visual enhancements is changed, the variable is m_glWidget is changed and a new shadowmap is calculated
void AVMainWindow::on_horizontalSlider_param1_valueChanged(int value)
{
    qDebug() << "AVMainWindow::on_horizontalSlider_param1_valueChanged. value: " << value;

    m_glWidget->setEnhancementParam(m_glWidget->getEnhancement(), 0, value);
    m_model->calculateShadowMap(m_glWidget->getEnhancement(),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),0),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),1),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),2));

    updateParamLineEdits();
    m_glWidget->fillBuffers();
    m_glWidget->updateGL();
}


//! If the slider for the second parameter of the visual enhancements is changed, the variable is m_glWidget is changed and a new shadowmap is calculated
void AVMainWindow::on_horizontalSlider_param2_valueChanged(int value)
{
    qDebug() << "AVMainWindow::on_horizontalSlider_param2_valueChanged. value: " << value;

    m_glWidget->setEnhancementParam(m_glWidget->getEnhancement(), 1, value);

    m_model->calculateShadowMap(m_glWidget->getEnhancement(),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),0),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),1),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),2));

    updateParamLineEdits();
    m_glWidget->fillBuffers();
    m_glWidget->updateGL();
}

void AVMainWindow::on_horizontalSlider_param3_valueChanged(int value)
{
    qDebug() << "AVMainWindow::on_horizontalSlider_param3_valueChanged. value: " << value;
    m_glWidget->setEnhancementParam(m_glWidget->getEnhancement(), 2, value);
    m_model->calculateShadowMap(m_glWidget->getEnhancement(),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),0),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),1),
                                m_glWidget->getEnhancementParam(m_glWidget->getEnhancement(),2));

    updateParamLineEdits();
    m_glWidget->fillBuffers();
    m_glWidget->updateGL();
}


//! In case manual input for the first parameter of the visual enhancements is given, the variable is m_glWidget is changed and a new shadowmap is calculated
/*! Only values from 0 to 99 are allowed and the sliders are synchronized
 */
void AVMainWindow::on_lineEdit_param1_editingFinished()
{
    if (ui->lineEdit_param1->text().toInt() < 0)
    {
        ui->lineEdit_param1->setText("0");
    }
    else if (ui->lineEdit_param1->text().toInt() > 99)
    {
        ui->lineEdit_param1->setText("99");
    }

    m_glWidget->setEnhancementParam(m_glWidget->getEnhancement(), 0, ui->lineEdit_param1->text().toInt());
    updateParamSliders();
}


//! In case manual input for the second parameter of the visual enhancements is given, the variable is m_glWidget is changed and a new shadowmap is calculated
/*! Only values from 0 to 10 are allowed and the sliders are synchronized
 */
void AVMainWindow::on_lineEdit_param2_editingFinished()
{
    if (ui->lineEdit_param2->text().toInt() < 0)
    {
        ui->lineEdit_param2->setText("0");
    }
    else if (ui->lineEdit_param2->text().toInt() > 10)
    {
        ui->lineEdit_param2->setText("10");
    }

    m_glWidget->setEnhancementParam(m_glWidget->getEnhancement(), 1, ui->lineEdit_param2->text().toInt());
    updateParamSliders();
}

int AVMainWindow::getCurrentLightIndex() const
{
    return m_currentLightIndex;
}
void AVMainWindow::setCurrentLightIndex(int currentLightIndex)
{
    m_currentLightIndex = currentLightIndex;
}



float AVMainWindow::getRotationAngleFromUI()
{
    float rotation = 0.0;

    if (ui->radioButton->isChecked()) rotation = 1.0;
    else if (ui->radioButton_2->isChecked()) rotation = 10.0;
    else if (ui->radioButton_3->isChecked()) rotation = 45.0;
    else if (ui->radioButton_4->isChecked()) rotation = 90.0;
    else if (ui->radioButton_5->isChecked()) rotation = 180.0;
    else if (ui->radioButton_6->isChecked()) rotation = ui->lineEdit->text().toDouble();

    return rotation;
}


// ************* Helper Functions *************
//! Try to eliminate crossing lines of annotations by calculating the height of the endpoints on the screen and sorting the annotations accordingly
void AVMainWindow::sortPointClouds()
{
    QMatrix4x4 mvpMatrix = m_glWidget->getMvpMatrix();
//    QMatrix4x4 LeftMvpMatrix = m_glWidget->getMvpMatrix();
//    QMatrix4x4 RightvpMatrix = m_glWidget->getMvpMatrix();
    for (int i=0; i < m_model->m_listOfPointClouds.size()-1; i++)
    {
        for (int j=i+1; j < m_model->m_listOfPointClouds.size()-1; j++)
        {
            if ((mvpMatrix * m_model->m_listOfPointClouds[i].points.first()).y() < (mvpMatrix * m_model->m_listOfPointClouds[j].points.first()).y())
                m_model->m_listOfPointClouds.swap(i,j);
//            if ((LeftMvpMatrix * m_model->m_listOfPointClouds[i].points.first()).y() < (LeftMvpMatrix * m_model->m_listOfPointClouds[j].points.first()).y())
//                m_model->m_listOfPointClouds.swap(i,j);
//            if ((RightMvpMatrix * m_model->m_listOfPointClouds[i].points.first()).y() < (RightMvpMatrix * m_model->m_listOfPointClouds[j].points.first()).y())
//                m_model->m_listOfPointClouds.swap(i,j);
        }
    }
}


//! Used to sort the points forming an area in a way that they form a concave form and are orderes circular around the center point
void AVMainWindow::sortVectorAngleList(QList<vectorAngle> &v)
{
    for (int i=0; i < v.size(); i++)
    {
        for (int j=i+1; j < v.size(); j++)
        {
            if (v[i].angle > v[j].angle)
                v.swap(i,j);
        }
    }
}





