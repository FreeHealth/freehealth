#include "searchatcindatabasedialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>

#include "ui_searchatcindatabasedialog.h"

#include <QSqlTableModel>
#include <QModelIndexList>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>

#include <QDebug>


using namespace DrugsDbCreator;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString iamDatabaseAbsPath()  {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}

namespace DrugsDbCreator {
    class SearchAtcInDatabaseDialogPrivate
    {
    public:
        QSqlTableModel *m_Model;
        QString m_LangFilter;
    };
}


SearchAtcInDatabaseDialog::SearchAtcInDatabaseDialog(QWidget *parent, const QString &term) :
    QDialog(parent),
    ui(new Ui::SearchAtcInDatabaseDialog),
    d(new SearchAtcInDatabaseDialogPrivate)
{
    ui->setupUi(this);
    ui->initial->setText(term);

    if (!Core::Tools::connectDatabase(Core::Constants::IAM_DATABASE_NAME, iamDatabaseAbsPath()))
        Utils::Log::addError(this, "unable to connect IAM database", __FILE__, __LINE__);

    d->m_Model = new QSqlTableModel(this, QSqlDatabase::database(Core::Constants::IAM_DATABASE_NAME));
    d->m_Model->setTable("ATC");
    ui->tableView->setModel(d->m_Model);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->term->setText(term);
    on_lang_currentIndexChanged(ui->lang->currentText());
}

SearchAtcInDatabaseDialog::~SearchAtcInDatabaseDialog()
{
    delete ui;
    delete d->m_Model;
    delete d;
}

void SearchAtcInDatabaseDialog::on_term_textChanged(const QString &text)
{
    d->m_Model->setFilter(QString("%1 LIKE '%%2%'").arg(ui->lang->currentText()).arg(text));
    ui->searchLabel->setText(d->m_Model->filter() + " // " + QString::number(d->m_Model->rowCount()));
    d->m_Model->select();
}

void SearchAtcInDatabaseDialog::on_lang_currentIndexChanged(const QString &text)
{
    d->m_Model->setFilter(QString("%1 LIKE '%%2%'").arg(text).arg(ui->term->text()));
    d->m_Model->select();
    // update tableView visible columns
    for(int i = 0; i < d->m_Model->columnCount(); ++i) {
        ui->tableView->setColumnHidden(i, true);
    }
    ui->tableView->setColumnHidden(1, false);
    ui->tableView->setColumnHidden(d->m_Model->database().record("ATC").indexOf(text), false);
}

void SearchAtcInDatabaseDialog::on_tableView_activated(const QModelIndex &index)
{
    QDialog::done(QDialog::Accepted);
}

QStringList SearchAtcInDatabaseDialog::getSelectedCodes()
{
    if (!ui->tableView->selectionModel()->hasSelection())
        return QStringList();
    QModelIndexList list = ui->tableView->selectionModel()->selectedIndexes();
    QStringList ret;
    foreach(const QModelIndex &idx, list) {
        ret << d->m_Model->index(idx.row(), 1).data().toString();
    }
    ret.removeDuplicates();
    qSort(ret);
    return ret;
}

void SearchAtcInDatabaseDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
