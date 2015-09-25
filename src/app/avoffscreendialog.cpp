#include "avoffscreendialog.h"
#include "ui_avoffscreendialog.h"

#include "QDebug"

enum presetEnum {LOW = 0, MEDIUM = 1, HIGH = 2};

AVOffscreenDialog::AVOffscreenDialog(int glWidgetWidth, int glWidgetheight, QWidget *parent) :
    m_glWidgetWidth(glWidgetWidth),
    m_glWidgetHeight(glWidgetheight),
    QDialog(parent),
    ui(new Ui::AVOffscreenDialog)
{
    ui->setupUi(this);

    /*
    QValidator *validator = new QIntValidator(1,10000,this);
    ui->lineEditHorizontalSize->setValidator(validator);
    ui->lineEditVerticalSize->setValidator(validator);
    */

    QObject::connect(ui->lineEditHorizontalSize, &QLineEdit::textEdited, this, &AVOffscreenDialog::onLineEditHorizonalTextEdited);
    QObject::connect(ui->lineEditVerticalSize,   &QLineEdit::textEdited, this, &AVOffscreenDialog::onLineEditVerticalTextEdited);

    QObject::connect(ui->pushButtonLow,    &QPushButton::clicked, this, &AVOffscreenDialog::onPushButtonLowPressed);
    QObject::connect(ui->pushButtonMedium, &QPushButton::clicked, this, &AVOffscreenDialog::onPushButtonMediumPressed);
    QObject::connect(ui->pushButtonHigh,   &QPushButton::clicked, this, &AVOffscreenDialog::onPushButtonHighPressed);

    setQualityPreset(MEDIUM);
}

void AVOffscreenDialog::getOptions(int *hSize, int *vSize, int *quality)
{
    *hSize = ui->lineEditHorizontalSize->text().toInt();
    *vSize = ui->lineEditVerticalSize->text().toInt();
    *quality = ui->spinBoxQuality->value();
}

AVOffscreenDialog::~AVOffscreenDialog()
{
    delete ui;
}

void AVOffscreenDialog::onLineEditHorizonalTextEdited(const QString &text)
{
    //qDebug() << "onLineEditHorizonalTextEdited: " << text;

    float width = text.toFloat();
    float height = width / (float)m_glWidgetWidth * (float)m_glWidgetHeight;
    ui->lineEditVerticalSize->setText(QString::number((int)height));
}

void AVOffscreenDialog::onLineEditVerticalTextEdited(const QString &text)
{
    //qDebug() << "onLineEditVerticalTextEdited: " << text;

    float height = text.toFloat();
    float width = height / (float)m_glWidgetHeight * (float)m_glWidgetWidth;
    ui->lineEditHorizontalSize->setText(QString::number((int)width));
}

void AVOffscreenDialog::onPushButtonLowPressed()
{
    setQualityPreset(LOW);
}

void AVOffscreenDialog::onPushButtonMediumPressed()
{
    setQualityPreset(MEDIUM);
}

void AVOffscreenDialog::onPushButtonHighPressed()
{
    setQualityPreset(HIGH);
}

void AVOffscreenDialog::setQualityPreset(int preset)
{
    int size = 0;
    int quality = 0;

    switch (preset)
    {
        case LOW :
            size = 2048;
            quality = 60;
            break;
        case MEDIUM :
            size = 4096;
            quality = 80;
            break;
        case HIGH :
            size = 8192;
            quality = 100;
            break;
        default:
            size = 2048;
            quality = 60;
            break;
    }

    if( m_glWidgetWidth >= m_glWidgetHeight)
    {
        ui->lineEditHorizontalSize->setText(QString::number(size));
        ui->lineEditVerticalSize->setText(QString::number((int)((float)size / (float)m_glWidgetWidth * (float)m_glWidgetHeight)));
        ui->spinBoxQuality->setValue(quality);
    }
    else
    {
        ui->lineEditVerticalSize->setText(QString::number(size));
        ui->lineEditHorizontalSize->setText(QString::number((int)((float)size / (float)m_glWidgetHeight * (float)m_glWidgetWidth)));
        ui->spinBoxQuality->setValue(quality);
    }

}
