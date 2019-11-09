#include <monitorwindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MonitorWindow w;
    w.show();

    return a.exec();
}
