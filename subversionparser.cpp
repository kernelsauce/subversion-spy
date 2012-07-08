#include "subversionparser.h"

namespace Spy{

SubversionParser::SubversionParser(QString path, bool updateFirst) : path(path)
{
    if (updateFirst)
    {
        updatePath();
    }

}

QList<SubversionLog> SubversionParser::getLogs(uint64_t fromRev, uint64_t toRev)
{
}

QList<SubversionLog> SubversionParser::getLogs()
{
    QProcess svnProc;
    const QString executeCmd = QString().
            append(SVN_LOG).
            append(SPACE).
            append(SVN_XML_OPT).
//            append(SPACE).
//            append(SVN_VERBOSE_OPT).
            append(SPACE) + path;

    svnProc.start(executeCmd);
    while(!svnProc.waitForFinished(100))
    {
    }

    QProcess::ExitStatus rc = svnProc.exitStatus();

    if (rc == QProcess::NormalExit)
    {
        QList<SubversionLog> parsedLogs;

        const QByteArray svnXmlOutput = svnProc.readAllStandardOutput();

        QDomDocument svnOutput;
        if (!svnOutput.setContent(svnXmlOutput)) throw E_PARSE_PROBLEMS;

        QDomNodeList logEntryNodes = svnOutput.elementsByTagName(SVN_XML_LOGENTRY);
        qDebug() << "Size of nodes " << logEntryNodes.size();
        for (int32_t currentNode = 0; currentNode <= logEntryNodes.size(); currentNode++){
            SubversionLog currentLogEntry;

            QDomNode logEntryNode = logEntryNodes.item(currentNode);

            currentLogEntry.comment = logEntryNode.firstChildElement(SVN_XML_MSG).nodeValue();
            currentLogEntry.author = logEntryNode.firstChildElement(SVN_XML_AUTHOR).nodeValue();
            currentLogEntry.date = logEntryNode.firstChildElement(SVN_XML_DATE).nodeValue();
            parsedLogs.push_front(currentLogEntry);
        }

        return parsedLogs;
    }
    else
    {
        throw E_GENERIC;
    }
}

bool SubversionParser::updatePath()
{
    QProcess svnProc;
    const QString executeCmd = QString().
            append(SVN_UPDATE).
            append(SPACE) + path;

    svnProc.start(executeCmd);
    while(!svnProc.waitForFinished(100))
    {
    }

    const QString svnOutput = svnProc.readAllStandardOutput();
    const QString svnError = svnProc.readAllStandardError();

    QProcess::ExitStatus rc = svnProc.exitStatus();

    return rc == QProcess::NormalExit ? true : false;
}

}
