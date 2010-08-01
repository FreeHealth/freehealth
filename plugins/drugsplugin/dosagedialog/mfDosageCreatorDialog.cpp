/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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

/**
  \class DrugsWidget::DosageCreator
  \brief Dialog for dosage creation / edition / modification. A dosage is a standard set of datas that will be used to help
  doctors when prescribing a drug.
  If you want to create a new dosage, you must create a new row onto the model BEFORE.
  If you want to edit or modify a dosage, you must inform the dialog of the row and the CIS of the drug.
  \ingroup freediams drugswidget
*/


#include "mfDosageCreatorDialog.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>

#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/dosagemodel.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/globaldrugsmodel.h>
#include <drugsbaseplugin/drugsearchengine.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/dialogs/helpdialog.h>
#include <coreplugin/constants_icons.h>

#include <QMessageBox>
#include <QModelIndex>
#include <QToolBar>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>

using namespace DrugsWidget::Constants;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

inline static DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
static inline DrugsDB::Internal::DrugSearchEngine *searchEngine()  { return DrugsDB::Internal::DrugSearchEngine::instance(); }
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}

namespace DrugsWidget {
namespace Internal {

/**
  \brief Private part of DosageDialog
  \internal
*/
class DosageCreatorDialogPrivate
{
public:
    DosageCreatorDialogPrivate(DosageCreatorDialog *parent) :
            m_DosageModel(0), m_SaveProtocolToBase(false), m_HelpMenu(0), m_ValidateMenu(0),
            prescribe(0), prescribeAndSave(0), save(0), test(0),
            q(parent)
    {}

    void createHelpMenu()
    {
        m_HelpMenu = new QMenu(q->tr("Help and Infos"), q);
        q->helpButton->setMenu(m_HelpMenu);
        q->helpButton->setText(m_HelpMenu->title());
        q->helpButton->setIcon(theme()->icon(DrugsDB::Constants::I_DRUGSINFOS, Core::ITheme::MediumIcon));

        // Show application's Help
        QAction *help = new QAction(tkTr(Trans::Constants::HELP_TEXT), m_HelpMenu);
        m_HelpMenu->addAction(help);
        q->connect(help, SIGNAL(triggered()), q, SLOT(helpRequested()));

        // Show database COMPLEMENTARY_WEBSITE
        if (drugsBase()->actualDatabaseInformations()) {
            const QString &url = drugsBase()->actualDatabaseInformations()->complementaryWebsite;
            if (!url.isEmpty()) {
                QAction *drugsDbWeb = new QAction(q->tr("Drugs database website"), m_HelpMenu);
                drugsDbWeb->setData(url);
                m_HelpMenu->addAction(drugsDbWeb);
                q->connect(drugsDbWeb, SIGNAL(triggered()), q, SLOT(drugsInformationsRequested()));
            }
        }

        // Show drugs database search engine actions
        int UID = m_DosageModel->drugUID();
        searchEngine()->setDrug(drugModel()->getDrug(UID));
        foreach(const QString &label, searchEngine()->processedLabels()) {
            foreach(const QString &url, searchEngine()->processedUrls(label)) {
                QAction *action = new QAction(label, m_HelpMenu);
                action->setData(url);
                action->setToolTip(url);
                m_HelpMenu->addAction(action);
                q->connect(action, SIGNAL(triggered()), q, SLOT(drugsInformationsRequested()));
            }
        }
    }

    void createValidateMenu()
    {
        prescribe = new QAction(q->tr("Prescribe only"), q->validateButton);
        prescribe->setIcon(theme()->icon(DrugsDB::Constants::I_PROTOCOL_PRESCRIBE, Core::ITheme::MediumIcon));
        prescribe->setIconVisibleInMenu(true);
        q->connect(prescribe, SIGNAL(triggered()), q, SLOT(prescribeRequested()));

        prescribeAndSave = new QAction(q->tr("Save protocol and prescribe"), q->validateButton);
        prescribeAndSave->setIcon(theme()->icon(DrugsDB::Constants::I_PROTOCOL_PRESCRIBEANDSAVE, Core::ITheme::MediumIcon));
        prescribeAndSave->setIconVisibleInMenu(true);
        q->connect(prescribeAndSave, SIGNAL(triggered()), q, SLOT(saveAndPrescribeRequested()));

        save = new QAction(q->tr("Save protocol only"), q->validateButton);
        save->setIcon(theme()->icon(DrugsDB::Constants::I_PROTOCOL_SAVE, Core::ITheme::MediumIcon));
        save->setIconVisibleInMenu(true);
        q->connect(save, SIGNAL(triggered()), q, SLOT(saveRequested()));

        test = new QAction(q->tr("Test interactions only"), q->validateButton);
        test->setIcon(theme()->icon(DrugsDB::Constants::I_PROTOCOL_TESTONLY, Core::ITheme::MediumIcon));
        test->setIconVisibleInMenu(true);
        q->connect(test, SIGNAL(triggered()), q, SLOT(addTestOnlyRequested()));

        q->validateButton->addAction(prescribeAndSave);
        q->validateButton->addAction(prescribe);
        q->validateButton->addAction(save);
        q->validateButton->addAction(test);
        q->validateButton->setDefaultAction(prescribeAndSave);

        q->cancelButton->setText(tkTr(Trans::Constants::CANCEL));
        q->cancelButton->setIcon(theme()->icon(Core::Constants::ICONEXIT, Core::ITheme::MediumIcon));
    }

    /** \brief Check the validity o the dosage. Warn if dosage is not valid */
    bool checkDosageValidity(const int row)
    {
        QStringList list = m_DosageModel->isDosageValid(row);
        if (list.count()) {
            Utils::warningMessageBox(QCoreApplication::translate("DosageCreatorDialog", "Dosage is not valid."),
                                        list.join("br />"),
                                        "", QCoreApplication::translate("DosageCreatorDialog", "Drug Dosage Creator"));
            return false;
        }
        return true;
    }

    /** \brief Save the current dirty rows of the model to the database */
    void saveToModel()
    {
        int row = q->availableDosagesListView->listView()->currentIndex().row();
        // if Inn is checked --> clear dosage CIS, feel INN + COMPO_DOSAGE
        if (!checkDosageValidity(row))
            return;
        m_DosageModel->database().transaction();
        if (m_DosageModel->submitAll()) {
            if (m_DosageModel->database().commit())
                Utils::Log::addMessage(q, QCoreApplication::translate("DosageCreatorDialog", "Dosage correctly saved to base"));
            else
                Utils::Log::addError(q, QCoreApplication::translate("DosageCreatorDialog", "SQL Error : Dosage can not be added to database : %1")
                                .arg(m_DosageModel->lastError().text()));
        } else {
            m_DosageModel->database().rollback();
            QMessageBox::warning(q, QCoreApplication::translate("DosageCreatorDialog", "Drug Dosage Creator"),
                                 tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2)
                                 .arg(m_DosageModel->lastError().text() + m_DosageModel->query().executedQuery())
                                 .arg(m_DosageModel->database().connectionName()));
        }
    }

    /** \brief Transforms the "reference dialog" to a prescription */
    void toPrescription()
    {
        int row = q->availableDosagesListView->listView()->currentIndex().row();
        m_DosageModel->toPrescription(row);
    }

public:
    DrugsDB::Internal::DosageModel *m_DosageModel;
    QString      m_ActualDosageUuid;
    bool         m_SaveProtocolToBase;
    int m_InitialNumberOfRow;
    QMenu *m_HelpMenu, *m_ValidateMenu;
    QAction *prescribe, *prescribeAndSave, *save, *test;

private:
    DosageCreatorDialog *q;
};

}  // End Internal
}  // End DrugsWidget

/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
 \todo use a QPersistentModelIndex instead of drugRow, dosageRow
*/
DosageCreatorDialog::DosageCreatorDialog(QWidget *parent, DrugsDB::Internal::DosageModel *dosageModel)
    : QDialog(parent),
    d(0)
{
    using namespace DrugsDB::Constants;
    // some initializations
    setObjectName( "DosageCreatorDialog" );
    d = new DosageCreatorDialogPrivate(this);
    d->m_DosageModel = dosageModel;

    // Ui initialization
    setupUi(this);
    setWindowTitle( tr( "Drug Dosage Creator" ) + " - " + qApp->applicationName() );
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    d->m_InitialNumberOfRow = dosageModel->rowCount();
    // Drug informations
    int UID = dosageModel->drugUID();
    drugNameLabel->setText(drugModel()->drugData(UID, Drug::Denomination).toString());
    QString toolTip = drugModel()->drugData(UID, Interaction::ToolTip).toString();
    interactionIconLabel->setPixmap(drugModel()->drugData(UID, Interaction::Icon).value<QIcon>().pixmap(16,16));
    interactionIconLabel->setToolTip(toolTip);
    toolTip = drugModel()->drugData(UID, Drug::CompositionString).toString();
    drugNameLabel->setToolTip(toolTip);
    // Various model intializations
    dosageViewer->setDosageModel(dosageModel);
    availableDosagesListView->setModel(dosageModel);
    availableDosagesListView->setModelColumn(Dosages::Constants::Label);
    availableDosagesListView->setEditTriggers( QListView::NoEditTriggers );

    d->createHelpMenu();
    d->createValidateMenu();

    // Create connections
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
//    connect(availableDosagesListView, SIGNAL());
//    connect(availableDosagesListView->listView(), SIGNAL(activated(QModelIndex)),
//            dosageViewer, SLOT(changeCurrentRow(QModelIndex)));
    connect(availableDosagesListView->listView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            dosageViewer, SLOT(changeCurrentRow(QModelIndex, QModelIndex)));
    QModelIndex idx = dosageModel->index(0,Dosages::Constants::Label);
    availableDosagesListView->setCurrentIndex(idx);
}

/** \brief Destructor */
DosageCreatorDialog::~DosageCreatorDialog()
{
    if (d) delete d;
    d=0;
}

/**
   \brief Validate the dialog
   \todo Check dosage validity before validate the dialog
*/
void DosageCreatorDialog::done(int r)
{
    int row = availableDosagesListView->listView()->currentIndex().row();

    if (!d->m_SaveProtocolToBase) {
        d->m_DosageModel->revertRow(row);
    }  else {
        DrugsDB::GlobalDrugsModel::updateAvailableDosages();
        dosageViewer->done(r);
        /** \todo check validity of the dosage before submition */
    }
    // if user deleted all protocols --> update cached data in global drugs model
//    qWarning() << d->m_DosageModel->rowCount() << d->m_InitialNumberOfRow;
//    if ((d->m_InitialNumberOfRow > 0) && (d->m_DosageModel->rowCount()==0)) {
//        DrugsDB::GlobalDrugsModel::updateAvailableDosages();
//    }
    QDialog::done(r);
}

/** \brief Save the "reference dosage" to the database and reject the dialog (no prescription's done) */
void DosageCreatorDialog::saveRequested()
{
    // modify focus for the mapper to commit changes
    cancelButton->setFocus();
    dosageViewer->commitToModel();
    d->saveToModel();
    dosageViewer->done(QDialog::Accepted);
    d->m_SaveProtocolToBase = true;
    done(QDialog::Rejected);
}

/** \brief Accept the dialog (prescription's done), no changes is done on the database. */
void DosageCreatorDialog::prescribeRequested()
{
    // modify focus for the mapper to commit changes
    cancelButton->setFocus();
    dosageViewer->commitToModel();
    d->toPrescription();
    dosageViewer->done(QDialog::Accepted);
    done(QDialog::Accepted);
}

/** \brief Save the "reference dosage" to the database and prescribe it then accept the dialog ( prescription's done) */
void DosageCreatorDialog::saveAndPrescribeRequested()
{
    // modify focus for the mapper to commit changes
    cancelButton->setFocus();
    dosageViewer->commitToModel();
    d->toPrescription();
    d->saveToModel();
    dosageViewer->done(QDialog::Accepted);
    d->m_SaveProtocolToBase = true;
    done(QDialog::Accepted);
}

/** \brief Opens a help dialog */
void DosageCreatorDialog::helpRequested()
{
    Core::HelpDialog::showPage("prescrire.html");
}

void DosageCreatorDialog::drugsInformationsRequested()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        QDesktopServices::openUrl(QUrl(action->data().toString()));
}

void DosageCreatorDialog::addTestOnlyRequested()
{
    drugModel()->setDrugData(d->m_DosageModel->drugUID(), DrugsDB::Constants::Prescription::OnlyForTest, true);
    dosageViewer->done(QDialog::Accepted);
    done(QDialog::Accepted);
}
