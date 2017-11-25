/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "searchatcindatabasedialog.h"
#include <ddiplugin/constants.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/database/ddidatabase.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>

#include "ui_searchatcindatabasedialog.h"

#include <QSqlQueryModel>
#include <QModelIndexList>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>

#include <QDebug>

using namespace DDI;

static const char *S_LANGUAGE = "SearchAtcDialogLanguage";
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DDI::Internal::DDIDatabase &ddiBase() {return DDI::DDICore::instance()->database();}

namespace DDI {
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

    d->m_Model = new QSqlQueryModel(this);
    ui->tableView->setModel(d->m_Model);
    ui->term->setText(term);
    ui->lang->setCurrentIndex(settings()->value(::S_LANGUAGE).toInt());

    setWindowTitle(tr("Search ATC code"));
    // setWindowIcon(theme()->icon());
    Utils::resizeAndCenter(this);
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
    QString term = ui->term->text();
    QHash<int, QString> where;
    QString cond = QString("LIKE \"%1%\"").arg(term.remove("\""));
    int field = Constants::ATC_FR;
    if (lang=="en")
        field = Constants::ATC_EN;
    else if (lang=="de")
        field = Constants::ATC_DE;
    where.insert(field, cond);

    QString req = ddiBase().select(Constants::Table_ATC, QList<int>()
                                   << Constants::ATC_FR
                                   << Constants::ATC_EN
                                   << Constants::ATC_DE
                                   << Constants::ATC_CODE, where);
    d->m_Model->setQuery(req, ddiBase().database());
    ui->searchLabel->setText("Found: " + QString::number(d->m_Model->rowCount()));

    ui->tableView->horizontalHeader()->setStretchLastSection(false);
    ui->tableView->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
}

void SearchAtcInDatabaseDialog::on_term_textChanged(const QString &)
{
    setFilter();
}

void SearchAtcInDatabaseDialog::on_lang_currentIndexChanged(const QString &)
{
    setFilter();
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
    foreach(const QModelIndex &idx, list)
        ret << d->m_Model->index(idx.row(), 3).data().toString();
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
    QString lang = ui->lang->currentText().left(2).toLower();
    int field = 0;
    if (lang=="en")
        field = 1;
    else if (lang=="de")
        field = 2;
    foreach(const QModelIndex &idx, list)
        ret << d->m_Model->index(idx.row(), field).data().toString();
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
