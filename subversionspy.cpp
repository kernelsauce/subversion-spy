#include "subversionspy.h"

namespace Spy{

SubversionSpy::SubversionSpy(QWidget *parent)
    : QMainWindow(parent),
      trayIconGraphic(QIcon(":/icons/res/spy-icon.png"))
{
    trayIcon = new QSystemTrayIcon(trayIconGraphic, this);
    trayMenu = new QMenu(this);
    addListenerPathsAction = new QAction("&Modify observers paths", this);
    quitAction = new QAction("&Quit...", this);
    aboutAction = new QAction("&About", this);

    /*
     * Add menu entries.
     */
    trayMenu->addAction(addListenerPathsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);
    trayMenu->addAction(aboutAction);

    /*
     * Connect signals.
     */
    connect(quitAction, SIGNAL(triggered()), this, SLOT(stopTray()));
    connect(addListenerPathsAction, SIGNAL(triggered()), this, SLOT(addListenerPaths()));
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
    addListenerPaths();
}

SubversionSpy::~SubversionSpy()
{
    qDebug() << "Destructing SubversionSpy instance.";
    delete trayIcon;
    delete trayMenu;
    delete addListenerPathsAction;
}

void SubversionSpy::addListenerPaths()
{
    ListenerPathForm* addForm = new ListenerPathForm(this,
                                                     &listenerPaths,
                                                     &listenerPathsMutex);

    addForm->exec();
    delete addForm;
    qDebug() << "Updated listener paths.";
}

void SubversionSpy::stopTray()
{
    qDebug() << "Stopping tray.";
    trayIcon->hide();
    trayMenu->close();
    close();
}

void SubversionSpy::errorString(QString err)
{
    qDebug() << err;
}

void SubversionSpy::displayNotification(QString message, SpyNotifications type)
{
    // TODO: Diff between types of notifications.
    trayIcon->showMessage("Subversion Spy", message);
}

}
