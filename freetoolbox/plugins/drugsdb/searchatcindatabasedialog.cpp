/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "searchatcindatabasedialog.h"
#include <drugsdb/tools.h>

#include <coreplugin/icore.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>

#include "ui_searchatcindatabasedialog.h"

#include <QSqlQueryModel>
#include <QModelIndexList>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>

#include <QDebug>

using namespace DrugsDB;

static const char *S_LANGUAGE = "SearchAtcDialogLanguage";

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString databaseAbsPath()  {return QString();}//DrugsDB::Tools::drugsDatabaseAbsFileName();}

namespace DrugsDB {
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

    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
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
