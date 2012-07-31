#include "subversionworker.h"

namespace Spy{

SubversionWorker::SubversionWorker(QString path,
                                   uint32_t *pollRate,
                                   QMutex* pollRateMutex) :
    parser(SubversionParserSyncro(path, false)),
    lastRevNumber(0),
    path(path),
    pollRate(pollRate),
    kill(false)
{
    this->pollRateMutex = pollRateMutex;
    setState(S_WAIT);
}

SubversionWorker::~SubversionWorker()
{
    setState(S_DIEING);
    killMutex.lock();
    kill = true;
    killMutex.unlock();
    wait();
}

QString SubversionWorker::getWorkingPath()
{
    return path;
}

SubversionWorkerState SubversionWorker::getState()
{
    SubversionWorkerState returnState;
    stateMutex.lock();
    returnState = state;
    stateMutex.unlock();
    return returnState;
}

uint32_t SubversionWorker::getThreadId()
{
    return threadId;
}

SVNLogVector* SubversionWorker::getLogs(QMutex** vectorMutex)
{
    *vectorMutex = &svnLogsMutex;
    return &svnLogs;
}

void SubversionWorker::setState(SubversionWorkerState state)
{
    stateMutex.lock();
    this->state = state;
    stateMutex.unlock();
}

void SubversionWorker::handleNewLogs(SVNLogVector* freshLogs)
{
    setState(S_UPDATING);
    if (!freshLogs->isEmpty())
    {
        svnLogsMutex.lock();
        svnLogs << *freshLogs;
        svnLogsMutex.unlock();

        // New logs incoming!
        SVNLogIterator it(*freshLogs);
        while (it.hasNext())
        {
            SubversionLog freshLog = it.next();
            lastRevNumber = freshLog.revNumber;
            emit giveUserFeedback(
                        "Caught check in at " + path + ". \r\n\r\n"
                        "Revision number: r" + QString::number(lastRevNumber) + "\r\n" +
                        "Author: " + freshLog.author + "\r\n" +
                        "Comment: " + freshLog.comment + "\r\n",
                        N_UPDATED_REPOSITORY);
        }
    }
}

bool SubversionWorker::initialLogFetch()
{
    setState(S_UPDATING);
    SVNLogVector fullLog = parser.getLogs();
    if (fullLog.isEmpty()) return false; // Empty logs, just bail.

    svnLogsMutex.lock();
    svnLogs << fullLog; // Append to logs.
    svnLogsMutex.unlock();

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
    threadId = (uint32_t)this->currentThreadId(); // TODO: surivive without mutex?

    if (lastRevNumber == 0)
    {
        if (!parser.updatePath())
        {
            setState(S_ERROR);
            emit giveUserFeedback("Could not update: " + path, N_NOT_SVN_DIR);
            return;
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
            setState(S_ERROR);
            emit giveUserFeedback("Exception: " + QString::number(e) +
                                  " while parsing: " + path, N_PARSE_PROBLEMS);
            break;
        }

        handleNewLogs(&freshLogs);

        // If kill is set, then we let the thread suspend.
        killMutex.lock();
        if (!kill)
        {
            killMutex.unlock();
            setState(S_WAIT);
            pollRateMutex->lock();
            uint32_t currentPollRate = *pollRate;
            pollRateMutex->unlock();
            sleep(currentPollRate);
        }
        else
        {
            killMutex.unlock();
            break;
        }

    }
}

}
