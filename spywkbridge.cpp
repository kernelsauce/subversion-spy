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

QStringList SpyWkBridge::getListenerPaths()
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
    delete notiLogs;
    return notiLogsSList;
}
}
