#ifndef SUBVERSIONSPY_H
#define SUBVERSIONSPY_H

#include <QtGui/QMainWindow>
#include <QSystemTrayIcon>
#include <QThread>
#include <QMenu>
#include <QPixmap>
#include <QDebug>
#include <QMutex>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>

#include <stdint.h>

#include "spytypes.h"
#include "subversionparser.h"
#include "threadmonitor.h"
#include "spywkbridge.h"

#define DBG 1
#define WK_WINDOW_NAME "Subversion Spy Configuration"
#define WK_WINDOW_HEIGHT 400
#define WK_WINDOW_WIDTH 800
#define THREAD_POLL_RATE 10

namespace Spy{

class SubversionSpy : public QMainWindow
{
    Q_OBJECT
    
private:
    QVector<QString> listenerPaths;     ///< Paths that the thread monitor will assign to workers.
    QMutex listenerPathsMutex;          ///< Lock this mutex when accessing listenerPaths.
    ThreadMonitor* monitor;             ///< Thread monitor for Subversion workers.
    QVector<NotificationEntry> notiLog; ///< Vector of notification entries.
    QMutex notiLogMutex;                ///< Lock when accessing notiLog.
    QWebView* wkGui;                    ///< Pointer to webkit browser.
    SpyWkBridge* bridge;                ///< Pointer to webkit javascript bridge.
    uint32_t pollRate;                  ///< Poll rate for threads in seconds.
    QMutex pollRateMutex;               ///< Lock when accessing pollRate.

    // Tray icon:
    QSystemTrayIcon* trayIcon;          ///< Pointer to the tray icon.
    QIcon trayIconGraphic;              ///< Tray icon graphics.
    QMenu* trayMenu;                    ///< Pointer to the tray menu.
    QAction* quitAction;
    QAction* configureAction;
    QAction* aboutAction;

public:
    SubversionSpy(QWidget *parent = 0);
    ~SubversionSpy();

    /**
     * Get listener paths.
     * @param mutex Lock this mutex before using the returned pointer.
     * @return pointer to listener paths vector.
     */
    QVector<QString>* getListenerPaths(QMutex **mutex);

    /**
     * Get notifications that have already been displayed.
     * @param mutex Lock this mutex before using the returned pointer.
     * @return pointer to notifications vector.
     */
    QVector<NotificationEntry>* getAllNotifications(QMutex **mutex);

    /**
     * Get certain amount of notifications that have already been displayed.
     * No mutex required, but pointer returned must be freed.
     * @param amount get n amount of the last logs added.
     * @return pointer to notifications vector.
     */
    QVector<NotificationEntry>* getNNotifications(uint32_t amount);

    /**
     * Get certain amount of notifications that have already been displayed.
     * No mutex required, but pointer returned must be freed.
     * @param amount get n amount of logs.
     * @param offset offset for logs.
     * @return pointer to notifications vector.
     */
    QVector<NotificationEntry>* getNNotifications(uint32_t amount,
                                                  uint32_t offset);

    /**
     * Set the poll rate of the listener threads.
     * @param seconds Wait n amount seconds before doing a recheck.
     */
    void setPollRate(uint32_t seconds);

    /**
     * Get current poll rate of the listener threads;
     * @return the current poll rate.
     */
    uint32_t getPollRate();

    /**
     * Get the thread monitor.
     * @returns pointer to the thread monitor.
     */
    ThreadMonitor* getThreadMonitor();

public slots:
    /**
     * Remove the tray icon.
     */
    void stopTray();

    /**
     * Display a tray notification with given type.
     * @see SpyNotifications enumeration.
     */
    void displayNotification(QString message, SpyNotifications type);

    /**
     * Opens the Webkit GUI.
     */
    void openWkGui();

};
}

#endif // SUBVERSIONSPY_H
