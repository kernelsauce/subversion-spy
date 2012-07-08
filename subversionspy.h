#ifndef SUBVERSIONSPY_H
#define SUBVERSIONSPY_H

#include <QtGui/QMainWindow>
#include <QDebug>
#include <subversionparser.h>
#include <stdint.h>

namespace Spy{

class SubversionSpy : public QMainWindow
{
    Q_OBJECT
    
public:
    SubversionSpy(QWidget *parent = 0);
    ~SubversionSpy();

private:

};
}

#endif // SUBVERSIONSPY_H
