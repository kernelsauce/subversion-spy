#ifndef SUBVERSIONWORKER_H
#define SUBVERSIONWORKER_H

#include <QObject>
#include <QVector>
#include <stdint.h>
#include "subversionparser.h"

namespace Spy{

class SubversionWorker : public QThread
{
    Q_OBJECT
public:
    explicit SubversionWorker(QString path = NULL);
    ~SubversionWorker();
    QString getWorkingPath();
    
private:
    SubversionParserSyncro parser;
    QVector<SubversionLog> svnLogs;
    uint64_t lastRevNumber;
    QString path;
    bool kill;

signals:
    void giveUserFeedback(QString message, SpyNotifications type);
    
protected:
    void run();

};

}
#endif // SUBVERSIONWORKER_H
