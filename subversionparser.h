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

class SubversionParserSyncro
{
public:
    SubversionParserSyncro(QString path, bool updateFirst);
    QVector<SubversionLog> getLogs(int64_t fromRev, int64_t toRev);
    QVector<SubversionLog> getLogs();
    bool updatePath();

private:
    QString path;
    QVector<SubversionLog> parseLogs(const QByteArray* xmlData);

};

}
#endif // SUBVERSIONPARSER_H
