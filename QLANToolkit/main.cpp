#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTime>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    // qsrand(0);
     QTime t=QTime::currentTime();
     qsrand(t.msec()+t.second()*1000);

    MainWindow w;

    w.show();

    return a.exec();
}
