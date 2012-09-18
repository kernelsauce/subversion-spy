#include "subversionspy.h"

namespace Spy{

SubversionSpy::SubversionSpy(QWidget *parent) :
    QMainWindow(parent),
    wk_gui(0),
    p_rate(THREAD_POLL_RATE),
    _tray_graphics(QIcon(":/icons/icons/spy-icon.svg"))
{
    tray = new QSystemTrayIcon(_tray_graphics, this);
    tray_menu = new QMenu(this);
    config_action = new QAction("&Configure", tray_menu);
    quit_action = new QAction("&Quit...", tray_menu);

    /*
     * Add menu entries.
     */
    tray_menu->addAction(config_action);
    tray_menu->addSeparator();
    tray_menu->addAction(quit_action);

    /*
     * Connect signals.
     */
    connect(quit_action, SIGNAL(triggered()), this, SLOT(stop_tray()));
    connect(config_action, SIGNAL(triggered()), this, SLOT(open_wk_gui()));
    tray->setContextMenu(tray_menu);

    /*
     * Init thread monitor. Which we use to stop/start SVN threads.
     */
    monitor = new ThreadMonitor(&listenpaths,
                                &listenpathsmutex,
                                &p_rate,
                                &p_rate_mutex,
                                this);
    monitor->start();

    qRegisterMetaType<SpyNotifications>("SpyNotifications"); // Register our enum as meta type to allow signal and slots with the type.
    connect(monitor, SIGNAL(send_notif(QString,SpyNotifications)), this, SLOT(displaynotific(QString,SpyNotifications)));

    restore_listen_paths();
    tray->show();
}

SubversionSpy::~SubversionSpy()
{
    qDebug() << "Destructing SubversionSpy instance.";
    delete tray;
    save_listen_paths();
}

QVector<QString> *SubversionSpy::get_listen_paths(QMutex** mutex)
{
    *mutex = &listenpathsmutex;
    return &listenpaths;
}

bool SubversionSpy::save_listen_paths()
{
    QVector<QString> *paths = 0;
    QMutex *mutex = 0;
    QSettings cfg("SubversionSpy Inc.", "Subversion Spy");
    QStringList compat_list;

    paths = get_listen_paths(&mutex);

    if (!mutex)
        return false;

    mutex->lock();

    if (paths){
        QVectorIterator<QString> path_itr(*paths);
        while (path_itr.hasNext()){
            compat_list << path_itr.next();
        }

        cfg.setValue("paths", compat_list);
    }

    mutex->unlock();
    return true;
}

void SubversionSpy::restore_listen_paths()
{
    QSettings cfg("SubversionSpy Inc.", "Subversion Spy");
    QStringList compat_list = cfg.value("paths").toStringList();

    listenpathsmutex.lock();
    for (int32_t i = 0; i < compat_list.size(); i++){
        listenpaths << compat_list[i];
    }
    listenpathsmutex.unlock();
}

QVector<NotificationEntry> *SubversionSpy::get_all_notific(QMutex **mutex)
{
    *mutex = &notilogs_mutex;
    return &notilogs;
}

QVector<NotificationEntry> *SubversionSpy::get_n_notific(uint32_t amount)
{
    notilogs_mutex.lock();
    size_t log_size = notilogs.size();
    notilogs_mutex.unlock();
    return get_n_notific(amount, amount - log_size);
}

QVector<NotificationEntry> *SubversionSpy::get_n_notific(uint32_t amount, uint32_t offset)
{
    QVector<NotificationEntry> *log_copy = new QVector<NotificationEntry>;

    notilogs_mutex.lock();
    size_t log_size = notilogs.size();

    // Deep copy of selected notifications logs.
    for (size_t i = offset; i < amount; i++)
    {
        if (i < log_size)
        {
            log_copy->append(notilogs[i]);
        }
        else
        {
            // Trying to access out of boundary index.
            break;
        }
    }

    notilogs_mutex.unlock();
    return log_copy;
}

void SubversionSpy::setpollrate(uint32_t secs)
{
    p_rate_mutex.lock();
    p_rate = secs;
    p_rate_mutex.unlock();
}

uint32_t SubversionSpy::getpollrate()
{
    uint32_t p_rate_retn = 0;

    p_rate_mutex.lock();
    p_rate_retn = p_rate;
    p_rate_mutex.unlock();
    return p_rate_retn;
}

ThreadMonitor *SubversionSpy::get_threadmonitor()
{
    return monitor;
}

void SubversionSpy::stop_tray()
{
    qDebug() << "Stopping tray.";
    tray->hide();
    tray_menu->close();
    close();
    qApp->quit();
}

void SubversionSpy::displaynotific(QString msg, SpyNotifications type)
{
    // Is there a message?
    if (msg.size() <= 0)
        return;

    // Add to notification log.
    NotificationEntry new_entry;
    new_entry.msg = msg;
    new_entry.type = type;
    notilogs_mutex.lock();
    notilogs.append(new_entry);
    notilogs_mutex.unlock();

    switch (type){
    case N_PARSE_PROBLEMS:
        tray->showMessage("Subversion Spy - Parse Problems", msg);
        break;

    case N_UPDATED_REPOSITORY:
        tray->showMessage("Subversion Spy - Updated Repository", msg);
        break;

    case N_ADDED_REPOSITORY:
        tray->showMessage("Subversion Spy - New Repository", msg);
        break;

    case N_NEW_REVISON:
        tray->showMessage("Subversion Spy - Check in alert!", msg);
        break;

    case N_NOT_SVN_DIR:
        tray->showMessage("Subversion Spy - Not a repository", msg);
        break;
    }
}

void SubversionSpy::open_wk_gui()
{
    if (!wk_gui){
        wk_gui = new QWebView;
        wk_gui->setAttribute(Qt::WA_DeleteOnClose);
        connect(wk_gui,SIGNAL(destroyed()), this, SLOT(wk_gui_closedcb()));
        wk_bridge = new SpyWkBridge(this, wk_gui);

#ifdef DBG
        wk_gui->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif

        wk_gui->page()->mainFrame()->addToJavaScriptWindowObject("SpyWkBridge", wk_bridge);
        wk_gui->load(QUrl("qrc:///wkGui/wkGui/index.html"));
        wk_gui->setFixedSize(WK_WINDOW_WIDTH, WK_WINDOW_HEIGHT);
        wk_gui->setWindowTitle(WK_WINDOW_NAME);
        wk_gui->setWindowIcon(QIcon(":/icons/icons/spy-icon.png"));
        wk_gui->show();
    } else {
        wk_gui->show();
        wk_gui->setFocus();
    }

}

void SubversionSpy::wk_gui_closedcb()
{
    wk_gui = 0;
    save_listen_paths();
}

}
