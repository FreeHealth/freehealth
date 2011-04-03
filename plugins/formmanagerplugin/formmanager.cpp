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
#include "constants_db.h"

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/itheme.h>
#include <coreplugin/contextmanager/contextmanager.h>

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
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}


namespace Form {
namespace Constants {
    const char * const XML_FORM_GENERAL_TAG = "FormXmlContent";
}

namespace Internal {
class FormManagerPrivate
{
public:
    FormManagerPrivate(FormManager *parent) :
            m_ActualEpisode(-1), q(parent)
    {}

    ~FormManagerPrivate()
    {
        qDeleteAll(m_RootForms);
        m_RootForms.clear();
    }

public:
    /** \todo create a EpisodeData class */
    int m_ActualEpisode;
    QString m_ActualEpisode_FormUid;
    QList<Form::FormMain *> m_RootForms;


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

FormManager::FormManager(QObject *parent) :
        FormActionHandler(parent),
        d(new Form::Internal::FormManagerPrivate(this))
{
    setObjectName("FormManager");

//    connect(this, SIGNAL(loadPatientForms(QString)), Core::ICore::instance(), SIGNAL(loadPatientForms(QString)));
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
}

/** \brief Return all available forms from the PluginManager object pool. \sa Form::FormMain */
QList<FormMain *> FormManager::forms() const
{
    return pluginManager()->getObjects<FormMain>();
}

bool FormManager::loadPatientFile()
{
    qDeleteAll(d->m_RootForms);
    d->m_RootForms.clear();

    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();

    // get form general form absPath from episodeBase
    QString absDirPath = episodeBase()->getGenericFormFile();
    if (absDirPath.isEmpty()) {
        /** \todo code here: manage no patient form file recorded in episodebase */
        return false;
    }

    foreach(Form::IFormIO *io, list) {
        if (io->canReadForms(absDirPath)) {
            d->m_RootForms << io->loadAllRootForms(absDirPath);
        }
    }

    loadSubForms();

    Q_EMIT patientFormsLoaded();
    return true;
}

Form::FormMain *FormManager::rootForm(const char *modeUniqueName)
{
    // get all root form from the plugin manager
    for(int i=0; i < d->m_RootForms.count(); ++i) {
        FormMain *root = d->m_RootForms.at(i);
        if (root->modeUniqueName().compare(QString(modeUniqueName), Qt::CaseInsensitive)==0) {
            return root;
        }
    }
    return 0;
}

bool FormManager::loadSubForms()
{
    // get sub-forms from database
    QHash<QString, QString> subs = episodeBase()->getSubFormFiles();
    if (subs.isEmpty()) {
        return true;
    }

    // read all sub-forms
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    QList<Form::FormMain *> subFormsRoot;
    foreach(Form::IFormIO *io, list) {
        QHashIterator<QString, QString> i(subs);
        while (i.hasNext()) {
            i.next();
            if (io->canReadForms(i.key())) {
                subFormsRoot << io->loadAllRootForms(i.key());
            }
        }
    }

    // insert sub-forms
    for(int i=0; i < subFormsRoot.count(); ++i) {
        FormMain *sub = subFormsRoot.at(i);
        QString insertIntoUuid = subs.value(sub->uuid());
//        qWarning() << "insert" << sub->uuid() << "to" << insertIntoUuid;
        if (insertIntoUuid == Constants::ROOT_FORM_TAG) {
            // insert into its mode root form
            FormMain *rootMode = rootForm(sub->modeUniqueName().toAscii());
            if (!rootMode) {
                LOG_ERROR("unable to find the mode root form: " + sub->modeUniqueName());
                continue;
            }
            foreach(Form::FormMain *form, sub->firstLevelFormMainChildren())
                form->setParent(rootMode);
        }
        // find point of insertion
        for(int j=0; j < d->m_RootForms.count(); ++j) {
            FormMain *parent = d->m_RootForms.at(j);
            // search inside flatten children
            QList<Form::FormMain *> children = parent->flattenFormMainChildren();
            for(int k=0; k < children.count(); ++k) {
                FormMain *child = children.at(k);
                if (child->uuid()==insertIntoUuid) {
                    foreach(Form::FormMain *form, sub->firstLevelFormMainChildren())
                        form->setParent(child);
                    break;
                }
            }
        }

    }

    return true;
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


FormActionHandler::FormActionHandler(QObject *parent) :
        QObject(parent),
        aAddEpisode(0),
        aValidateEpisode(0),
        aAddForm(0)
{
    Core::ActionManager *am = actionManager();
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();
    QList<int> formContext = QList<int>() << uid->uniqueIdentifier(Constants::C_FORM_PLUGINS);

    // Form's Contextual Menu
//    Core::ActionContainer *editMenu = am->actionContainer(Core::Constants::M_EDIT);
//    Core::ActionContainer *cmenu = am->actionContainer(Core::Constants::M_EDIT_LIST);
//    if (!cmenu) {
//        cmenu = am->createMenu(Core::Constants::M_EDIT_LIST);
//        cmenu->appendGroup(Core::Constants::G_EDIT_LIST);
//        cmenu->setTranslations(Trans::Constants::M_EDIT_LIST_TEXT);
//        if (editMenu)
//            editMenu->addMenu(cmenu, Core::Constants::G_EDIT_LIST);
//    }

    QAction *a = aAddEpisode = new QAction(this);
    a->setObjectName("aAddEpisode");
    a->setIcon(th->icon(Core::Constants::ICONADD));
    Core::Command *cmd = am->registerAction(a, Constants::A_ADDEPISODE, formContext);
    cmd->setTranslations(Constants::ADDEPISODE_TEXT, Constants::ADDEPISODE_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aValidateEpisode = new QAction(this);
    a->setObjectName("aValidateEpisode");
    a->setIcon(th->icon(Core::Constants::ICONVALIDATEDARK));
    cmd = am->registerAction(a, Constants::A_VALIDATEEPISODE, formContext);
    cmd->setTranslations(Constants::VALIDATEEPISODE_TEXT, Constants::VALIDATEEPISODE_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aAddForm = new QAction(this);
    a->setObjectName("aAddForm");
    a->setIcon(th->icon(Core::Constants::ICONFORMS));
    cmd = am->registerAction(a, Constants::A_ADDFORM, formContext);
    cmd->setTranslations(Constants::ADDFORM_TEXT, Constants::ADDFORM_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    contextManager()->updateContext();
}

FormActionHandler::~FormActionHandler()
{
}
