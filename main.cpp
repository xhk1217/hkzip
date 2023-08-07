
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // qt中设计界面让大小和实际相同,网上查的
    if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
