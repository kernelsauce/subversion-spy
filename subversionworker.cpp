#include "subversionworker.h"

namespace Spy{

SubversionWorker::SubversionWorker(QString path) :
    parser(SubversionParserSyncro(path, false)),
    lastRevNumber(0)
{
}

SubversionWorker::~SubversionWorker()
{
}

void SubversionWorker::process()
{
    if (lastRevNumber == 0)
    {
        if (!parser.updatePath())
        {
            throw E_NO_CONNECTION;
        }
    }

    QVector<SubversionLog> fullLog = parser.getLogs();
    if (fullLog.isEmpty()) return; // Empty logs, just bail.
    SubversionLog lastLog = fullLog.back();
    lastRevNumber = lastLog.revNumber;

    // Sleep;

    while(1)
    {
        QVector<SubversionLog> freshLogs;
        try
        {
            freshLogs = parser.getLogs(lastRevNumber, NULL);
        }
        catch(ParserException e){
            qDebug() << "SubversionParser throw: " << e;
        }

        if (!freshLogs.isEmpty()){
            // New logs incoming!
            QVectorIterator<SubversionLog> it(freshLogs);
            while (it.hasNext())
            {
                SubversionLog freshLog = it.next();
                sendNotification(freshLog.comment, N_NEW_REVISON); // TODO just a sample.
            }
        }

    }
}

}
