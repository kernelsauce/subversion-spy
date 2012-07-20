#ifndef SPYTYPES_H
#define SPYTYPES_H
#include <QStringList>
#include <QString>
#include <stdint.h>

namespace Spy{

/**
 * Enumeration of exceptions thrown by different classes.
 */
enum SpyExceptions{
    E_GENERIC_EXCEPTION,/// Generic exception.
    E_BAD_USER_INPUT,   /// User input is rubish.
    E_THREAD_ISSUE      /// Problems with threading.
};

/**
 * Exceptions thrown from SubversionParser class.
 */
enum ParserException{
    E_PARSE_PROBLEMS,   /// Parse error. Invalid XML.
    E_NO_CONNECTION,    /// No connection, could not connect to svn repository.
    E_GENERIC,          /// Generic error, we don't know what happened!
    E_TIMED_OUT         /// Subversion client timed out.
};

/**
 * Enumeration of notification types for the tray.
 */
enum SpyNotifications{
    N_NOT_SVN_DIR,      /// Path added to listener paths is not a SVN directory.
    N_NEW_REVISON       /// A new revision notification.
};

/**
 * Container for a single Subversion commit/log.
 */
typedef struct {
    uint64_t revNumber; /// Revision number.
    QString comment;    /// Author comment.
    QString author;     /// Author's username.
    QString date;       /// Date of commit.
    QStringList files;  /// Changed files during commit. TODO: change to vector.
} SubversionLog;

}
#endif // SPYTYPES_H
