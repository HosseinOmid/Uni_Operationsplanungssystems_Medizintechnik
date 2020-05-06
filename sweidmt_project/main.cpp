//#include "mainwindow.h"
#include "sweidmt.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SWEIDMT w;
    //ImageLoader w;
    w.show();
    return a.exec();
}
