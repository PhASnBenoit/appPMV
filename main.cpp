//#include "cihm.h"
#include "capp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CApp app;
    return a.exec();
}
