#include "subversionworker.h"

namespace Spy{

SubversionWorker::SubversionWorker(QString path,
                                   uint32_t *p_rate,
                                   QMutex *p_rate_mutex,
                                   QObject *parent) :
    QThread(parent),
    parser(SubversionParserSyncro(path, false)),
    last_rev(0),
    path(path),
    p_rate(p_rate),
    kill(false)
{
    this->p_rate_mutex = p_rate_mutex;
    set_state(S_WAIT);
}

SubversionWorker::~SubversionWorker()
{
    qDebug() << "Killing SubversionWorkerThread: " << get_thread_id();
    set_state(S_DIEING);
    kill_mutex.lock();
    kill = true;
    kill_mutex.unlock();
    wait();
}

QString SubversionWorker::get_working_path()
{
    return path;
}

SubversionWorkerState SubversionWorker::get_state()
{
    SubversionWorkerState returnState;
    state_mutex.lock();
    returnState = state;
    state_mutex.unlock();
    return returnState;
}

uint32_t SubversionWorker::get_thread_id()
{
    return thread_id;
}

QVector<SubversionLog> *SubversionWorker::get_logs(QMutex** vector_mutex)
{
    *vector_mutex = &svn_logs_mutex;
    return &svn_logs;
}

void SubversionWorker::set_state(SubversionWorkerState state)
{
    state_mutex.lock();
    this->state = state;
    state_mutex.unlock();
}

void SubversionWorker::handle_new_logs(QVector<SubversionLog> *fresh_logs)
{
    set_state(S_UPDATING);
    if (!fresh_logs->isEmpty()){
        svn_logs_mutex.lock();
        svn_logs << *fresh_logs;
        svn_logs_mutex.unlock();

        // New logs incoming!
        QVectorIterator<SubversionLog> it(*fresh_logs);
        while (it.hasNext()){
            SubversionLog freshLog = it.next();
            last_rev = freshLog.revNumber;
            emit give_feedback("Caught check in at " + path + ". \r\n\r\n"
                        "Revision number: r" + QString::number(last_rev) + "\r\n" +
                        "Author: " + freshLog.author + "\r\n" +
                        "Comment: " + freshLog.comment + "\r\n",
                        N_UPDATED_REPOSITORY);
        }
    }
}

bool SubversionWorker::fetch_initial_logs()
{
    set_state(S_UPDATING);
    QVector<SubversionLog> full_log = parser.get_logs();
    if (full_log.isEmpty()) return false; // Empty logs, just bail.

    svn_logs_mutex.lock();
    svn_logs << full_log; // Append to logs.
    svn_logs_mutex.unlock();

    SubversionLog last_log = full_log.front();
    last_rev = last_log.revNumber;

    // Update user about success.
    emit give_feedback("Full log for " + path + " fetched. \r\n\r\n"
                "Last revision number is r" + QString::number(last_rev) + "\r\n" +
                "Author: " + last_log.author + "\r\n" +
                "Comment: " + last_log.comment + "\r\n",
                N_UPDATED_REPOSITORY);

    return true;
}

void SubversionWorker::run()
{
    thread_id = (uint32_t)this->currentThreadId(); // TODO: surivive without mutex?

    if (last_rev == 0){
        if (!parser.update()){
            set_state(S_ERROR);
            emit give_feedback("Could not update: " + path, N_NOT_SVN_DIR);
            return;
        }
    }

    fetch_initial_logs();

    /*
     * Main event loop of the Subversion Worker.
     */
    while(1){
        QVector<SubversionLog> fresh_logs;
        try{
            fresh_logs = parser.get_logs(last_rev + 1, -1);
        }
        catch(ParserException e){
            set_state(S_ERROR);
            emit give_feedback("Exception: " + QString::number(e) +
                                  " while parsing: " + path, N_PARSE_PROBLEMS);
            break;
        }

        handle_new_logs(&fresh_logs);

        // If kill is set, then we let the thread suspend.
        kill_mutex.lock();
        if (!kill){
            kill_mutex.unlock();
            set_state(S_WAIT);
            p_rate_mutex->lock();
            uint32_t current_p_rate = *p_rate;
            p_rate_mutex->unlock();
            sleep(current_p_rate);
        } else {
            kill_mutex.unlock();
            break;
        }

    }
}

}
