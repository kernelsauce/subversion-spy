#ifndef SPYWKBRIDGE_H
#define SPYWKBRIDGE_H

#include <QObject>
#include <QStringList>
#include <QFileDialog>
#include <QVariantMap>
#include <QVectorIterator>

#include "spytypes.h"

namespace Spy{

class SubversionSpy; // Class declaration.

/** @brief Webkit glue class.

    Helping the export of various functions from SubversionSpy into Webkit
    Javascript context.
 */
class SpyWkBridge : public QObject
{
    Q_OBJECT
public:
    SpyWkBridge(SubversionSpy *spy, QObject *parent = 0);
    
signals:

private:
    SubversionSpy *spy;
    
public slots:
    /**
     * Open a native directory browser.
     * @return The path selected by the user.
     */
    QString openDirBrowser();

    /**
     * Send a notifcation through the tray icon.
     * @param message.
     */
    void sendNotification(QString msg, int type);
    QStringList getAllNotifications();
    QStringList getListenerPaths();
    void addListenerPath(QString path);
    bool removeListenerPath(QString path);
    void setPollRate(int seconds);
    int getPollRate();
    QVariantMap getThreadState();
    QVariantMap getLogs(QString path);
};
}
#endif // SPYWKBRIDGE_H
