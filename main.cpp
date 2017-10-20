#include "parlamenttimer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ParlamentTimer w;
    w.show();

    return a.exec();
}
