#include "mainwindow.h"

#include <QApplication>
#include <qdebug.h>
#include <omp.h>
int main(int argc, char* argv[])
{
    qDebug() << "number of available processors: " << omp_get_num_procs();
    qDebug() << "number of threads: " << omp_get_max_threads();
    qDebug() << "Qt version:" << QT_VERSION_STR;

    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
