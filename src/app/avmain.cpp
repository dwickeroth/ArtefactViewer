#include "avcontroller.h"
#include "avpqreader.h"
#include "iostream"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AVController* p_controller = AVController::instance();
    (void*) p_controller;
    AVPQReader sample;
    int err_code=sample.Init();
    if(err_code != PQMTE_SUCCESS){
        std::cout << "no success" << std::endl;
    }
    return a.exec();
}
