#include "threadmonitor.h"

namespace Spy{

ThreadMonitor::ThreadMonitor(QVector<QString> *listener_paths,
                             QMutex *listener_paths_mutex,
                             uint32_t* p_rate,
                             QMutex *p_rate_mutex,
                             QObject *parent) :
    QThread(parent),
    listener_paths(listener_paths),
    listener_paths_mutex(listener_paths_mutex),
    kill(false),
    p_rate(p_rate),
    p_rate_mutex(p_rate_mutex)
{
}

ThreadMonitor::~ThreadMonitor()
{
    qDebug() << "Killing ThreadMonitor.";
    kill_mutex.lock();
    kill = true;
    kill_mutex.unlock();
    wait();

    qDeleteAll(worker_pool);
    worker_pool.empty();
}

void ThreadMonitor::run()
{
    while(1){
        kill_mutex.unlock();
        if (kill){
            kill_mutex.unlock();
            break;
        }
        kill_mutex.unlock();

        listener_paths_mutex->lock();
        QVectorIterator<QString> listener_path_itr(*listener_paths);

        // Spawn a new thread for each path that is not already there.
        while (listener_path_itr.hasNext()){
            QString listener_path = listener_path_itr.next();
            if (get_worker_by_work(listener_path) == 0){
                //propagate_notif("Added " + listener_path + " to listener.", N_ADDED_REPOSITORY);
                SubversionWorker* worker = new SubversionWorker(listener_path, p_rate, p_rate_mutex);
                worker_pool.append(worker);
                worker->start();
                connect(worker, SIGNAL(give_feedback(QString,SpyNotifications)), this, SLOT(propagate_notif(QString,SpyNotifications)));
            }
        }

        // Stop workers that no longer is in list.
        QVectorIterator<SubversionWorker*> worker_itr(worker_pool);
        int32_t i = 0;

        while (worker_itr.hasNext()){
            SubversionWorker* worker_in_pool = worker_itr.next();
            if (!is_in_pool(worker_in_pool->get_working_path())){
                if (worker_pool.size() > i){ // Check if in range.
                    listener_paths_mutex->unlock();
                    worker_pool.remove(i);
                    delete worker_in_pool;
                    listener_paths_mutex->lock();
                } else {
                    break;
                }
            }
            i++;
        }

        listener_paths_mutex->unlock();

        sleep(THREAD_MONITOR_ITERATION);
    }
}

void ThreadMonitor::propagate_notif(QString message, SpyNotifications type)
{
    emit send_notif(message, type);
}

SubversionWorker* ThreadMonitor::get_worker_by_work(QString listenerPath)
{
    QVectorIterator<SubversionWorker*> it(worker_pool);
    while (it.hasNext())
    {
        SubversionWorker* worker = it.next();
        if (worker->get_working_path() == listenerPath)
        {
            return worker;
        }
    }
    return 0;
}

inline bool ThreadMonitor::is_in_pool(QString listenerPath)
{
    bool exists = false;
    QVectorIterator<QString> path(*listener_paths);
    while (path.hasNext()){
        if (path.next() == listenerPath)
            exists = true;
    }
    return exists;
}

QVariantMap ThreadMonitor::get_threads_state()
{
    QVariantMap threads_state;

    QVectorIterator<SubversionWorker*> workerIt(worker_pool);
    while (workerIt.hasNext()){
        QVariantMap thread_state;

        SubversionWorker* workerInPool = workerIt.next();
        thread_state["state"] = workerInPool->get_state();
        thread_state["id"] = workerInPool->get_thread_id();

        threads_state[workerInPool->get_working_path()] = thread_state;
    }

    return threads_state;
}

QVector<SubversionLog> *ThreadMonitor::get_worker_logs(QString path, QMutex **vector_mutex)
{
    SubversionWorker *worker = get_worker_by_work(path);
    QVector<SubversionLog> *svn_logs = worker->get_logs(vector_mutex);
    return svn_logs;
}

} // namespace Spy
