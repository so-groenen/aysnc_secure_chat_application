#include "mainwindow.h"
#include <QObject>
#include <QApplication>
#include <QStringView>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow main_window{};
    main_window.show();
    return a.exec();
}
