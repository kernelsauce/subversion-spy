#include "subversionworker.h"

namespace Spy{

SubversionWorker::SubversionWorker(QString path) :
    parser(SubversionParserSyncro(path, false)),
    lastRevNumber(0),
    path(path),
    kill(false)
{
}

SubversionWorker::~SubversionWorker()
{
    kill = true;
    wait();
}

QString SubversionWorker::getWorkingPath()
{
    return path;
}

void SubversionWorker::run()
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
    SubversionLog lastLog = fullLog.front();
    lastRevNumber = lastLog.revNumber;

    // Update user about success.
    emit giveUserFeedback(
                "Full log for " + path + " fetched. \r\n\r\n"
                "Last revision number is r" + QString::number(lastRevNumber) + "\r\n" +
                "Author: " + lastLog.author + "\r\n" +
                "Comment: " + lastLog.comment + "\r\n",
                N_UPDATED_REPOSITORY);

    while(!kill)
    {
        QVector<SubversionLog> freshLogs;
        try
        {
            freshLogs = parser.getLogs(lastRevNumber + 1, -1);
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
                emit giveUserFeedback(freshLog.comment, N_NEW_REVISON); // TODO just a sample.
            }
        }

        if (!kill) sleep(10);

    }
}

}
