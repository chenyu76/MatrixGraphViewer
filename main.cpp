#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    qDebug() << "Start";
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Matrix Graph Viewer"); // 设置窗口标题
    w.show();
    return a.exec();
}
