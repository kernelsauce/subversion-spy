#include "threadmonitor.h"

namespace Spy{

ThreadMonitor::ThreadMonitor(QObject *parent,
                             QVector<QString> *listenerPaths,
                             QMutex* listenerPathsMutex) :
    listenerPaths(listenerPaths),
    listenerPathsMutex(listenerPathsMutex),
    kill(false)
{
}

ThreadMonitor::~ThreadMonitor()
{
    kill = true;
    eventLoopRunning.lock();
    emit finished();
    eventLoopRunning.unlock();
}

void ThreadMonitor::run()
{
    eventLoopRunning.lock();
    while(1)
    {
        if (kill)
        {
            eventLoopRunning.unlock();
            emit finished();
            break;
        }
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

        QVectorIterator<SubversionWorker*> workerIt(workerPool);
        uint32_t i = 0;
        while (workerIt.hasNext())
        {
            SubversionWorker* workerInPool = workerIt.next();
            if (!inListenerPaths(workerInPool->getWorkingPath()))
            {
                workerPool.remove(i);
                delete workerInPool;
            }
            i++;
        }

        listenerPathsMutex->unlock();

        sleep(2);
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

bool ThreadMonitor::inListenerPaths(QString listenerPath)
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
