#include "threadmonitor.h"

namespace Spy{

ThreadMonitor::ThreadMonitor(QVector<QString> *listenerPaths,
                             QMutex *listenerPathsMutex,
                             uint32_t* pollRate, QMutex *pollRateMutex,
                             QObject *parent) :
    QThread(parent),
    listenerPaths(listenerPaths),
    listenerPathsMutex(listenerPathsMutex),
    kill(false),
    pollRate(pollRate),
    pollRateMutex(pollRateMutex)
{
}

ThreadMonitor::~ThreadMonitor()
{
    qDebug() << "Killing ThreadMonitor.";
    killMutex.lock();
    kill = true;
    killMutex.unlock();
    wait();

    qDeleteAll(workerPool);
    workerPool.empty();
}

void ThreadMonitor::run()
{
    while(1){
        killMutex.unlock();
        if (kill){
            killMutex.unlock();
            break;
        }
        killMutex.unlock();

        listenerPathsMutex->lock();
        QVectorIterator<QString> listenerPathIt(*listenerPaths);

        // Spawn a new thread for each path that is not already there.
        while (listenerPathIt.hasNext()){
            QString listenerPath = listenerPathIt.next();
            if (findWorkerByWork(listenerPath) == 0){
                propagateNotifications("Added " + listenerPath + " to listener.", N_ADDED_REPOSITORY);
                SubversionWorker* worker = new SubversionWorker(listenerPath, pollRate, pollRateMutex);
                workerPool.append(worker);
                worker->start();
                connect(worker, SIGNAL(giveUserFeedback(QString,SpyNotifications)), this, SLOT(propagateNotifications(QString,SpyNotifications)));
            }
        }

        // Stop workers that no longer is in list.
        QVectorIterator<SubversionWorker*> workerIt(workerPool);
        int32_t index = 0;
        while (workerIt.hasNext())
        {
            SubversionWorker* workerInPool = workerIt.next();
            if (!inListenerPaths(workerInPool->getWorkingPath()))
            {
                if (workerPool.size() > index) // Check if in range.
                {
                    listenerPathsMutex->unlock();
                    workerPool.remove(index);
                    delete workerInPool;
                    listenerPathsMutex->lock();
                }
                else {
                    break;
                }
            }
            index++;
        }

        listenerPathsMutex->unlock();

        sleep(THREAD_MONITOR_ITERATION);
    }
}

void ThreadMonitor::propagateNotifications(QString message, SpyNotifications type)
{
    emit sendNotifications(message, type);
}

SubversionWorker* ThreadMonitor::findWorkerByWork(QString listenerPath)
{
    QVectorIterator<SubversionWorker*> it(workerPool);
    while (it.hasNext())
    {
        SubversionWorker* worker = it.next();
        if (worker->getWorkingPath() == listenerPath)
        {
            return worker;
        }
    }
    return 0;
}

inline bool ThreadMonitor::inListenerPaths(QString listenerPath)
{
    bool exists = false;
    QVectorIterator<QString> path(*listenerPaths);
    while (path.hasNext()){
        if (path.next() == listenerPath)
            exists = true;
    }
    return exists;
}

QVariantMap ThreadMonitor::getThreadState()
{
    QVariantMap threadsState;

    QVectorIterator<SubversionWorker*> workerIt(workerPool);
    while (workerIt.hasNext()){
        QVariantMap threadState;

        SubversionWorker* workerInPool = workerIt.next();
        threadState["state"] = workerInPool->getState();
        threadState["id"] = workerInPool->getThreadId();

        threadsState[workerInPool->getWorkingPath()] = threadState;
    }

    return threadsState;
}

QVector<SubversionLog> *ThreadMonitor::getLogsFromWorker(QString path, QMutex **vectorMutex)
{
    SubversionWorker *worker = findWorkerByWork(path);
    QVector<SubversionLog> *svnLogs = worker->getLogs(vectorMutex);
    return svnLogs;
}

} // namespace Spy
