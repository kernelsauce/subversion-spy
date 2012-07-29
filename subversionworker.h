#ifndef SUBVERSIONWORKER_H
#define SUBVERSIONWORKER_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <stdint.h>
#include "subversionparser.h"

namespace Spy{

class SubversionWorker : public QThread
{
    Q_OBJECT
public:
    explicit SubversionWorker(QString path = NULL,
                              uint32_t *pollRate = NULL,
                              QMutex* pollRateMutex = NULL);
    ~SubversionWorker();
    QString getWorkingPath();
    SubversionWorkerState getState();
    uint32_t getThreadId();
    
private:
    SubversionParserSyncro parser;
    QVector<SubversionLog> svnLogs;
    uint64_t lastRevNumber;
    QString path;
    uint32_t* pollRate;
    QMutex* pollRateMutex;
    bool kill;
    QMutex killMutex;
    SubversionWorkerState state;
    QMutex stateMutex;
    uint32_t threadId;

    void handleNewLogs(QVector<SubversionLog> *freshLogs);
    bool initialLogFetch();
    void setState(SubversionWorkerState state);

signals:
    void giveUserFeedback(QString message, SpyNotifications type);
    
protected:
    void run();

};

}
#endif // SUBVERSIONWORKER_H
