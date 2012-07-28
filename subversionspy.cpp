#include "subversionspy.h"

namespace Spy{

SubversionSpy::SubversionSpy(QWidget *parent)
    : QMainWindow(parent),
      trayIconGraphic(QIcon(":/icons/res/spy-icon.png"))
{
    trayIcon = new QSystemTrayIcon(trayIconGraphic, this);
    trayMenu = new QMenu(this);
    addListenerPathsAction = new QAction("&Modify observers paths", this);
    configureAction = new QAction("&Configure", this);
    quitAction = new QAction("&Quit...", this);
    aboutAction = new QAction("&About", this);

    /*
     * Add menu entries.
     */
    trayMenu->addAction(addListenerPathsAction);
    trayMenu->addAction(configureAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);
    trayMenu->addAction(aboutAction);

    /*
     * Connect signals.
     */
    connect(quitAction, SIGNAL(triggered()), this, SLOT(stopTray()));
    connect(addListenerPathsAction, SIGNAL(triggered()), this, SLOT(addListenerPaths()));
    connect(configureAction, SIGNAL(triggered()), this, SLOT(openWkGui()));
    trayIcon->setContextMenu(trayMenu);

    /*
     * Init thread monitor. Which we use to stop/start SVN threads.
     */
    monitor = new ThreadMonitor(this, &listenerPaths, &listenerPathsMutex);
    monitor->start();

    qRegisterMetaType<SpyNotifications>("SpyNotifications"); // Register our enum as meta type to allow signal and slots with the type.
    connect(monitor, SIGNAL(sendNotifications(QString, SpyNotifications)), this, SLOT(displayNotification(QString,SpyNotifications)));

    trayIcon->show();
    trayIcon->showMessage("Subversion Spy started!",  "Please add path observers from the tray icon.");
}

SubversionSpy::~SubversionSpy()
{
    qDebug() << "Destructing SubversionSpy instance.";
    delete trayIcon;
    delete trayMenu;
    delete addListenerPathsAction;
    delete monitor;
}

QVector<QString> *SubversionSpy::getListenerPaths(QMutex** mutex)
{
    *mutex = &listenerPathsMutex;
    return &listenerPaths;
}

QVector<NotificationEntry> *SubversionSpy::getAllNotifications(QMutex **mutex)
{
    *mutex = &notiLogMutex;
    return &notiLog;
}

QVector<NotificationEntry> *SubversionSpy::getNNotifications(uint32_t amount)
{
    notiLogMutex.lock();
    uint32_t notiLogSize = notiLog.size();
    notiLogMutex.unlock();
    return getNNotifications(amount, amount - notiLogSize);
}

QVector<NotificationEntry> *SubversionSpy::getNNotifications(uint32_t amount, uint32_t offset)
{
    QVector<NotificationEntry>* notiLogCopy = new QVector<NotificationEntry>;

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

void SubversionSpy::addListenerPaths()
{
    ListenerPathForm* addForm = new ListenerPathForm(this,
                                                     &listenerPaths,
                                                     &listenerPathsMutex);

    addForm->exec();
    delete addForm;
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
    wkGui->setFixedSize(600, 300);
    wkGui->setWindowTitle(WK_WINDOW_NAME);
    wkGui->show();
}

}
