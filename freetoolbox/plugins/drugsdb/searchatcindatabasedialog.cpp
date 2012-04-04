#include "searchatcindatabasedialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>

#include "ui_searchatcindatabasedialog.h"

#include <QSqlQueryModel>
#include <QModelIndexList>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>

#include <QDebug>


using namespace DrugsDbCreator;

static const char *S_LANGUAGE = "SearchAtcDialogLanguage";

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString databaseAbsPath()  {return Core::Tools::drugsDatabaseAbsFileName();}

namespace DrugsDbCreator {
    class SearchAtcInDatabaseDialogPrivate
    {
    public:
        QSqlQueryModel *m_Model;
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

    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        Utils::Log::addError(this, "unable to connect database", __FILE__, __LINE__);

    d->m_Model = new QSqlQueryModel(this);
    ui->tableView->setModel(d->m_Model);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->term->setText(term);
    ui->lang->setCurrentIndex(settings()->value(::S_LANGUAGE).toInt());
}

SearchAtcInDatabaseDialog::~SearchAtcInDatabaseDialog()
{
    settings()->setValue(::S_LANGUAGE, ui->lang->currentIndex());
    delete ui;
    delete d->m_Model;
    delete d;
}

void SearchAtcInDatabaseDialog::setFilter()
{
    QString lang = ui->lang->currentText().left(2).toLower();
    const QString &term = ui->term->text();
    QString req = QString("SELECT ATC.CODE, LABELS.LABEL FROM ATC "
                          "JOIN ATC_LABELS ON ATC_LABELS.ATC_ID=ATC.ATC_ID "
                          "JOIN LABELS_LINK ON LABELS_LINK.MASTER_LID=ATC_LABELS.MASTER_LID "
                          "JOIN LABELS ON LABELS_LINK.LID=LABELS.LID "
                          "WHERE LABELS.LANG=\"%1\" AND LABELS.LABEL like \"%%2%\"; ")
            .arg(lang).arg(term);

    d->m_Model->setQuery(req, QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME));

    ui->searchLabel->setText("Found: " + QString::number(d->m_Model->rowCount()));
}

void SearchAtcInDatabaseDialog::on_term_textChanged(const QString &)
{
    setFilter();
}

void SearchAtcInDatabaseDialog::on_lang_currentIndexChanged(const QString &)
{
    setFilter();
//    d->m_Model->setFilter(QString("%1 LIKE '%%2%'").arg(text).arg(ui->term->text()));
//    d->m_Model->select();
//    // update tableView visible columns
}

void SearchAtcInDatabaseDialog::on_tableView_activated(const QModelIndex &)
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
        ret << d->m_Model->index(idx.row(), 0).data().toString();
    }
    ret.removeDuplicates();
    qSort(ret);
    return ret;
}

QStringList SearchAtcInDatabaseDialog::getSelectedLabels()
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
