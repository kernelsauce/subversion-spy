#ifndef LISTENERPATHFORM_H
#define LISTENERPATHFORM_H

#include <QVector>
#include <QVectorIterator>
#include <QDebug>
#include <QString>
#include <QDialog>
#include <QMutex>
#include <QFileDialog>
#include <stdint.h>

#define WINDOW_TITLE "Subversion Spy - Modify observer paths"

namespace Ui {
class ListenerPathForm;
}

class ListenerPathForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit ListenerPathForm(QWidget *parent = 0,
                              QVector<QString>* listenerPaths = NULL,
                              QMutex* listenerPathsMutex = NULL);
    ~ListenerPathForm();
    QVector<QString> getSelectedPaths();
    
private slots:
    void on_browseButton_clicked();
    void on_addButton_clicked();
    void on_saveButton_clicked();

    void on_removeButton_clicked();

private:
    Ui::ListenerPathForm *ui;
    QVector<QString>* listenerPaths;
    QMutex* listenerPathsMutex;
    void updateListing();
};

#endif // LISTENERPATHFORM_H
