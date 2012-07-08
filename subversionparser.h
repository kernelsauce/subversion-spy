#ifndef SUBVERSIONPARSER_H
#define SUBVERSIONPARSER_H

#include <QString>
#include <QtXml>
#include <QList>
#include <QDebug>
#include <stdint.h>

/************  SVN Commands  *************/
#define SVN_CMD                 "svn "
#define SVN_UPDATE SVN_CMD      "up"
#define SVN_LOG SVN_CMD         "log"
#define SVN_DIFF SVN_CMD        "diff"
/************  SVN Options   *************/
#define SVN_XML_OPT             "--xml"
#define SVN_VERBOSE_OPT         "-v"
/************  SVN Keywords  *************/
#define SVN_XML_LOGENTRY        "logentry"
#define SVN_XML_AUTHOR          "author"
#define SVN_XML_MSG             "msg"
#define SVN_XML_DATE            "date"
#define SPACE                   " "


namespace Spy{

typedef struct {
    uint64_t revNumber;
    QString comment;
    QString author;
    QString date;
    QStringList files;
} SubversionLog;

enum ParserException{
    E_PARSE_PROBLEMS,
    E_NO_CONNECTION,
    E_GENERIC,
    E_TIMED_OUT
};

class SubversionParser
{
public:
    SubversionParser(QString path, bool updateFirst);
    QList<SubversionLog> getLogs(uint64_t fromRev, uint64_t toRev);
    QList<SubversionLog> getLogs();
    bool updatePath();

private:
    QString path;

};

}
#endif // SUBVERSIONPARSER_H
