#include "subversionparser.h"

namespace Spy{

/// Node keys.
static const QString svnXmlMsgKey = SVN_XML_MSG;
static const QString svnXmlAuthorKey = SVN_XML_AUTHOR;
static const QString svnXmlDateKey = SVN_XML_DATE;
static const QString svnXmlRevKey = SVN_XML_REVISION;
static const QString svnXmlPathsKey = SVN_XML_PATHS;
static const QString svnXmlPathKey = SVN_XML_PATH;

SubversionParserSyncro::SubversionParserSyncro(QString path, bool update_first) :
    path(path)
{
    qDebug() << "SubversionParserSyncro constructed.";
    if (update_first) update();
}

QVector<SubversionLog> SubversionParserSyncro::get_logs(int64_t from, int64_t to)
{
    QProcess svn_proc;
    QString execute_cmd = QString().
            append(SVN_LOG).
            append(SPACE).
            append(SVN_XML_OPT).
            append(SPACE).
            append(SVN_VERBOSE_OPT);

    // Are we getting specified revision or not?
    if ( !(from == -1 && to == -1) )
    {
        execute_cmd.
                append(SPACE).
                append(SVN_REVISION_OPT);
        if (from != -1) execute_cmd.append(QString::number(from));
        else execute_cmd.append("0");

        execute_cmd.append(":");

        if (to != -1) execute_cmd.append(QString::number(to));
        else execute_cmd.append("HEAD");
    }

    // Add path.
    execute_cmd.append(SPACE).append(path);
    svn_proc.start(execute_cmd);
    qDebug() << execute_cmd;
    while(!svn_proc.waitForFinished(500))
    {
    }

    QProcess::ExitStatus rc = svn_proc.exitStatus();

    if (rc == QProcess::NormalExit)
    {
        QVector<SubversionLog> logs;
        const QByteArray xml_output = svn_proc.readAllStandardOutput();
        try
        {
            logs = parse_logs(&xml_output);
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

QVector<SubversionLog> SubversionParserSyncro::get_logs()
{
    return get_logs(-1 , -1);
}



bool SubversionParserSyncro::update()
{
    QProcess svn_proc;
    const QString execute_cmd = QString().
            append(SVN_UPDATE).
            append(SPACE) + path;

    svn_proc.start(execute_cmd);
    while(!svn_proc.waitForFinished(500))
    {
    }

    const QString svn_output = svn_proc.readAllStandardOutput();

    QProcess::ExitStatus rc = svn_proc.exitStatus();

    return rc == QProcess::NormalExit ? true : false;
}

QVector<SubversionLog> SubversionParserSyncro::parse_logs(const QByteArray *xml)
{
    QVector<SubversionLog> parsed_logs;
    QDomDocument svn_output;
    if (!svn_output.setContent(*xml)) throw E_PARSE_PROBLEMS;

    QDomNodeList log_entry_nodes = svn_output.elementsByTagName(SVN_XML_LOGENTRY);

    size_t nodes_size = log_entry_nodes.size();
    for (size_t i = 0; i < nodes_size; i++){
        SubversionLog new_log_entry;

        const QDomNode log_entry_node = log_entry_nodes.item(i);
        new_log_entry.revNumber = (uint64_t) log_entry_node.toElement().attribute(svnXmlRevKey).toLongLong();
        new_log_entry.comment = log_entry_node.firstChildElement(svnXmlMsgKey).text();
        new_log_entry.author = log_entry_node.firstChildElement(svnXmlAuthorKey).text();
        new_log_entry.date = log_entry_node.firstChildElement(svnXmlDateKey).text();

        // Do we have changed files?
        const QDomElement paths_element = log_entry_node.firstChildElement(svnXmlPathsKey);
        if (paths_element.isElement()){
            new_log_entry.files = QStringList();
            const QDomNodeList paths_node = paths_element.elementsByTagName(svnXmlPathKey);

            size_t paths_node_size = paths_node.size();
            for (size_t path_i = 0; path_i < paths_node_size; path_i++){
                const QDomNode path_node = paths_node.item(path_i);
                new_log_entry.files.append(path_node.toElement().text());
            }
        }

        parsed_logs.append(new_log_entry);
    }

    return parsed_logs;
}

}
