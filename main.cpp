#include <QtGui/QApplication>
#include "subversionspy.h"
#include "subversionparser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    Spy::SubversionSpy w;



//    Spy::SubversionParser parser = Spy::SubversionParser(QString("/home/jab/Dev/spamassassin"), false);
//    QVector<Spy::SubversionLog> logs = parser.getLogs();

//    while (!logs.isEmpty())
//    {
//        Spy::SubversionLog log = logs.back();
//        qDebug() << "Author: " << log.author;
//        qDebug() << "Comments: " << log.comment;
//        qDebug() << "Date: " << log.date;
//        qDebug() << "Rev: " << log.revNumber;

//        while (!log.files.isEmpty())
//        {
//            qDebug() << "File: " << log.files.back();
//            log.files.pop_back();
//        }

//        logs.pop_back();

//        qDebug() << "\n";
//    }

    return a.exec();
}


