/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#include "patientwidgetmanager.h"
#include "constants_menus.h"
#include "constants_trans.h"
#include "patientbase.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>

using namespace Patients::Constants;
using namespace Patients::Internal;
using namespace Patients;
using namespace Trans::ConstantTranslations;

inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
PatientWidgetManager *PatientWidgetManager::m_Instance = 0;

PatientWidgetManager *PatientWidgetManager::instance()
{
    if (!m_Instance)
        m_Instance = new PatientWidgetManager(qApp);
    return m_Instance;
}

PatientWidgetManager::PatientWidgetManager(QObject *parent) : PatientActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("PatientWidgetManager");
}

void PatientWidgetManager::updateContext(Core::IContext *object)
{
    //    qWarning() << "DrugsManager::updateContext(Core::IContext *object)";
    //    if (object)
    //        qWarning() << "DrugsManager::updateContext(Core::IContext *object)" << object->widget();

    PatientSelector *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            //            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<PatientSelector *>(object->widget());
        if (!view) {
            if (!m_CurrentView)
                return;

            //            m_CurrentView = 0;   // keep trace of the last active view (we need it in dialogs)
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        PatientActionHandler::setCurrentView(view);
    }
}

PatientSelector *PatientWidgetManager::selector() const
{
    return PatientActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actions :
// Toggle search patient view
// Search method ?
// Limit database usage ?
// View patients informations
// Patient selection history
PatientActionHandler::PatientActionHandler(QObject *parent) :
        QObject(parent),
        aViewPatientInformations(0),
        aPrintPatientInformations(0),
        aShowPatientDatabaseInformations(0)
{
    setObjectName("DrugsActionHandler");
    Utils::Log::addMessage(this, "Instance created");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(Patients::Constants::C_PATIENTS);
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_PATIENTS);
    if (!menu) {
        Utils::warningMessageBox(tr("Fatal Error"), tr("%1 : Unable to retreive patients menu.").arg(objectName()));
    }
    Q_ASSERT(menu);
    menu->appendGroup(Constants::G_PATIENTS);
    menu->appendGroup(Constants::G_PATIENTS_NEW);
    menu->appendGroup(Constants::G_PATIENTS_HISTORY);
    menu->appendGroup(Constants::G_PATIENTS_INFORMATIONS);

//    actionManager()->actionContainer(Core::Constants::M_PATIENTS)->addMenu(menu, Core::Constants::G_PATIENTS);

    // Create local actions
    a = aViewPatientInformations = new QAction(this);
//    a->setIcon(th->icon(Core::Constants::ICONCLEAR));
    cmd = actionManager()->registerAction(a, Constants::A_VIEWPATIENT_INFOS, ctx);
    cmd->setTranslations(Trans::Constants::PATIENT_INFORMATION);
    menu->addAction(cmd, Constants::G_PATIENTS_INFORMATIONS);
    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    // Databases informations
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    a = aShowPatientDatabaseInformations = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_VIEWPATIENTDATABASE_INFOS, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Trans::Constants::PATIENT_DATABASE);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
        contextManager()->updateContext();
    }
    connect(aShowPatientDatabaseInformations,SIGNAL(triggered()), this, SLOT(showPatientDatabaseInformations()));
}

void PatientActionHandler::updateActions()
{}

void PatientActionHandler::setCurrentView(PatientSelector *view)
{}

void PatientActionHandler::viewPatientInformations()
{}

void PatientActionHandler::printPatientsInformations()
{}

void PatientActionHandler::showPatientDatabaseInformations()
{
    QDialog dlg(qApp->activeWindow(), Qt::WindowFlags(Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint));
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();
    PatientBase::instance()->toTreeWidget(&tree);
    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}
