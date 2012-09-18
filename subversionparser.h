#ifndef SUBVERSIONPARSER_H
#define SUBVERSIONPARSER_H

#include <QString>
#include <QtXml>
#include <QList>
#include <QDebug>
#include <stdint.h>
#include "spytypes.h"

/************  SVN Commands  *************/
#define SVN_CMD                 "svn "
#define SVN_UPDATE SVN_CMD      "up"
#define SVN_LOG SVN_CMD         "log"
#define SVN_DIFF SVN_CMD        "diff"
/************  SVN Options   *************/
#define SVN_XML_OPT             "--xml"
#define SVN_VERBOSE_OPT         "-v"
#define SVN_REVISION_OPT        "-r"
/************  SVN Keywords  *************/
#define SVN_XML_LOGENTRY        "logentry"
#define SVN_XML_REVISION        "revision"
#define SVN_XML_AUTHOR          "author"
#define SVN_XML_MSG             "msg"
#define SVN_XML_DATE            "date"
#define SVN_XML_PATHS           "paths"
#define SVN_XML_PATH            "path"
#define SPACE                   " "

namespace Spy{

/** @brief A command-line Subversion client parser.

    As we don't want to maintain our own Subversion client we use the command-line client
    to get information from repositories. The client will output XML to stdout if given
    the correct parameters.

    All work done in this class is done in-sync.
 */
class SubversionParserSyncro
{
public:
    /**
     * Create new Subversion parser.
     * @param path Path to the repository. Either remote or local fs path.
     * @param updateFirst Should the parser do "svn update" in constructor.
     */
    SubversionParserSyncro(QString path, bool update_first);

    /**
     * Get logs from and to given revision.
     * @param fromRev From revision. Use -1 if undefined.
     * @param toRev To revision. Use -1 if undefined (i.e this means HEAD).
     * @return Vector of Subversion logs.
     */
    QVector<SubversionLog> get_logs(int64_t from, int64_t to);

    /**
     * Get all logs since creation of repository.
     * @return Vector of Subversion logs.
     */
    QVector<SubversionLog> get_logs();

    /**
     * Update the repository. (svn update)
     * @returns true on success else false.
     */
    bool update();

private:
    QString path; ///< Path for the parser.

    /**
     * Parse a svn XML byte array.
     * @param xmlData Pointer to xml byte array.
     * @return Vector of Subversion logs.
     */
    QVector<SubversionLog> parse_logs(const QByteArray* xml);

};

}
#endif // SUBVERSIONPARSER_H
