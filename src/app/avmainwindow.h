#ifndef AVMAINWINDOW_H
#define AVMAINWINDOW_H

#include <QMainWindow>
#include <QVector3D>
#include <QMutex>

namespace Ui {
class AVMainWindow;
}
class AVGLWidget;
class AVModel;

class AVMainWindow : public QMainWindow
{
    Q_OBJECT

    struct vectorAngle {
        QVector3D vector;
        float angle;
    };

public:
    static AVMainWindow* instance()
    {
        static QMutex mutex;
        if(!m_instance)
        {
            mutex.lock();
            if(!m_instance)
                m_instance = new AVMainWindow;
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
    explicit AVMainWindow(QWidget *parent = 0);
    AVMainWindow(const AVMainWindow &); //hide copy constructor
    AVMainWindow& operator =(const AVMainWindow &); //hide assign op
    ~AVMainWindow();

public:
    void setGLWidget(AVGLWidget* glWidget);
    void initialize();
    void setCheckBoxes(bool use_lighting, bool use_vertexColors, bool paintAnnotations);
    void updateInfoGroup();

    int getCurrentLightIndex() const;
    void setCurrentLightIndex(int currentLightIndex);

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private:
    void updateParamSliders();
    void updateParamLineEdits();

public slots:
    void updatePushButtons();
    void updateLightComboBox();
    void currentAnnotationChanged();

private slots:
    void on_pushButton_up_clicked();
    void on_pushButton_down_clicked();
    void on_pushButton_right_clicked();
    void on_pushButton_left_clicked();
    void on_pushButton_center_clicked();
    void on_pushButton_clock_clicked();
    void on_pushButton_cclock_clicked();

    void on_lineEdit_editingFinished();

    void on_comboBox_currentIndexChanged(int index);

    void on_checkBox_lightingSwitch_toggled(bool checked);
    void on_checkBox_lightSwitch_toggled(bool checked);
    void on_checkBox_lightVisible_toggled(bool checked);
    void on_checkBox_vertexColors_toggled(bool checked);
    void on_checkBox_annotations_toggled(bool checked);

    void on_slider_lightIntensity_valueChanged(int value);
    void on_slider_lightDistance_valueChanged(int value);
    void on_slider_lightHRotation_valueChanged(int value);
    void on_slider_lightVRotation_valueChanged(int value);

    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionClose_triggered();
    void on_actionScreenshot_triggered();
    void on_actionOffscreen_triggered();
    void on_actionBackground_triggered();
    void on_actionArtefact_triggered();
    void on_actionUser_Manual_triggered();
    void on_actionAbout_triggered();

    void on_pushButton_note_clicked();
    void on_pushButton_reset_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_editAnnotation_clicked();
    void on_pushButton_rearrange_clicked();
    void on_pushButton_distance_clicked();
    void on_pushButton_angle_clicked();
    void on_pushButton_area_clicked();
    void on_pushButton_removeSelected_clicked();

    void on_comboBox_enhance_currentIndexChanged(int index);
    void on_horizontalSlider_param1_valueChanged(int value);
    void on_horizontalSlider_param2_valueChanged(int value);
    void on_horizontalSlider_param3_valueChanged(int value);

    void on_lineEdit_param1_editingFinished();
    void on_lineEdit_param2_editingFinished();


private:

    static AVMainWindow*    m_instance;

    AVGLWidget*         m_glWidget;
    AVModel*            m_model;
    Ui::AVMainWindow*   ui;

    int m_currentLightIndex;

    float getRotationAngleFromUI();
    void sortPointClouds();
    void sortVectorAngleList(QList<vectorAngle> &v);

};

#endif // AVMAINWINDOW_H
