#ifndef SPYWKBRIDGE_H
#define SPYWKBRIDGE_H

#include <QObject>
#include <QStringList>

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
    void sendNotification(QString message, int type);
    QStringList getListenerPaths();
};
}
#endif // SPYWKBRIDGE_H
