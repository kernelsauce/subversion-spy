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
    explicit SubversionWorker(QString path = 0,
                              uint32_t *p_rate = 0,
                              QMutex *p_rate_mutex = 0,
                              QObject *parent = 0);
    ~SubversionWorker();

    /**
     * Get this threads working path.
     * @param The path.
     */
    QString get_working_path();

    /**
     * Get this threads current state.
     * @returns Enumeration of the current state.
     */
    SubversionWorkerState get_state();

    /**
     * Get this threads Id.
     * @return Internal Thread ID.
     */
    uint32_t get_thread_id();


    /**
     * Get logs processed by the worker.
     * As we are returning a reference to the vector off logs, the caller must
     * lock the mutex before accessing returned pointer.
     * @param mutex Lock this when accessing the returned pointer.
     * @return Pointer to QVector.
     */
    QVector<SubversionLog> *get_logs(QMutex** vector_mutex);
    
private:
    SubversionParserSyncro parser;      ///< Instance of a SubversionParser class, being used.
    QVector<SubversionLog> svn_logs;    ///< Vector of subversion logs. Contains all logs every fetched.
    QMutex svn_logs_mutex;              ///< Lock this when accessing svnLogs.
    uint64_t last_rev;                  ///< Last revision number fetched from repository.
    QString path;                       ///< The path to the repository.
    uint32_t* p_rate;                   ///< Poll rate setting.
    QMutex* p_rate_mutex;               ///< Lock this when accessing pollRate.
    bool kill;                          ///< Kill switch for this thread.
    QMutex kill_mutex;                  ///< Lock this when accessing kill.
    SubversionWorkerState state;        ///< Current state of this thread.
    QMutex state_mutex;                 ///< Lock this when accessing state.
    uint32_t thread_id;                 ///< The thread ID for this thread.

    /**
     * Called when the worker has fetched logs.
     * @param freshLogs Pointer to vector of logs.
     */
    void handle_new_logs(QVector<SubversionLog>* fresh_logs);

    /**
     * Called by thread before entering event loop to get all data from the
     * repository intially.
     * @return true on success else false.
     */
    bool fetch_initial_logs();

    /**
     * Set this threads current state.
     * @param state The state we are switching to.
     */
    void set_state(SubversionWorkerState state);

signals:
    /**
     * Send new notification to the main thread, i.e posting a notification
     * to the tray icon.
     * @param message The notification message to post.
     * @param type The type of notifcation you are posting.
     */
    void give_feedback(QString msg, SpyNotifications type);
    
protected:
    void run();

};

}
#endif // SUBVERSIONWORKER_H
