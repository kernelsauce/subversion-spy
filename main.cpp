#include <QtGui/QApplication>
#include "subversionspy.h"
#include "subversionparser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    Spy::SubversionSpy w;

    return a.exec();
}


