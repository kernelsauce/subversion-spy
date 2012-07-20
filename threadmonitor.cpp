#include "threadmonitor.h"

// QThread has static sleep functions; but they're protected (duh).
// So we provide wrapper functions:
//
//    void MyLib::sleep (unsigned long secs)
//    void MyLib::msleep (unsigned long msecs)
//    void MyLib::usleep (unsigned long usecs)

#include <QThread>

namespace SleepLib
{
class DerivedFromQThread : protected QThread
{
public:
    static void sleep (unsigned long secs)   { QThread::sleep (secs) ; }
    static void msleep (unsigned long msecs) { QThread::msleep (msecs) ; }
    static void usleep (unsigned long usecs) { QThread::usleep (usecs) ; }
} ;
void sleep (unsigned long secs)   { DerivedFromQThread::sleep (secs) ; }
void msleep (unsigned long msecs) { DerivedFromQThread::msleep (msecs) ; }
void usleep (unsigned long usecs) { DerivedFromQThread::usleep (usecs) ; }
} // namespace SleepLib


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

void ThreadMonitor::process()
{
    eventLoopRunning.lock();
    while(1)
    {
        if (kill)
        {
            eventLoopRunning.unlock();
            break;
        }
        listenerPathsMutex->lock();
        QVectorIterator<QString> it(*listenerPaths);

        while (it.hasNext()){
            propagateNotifications(it.next(), N_NEW_REVISON);
        }
        listenerPathsMutex->unlock();

        SleepLib::sleep(2);
    }
}

void ThreadMonitor::propagateNotifications(QString message, SpyNotifications type)
{
    emit sendNotifications(message, type);
}

} // namespace Spy
