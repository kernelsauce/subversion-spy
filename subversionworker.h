#ifndef SUBVERSIONWORKER_H
#define SUBVERSIONWORKER_H

#include <QObject>
#include <QVector>
#include <stdint.h>
#include "subversionparser.h"

namespace Spy{

class SubversionWorker : public QObject
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

signals:
    void sendNotification(QString message, SpyNotifications type);
    
public slots:
    void process();

};

}
#endif // SUBVERSIONWORKER_H
