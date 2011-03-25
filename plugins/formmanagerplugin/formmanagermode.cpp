/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "formmanagermode.h"
#include "formplaceholder.h"
#include "iformitem.h"
#include "iformio.h"
#include "episodebase.h"
#include "constants_db.h"

#include <coreplugin/icore.h>
//#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>

#include <extensionsystem/pluginmanager.h>

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

using namespace Form;
using namespace Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}


FormManagerMode::FormManagerMode(QObject *parent) :
        Core::BaseMode(parent)
{
    setName(tr("Patients Files"));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTFILES, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_FILE);
    setUniqueModeName(Core::Constants::MODE_PATIENT_FILE);

    m_Holder = new FormPlaceHolder;
    m_Holder->setObjectName("EpisodesFormPlaceHolder");

//    const QList<int> &context;
//    setContext();
    setWidget(m_Holder);
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(loadPatientFile()));
}

FormManagerMode::~FormManagerMode()
{
    // m_Holder is deleted by Core::BaseMode
}

/**
  \brief Get the patient form from the episode database, send the load signal with the form absPath and load it.
  \sa Core::ICore::loadPatientForms()
*/
bool FormManagerMode::loadPatientFile()
{
    qWarning() << Q_FUNC_INFO;
    // get form general form absPath from episodeBase
    QString absDirPath = episodeBase()->getGenericFormFile();

    if (absDirPath.isEmpty()) {
        /** \todo code here: manage no patient form file recorded in episodebase */
        return false;
    }

    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();

    // try to read form
    Form::FormMain *root = 0;
    foreach(Form::IFormIO *io, list) {
        if (io->setFileName(absDirPath + "/central.xml") && io->canReadFile()) {
            root = io->loadForm();
            if (root)
                break;
        }
    }

    // Tell the PlaceHolder of the FormMain to use as root item
    // FormPlaceHolder will manage deletion of the item
    m_Holder->setRootForm(root);
    return (root);
}



