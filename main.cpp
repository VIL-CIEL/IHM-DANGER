#include "ihmdanger.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IHMDanger w;
    w.show();
    return a.exec();
}
