#include "subversionspy.h"
#include "spywkbridge.h"

namespace Spy{
SpyWkBridge::SpyWkBridge(SubversionSpy *spy) : spy(spy)
{
}

QString SpyWkBridge::openDirBrowser()
{
    return QFileDialog::getExistingDirectory();
}

void SpyWkBridge::sendNotification(QString message, int type)
{
    this->spy->displayNotification(message, (SpyNotifications)type);
}

QStringList SpyWkBridge::getAllNotifications()
{
    QStringList notiLogsSList;
    QMutex* notiLogMutex;

    NotificationVector* notiLogs = spy->getAllNotifications(&notiLogMutex);
    notiLogMutex->lock();
    NotificationIterator notiItr(*notiLogs);

    while (notiItr.hasNext())
    {
        NotificationEntry nextLogEntry = notiItr.next();
        notiLogsSList.append(nextLogEntry.message.append(":" + QString::number((int)nextLogEntry.type)));
    }

    notiLogMutex->unlock();
    return notiLogsSList;
}

QStringList SpyWkBridge::getListenerPaths()
{
    QStringList listenerPathSList; // Compatible string list to be returned.
    QMutex* listenerPathsMutex;
    QVector<QString> *listenerPaths = spy->getListenerPaths(&listenerPathsMutex);

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
    QVector<QString>* listenerPaths = spy->getListenerPaths(&listenerPathsMutex);

    listenerPathsMutex->lock();
    listenerPaths->append(path);
    listenerPathsMutex->unlock();
}

bool SpyWkBridge::removeListenerPath(QString path)
{
    QMutex* listenerPathsMutex;
    QVector<QString>* listenerPaths = spy->getListenerPaths(&listenerPathsMutex);

    int32_t index = 0;
    listenerPathsMutex->lock();
    QVectorIterator<QString> listenerPathsIt(*listenerPaths);

    while (listenerPathsIt.hasNext())
    {
        if (listenerPathsIt.next() == path)
        {
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
    spy->setPollRate((uint32_t)seconds);
}

int SpyWkBridge::getPollRate()
{
    return spy->getPollRate();
}

QVariantMap SpyWkBridge::getThreadState()
{
    return spy->getThreadMonitor()->getThreadState();
}

QVariantMap SpyWkBridge::getLogs(QString path)
{
    QVariantMap fullLogList;

    QMutex* vectorMutex = NULL;
    SVNLogVector* logs = spy->getThreadMonitor()->getLogsFromWorker(path, &vectorMutex);

    vectorMutex->lock();

    // Build our big monster log blob.
    SVNLogIterator logItr(*logs);
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
