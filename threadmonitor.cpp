#include "threadmonitor.h"

namespace Spy{

ThreadMonitor::ThreadMonitor(QVector<QString> *listenerPaths,
                             QMutex *listenerPathsMutex,
                             uint32_t* pollRate, QMutex *pollRateMutex) :
    listenerPaths(listenerPaths),
    listenerPathsMutex(listenerPathsMutex),
    kill(false),
    pollRate(pollRate),
    pollRateMutex(pollRateMutex)
{
}

ThreadMonitor::~ThreadMonitor()
{
    killMutex.lock();
    kill = true;
    killMutex.unlock();
    wait();
}

void ThreadMonitor::run()
{
    while(1)
    {
        killMutex.unlock();
        if (kill)
        {
            killMutex.unlock();
            break;
        }
        killMutex.unlock();

        listenerPathsMutex->lock();
        QVectorIterator<QString> listenerPathIt(*listenerPaths);

        // Spawn a new thread for each path that is not already there.
        while (listenerPathIt.hasNext()){
            QString listenerPath = listenerPathIt.next();
            if (findWorkerByWork(listenerPath) == NULL){
                propagateNotifications("Added " + listenerPath + " to listener.", N_ADDED_REPOSITORY);
                SubversionWorker* worker = new SubversionWorker(listenerPath);
                workerPool.append(worker);
                worker->start();
                connect(worker, SIGNAL(giveUserFeedback(QString,SpyNotifications)), this, SLOT(propagateNotifications(QString,SpyNotifications)));
            }
        }

        // Stop workers that no longer is in list.
        QVectorIterator<SubversionWorker*> workerIt(workerPool);
        uint32_t index = 0;
        while (workerIt.hasNext())
        {
            SubversionWorker* workerInPool = workerIt.next();
            if (!inListenerPaths(workerInPool->getWorkingPath()))
            {
                workerPool.remove(index);
                delete workerInPool;
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
    return NULL;
}

inline bool ThreadMonitor::inListenerPaths(QString listenerPath)
{
    bool exists = false;
    QVectorIterator<QString> path(*listenerPaths);
    while (path.hasNext())
    {
        if (path.next() == listenerPath) exists = true;
    }
    return exists;
}

} // namespace Spy
