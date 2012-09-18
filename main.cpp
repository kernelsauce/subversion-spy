#include <QtGui/QApplication>
#include "subversionspy.h"
#include "subversionparser.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    Spy::SubversionSpy w;

    return app.exec();
}


