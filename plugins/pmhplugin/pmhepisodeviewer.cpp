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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Authors:                                                               *
 *  Eric MAEKER, <eric.maeker@gmail.com>                                   *
 *  NAME <MAIL@ADDRESS.COM>                                                *
 ***************************************************************************/
/**
  \class PmhEpisodeViewer
  \brief PMHx episode viewer widget. Allow to show, modify, add & remove PMH
  episodes.

  This class uses TableView class to create the view.
*/

#include "pmhepisodeviewer.h"
#include "pmhepisodemodel.h"
#include "pmhdata.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <icdplugin/icdcollectiondialog.h>

#include <utils/widgets/datetimedelegate.h>
#include <utils/log.h>

#include <QDialog>


#include "ui_pmhepisodeviewer.h"

using namespace PMH;
using namespace Internal;


static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }


namespace PMH {
namespace Internal {
class PmhEpisodeViewerPrivate
{
public:
    PmhEpisodeViewerPrivate() : m_Pmh(0) {}

public:
    PmhData *m_Pmh;
};
}  // End namespace Internal
}  // End namespace PMH

// TODO: add Contacts
PmhEpisodeViewer::PmhEpisodeViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PmhEpisodeViewer),
    d(new Internal::PmhEpisodeViewerPrivate)
{
    ui->setupUi(this);

    // ICD10 coding
    connect(ui->tableView, SIGNAL(activated(QModelIndex)),
            this, SLOT(itemActivated(QModelIndex)));
}

PmhEpisodeViewer::~PmhEpisodeViewer()
{
    delete ui;
    if (d)
        delete d;
    d = 0;
}

void PmhEpisodeViewer::setPmhData(Internal::PmhData *pmh)
{
    Q_ASSERT(pmh);
    if (!pmh)
        return;
    if (patient()) {
        // Create date delegates
        Utils::DateTimeDelegate *start = new Utils::DateTimeDelegate(this, true);
        Utils::DateTimeDelegate *end = new Utils::DateTimeDelegate(this, true);
        ui->tableView->tableView()->setItemDelegateForColumn(PmhEpisodeModel::DateStart, start);
        ui->tableView->tableView()->setItemDelegateForColumn(PmhEpisodeModel::DateEnd, end);

        QDate birth = patient()->data(Core::IPatient::DateOfBirth).toDate();
        if (birth.isValid()) {
            start->setDateRange(birth, birth.addYears(150));
            end->setDateRange(birth, birth.addYears(150));
        }
    }
    d->m_Pmh = pmh;
    // If SimpleView -> test pmhepisodedata -> create a default one
    if (pmh->episodeModel()->rowCount()==0) {
        pmh->episodeModel()->insertRow(0);
    }
    ui->tableView->setModel(pmh->episodeModel());

    connect(ui->tableView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(resizeTableView()));
    connect(ui->tableView->model(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(resizeTableView()));

    ui->tableView->hideColumn(PmhEpisodeModel::IcdXml);
    ui->tableView->hideColumn(PmhEpisodeModel::Contact);
    ui->tableView->hideColumn(PmhEpisodeModel::EmptyColumn);
    //ui->tableView->horizontalHeader()->setSectionResizeMode(PmhEpisodeModel::Label, QHeaderView::Stretch);
    //ui->tableView->horizontalHeader()->setSectionResizeMode(PmhEpisodeModel::DateEnd, QHeaderView::Stretch);
    //ui->tableView->horizontalHeader()->setSectionResizeMode(PmhEpisodeModel::DateStart, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    //ui->tableView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setVisible(false);

    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    resizeTableView();
}

void PmhEpisodeViewer::clear()
{
    ui->tableView->setModel(0);
}

void PmhEpisodeViewer::itemActivated(const QModelIndex &item)
{
    if (item.column()==PmhEpisodeModel::IcdCodeList) {
        // create an ICD10 collection dialog
        ICD::IcdCollectionDialog dlg(this);
        // get the XML ICD10 coding
        QString xml = item.model()->index(item.row(), PmhEpisodeModel::IcdXml).data(Qt::EditRole).toString();
//        LOG(xml);
        dlg.setXmlIcdCollection(xml);
        if (dlg.exec()==QDialog::Accepted) {
            // retrieve selected codes to the PmhEpisodeModel
            d->m_Pmh->episodeModel()->setData(item.model()->index(item.row(), PmhEpisodeModel::IcdXml), dlg.xmlIcdCollection());
        }
    }
}

void PmhEpisodeViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  Resize tableView to display all episodes. Called during view creation and when
  a new episode is added or removed.
*/
void PmhEpisodeViewer::resizeTableView()
{
    const int nNumRows = ui->tableView->model()->rowCount();
    int nRowHeight = ui->tableView->tableView()->rowHeight(0);
    int nHorizontalHeaderHeight = ui->tableView->horizontalHeader()->height();
    int nTableHeight = ((nNumRows + 1) * nRowHeight) + nHorizontalHeaderHeight +
                       2 * ui->tableView->tableView()->lineWidth();
    if (nNumRows > 0) {
    ui->tableView->setMinimumHeight(nTableHeight + 2);
    ui->tableView->setMaximumHeight(nTableHeight + 2);
    }
}
