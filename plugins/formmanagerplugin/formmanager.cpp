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

/**
   \class Form::FormManager
   \brief This class manages all aspect of the patient's forms.
*/

#include "formmanager.h"
#include "iformitem.h"
#include "iformio.h"
#include "formplaceholder.h"
#include "episodebase.h"

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/ipatient.h>

#include <utils/global.h>
#include <utils/log.h>

#include <extensionsystem/pluginmanager.h>

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QStackedLayout>
#include <QSplitter>
#include <QWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPointer>
#include <QMap>
#include <QApplication>

enum { WarnFormCreation = false };

using namespace Form;
using namespace Form::Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}


namespace Form {
namespace Constants {
    const char * const XML_FORM_GENERAL_TAG = "FormXmlContent";
}

namespace Internal {
class FormManagerPrivate
{
public:
    FormManagerPrivate(FormManager *parent) :
            m_Holder(0), m_ActualEpisode(-1), q(parent)
    {}

    ~FormManagerPrivate()
    {
        // DO NOT DELETE m_UuidManager
        if (m_Holder) {
            delete m_Holder;
            m_Holder = 0;
        }
    }

public:
    QPointer<FormPlaceHolder> m_Holder;

    /** \todo create a EpisodeData class */
    int m_ActualEpisode;
    QString m_ActualEpisode_FormUid;


private:
    FormManager *q;
};
} // End Internal
} // End Form


FormManager *FormManager::m_Instance = 0;

FormManager *FormManager::instance()
{
    if (!m_Instance) {
        m_Instance = new FormManager(qApp);
    }
    return m_Instance;
}

FormManager::FormManager(QObject *parent)
        : QObject(parent), d(new Form::Internal::FormManagerPrivate(this))
{
    setObjectName("FormManager");
    /** \todo Need to modify UID code to create a new private uid */
    d->m_Holder = new FormPlaceHolder;

    connect(this, SIGNAL(loadPatientForms(QString)), Core::ICore::instance(), SIGNAL(loadPatientForms(QString)));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(loadPatientFile()));
}

FormManager::~FormManager()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** \brief Activate the Form Mode in the main window. */
void FormManager::activateMode()
{
    modeManager()->activateMode(Core::Constants::MODE_PATIENT_FILE);
    d->m_Holder->formTree()->expandAll();
}

/** \brief Return all available forms from the PluginManager object pool. \sa Form::FormMain */
QList<FormMain *> FormManager::forms() const
{
    return pluginManager()->getObjects<FormMain>();
}

/**
  \brief Get the patient form from the episode database, send the load signal with the form absPath and load it.
  \sa Core::ICore::loadPatientForms()

*/
bool FormManager::loadPatientFile()
{
    // get form general form absPath from episodeBase
    QString absDirPath = episodeBase()->getGenericFormFile();

    qWarning() << Q_FUNC_INFO << absDirPath;

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
    d->m_Holder->setRootForm(root);

    if (root) {
        Q_EMIT loadPatientForms(absDirPath);
    } else {
        return false;
    }
    return true;
}

/** \brief Return the unique Form::FormPlaceHolder. */
FormPlaceHolder *FormManager::formPlaceHolder() const
{
    return d->m_Holder;
}

void FormManager::setCurrentPatient(const QString &uuid)
{
    Q_UNUSED(uuid);
    /** \todo code here */
//    QString formUuid = episodeModel()->index(0, Form::EpisodeModel::FormUuid, QModelIndex()).data().toString();
    d->m_Holder->formTree()->expandAll();
//    d->m_Holder->setCurrentForm(formUuid);
}

bool FormManager::translateForms()
{
    /** \todo code here ??? */
    // Translate the tree
//    translateTreeItem( m_Tree->topLevelItem( 0 ) );
//    m_Tree->resizeColumnToContents( LabelColumn );
//    retranslateUi();
    return true;
}
