#include "mainwindow.h"
#include "global.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(":/img/ico.ico"));
    w.resize(WW, WH);
    w.setMinimumSize(WW*0.82, WH*0.82);
    w.show();
    return a.exec();
}
