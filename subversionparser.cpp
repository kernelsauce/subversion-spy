#include "subversionparser.h"

namespace Spy{

/// Node keys.
static const QString svnXmlMsgKey = SVN_XML_MSG;
static const QString svnXmlAuthorKey = SVN_XML_AUTHOR;
static const QString svnXmlDateKey = SVN_XML_DATE;
static const QString svnXmlRevKey = SVN_XML_REVISION;
static const QString svnXmlPathsKey = SVN_XML_PATHS;
static const QString svnXmlPathKey = SVN_XML_PATH;

SubversionParserSyncro::SubversionParserSyncro(QString path, bool updateFirst) : path(path)
{
    qDebug() << "SubversionParserSyncro constructed.";
    if (updateFirst) updatePath();
}

QVector<SubversionLog> SubversionParserSyncro::getLogs(int64_t fromRev, int64_t toRev)
{
    QProcess svnProc;
    QString executeCmd = QString().
            append(SVN_LOG).
            append(SPACE).
            append(SVN_XML_OPT).
            append(SPACE).
            append(SVN_VERBOSE_OPT);

    // Are we getting specified revision or not?
    if ( !(fromRev == -1 && toRev == -1) )
    {
        executeCmd.
                append(SPACE).
                append(SVN_REVISION_OPT);
        if (fromRev != -1) executeCmd.append(QString::number(fromRev));
        else executeCmd.append("0");

        executeCmd.append(":");

        if (toRev != -1) executeCmd.append(QString::number(toRev));
        else executeCmd.append("HEAD");
    }

    // Add path.
    executeCmd.append(SPACE).append(path);
    svnProc.start(executeCmd);
    qDebug() << executeCmd;
    while(!svnProc.waitForFinished(500))
    {
    }

    QProcess::ExitStatus rc = svnProc.exitStatus();

    if (rc == QProcess::NormalExit)
    {
        QVector<SubversionLog> logs;
        const QByteArray svnXmlOutput = svnProc.readAllStandardOutput();
        try
        {
            logs = parseLogs(&svnXmlOutput);
        }
        catch(...){
            // Do nothing at this point in time :p.
        }
        return logs;
    }
    else
    {
        throw E_GENERIC;
    }
}

QVector<SubversionLog> SubversionParserSyncro::getLogs()
{
    return getLogs(-1 , -1);
}



bool SubversionParserSyncro::updatePath()
{
    QProcess svnProc;
    const QString executeCmd = QString().
            append(SVN_UPDATE).
            append(SPACE) + path;

    svnProc.start(executeCmd);
    while(!svnProc.waitForFinished(500))
    {
    }

    const QString svnOutput = svnProc.readAllStandardOutput();
    const QString svnError = svnProc.readAllStandardError();

    QProcess::ExitStatus rc = svnProc.exitStatus();

    return rc == QProcess::NormalExit ? true : false;
}

QVector<SubversionLog> SubversionParserSyncro::parseLogs(const QByteArray *xmlData)
{
    QVector<SubversionLog> parsedLogs;
    QDomDocument svnOutput;
    if (!svnOutput.setContent(*xmlData)) throw E_PARSE_PROBLEMS;

    QDomNodeList logEntryNodes = svnOutput.elementsByTagName(SVN_XML_LOGENTRY);

    for (int32_t currentLogEntryNode = 0; currentLogEntryNode < logEntryNodes.size(); currentLogEntryNode++){
        SubversionLog currentLogEntry;

        const QDomNode logEntryNode = logEntryNodes.item(currentLogEntryNode);
        currentLogEntry.revNumber = (uint64_t) logEntryNode.toElement().attribute(svnXmlRevKey).toLongLong();
        currentLogEntry.comment = logEntryNode.firstChildElement(svnXmlMsgKey).text();
        currentLogEntry.author = logEntryNode.firstChildElement(svnXmlAuthorKey).text();
        currentLogEntry.date = logEntryNode.firstChildElement(svnXmlDateKey).text();

        // Do we have changed files?
        const QDomElement pathsElement = logEntryNode.firstChildElement(svnXmlPathsKey);
        if (pathsElement.isElement())
        {
            currentLogEntry.files = QStringList();
            const QDomNodeList pathsNode = pathsElement.elementsByTagName(svnXmlPathKey);
            for (int32_t currentPath = 0; currentPath < pathsNode.size(); currentPath++ )
            {
                const QDomNode pathNode = pathsNode.item(currentPath);
                currentLogEntry.files.append(pathNode.toElement().text());
            }
        }

        parsedLogs.append(currentLogEntry);
    }

    return parsedLogs;
}

}
