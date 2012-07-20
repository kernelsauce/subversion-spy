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
    qDebug() << "Constr. new ListenerPathForm";
    ui->setupUi(this);
    ui->pathEdit->placeholderText();
    setFixedSize(480, 640);
    setWindowTitle("Subversion Spy - Modify observer paths");
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
