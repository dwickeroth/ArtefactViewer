#include "avcontroller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AVController* p_controller = AVController::instance();
    (void*) p_controller;
    return a.exec();
}
