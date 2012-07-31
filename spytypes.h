#ifndef SPYTYPES_H
#define SPYTYPES_H
#include <QStringList>
#include <QString>
#include <QVectorIterator>

#include <stdint.h>

namespace Spy{

/**
 * Enumeration of exceptions thrown by different classes.
 */
enum SpyExceptions{
    E_GENERIC_EXCEPTION,///< Generic exception.
    E_BAD_USER_INPUT,   ///< User input is rubish.
    E_THREAD_ISSUE      ///< Problems with threading.
};

/**
 * Exceptions thrown from SubversionParser class.
 */
enum ParserException{
    E_PARSE_PROBLEMS,   ///< Parse error. Invalid XML.
    E_NO_CONNECTION,    ///< No connection, could not connect to svn repository.
    E_GENERIC,          ///< Generic error, we don't know what happened!
    E_TIMED_OUT         ///< Subversion client timed out.
};

/**
 * Enumeration of notification types for the tray.
 */
enum SpyNotifications{
    N_NOT_SVN_DIR,          ///< Path added to listener paths is not a SVN directory.
    N_NEW_REVISON,          ///< A new revision notification.
    N_ADDED_REPOSITORY,     ///< A new repository was added to listener.
    N_UPDATED_REPOSITORY,   ///< Repository was updated successfully.
    N_PARSE_PROBLEMS
};

enum SubversionWorkerState{
    S_UPDATING,             ///< Thread is running a SVN process waiting for completion.
    S_STOPPED,              ///< Thread has stopped.
    S_ERROR,                ///< Thread has encountered a error.
    S_WAIT,                 ///< Thread is sleeping according to poll rate.
    S_DIEING                ///< Thread is being killed.
};

/** @brief Container for a single Subversion commit/log. */
typedef struct {
    uint64_t revNumber; ///< Revision number.
    QString comment;    ///< Author comment.
    QString author;     ///< Author's username.
    QString date;       ///< Date of commit.
    QStringList files;  ///< Changed files during commit.
}   SubversionLog;

/** @brief Container for a notification entry. */
typedef struct {
    QString message;        ///< Notification message.
    SpyNotifications type;  ///< Notification type from SpyNotifications enumeration.
} NotificationEntry;

// Convinience types :-).
typedef QVector<SubversionLog>              SVNLogVector;
typedef QVectorIterator<SubversionLog>      SVNLogIterator;
typedef QVector<NotificationEntry>          NotificationVector;
typedef QVectorIterator<NotificationEntry>  NotificationIterator;

class SubversionWorker; // Predeclare.
typedef QVector<SubversionWorker*>          SVNWorkerPool;
typedef QVectorIterator<SubversionWorker*>  SVNWorkerIterator;

}
#endif // SPYTYPES_H
