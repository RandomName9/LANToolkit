#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTime>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));


    MainWindow w;

    w.show();

    return a.exec();
}
