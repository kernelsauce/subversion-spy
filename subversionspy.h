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
#include <QSettings>

#include <stdint.h>

#include "spytypes.h"
#include "subversionparser.h"
#include "threadmonitor.h"
#include "spywkbridge.h"

#define DBG 1
#define WK_WINDOW_NAME "Subversion Spy Configuration"
#define WK_WINDOW_HEIGHT 400
#define WK_WINDOW_WIDTH 800
#define THREAD_POLL_RATE 30

namespace Spy{

/** @brief Main class of Subversion Spy.

    One instance is one application :).
 */
class SubversionSpy : public QMainWindow
{
    Q_OBJECT
    
private:
    QVector<QString> listenpaths;           ///< Paths that the thread monitor will assign to workers.
    QMutex listenpathsmutex;                ///< Lock this mutex when accessing listenerPaths.
    ThreadMonitor* monitor;                 ///< Thread monitor for Subversion workers.
    QVector<NotificationEntry> notilogs;    ///< Vector of notification entries.
    QMutex notilogs_mutex;                  ///< Lock when accessing notiLog.
    QWebView* wk_gui;                       ///< Pointer to webkit browser.
    SpyWkBridge* wk_bridge;                 ///< Pointer to webkit javascript bridge.
    uint32_t p_rate;                        ///< Poll rate for threads in seconds.
    QMutex p_rate_mutex;                    ///< Lock when accessing pollRate.

    // Tray icon:
    QSystemTrayIcon* tray;                  ///< Pointer to the tray icon.
    QIcon _tray_graphics;                   ///< Tray icon graphics.
    QMenu* tray_menu;                       ///< Pointer to the tray menu.
    QAction* quit_action;
    QAction* config_action;

public:
    SubversionSpy(QWidget *parent = 0);
    ~SubversionSpy();

    /**
     * Get listener paths.
     * @param mutex Lock this mutex before using the returned pointer.
     * @return pointer to listener paths vector.
     */
    QVector<QString>* get_listen_paths(QMutex **mutex);

    bool save_listen_paths();
    void restore_listen_paths();

    /**
     * Get notifications that have already been displayed.
     * @param mutex Lock this mutex before using the returned pointer.
     * @return pointer to notifications vector.
     */
    QVector<NotificationEntry> *get_all_notific(QMutex **mutex);

    /**
     * Get certain amount of notifications that have already been displayed.
     * No mutex required, but pointer returned must be freed.
     * @param amount get n amount of the last logs added.
     * @return pointer to notifications vector.
     */
    QVector<NotificationEntry> *get_n_notific(uint32_t amount);

    /**
     * Get certain amount of notifications that have already been displayed.
     * No mutex required, but pointer returned must be freed.
     * @param amount get n amount of logs.
     * @param offset offset for logs.
     * @return pointer to notifications vector.
     */
    QVector<NotificationEntry> *get_n_notific(uint32_t amount,
                                                  uint32_t offset);

    /**
     * Set the poll rate of the listener threads.
     * @param seconds Wait n amount seconds before doing a recheck.
     */
    void setpollrate(uint32_t secs);

    /**
     * Get current poll rate of the listener threads;
     * @return the current poll rate.
     */
    uint32_t getpollrate();

    /**
     * Get the thread monitor.
     * @returns pointer to the thread monitor.
     */
    ThreadMonitor* get_threadmonitor();

public slots:
    /**
     * Remove the tray icon.
     */
    void stop_tray();

    /**
     * Display a tray notification with given type.
     * @see SpyNotifications enumeration.
     */
    void displaynotific(QString msg, SpyNotifications type);

    /**
     * Opens the Webkit GUI.
     */
    void open_wk_gui();
    void wk_gui_closedcb();

};
}

#endif // SUBVERSIONSPY_H
