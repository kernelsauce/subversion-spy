#ifndef SUBVERSIONWORKER_H
#define SUBVERSIONWORKER_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <stdint.h>
#include "subversionparser.h"

namespace Spy{

/** @brief A worker class for SubversionParserSyncro.

    As SubversionParserSyncro is a in-sync worker we need threads to
    help out. One worker will monitor one Subversion repository.
 */
class SubversionWorker : public QThread
{
    Q_OBJECT
public:
    /**
     * Construct a new Subversion worker.
     * As the class operate on memory shared through threads it requires mutexes.
     * @param path The path to listen to for changes.
     * @param pollRate Pointer to the poll rate setting.
     * @param pollRateMutex Pointer to the poll rate mutex.
     */
    explicit SubversionWorker(QString path = NULL,
                              uint32_t *pollRate = NULL,
                              QMutex* pollRateMutex = NULL);
    ~SubversionWorker();

    /**
     * Get this threads working path.
     * @param The path.
     */
    QString getWorkingPath();

    /**
     * Get this threads current state.
     * @returns Enumeration of the current state.
     */
    SubversionWorkerState getState();

    /**
     * Get this threads Id.
     * @return Internal Thread ID.
     */
    uint32_t getThreadId();
    
private:
    SubversionParserSyncro parser;      ///< Instance of a SubversionParser class, being used.
    QVector<SubversionLog> svnLogs;     ///< Vector of subversion logs. Contains all logs every fetched.
    uint64_t lastRevNumber;             ///< Last revision number fetched from repository.
    QString path;                       ///< The path to the repository.
    uint32_t* pollRate;                 ///< Poll rate setting.
    QMutex* pollRateMutex;              ///< Lock this when accessing pollRate.
    bool kill;                          ///< Kill switch for this thread.
    QMutex killMutex;                   ///< Lock this when accessing kill.
    SubversionWorkerState state;        ///< Current state of this thread.
    QMutex stateMutex;                  ///< Lock this when accessing state.
    uint32_t threadId;                  ///< The thread ID for this thread.

    /**
     * Called when the worker has fetched logs.
     * @param freshLogs Pointer to vector of logs.
     */
    void handleNewLogs(QVector<SubversionLog> *freshLogs);

    /**
     * Called by thread before entering event loop to get all data from the
     * repository intially.
     * @return true on success else false.
     */
    bool initialLogFetch();

    /**
     * Set this threads current state.
     * @param state The state we are switching to.
     */
    void setState(SubversionWorkerState state);

signals:
    /**
     * Send new notification to the main thread, i.e posting a notification
     * to the tray icon.
     * @param message The notification message to post.
     * @param type The type of notifcation you are posting.
     */
    void giveUserFeedback(QString message, SpyNotifications type);
    
protected:
    void run();

};

}
#endif // SUBVERSIONWORKER_H
