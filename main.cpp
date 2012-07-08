#include <QtGui/QApplication>
#include "subversionspy.h"
#include "subversionparser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Spy::SubversionSpy w;
    //w.show();
    Spy::SubversionParser parser = Spy::SubversionParser(QString("/home/jab/Dev/spamassassin"), false);
    parser.getLogs();
    
    a.quitOnLastWindowClosed();
    //a.exec();
    return EXIT_SUCCESS;
}
