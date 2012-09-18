#include "subversionspy.h"
#include "spywkbridge.h"

namespace Spy{
SpyWkBridge::SpyWkBridge(SubversionSpy *spy, QObject *parent) :
    QObject(parent),
    spy(spy)
{
}

QString SpyWkBridge::openDirBrowser()
{
    return QFileDialog::getExistingDirectory();
}

void SpyWkBridge::sendNotification(QString message, int type)
{
    this->spy->displaynotific(message, (SpyNotifications)type);
}

QStringList SpyWkBridge::getAllNotifications()
{
    QStringList ret_lst;
    QMutex* notiLogMutex = 0;

    QVector<NotificationEntry> *noti_logs = spy->get_all_notific(&notiLogMutex);

    if (!notiLogMutex)
        return ret_lst;

    notiLogMutex->lock();
    QVectorIterator<NotificationEntry> noti_itr(*noti_logs);

    while (noti_itr.hasNext())
    {
        NotificationEntry nextLogEntry = noti_itr.next();
        ret_lst.append(nextLogEntry.msg.append(":" + QString::number((int)nextLogEntry.type)));
    }

    notiLogMutex->unlock();
    return ret_lst;
}

QStringList SpyWkBridge::getListenerPaths()
{
    QStringList listenerPathSList; // Compatible string list to be returned.
    QMutex* listenerPathsMutex;
    QVector<QString> *listenerPaths = spy->get_listen_paths(&listenerPathsMutex);

    listenerPathsMutex->lock();

    QVectorIterator<QString> listenerPathsIt(*listenerPaths);
    while (listenerPathsIt.hasNext())
    {
        listenerPathSList.append(listenerPathsIt.next());
    }

    listenerPathsMutex->unlock();

    return listenerPathSList;
}

void SpyWkBridge::addListenerPath(QString path)
{
    QMutex* listenerPathsMutex;
    QVector<QString>* listenerPaths = spy->get_listen_paths(&listenerPathsMutex);

    listenerPathsMutex->lock();
    listenerPaths->append(path);
    listenerPathsMutex->unlock();
}

bool SpyWkBridge::removeListenerPath(QString path)
{
    QMutex* listenerPathsMutex;
    QVector<QString>* listenerPaths = spy->get_listen_paths(&listenerPathsMutex);

    int32_t index = 0;
    listenerPathsMutex->lock();
    QVectorIterator<QString> listenerPathsIt(*listenerPaths);

    while (listenerPathsIt.hasNext()){
        if (listenerPathsIt.next() == path){
            if (listenerPaths->size() > index) listenerPaths->remove(index); // Check boundary.
            listenerPathsMutex->unlock();
            return true;
        }
        index++;
    }
    listenerPathsMutex->unlock();
    return false;
}

void SpyWkBridge::setPollRate(int seconds)
{
    spy->setpollrate((uint32_t)seconds);
}

int SpyWkBridge::getPollRate()
{
    return spy->getpollrate();
}

QVariantMap SpyWkBridge::getThreadState()
{
    return spy->get_threadmonitor()->getThreadState();
}

QVariantMap SpyWkBridge::getLogs(QString path)
{
    QVariantMap fullLogList;

    QMutex* vectorMutex = 0;
    QVector<SubversionLog>* logs = spy->get_threadmonitor()->getLogsFromWorker(path, &vectorMutex);

    vectorMutex->lock();

    // Build our big monster log blob.
    QVectorIterator<SubversionLog> logItr(*logs);
    while (logItr.hasNext())
    {
        SubversionLog currentLog = logItr.next();

        // Build details.
        QVariantMap details;
        details["revNumber"] = (uint32_t)currentLog.revNumber;
        details["comment"] = currentLog.comment;
        details["author"] = currentLog.author;
        details["date"] = currentLog.date;
        details["files"] = currentLog.files;

        fullLogList[QString::number(currentLog.revNumber)] = details;
    }
    vectorMutex->unlock();

    return fullLogList;
}

}
