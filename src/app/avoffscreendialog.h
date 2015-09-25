#ifndef AVOFFSCREENDIALOG_H
#define AVOFFSCREENDIALOG_H

#include <QDialog>

namespace Ui {
class AVOffscreenDialog;
}

class AVOffscreenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AVOffscreenDialog(int glWidgetWidth, int glWidgetHeight, QWidget *parent = 0);
    void getOptions(int* hSize, int* vSize, int* quality);
    ~AVOffscreenDialog();

private slots:
    void onLineEditHorizonalTextEdited(const QString &text);
    void onLineEditVerticalTextEdited(const QString &text);

    void onPushButtonLowPressed();
    void onPushButtonMediumPressed();
    void onPushButtonHighPressed();

private:
    Ui::AVOffscreenDialog *ui;
    int m_glWidgetWidth;
    int m_glWidgetHeight;

    void setQualityPreset(int preset);
};

#endif // AVOFFSCREENDIALOG_H
