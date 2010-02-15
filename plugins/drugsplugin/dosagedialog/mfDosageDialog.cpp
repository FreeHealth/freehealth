/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mfDosageDialog.h"

#include <drugsbaseplugin/dosagemodel.h>
#include <drugsbaseplugin/drugsmodel.h>

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
#include <QDataWidgetMapper>
#include <QMessageBox>

using namespace DrugsWidget::Constants;
//using namespace mfDosagesConstants;
//using namespace mfInteractionsConstants;
using namespace DrugsWidget::Internal;

inline static DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }

namespace DrugsWidget {
namespace Internal {

class DosageDialogPrivate
{
public:
    DosageDialogPrivate() : m_DosageModel(0), m_CIS(-1), m_UserFormButtonPopup(0) {}

    DosageModel*m_DosageModel;
    QString     m_ActualDosageUuid;
    int m_CIS, m_DrugRow;
    QMenu *m_UserFormButtonPopup;
};

} // namespace Internal
} // namespace DrugsWidget


/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
*/
// **************************************************************************************************************
// TODO : with FreeMedForms check compatibility between patient parameters and dosage (weight, age, clearance...)
// **************************************************************************************************************

/**
  \brief Constructor
  \param parent : parent QWidget
*/
DosageDialog::DosageDialog(QWidget *parent)
    : QDialog( parent ),
    d(0)
{
    setObjectName("DosageDialog");
    d = new DosageDialogPrivate();
    setupUi(this);
    innButton->setIcon(Core::ICore::instance()->theme()->icon(DrugsDB::Constants::I_SEARCHINN));
    setWindowTitle(tr("Drug Dosage") + " - " + qApp->applicationName() );

    // make connections
    connect( drugModel(), SIGNAL(prescriptionResultChanged(const QString &)),
             resultTextBrowser, SLOT(setPlainText(const QString &)));
}

/** \brief Destructor, frees mapper */
DosageDialog::~DosageDialog()
{
    if (d) delete d; d=0;
}

/**
  \brief Change the current row of the drug model
  \todo Manage dosagemodel
*/
void DosageDialog::changeRow( const int CIS, const int drugRow )
{
    using namespace DrugsDB::Constants;
    Q_ASSERT(drugModel()->containsDrug(CIS));
    d->m_CIS = CIS;
    d->m_DrugRow = drugRow;
    dosageViewer->useDrugsModel(CIS, drugRow);
    innButton->setChecked(drugModel()->drugData( d->m_CIS, Prescription::IsINNPrescription).toBool() );

    // retreive drug informations before drugmodel changes
    QString name = drugModel()->drugData(CIS, Drug::Denomination).toString();
    if (drugModel()->drugData(CIS, Prescription::IsINNPrescription).toBool())
        drugNameButton->setText(drugModel()->drugData(d->m_CIS, Drug::InnCompositionString).toString());
    else
        drugNameButton->setText(name.left( name.lastIndexOf(",")));
    QString toolTip = drugModel()->drugData(CIS, Interaction::ToolTip ).toString();
    iconInteractionLabel->setToolTip( toolTip );
    iconInteractionLabel->setPixmap( drugModel()->drugData(CIS, Interaction::Icon ).value<QIcon>().pixmap(16,16) );
    toolTip = drugModel()->drugData(CIS, Drug::CompositionString ).toString();
    drugNameButton->setToolTip( toolTip );
    innButton->setEnabled( drugModel()->drugData(CIS, Drug::AllInnsKnown ).toBool() );
}

/**
  \brief Closes the dialog.
  \li If the dialog is accepted, retreive the prescribed form and store it into the settings is needed.
  \todo If the dialog is accepted and no dosage exists in the dosage model --> create a dosage in the dosage model
*/
void DosageDialog::done(int r)
{
    // modify focus for the dosage viewer mapper to commit changes
    drugNameButton->setFocus();
    dosageViewer->commitToModel();

    if ( r == QDialog::Accepted ) {
        dosageViewer->done(r);
    }
    QDialog::done(r);
}

/** \brief Show the information dialog for the drug */
void DosageDialog::on_drugNameButton_clicked()
{
    DrugInfo dialog(d->m_CIS, this );
    dialog.exec();
}

void DosageDialog::on_innButton_clicked()
{
    using namespace DrugsDB::Constants;
    drugModel()->setDrugData(d->m_CIS, Prescription::IsINNPrescription, innButton->isChecked() );
    if (innButton->isChecked())
        drugNameButton->setText(drugModel()->drugData(d->m_CIS, Drug::InnCompositionString).toString());
    else {
        QString name = drugModel()->drugData(d->m_CIS, Drug::Denomination).toString();
        drugNameButton->setText(name.left( name.lastIndexOf(",")));
    }
}

