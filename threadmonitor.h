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

/** @brief A event loop that monitors threads creation and killing.

    Used to monitor the SubversionWorker's. Every iteration it will look for changes
    in the listener paths vector to check for addition or removals. It will act accordingly
    upon these changes.
 */
class ThreadMonitor : public QThread
{
    Q_OBJECT
public:
    /**
     * Construct new thread monitor.
     * As the class operate on memory shared through threads it requires mutexes.
     * @param listenerPaths Pointer to vector of paths.
     * @param listenerPathsMutex Pointer to mutex for vector of paths.
     * @param pollRate Pointer to poll rate setting.
     * @param pollRateMutex Pointer to poll rate mutex.
     */
    ThreadMonitor(QVector<QString>* listenerPaths = NULL,
                  QMutex* listenerPathsMutex = NULL,
                  uint32_t* pollRate = NULL,
                  QMutex* pollRateMutex = NULL);

    ~ThreadMonitor();

    /**
     * Get all threads and their current state.
     * @returns Hashmap with different data.
     * @see SubversionWorkerState
     */
    QVariantMap getThreadState();
    
signals:
    /**
     * Signal to main threads to post a notifications through the tray icon.
     * @param message The notification message to post.
     * @param type The type of notification your are posting.
     */
    void sendNotifications(QString message, SpyNotifications type);

private:
    QVector<QString>* listenerPaths;        ///< Pointer to vector of paths.
    QMutex* listenerPathsMutex;             ///< Lock this when accessing listenerPaths.
    QVector<SubversionWorker*> workerPool;  ///< Pool of subversion worker pointer's.
    bool kill;                              ///< Kill switch for thread monitor.
    QMutex killMutex;                       ///< Lock this when accessing kill.
    uint32_t* pollRate;                     ///< Pointer to poll rate setting.
    QMutex* pollRateMutex;                  ///< Lock this when accessing pollRate.

    /**
     * Find a worker by specified path.
     * @param listenerPath Specified path to search for.
     * @returns Pointer to subversion worker or NULL if no worker found.
     */
    SubversionWorker* findWorkerByWork(QString listenerPath);

    /**
     * Check if a given path is in the vector of paths.
     * @parm listenerPath Specified path to search for.
     * @returns true if given path is in the vector else false.
     */
    inline bool inListenerPaths(QString listenerPath);

public slots:
    void propagateNotifications(QString message, SpyNotifications type);

protected:
    void run();
    
};
}

#endif // THREADMONITOR_H
