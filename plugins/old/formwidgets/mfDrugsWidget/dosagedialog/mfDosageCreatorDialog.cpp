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

/**
  \class mfDosageCreator
  \brief Dialog for dosage creation / edition / modification. A dosage is a standard set of datas that will be used to help
  doctors when prescribing a drug.
  If you want to create a new dosage, you must create a new row onto the model BEFORE.
  If you want to edit or modify a dosage, you must inform the dialog of the row and the CIS of the drug.
  \ingroup freediams drugswidget
*/


#include "mfDosageCreatorDialog.h"

// include drugwidget headers
//#include <drugsmodel/mfDosage.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDosageModel.h>
#include <drugsmodel/mfDrugsModel.h>
#include <mfDrugsConstants.h>
#include <mfDrugsManager.h>

// include toolkit
#include <tkLog.h>
#include <tkStringListModel.h>
#include <tkTheme.h>
#include <tkSettings.h>
#include <tkConstantTranslations.h>
#include <tkHelpDialog.h>

// include Qt headers
#include <QMessageBox>
#include <QModelIndex>

using namespace mfDrugsConstants;
using namespace mfDosagesConstants;
using namespace mfInteractionsConstants;

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

/**
  \brief Private part of mfDosageDialog
  \internal
*/
class mfDosageCreatorDialogPrivate
{
public:
    mfDosageCreatorDialogPrivate(mfDosageCreatorDialog *parent) :
            m_DosageModel(0), m_Parent(parent) {}

    /** \brief Check the validity o the dosage. Warn if dosage is not valid */
    bool checkDosageValidity(const int row)
    {
        QStringList list = m_DosageModel->isDosageValid(row);
        if (list.count()) {
            tkGlobal::warningMessageBox(QCoreApplication::translate("mfDosageCreatorDialog", "Dosage is not valid."),
                                        list.join("br />"),
                                        "", QCoreApplication::translate("mfDosageCreatorDialog", "Drug Dosage Creator"));
            return false;
        }
        return true;
    }

    /** \brief Save the current dirty rows of the model to the database */
    void saveToModel()
    {
        int row = m_Parent->availableDosagesListView->listView()->currentIndex().row();
        // if Inn is checked --> clear dosage CIS, feel INN + COMPO_DOSAGE
        if (!checkDosageValidity(row))
            return;
        m_DosageModel->database().transaction();
        if (m_DosageModel->submitAll()) {
            if (m_DosageModel->database().commit())
                tkLog::addMessage(m_Parent, QCoreApplication::translate("mfDosageCreatorDialog", "Dosage correctly saved to base"));
            else
                tkLog::addError(m_Parent, QCoreApplication::translate("mfDosageCreatorDialog", "SQL Error : Dosage can not be added to database : %1")
                                .arg(m_DosageModel->lastError().text()));
        } else {
            m_DosageModel->database().rollback();
            QMessageBox::warning(m_Parent, QCoreApplication::translate("mfDosageCreatorDialog", "Drug Dosage Creator"),
                                 tkTr(ERROR_1_FROM_DATABASE_2)
                                 .arg(m_DosageModel->database().lastError().text())
                                 .arg(m_DosageModel->database().connectionName()));
        }
    }

    /** \brief Transforms the "reference dialog" to a prescription */
    void toPrescription()
    {
        int row = m_Parent->availableDosagesListView->listView()->currentIndex().row();
        m_DosageModel->toPrescription(row);
    }

public:
    mfDosageModel      *m_DosageModel;
    QString             m_ActualDosageUuid;
private:
    mfDosageCreatorDialog *m_Parent;
};


/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
 \todo use a QPersistentModelIndex instead of drugRow, dosageRow
*/
mfDosageCreatorDialog::mfDosageCreatorDialog( QWidget *parent, mfDosageModel *dosageModel )
    : QDialog( parent ),
    d(0)
{
    // some initializations
    setObjectName( "mfDosageCreatorDialog" );
    d = new mfDosageCreatorDialogPrivate(this);
    d->m_DosageModel = dosageModel;

    // Ui initialization
    setupUi(this);
    setWindowTitle( tr( "Drug Dosage Creator" ) + " - " + qApp->applicationName() );

    // Drug informations
    mfDrugsModel *m = DRUGMODEL;
    int CIS = dosageModel->drugCIS();
    drugNameLabel->setText( m->drugData(CIS, Drug::Denomination).toString() );
    QString toolTip = m->drugData(CIS, Interaction::ToolTip ).toString();
    interactionIconLabel->setPixmap( m->drugData(CIS, Interaction::Icon).value<QIcon>().pixmap(16,16) );
    interactionIconLabel->setToolTip( toolTip );
    toolTip = m->drugData(CIS, Drug::CompositionString ).toString();
    drugNameLabel->setToolTip( toolTip );
    // Various model intializations
    dosageViewer->setDosageModel(dosageModel);
    availableDosagesListView->setModel(dosageModel);
    availableDosagesListView->setModelColumn(Dosage::Label);
    availableDosagesListView->setEditTriggers( QListView::NoEditTriggers );
    if (dosageModel->rowCount()==0) {
        dosageModel->insertRow(0);
        dosageViewer->changeCurrentRow(0);
    } else {
        dosageViewer->changeCurrentRow(0);
    }

    // Create connections
    connect(availableDosagesListView->listView(), SIGNAL(activated(QModelIndex)),dosageViewer,SLOT(changeCurrentRow(QModelIndex)));
    QModelIndex idx = dosageModel->index(0,Dosage::Label);
    availableDosagesListView->listView()->setCurrentIndex(idx);
}

/** \brief Destructor */
mfDosageCreatorDialog::~mfDosageCreatorDialog()
{
    if (d) delete d;
    d=0;
}

/**
   \brief Validate the dialog
   \todo Check dosage validity before validate the dialog
*/
void mfDosageCreatorDialog::done( int r )
{
    int row = availableDosagesListView->listView()->currentIndex().row();

    if ( r == QDialog::Rejected ) {
        d->m_DosageModel->revertRow( row );
    }  else {
        dosageViewer->done(r);

//        // ******************************************************
//        // * TODO check validity of the dosage before submition *
//        // ******************************************************
    }
    QDialog::done(r);
}

/** \brief Save the "reference dosage" to the database and reject the dialog (no prescription's done) */
void mfDosageCreatorDialog::on_saveButton_clicked()
{
    // modify focus for the mapper to commit changes
    saveButton->setFocus();
    d->saveToModel();
    dosageViewer->done(QDialog::Rejected);
    done(QDialog::Rejected);
}

/** \brief Accept the dialog (prescription's done), no changes is done on the database. */
void mfDosageCreatorDialog::on_prescribeButton_clicked()
{
    // modify focus for the mapper to commit changes
    prescribeButton->setFocus();
    d->toPrescription();
    dosageViewer->done(QDialog::Accepted);
    done(QDialog::Accepted);
}

/** \brief Save the "reference dosage" to the database and prescribe it then accept the dialog ( prescription's done) */
void mfDosageCreatorDialog::on_saveAndPrescribeButton_clicked()
{
    // modify focus for the mapper to commit changes
    saveAndPrescribeButton->setFocus();
    d->toPrescription();
    d->saveToModel();
    dosageViewer->done(QDialog::Accepted);
    done(QDialog::Accepted);
}

/** \brief Opens a help dialog */
void mfDosageCreatorDialog::on_helpButton_clicked()
{
    tkHelpDialog::showPage("prescrire.html");
}

void mfDosageCreatorDialog::on_testOnlyButton_clicked()
{
    DRUGMODEL->setDrugData(d->m_DosageModel->drugCIS(), Prescription::OnlyForTest, true);
    dosageViewer->done(QDialog::Accepted);
    done(QDialog::Accepted);
}
