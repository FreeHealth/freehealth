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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class DrugsWidget::DosageCreator
  \brief Dialog for dosage creation / edition / modification. A dosage is a standard set of data that will be used to help
  doctors when prescribing a drug.
  If you want to create a new dosage, you must create a new row onto the model BEFORE.
  If you want to edit or modify a dosage, you must inform the dialog of the row and the CIS of the drug.
  \ingroup freediams drugswidget
*/


#include "mfDosageCreatorDialog.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/interactionsynthesisdialog.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/dosagemodel.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/globaldrugsmodel.h>
#include <drugsbaseplugin/drugsearchengine.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/dialogs/helpdialog.h>
#include <coreplugin/constants_icons.h>

#include <QModelIndex>
#include <QToolBar>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QKeyEvent>

using namespace DrugsWidget::Constants;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
static inline DrugsDB::Internal::DrugSearchEngine *searchEngine()  { return DrugsDB::Internal::DrugSearchEngine::instance(); }
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

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

    void createHelpMenu(const QString &menuName, const QString &drugDBLabel)
    {
        m_HelpMenu = new QMenu(menuName, q);
        q->helpButton->setMenu(m_HelpMenu);
        q->helpButton->setText(m_HelpMenu->title());
        q->helpButton->setIcon(theme()->icon(DrugsDB::Constants::I_DRUGSINFOS, Core::ITheme::MediumIcon));

        // Show application's Help
        QAction *help = new QAction(tkTr(Trans::Constants::HELP_TEXT), m_HelpMenu);
        m_HelpMenu->addAction(help);
        q->connect(help, SIGNAL(triggered()), q, SLOT(helpRequested()));

        // Show database COMPLEMENTARY_WEBSITE
        if (drugsBase().actualDatabaseInformation()) {
            const QString &url = drugsBase().actualDatabaseInformation()->complementaryWebsite;
            if (!url.isEmpty()) {
                QAction *drugsDbWeb = new QAction(drugDBLabel, m_HelpMenu);
                drugsDbWeb->setData(url);
                m_HelpMenu->addAction(drugsDbWeb);
                q->connect(drugsDbWeb, SIGNAL(triggered()), q, SLOT(drugsInformationRequested()));
            }
        }

        // Show drugs database search engine actions
        const QVariant &drugId = m_DosageModel->drugId();
        searchEngine()->setDrug(drugModel()->getDrug(drugId));
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

    void createValidateMenu(const QString &prescribeOnly,
                            const QString &savePrescribe,
                            const QString &saveOnly,
                            const QString &testOnly)
    {
        prescribe = new QAction(prescribeOnly, q->validateButton);
        prescribe->setIcon(theme()->icon(DrugsDB::Constants::I_PROTOCOL_PRESCRIBE, Core::ITheme::MediumIcon));
        prescribe->setIconVisibleInMenu(true);
        q->connect(prescribe, SIGNAL(triggered()), q, SLOT(prescribeRequested()));

        prescribeAndSave = new QAction(savePrescribe, q->validateButton);
        prescribeAndSave->setIcon(theme()->icon(DrugsDB::Constants::I_PROTOCOL_PRESCRIBEANDSAVE, Core::ITheme::MediumIcon));
        prescribeAndSave->setIconVisibleInMenu(true);
        q->connect(prescribeAndSave, SIGNAL(triggered()), q, SLOT(saveAndPrescribeRequested()));

        save = new QAction(saveOnly, q->validateButton);
        save->setIcon(theme()->icon(DrugsDB::Constants::I_PROTOCOL_SAVE, Core::ITheme::MediumIcon));
        save->setIconVisibleInMenu(true);
        q->connect(save, SIGNAL(triggered()), q, SLOT(saveRequested()));

        test = new QAction(testOnly, q->validateButton);
        test->setIcon(theme()->icon(DrugsDB::Constants::I_PROTOCOL_TESTONLY, Core::ITheme::MediumIcon));
        test->setIconVisibleInMenu(true);
        q->connect(test, SIGNAL(triggered()), q, SLOT(addTestOnlyRequested()));

        q->validateButton->addAction(prescribeAndSave);
        q->validateButton->addAction(prescribe);
        q->validateButton->addAction(save);
        q->validateButton->addAction(test);

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
        int row = q->availableDosagesListView->currentIndex().row();
        // if Inn is checked --> clear dosage CIS, feel INN + COMPO_DOSAGE
        if (!checkDosageValidity(row))
            return;
        m_DosageModel->database().transaction();
        if (m_DosageModel->submitAll()) {
            if (m_DosageModel->database().commit())
                Utils::Log::addMessage(q, QCoreApplication::translate("DosageCreatorDialog", "Dosage successfully saved to base"));
            else
                Utils::Log::addError(q, QCoreApplication::translate("DosageCreatorDialog", "SQL Error: Dosage can not be added to database: %1")
                                     .arg(m_DosageModel->lastError().text()), __FILE__, __LINE__);
        } else {
            m_DosageModel->database().rollback();
            Utils::warningMessageBox(QCoreApplication::translate("DosageCreatorDialog", "Drug Dosage Creator"),
                                 tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2)
                                 .arg(m_DosageModel->lastError().text() + m_DosageModel->query().executedQuery())
                                 .arg(m_DosageModel->database().connectionName()));
        }
    }

    /** \brief Transforms the "reference dialog" to a prescription */
    void toPrescription()
    {
        int row = q->availableDosagesListView->currentIndex().row();
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
    setObjectName("DosageCreatorDialog");
    d = new DosageCreatorDialogPrivate(this);
    d->m_DosageModel = dosageModel;

    // Ui initialization
    setupUi(this);
    setWindowTitle(tr("Drug Dosage Creator") + " - " + qApp->applicationName());
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    d->m_InitialNumberOfRow = dosageModel->rowCount();
    // Drug information
    const QVariant &drugId = dosageModel->drugId();
    drugNameLabel->setText(drugModel()->drugData(drugId, Drug::Denomination).toString());
    QString toolTip = drugModel()->drugData(drugId, Interaction::ToolTip).toString();
    interactionIconLabel->setPixmap(drugModel()->drugData(drugId, Interaction::Icon).value<QIcon>().pixmap(16,16));
    interactionIconLabel->setToolTip(toolTip);
    toolTip = drugModel()->drugData(drugId, Drug::CompositionString).toString();
    drugNameLabel->setToolTip(toolTip);
    // Various model intializations
    dosageViewer->setDosageModel(dosageModel);
    availableDosagesListView->setModel(dosageModel);
    availableDosagesListView->setModelColumn(Dosages::Constants::Label);
    availableDosagesListView->setEditTriggers( QListView::NoEditTriggers );

    d->createHelpMenu(tr("Help and Info"), tr("Current database website"));
    d->createValidateMenu(tr("Prescribe only"), tr("Save protocol and prescribe"), tr("Save protocol only"), tr("Test interactions only"));

    // Create connections
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

//    connect(availableDosagesListView, SIGNAL());
//    connect(availableDosagesListView->listView(), SIGNAL(activated(QModelIndex)),
//            dosageViewer, SLOT(changeCurrentRow(QModelIndex)));

    connect(availableDosagesListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            dosageViewer, SLOT(changeCurrentRow(QModelIndex, QModelIndex)));

    QModelIndex idx = dosageModel->index(0,Dosages::Constants::Label);
    availableDosagesListView->setCurrentIndex(idx);

    // interaction button
    showInteractionSynthesisButton->setIcon(theme()->icon(DrugsDB::Constants::I_DRUGENGINE));
    connect(showInteractionSynthesisButton, SIGNAL(clicked()), this, SLOT(showInteractionSynthesisRequested()));
    if (drugModel()->drugData(drugId, Drug::Interacts).toBool()) {
        showInteractionSynthesisButton->show();
    } else {
        showInteractionSynthesisButton->hide();
    }

    updateSettings();
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
    int row = availableDosagesListView->currentIndex().row();

    if (!d->m_SaveProtocolToBase) {
        d->m_DosageModel->revertRow(row);
    }  else {
        DrugsDB::GlobalDrugsModel::updateAvailableDosages();
        dosageViewer->done(r);
        // TODO: check validity of the dosage before submition
    }
    // if user deleted all protocols --> update cached data in global drugs model
//    qWarning() << d->m_DosageModel->rowCount() << d->m_InitialNumberOfRow;
//    if ((d->m_InitialNumberOfRow > 0) && (d->m_DosageModel->rowCount()==0)) {
//        DrugsDB::GlobalDrugsModel::updateAvailableDosages();
//    }
    QDialog::done(r);
}

/** \brief Catch Enter key to validate the dialog. */
void DosageCreatorDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_Enter || e->key() == Qt::Key_Return) {
//        qWarning() << "void DosageCreatorDialog::keyPressEvent(QKeyEvent *e)" << validateButton->defaultAction()->text();
        validateButton->defaultAction()->trigger();
        return;
    }
    QDialog::keyPressEvent(e);
}

/** \brief Update the settings of the dialog */
void DosageCreatorDialog::updateSettings()
{
    if (settings()->value(Constants::S_PROTOCOLCREATOR_AUTOCHANGE).toBool())
        connect(dosageViewer, SIGNAL(protocolDataschanged()), this, SLOT(protocolDataChanged()));
    else
        disconnect(dosageViewer, SIGNAL(protocolDataschanged()), this, SLOT(protocolDataChanged()));

    const QString &defButton = settings()->value(Constants::S_PROTOCOLCREATOR_DEFAULTBUTTON).toString();
    validateButton->setDefaultAction(d->prescribe);
    if (defButton==Constants::S_VALUE_PRESCRIBEONLY)
        validateButton->setDefaultAction(d->prescribe);
    else if (defButton==Constants::S_VALUE_SAVEPRESCRIBE)
        validateButton->setDefaultAction(d->prescribeAndSave);
    else if (defButton==Constants::S_VALUE_SAVEONLY)
        validateButton->setDefaultAction(d->save);
    else if (defButton==Constants::S_VALUE_TESTONLY)
        validateButton->setDefaultAction(d->test);
}

void DosageCreatorDialog::protocolDataChanged()
{
    // Set window modified
    const QString &winTitle = windowTitle();
    if (!winTitle.endsWith(" [*]")) {
        setWindowTitle(winTitle + " [*]");
        setWindowModified(true);
    }

    // Manage the validate button
    const QString &defButton = settings()->value(Constants::S_PROTOCOLCREATOR_AUTOCHANGE_BUTTON).toString();
    validateButton->setDefaultAction(d->prescribe);
    if (defButton==Constants::S_VALUE_PRESCRIBEONLY)
        validateButton->setDefaultAction(d->prescribe);
    else if (defButton==Constants::S_VALUE_SAVEPRESCRIBE)
        validateButton->setDefaultAction(d->prescribeAndSave);
    else if (defButton==Constants::S_VALUE_SAVEONLY)
        validateButton->setDefaultAction(d->save);
    else if (defButton==Constants::S_VALUE_TESTONLY)
        validateButton->setDefaultAction(d->test);
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

/** \brief Accept the dialog (prescription's done), no changes are done on the database. */
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
    Core::HelpDialog::showPage(Constants::H_PROTOCOL_DIALOG);
}

/** \brief Open the drug information dialog */
void DosageCreatorDialog::drugsInformationRequested()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        QDesktopServices::openUrl(QUrl(action->data().toString()));
}

/** \brief Add the selected drug as 'ForInteractionsTestOnly' */
void DosageCreatorDialog::addTestOnlyRequested()
{
    drugModel()->setDrugData(d->m_DosageModel->drugId(), DrugsDB::Constants::Prescription::OnlyForTest, true);
    dosageViewer->done(QDialog::Accepted);
    done(QDialog::Accepted);
}

void DosageCreatorDialog::showInteractionSynthesisRequested()
{
    InteractionSynthesisDialog dlg(drugModel(), this);
    Utils::resizeAndCenter(&dlg, Core::ICore::instance()->mainWindow());
    dlg.exec();
}
