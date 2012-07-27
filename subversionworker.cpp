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
    killMutex.lock();
    kill = true;
    killMutex.unlock();
    wait();
}

QString SubversionWorker::getWorkingPath()
{
    return path;
}

void SubversionWorker::handleNewLogs(QVector<SubversionLog>* freshLogs)
{
    if (!freshLogs->isEmpty())
    {
        svnLogs << *freshLogs;
        // New logs incoming!
        QVectorIterator<SubversionLog> it(*freshLogs);
        while (it.hasNext())
        {
            SubversionLog freshLog = it.next();
            emit giveUserFeedback(freshLog.comment, N_NEW_REVISON); // TODO just a sample.
        }
    }
}

bool SubversionWorker::initialLogFetch()
{
    QVector<SubversionLog> fullLog = parser.getLogs();
    if (fullLog.isEmpty()) return false; // Empty logs, just bail.
    svnLogs << fullLog; // Append to logs.

    SubversionLog lastLog = fullLog.front();
    lastRevNumber = lastLog.revNumber;

    // Update user about success.
    emit giveUserFeedback(
                "Full log for " + path + " fetched. \r\n\r\n"
                "Last revision number is r" + QString::number(lastRevNumber) + "\r\n" +
                "Author: " + lastLog.author + "\r\n" +
                "Comment: " + lastLog.comment + "\r\n",
                N_UPDATED_REPOSITORY);

    return true;
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

    initialLogFetch();

    /*
     * Main event loop of the Subversion Worker.
     */
    while(1)
    {
        QVector<SubversionLog> freshLogs;
        try
        {
            freshLogs = parser.getLogs(lastRevNumber + 1, -1);
        }
        catch(ParserException e){
            qDebug() << "SubversionParser throw: " << e;
        }

        handleNewLogs(&freshLogs);

        // If kill is set, then we let the thread suspend.
        killMutex.lock();
        if (!kill)
        {
            killMutex.unlock();
            sleep(10); // TODO: wait should be customizable.
        }
        else
        {
            killMutex.unlock();
            break;
        }

    }
}

}
