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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "dosagedialog.h"

#include <drugsbaseplugin/dosagemodel.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/constants.h>

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidget/druginfo.h>
#include <drugsplugin/drugswidgetmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

// include Qt headers
#include <QHeaderView>
#include <QRadioButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QSpinBox>

using namespace DrugsWidget::Constants;
using namespace DrugsWidget::Internal;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }

namespace DrugsWidget {
namespace Internal {

class DosageDialogPrivate
{
public:
    DosageDialogPrivate() : m_DosageModel(0), m_UserFormButtonPopup(0) {}

    DosageModel*m_DosageModel;
    QString m_ActualDosageUuid;
    QVariant m_DrugUid;
    int m_DrugRow;
    QMenu *m_UserFormButtonPopup;
};

} // namespace Internal
} // namespace DrugsWidget


/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
 \todo check compatibility between patient parameters and dosage (weight, age, clearance...)
*/


/**
  \brief Constructor
  \param parent : parent QWidget
*/
DosageDialog::DosageDialog(QWidget *parent)
    : QDialog(parent),
    d(0)
{
    setObjectName("DosageDialog");
    d = new DosageDialogPrivate();
    setupUi(this);
    innButton->setIcon(Core::ICore::instance()->theme()->icon(DrugsDB::Constants::I_SEARCHINN));
    setWindowTitle(tr("Drug Dosage") + " - " + qApp->applicationName());

    // make connections
    connect(drugModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             this, SLOT(updatePosologicSentence(QModelIndex,QModelIndex)));
}

/** \brief Destructor, frees mapper */
DosageDialog::~DosageDialog()
{
    if (d)
        delete d;
    d=0;
}

/**
  \brief Change the current row of the drug model
  \todo Manage dosagemodel
*/
void DosageDialog::changeRow(const QVariant &drugUid, const int drugRow)
{
    using namespace DrugsDB::Constants;
    Q_ASSERT(drugModel()->containsDrug(drugUid));
    d->m_DrugUid = drugUid;
    d->m_DrugRow = drugRow;
    dosageViewer->useDrugsModel(d->m_DrugUid, drugRow);
    innButton->setChecked(drugModel()->drugData(d->m_DrugUid, Prescription::IsINNPrescription).toBool());

    // retrieve drug information before drugmodel changes
    QString name = drugModel()->drugData(d->m_DrugUid, Drug::Denomination).toString();
    if (drugModel()->drugData(d->m_DrugUid, Prescription::IsINNPrescription).toBool())
        drugNameButton->setText(drugModel()->drugData(d->m_DrugUid, Drug::InnCompositionString).toString());
    else
        drugNameButton->setText(name);
    QString toolTip = drugModel()->drugData(d->m_DrugUid, Interaction::ToolTip).toString();
    iconInteractionLabel->setToolTip(toolTip);
    iconInteractionLabel->setPixmap(drugModel()->drugData(d->m_DrugUid, Interaction::Icon).value<QIcon>().pixmap(16,16));
    toolTip = drugModel()->drugData(d->m_DrugUid, Drug::CompositionString).toString();
    drugNameButton->setToolTip(toolTip);
    innButton->setEnabled(drugModel()->drugData(d->m_DrugUid, Drug::AllInnsKnown).toBool());
}

/**
  \brief Closes the dialog.
  \li If the dialog is accepted, retrieve the prescribed form and store it into the settings is needed.
*/
void DosageDialog::done(int r)
{
    // modify focus for the dosage viewer mapper to commit changes
    drugNameButton->setFocus();

    disconnect(drugModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             this, SLOT(updatePosologicSentence(QModelIndex,QModelIndex)));

    dosageViewer->commitToModel();

    if (r == QDialog::Accepted) {
        dosageViewer->done(r);
    }
    QDialog::done(r);
}

/** \brief Show the information dialog for the drug */
void DosageDialog::on_drugNameButton_clicked()
{
    DrugInfo dialog(d->m_DrugUid, this);
    dialog.exec();
}

void DosageDialog::on_innButton_clicked()
{
    using namespace DrugsDB::Constants;
    drugModel()->setDrugData(d->m_DrugUid, Prescription::IsINNPrescription, innButton->isChecked());
    if (innButton->isChecked())
        drugNameButton->setText(drugModel()->drugData(d->m_DrugUid, Drug::InnCompositionString).toString());
    else {
        QString name = drugModel()->drugData(d->m_DrugUid, Drug::Denomination).toString();
        drugNameButton->setText(name);
    }
}

void DosageDialog::updatePosologicSentence(const QModelIndex &, const QModelIndex &)
{
    resultTextBrowser->setPlainText(drugModel()->data(drugModel()->index(d->m_DrugRow, DrugsDB::Constants::Drug::FullPrescription)).toString());
}
