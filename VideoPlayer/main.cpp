#include "mainwindow.h"
#include "setdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    SetDialog dlg;
    w.show();

    QObject::connect(&w,SIGNAL(setdlgshow()),&dlg,SLOT(SetDlgShow()));
    QObject::connect(&dlg,SIGNAL(speedDown()),&w,SLOT(SpeedDown()));
    QObject::connect(&dlg,SIGNAL(speedUp()),&w,SLOT(SpeedUp()));
    QObject::connect(&dlg,SIGNAL(speedNormal()),&w,SLOT(SpeedNormal()));


    return a.exec();
}
