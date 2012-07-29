#ifndef SPYWKBRIDGE_H
#define SPYWKBRIDGE_H

#include <QObject>
#include <QStringList>
#include <QFileDialog>

#include "spytypes.h"

namespace Spy{
class SubversionSpy;

class SpyWkBridge : public QObject
{
    Q_OBJECT
public:
    SpyWkBridge(SubversionSpy *spy);
    
signals:

private:
    SubversionSpy *spy;
    
public slots:
    QString openDirBrowser();
    void sendNotification(QString message, int type);
    QStringList getAllNotifications();
    QStringList getListenerPaths();
    void addListenerPath(QString path);
    bool removeListenerPath(QString path);
    void setPollRate(int seconds);
    int getPollRate();
};
}
#endif // SPYWKBRIDGE_H
