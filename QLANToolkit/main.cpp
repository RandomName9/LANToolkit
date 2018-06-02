#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTime>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QTime t=QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);

    MainWindow w;

    w.show();

    return a.exec();
}
