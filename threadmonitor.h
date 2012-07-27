#ifndef THREADMONITOR_H
#define THREADMONITOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QMutex>
#include <QDebug>
#include "subversionworker.h"

#define THREAD_MONITOR_ITERATION 2

namespace Spy{

class ThreadMonitor : public QThread
{
    Q_OBJECT
public:
    ThreadMonitor(QObject *parent = 0,
                  QVector<QString>* listenerPaths = NULL,
                  QMutex* listenerPathsMutex = NULL);
    ~ThreadMonitor();
    
signals:
    void finished();
    void error(QString err);
    void sendNotifications(QString message, SpyNotifications type);

private:
    QVector<QString>* listenerPaths;
    QMutex* listenerPathsMutex;
    QVector<SubversionWorker*> workerPool;
    bool kill;
    QMutex killMutex;

    SubversionWorker* findWorkerByWork(QString listenerPath);
    inline bool inListenerPaths(QString listenerPath);

public slots:
    void propagateNotifications(QString message, SpyNotifications type);

protected:
    void run();
    
};
}

#endif // THREADMONITOR_H