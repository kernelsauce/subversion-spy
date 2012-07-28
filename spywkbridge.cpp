#include "subversionspy.h"
#include "spywkbridge.h"

namespace Spy{
SpyWkBridge::SpyWkBridge(SubversionSpy *spy) : spy(spy)
{
}

void SpyWkBridge::sendNotification(QString message, int type)
{
    this->spy->displayNotification(message, (SpyNotifications)type);
}

QStringList SpyWkBridge::getAllNotifications()
{
    QStringList notiLogsSList;
    QMutex* notiLogMutex;

    QVector<NotificationEntry>* notiLogs = spy->getAllNotifications(&notiLogMutex);
    notiLogMutex->lock();
    QVectorIterator<NotificationEntry> notiItr(*notiLogs);

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

    uint32_t index = 0;
    listenerPathsMutex->lock();
    QVectorIterator<QString> listenerPathsIt(*listenerPaths);

    while (listenerPathsIt.hasNext())
    {
        if (listenerPathsIt.next() == path)
        {
            listenerPaths->remove(index);
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

}
