/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mfDosageDialog.h"

// include drugwidget headers
#include <mfDrugsConstants.h>
#include <drugsmodel/mfDosageModel.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugswidget/mfDrugInfo.h>
#include <mfDrugsManager.h>

// include toolkit
#include <tkLog.h>
#include <tkStringListModel.h>
#include <tkSettings.h>
#include <tkTheme.h>
#include <tkConstantTranslations.h>

// include Qt headers
#include <QHeaderView>
#include <QRadioButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QSpinBox>
#include <QDataWidgetMapper>
#include <QMessageBox>

using namespace mfDrugsConstants;
using namespace mfDosagesConstants;
using namespace mfInteractionsConstants;

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

class mfDosageDialogPrivate
{
public:
    mfDosageDialogPrivate() : m_DosageModel(0), m_CIS(-1), m_UserFormButtonPopup(0) {}

    mfDosageModel      *m_DosageModel;
    QString             m_ActualDosageUuid;
    int m_CIS, m_DrugRow;
    QMenu *m_UserFormButtonPopup;
};


/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
*/
// **************************************************************************************************************
// TODO : with FreeMedForms check compatibility between patient parameters and dosage (weight, age, clearance...)
// **************************************************************************************************************

/**
  \brief Constructor
  \param parent : parent QWidget
  \param drugRow : row of the drug inside the actual model \todo Change to QPersistentModelIndex
  \param dosageRow : row of the dosage to use in the actual model \todo Change to QPersistentModelIndex
*/
mfDosageDialog::mfDosageDialog( QWidget *parent )
    : QDialog( parent ),
    d(0)
{
    setObjectName( "mfDosageDialog" );
    d = new mfDosageDialogPrivate();
    setupUi(this);
    innButton->setIcon( tkTheme::icon(MFDRUGS_ICONSEARCHINN));
    setWindowTitle( tr( "Drug Dosage" ) + " - " + qApp->applicationName() );

    // make connections
    connect( DRUGMODEL, SIGNAL(prescriptionResultChanged(const QString &)),
             resultTextBrowser, SLOT(setPlainText(const QString &)));
}

/** \brief Destructor, frees mapper */
mfDosageDialog::~mfDosageDialog()
{
    if (d) delete d; d=0;
}

/**
  \brief Change the current row of the drug model
  \todo Manage dosagemodel
*/
void mfDosageDialog::changeRow( const int CIS, const int drugRow )
{
    Q_ASSERT(DRUGMODEL->containsDrug(CIS));
    d->m_CIS = CIS;
    d->m_DrugRow = drugRow;
    mfDrugsModel *m = DRUGMODEL;
    dosageViewer->useDrugsModel(CIS, drugRow);
    innButton->setChecked(m->drugData( d->m_CIS, Prescription::IsINNPrescription).toBool() );

    // retreive drug informations before drugmodel changes
    QString name = m->drugData(CIS, Drug::Denomination).toString();
    if (m->drugData(CIS, Prescription::IsINNPrescription).toBool())
        drugNameButton->setText(m->drugData(d->m_CIS, Drug::InnCompositionString).toString());
    else
        drugNameButton->setText(name.left( name.lastIndexOf(",")));
    QString toolTip = m->drugData(CIS, Interaction::ToolTip ).toString();
    iconInteractionLabel->setToolTip( toolTip );
    iconInteractionLabel->setPixmap( m->drugData(CIS, Interaction::Icon ).value<QIcon>().pixmap(16,16) );
    toolTip = m->drugData(CIS, Drug::CompositionString ).toString();
    drugNameButton->setToolTip( toolTip );
    innButton->setEnabled( m->drugData(CIS, Drug::AllInnsKnown ).toBool() );
}

/**
  \brief Closes the dialog.
  \li If the dialog is accepted, retreive the prescribed form and store it into the settings is needed.
  \todo If the dialog is accepted and no dosage exists in the dosage model --> create a dosage in the dosage model
*/
void mfDosageDialog::done(int r)
{
    // modify focus for the dosage viewer mapper to commit changes
    drugNameButton->setFocus();

    if ( r == QDialog::Accepted ) {
        dosageViewer->done(r);
    }
    QDialog::done(r);
}

/** \brief Show the information dialog for the drug */
void mfDosageDialog::on_drugNameButton_clicked()
{
    mfDrugInfo dialog(d->m_CIS, this );
    dialog.exec();
}

// TODO: code this */
void mfDosageDialog::on_innButton_clicked()
{
    mfDrugsModel *m = DRUGMODEL;
    m->setDrugData(d->m_CIS, Prescription::IsINNPrescription, innButton->isChecked() );
    if (innButton->isChecked())
        drugNameButton->setText(m->drugData(d->m_CIS, Drug::InnCompositionString).toString());
    else {
        QString name = m->drugData(d->m_CIS, Drug::Denomination).toString();
        drugNameButton->setText(name.left( name.lastIndexOf(",")));
    }
}
