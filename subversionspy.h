#ifndef SUBVERSIONSPY_H
#define SUBVERSIONSPY_H

#include <QtGui/QMainWindow>
#include <QSystemTrayIcon>
#include <QThread>
#include <QMenu>
#include <QPixmap>
#include <QDebug>
#include <QMutex>
#include <stdint.h>
#include "spytypes.h"
#include "listenerpathform.h"
#include "subversionparser.h"
#include "threadmonitor.h"

namespace Spy{

class SubversionSpy : public QMainWindow
{
    Q_OBJECT
    
public:
    SubversionSpy(QWidget *parent = 0);
    ~SubversionSpy();

private:
    QVector<QString> listenerPaths;
    QMutex listenerPathsMutex;
    QThread* monitorThread;
    ThreadMonitor* monitor;
    QVector<QThread>* svnWorkers;
    QSystemTrayIcon* trayIcon;
    QIcon trayIconGraphic;
    QMenu* trayMenu;

    QAction* addListenerPathsAction;
    QAction* removeListenerPathsAction;
    QAction* quitAction;
    QAction* aboutAction;

public slots:
    void addListenerPaths();
    void stopTray();
    void errorString(QString err);
    void displayNotification(QString message, SpyNotifications type);
};
}

#endif // SUBVERSIONSPY_H
