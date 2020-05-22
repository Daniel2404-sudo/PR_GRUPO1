#include "proyecto_1.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    proyecto_1 w;
    w.show();
    return a.exec();
}
