#include "listenerpathform.h"
#include "ui_listenerpathform.h"

ListenerPathForm::ListenerPathForm(QWidget *parent,
                                   QVector<QString> *listenerPaths,
                                   QMutex *listenerPathsMutex) :
    QDialog(parent),
    ui(new Ui::ListenerPathForm),
    listenerPaths(listenerPaths),
    listenerPathsMutex(listenerPathsMutex)
{
    ui->setupUi(this);
    this->setFixedSize(this->size()); // Don't allow resize.

    setWindowTitle(WINDOW_TITLE);
    setWindowIcon(QIcon(":/icons/res/spy-icon.png"));
    updateListing();
}

ListenerPathForm::~ListenerPathForm()
{
    delete ui;
}

void ListenerPathForm::updateListing()
{
    listenerPathsMutex->lock();
    ui->listWidget->clear();
    QVectorIterator<QString> pathIt(*listenerPaths);

    while (pathIt.hasNext())
    {
        ui->listWidget->insertItem(0, pathIt.next());
    }
    listenerPathsMutex->unlock();
}

void ListenerPathForm::on_browseButton_clicked()
{
    QString selectedDir = QFileDialog::getExistingDirectory(this);
    if (selectedDir.length() != 0) listenerPaths->append(selectedDir);
    updateListing();
}

void ListenerPathForm::on_addButton_clicked()
{
    const QString pathToAdd = ui->pathEdit->text();
    if (pathToAdd.length() != 0)
    {
        listenerPathsMutex->lock();
        listenerPaths->append(pathToAdd);
        listenerPathsMutex->unlock();
        updateListing();
    }
}

void ListenerPathForm::on_saveButton_clicked()
{
    done(QDialog::Accepted);
}

void ListenerPathForm::on_removeButton_clicked()
{
    QList<QListWidgetItem*> selectedListeners = ui->listWidget->selectedItems();
    QListIterator<QListWidgetItem*> selectedListenersIt(selectedListeners);
    while (selectedListenersIt.hasNext())
    {
        const QString listenerPathToRemove = selectedListenersIt.next()->text();

        listenerPathsMutex->lock();
        QVectorIterator<QString> listenerPathsIt(*listenerPaths);

        int32_t index = 0;
        while (listenerPathsIt.hasNext())
        {
            if (listenerPathsIt.next() == listenerPathToRemove)
            {
                listenerPaths->remove(index);
                break;
            }
            index++;
        }
        listenerPathsMutex->unlock();

        updateListing();
    }
}
