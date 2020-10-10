#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Mortar"); //info for QSettings
    QCoreApplication::setOrganizationName("kwahoo");
    MainWindow w;
    w.show();
    return a.exec();
}
