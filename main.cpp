#include "resultanalisator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ResultAnalisator w;
    w.show();
    return a.exec();
}
