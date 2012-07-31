#include "subversionspy.h"

namespace Spy{

SubversionSpy::SubversionSpy(QWidget *parent)
    : QMainWindow(parent),
      pollRate(THREAD_POLL_RATE),
      trayIconGraphic(QIcon(":/icons/icons/spy-icon.png"))

{
    trayIcon = new QSystemTrayIcon(trayIconGraphic, this);
    trayMenu = new QMenu(this);
    configureAction = new QAction("&Configure", this);
    quitAction = new QAction("&Quit...", this);
    aboutAction = new QAction("&About", this);

    /*
     * Add menu entries.
     */
    trayMenu->addAction(configureAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);
    trayMenu->addAction(aboutAction);

    /*
     * Connect signals.
     */
    connect(quitAction, SIGNAL(triggered()), this, SLOT(stopTray()));
    connect(configureAction, SIGNAL(triggered()), this, SLOT(openWkGui()));
    trayIcon->setContextMenu(trayMenu);

    /*
     * Init thread monitor. Which we use to stop/start SVN threads.
     */
    monitor = new ThreadMonitor(&listenerPaths,
                                &listenerPathsMutex,
                                &pollRate,
                                &pollRateMutex);
    monitor->start();

    qRegisterMetaType<SpyNotifications>("SpyNotifications"); // Register our enum as meta type to allow signal and slots with the type.
    connect(monitor, SIGNAL(sendNotifications(QString, SpyNotifications)), this, SLOT(displayNotification(QString,SpyNotifications)));

    trayIcon->show();
    //trayIcon->showMessage("Subversion Spy started!",  "Please add path observers from the tray icon.");
    openWkGui();
}

SubversionSpy::~SubversionSpy()
{
    qDebug() << "Destructing SubversionSpy instance.";
    delete trayIcon;
    delete trayMenu;
    delete configureAction;
    delete aboutAction;
    delete quitAction;
    delete monitor;
}

QVector<QString> *SubversionSpy::getListenerPaths(QMutex** mutex)
{
    *mutex = &listenerPathsMutex;
    return &listenerPaths;
}

NotificationVector *SubversionSpy::getAllNotifications(QMutex **mutex)
{
    *mutex = &notiLogMutex;
    return &notiLog;
}

NotificationVector *SubversionSpy::getNNotifications(uint32_t amount)
{
    notiLogMutex.lock();
    uint32_t notiLogSize = notiLog.size();
    notiLogMutex.unlock();
    return getNNotifications(amount, amount - notiLogSize);
}

NotificationVector *SubversionSpy::getNNotifications(uint32_t amount, uint32_t offset)
{
    NotificationVector* notiLogCopy = new NotificationVector;

    notiLogMutex.lock();
    uint32_t notiLogSize = notiLog.size();

    // Deep copy of selected notifications logs.
    for (uint32_t index = offset; index < amount; index++)
    {
        if (index < notiLogSize)
        {
            notiLogCopy->append(notiLog[index]);
        }
        else
        {
            // Trying to access out of boundary index.
            break;
        }
    }

    notiLogMutex.unlock();
    return notiLogCopy;
}

void SubversionSpy::setPollRate(uint32_t seconds)
{
    pollRateMutex.lock();
    pollRate = seconds;
    pollRateMutex.unlock();
}

uint32_t SubversionSpy::getPollRate()
{
    uint32_t pollRateReturn;
    pollRateMutex.lock();
    pollRateReturn = pollRate;
    pollRateMutex.unlock();
    return pollRateReturn;
}

ThreadMonitor *SubversionSpy::getThreadMonitor()
{
    return monitor;
}

void SubversionSpy::stopTray()
{
    qDebug() << "Stopping tray.";
    trayIcon->hide();
    trayMenu->close();
    close();
}

void SubversionSpy::displayNotification(QString message, SpyNotifications type)
{
    // Is there a message?
    if (message.size() <= 0) return;

    // Add to notification log.
    NotificationEntry newEntry;
    newEntry.message = message;
    newEntry.type = type;
    notiLogMutex.lock();
    notiLog.append(newEntry);
    notiLogMutex.unlock();

    switch (type){
    case N_PARSE_PROBLEMS:
        trayIcon->showMessage("Subversion Spy - Parse Problems", message);
        break;

    case N_UPDATED_REPOSITORY:
        trayIcon->showMessage("Subversion Spy - Updated Repository", message);
        break;

    case N_ADDED_REPOSITORY:
        trayIcon->showMessage("Subversion Spy - New Repository", message);
        break;

    case N_NEW_REVISON:
        trayIcon->showMessage("Subversion Spy - Check in alert!", message);
        break;

    case N_NOT_SVN_DIR:
        trayIcon->showMessage("Subversion Spy - Not a repository", message);
        break;
    }
}

void SubversionSpy::openWkGui()
{
    wkGui = new QWebView;

    // Add Webkit bridge.
    bridge = new SpyWkBridge(this);

#ifdef DBG
    wkGui->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif

    wkGui->page()->mainFrame()->addToJavaScriptWindowObject("SpyWkBridge", bridge);
    wkGui->load(QUrl("qrc:///wkGui/wkGui/index.html"));
    wkGui->setFixedSize(WK_WINDOW_WIDTH, WK_WINDOW_HEIGHT);
    wkGui->setWindowTitle(WK_WINDOW_NAME);
    wkGui->setWindowIcon(QIcon(":/icons/icons/spy-icon.png"));
    wkGui->show();
}

}
