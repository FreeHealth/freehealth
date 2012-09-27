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
 * \class Form::FormManager
 * This class manages all aspect of the patient's forms. It loads the patient
 * central form and all included forms using the Form::IFormIO objects. These forms
 * are used to create the patient forms and integrate them in the UI.\n
 *
 * Form::FormManager also manages :
 * - a cache system to fasten the form's access and construction ;
 *
*/
// - keeps duplicates of each forms to use in specific computation (statistics...).

// TODO: correctly manages subforms (keep them in a list and in cache)
// TODO: correctly manages subforms duplicates

#include "formmanager.h"
#include "iformitem.h"
#include "iformio.h"
#include "formplaceholder.h"
#include "episodebase.h"
#include "episodemodel.h"
#include "constants_db.h"
#include "subforminsertionpoint.h"

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iscriptmanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <patientbaseplugin/constants_menus.h>

#include <utils/global.h>
#include <utils/log.h>
#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>

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
#include <QPixmap>

#include <QDebug>

enum { WarnFormCreation = false };

enum { ManageDuplicates = true };

using namespace Form;
using namespace Form::Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

namespace Form {
namespace Constants {
    const char * const XML_FORM_GENERAL_TAG = "FormXmlContent";
}

namespace Internal {
class FormManagerPrivate
{
public:
    FormManagerPrivate(FormManager *parent) :
        q(parent)
    {}

    ~FormManagerPrivate()
    {
        qDeleteAll(m_RootForms);
        m_RootForms.clear();
        _formParents.clear();
        // EpisodeModel are deleted by QObject mecanism
    }

    bool isFormLoaded(QString formUid)
    {
        QHashIterator<Form::FormMain *, Form::FormMain *> i(_formParents);
         while (i.hasNext()) {
             i.next();
             if (i.value()->uuid() == formUid)
                 return true;
         }
         return false;
    }

    QList<Form::FormMain *> loadFormFile(const QString &formUid, bool useCache = true)
    {
        QList<Form::FormMain *> toReturn;

        if (formUid.isEmpty()) {
            LOG_ERROR_FOR(q, "No formUid to load...");
            return toReturn;
        }

        // Check from cache (we suppose that the forms are reparented to their original empty root parent)
        if (useCache && isFormLoaded(formUid)) {
            QHashIterator<Form::FormMain *, Form::FormMain *> i(_formParents);
             while (i.hasNext()) {
                 i.next();
                 if (i.value()->uuid() == formUid)
                     toReturn << i.value();
             }
            return toReturn;
        }


        // Not in cache -> ask IFormIO plugins
        QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
        if (list.isEmpty()) {
            LOG_ERROR_FOR(q, "No IFormIO loaded...");
            return toReturn;
        }

        // Load root forms
        foreach(Form::IFormIO *io, list) {
            if (io->canReadForms(formUid)) {
                QList<Form::FormMain *> list = io->loadAllRootForms(formUid);
                // Keep trace of form parents (first QObject of the rootForm)
                for(int i=0; i < list.count(); ++i) {
                    Form::FormMain *rootForm = list.at(i);
                    foreach(Form::FormMain *form, rootForm->firstLevelFormMainChildren()) {
                        _formParents.insert(form, rootForm);
                    }
                }
                toReturn << list;
            }
        }
        return toReturn;
    }

    bool getRootForm()
    {
        // get form general form absPath from episodeBase
        QString absDirPath = episodeBase()->getGenericFormFile();
        if (absDirPath.isEmpty()) {
            // TODO: code here: manage no patient form file recorded in episodebase
            LOG_ERROR_FOR(q, "No patient central form defined");
            return false;
        }

        // load central root forms, create cache and duplicates
        m_RootForms = loadFormFile(absDirPath); // also create the cache
        if (ManageDuplicates) {
            qDeleteAll(m_RootFormsDuplicates);
            m_RootFormsDuplicates.clear();
            m_RootFormsDuplicates = loadFormFile(absDirPath, false); // force reloading of forms
        }
        // load pmhx
        if (!m_RootForms.isEmpty()) {
            m_RootForms.at(0)->reader()->loadPmhCategories(absDirPath);
            return true;
        }
        return false;
    }

    /**
     * Load all requiered subforms and insert them according to data extracted from the episode database.
     */
    bool loadSubForms()
    {
        // TODO: ???   d->m_SubFormsEmptyRoot.clear();
        // get sub-forms from database
        const QVector<SubFormInsertionPoint> &subs = episodeBase()->getSubFormFiles();
        if (subs.isEmpty())
            return true;

        bool ok = true;
        for(int i = 0; i < subs.count(); ++i) {
            if (!insertSubForm(subs.at(i))) {
                LOG_ERROR_FOR(q, "Unable to insert sub-form: " + subs.at(i).subFormUid());
                ok = false;
            }
        }
        return ok;
    }

    bool insertSubForm(const SubFormInsertionPoint &insertionPoint)
    {
        // read all sub-forms and emit signal if requiered
        QList<Form::FormMain*> subs = loadFormFile(insertionPoint.subFormUid());
        if (ManageDuplicates)
            m_SubFormsEmptyRootDuplicates << loadFormFile(insertionPoint.subFormUid(), false);

        if (insertionPoint.emitInsertionSignal())
            Q_EMIT q->subFormLoaded(insertionPoint.subFormUid());

        // insert sub-forms to the forms tree
        const QString &insertIntoUuid = insertionPoint.receiverUid();
        for(int i=0; i < subs.count(); ++i) {
            FormMain *sub = subs.at(i);
            qWarning() << "insert" << sub;
            if (insertIntoUuid == Constants::ROOT_FORM_TAG) {
                // insert into its mode root form
                FormMain *rootMode = q->rootForm(sub->modeUniqueName().toAscii());
                if (!rootMode) {
                    LOG_ERROR_FOR(q, "Unable to find the mode root form: " + sub->modeUniqueName());
                    continue;
                }
                foreach(Form::FormMain *form, sub->firstLevelFormMainChildren())
                    form->setParent(rootMode);
            }

            // find point of insertion
            QList<Form::FormMain *> allForms = q->forms();
            for(int j=0; j < allForms.count(); ++j) {
                FormMain *parent = allForms.at(j);
                // search inside flatten children
                QList<Form::FormMain *> children = parent->flattenFormMainChildren();
                for(int k=0; k < children.count(); ++k) {
                    FormMain *child = children.at(k);
                    if (child->uuid()==insertIntoUuid) {
                        qWarning() << "inserting subForm"<< insertionPoint;
                        foreach(Form::FormMain *form, sub->firstLevelFormMainChildren()) {
                            form->setParent(child);
                        }
                        break;
                    }
                }
            }
        }
        return true;
    }

    bool readPmhxCategories(const QString &formUuidOrAbsPath)
    {
        Q_UNUSED(formUuidOrAbsPath);
        // TODO: code here why don't we use the \e formUuidOrAbsPath param?
        // get all form readers (IFormIO)
        QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();

        // get form general form absPath from episodeBase
        QString absDirPath = episodeBase()->getGenericFormFile();
        if (absDirPath.isEmpty()) {
            // TODO: code here: manage no patient form file recorded in episodebase
            return false;
        }
        foreach(Form::IFormIO *io, list) {
            if (io->canReadForms(absDirPath)) {
                if (io->loadPmhCategories(absDirPath))
                    break;
            }
        }
        return true;
    }

public:
    QVector<Form::FormPage *> _formPages;
    QList<Form::FormMain *> m_RootForms, m_RootFormsDuplicates, m_SubFormsEmptyRoot, m_SubFormsEmptyRootDuplicates;
    QHash<Form::FormMain *, Form::FormMain *> _formParents; // keep the formMain parent in cache (K=form to reparent, V=emptyrootform)
    QHash<Form::FormMain *, EpisodeModel *> _episodeModels;

private:
    FormManager *q;
};
} // namespace Internal
} // namespace Form

/** Ctor of the Form::FormManager. Does nothing. The singleton is owned by the Form::FormCore object */
FormManager::FormManager(QObject *parent) :
    QObject(parent),//FormActionHandler(parent),
    d(new Form::Internal::FormManagerPrivate(this))
{
    setObjectName("FormManager");
}

FormManager::~FormManager()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** Initialize the object (called when all plugins are initialized) \sa Form::FormCore::initialize() */
bool FormManager::initialize()
{
    //    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));

    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    connect(packManager(), SIGNAL(packRemoved(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    //    connect(packManager(), SIGNAL(packUpdated(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    return true;
}

/**  Activate the Form Mode in the main window. */
void FormManager::activateMode()
{
    modeManager()->activateMode(Core::Constants::MODE_PATIENT_FILE);
}

/** Create a Form::FormPage with the specified \e uuid. There are no uuid duplicates. */
FormPage *FormManager::createFormPage(const QString &uuid)
{
    for(int i=0; i < d->_formPages.count(); ++i) {
        FormPage *p = d->_formPages.at(i);
        if (p->uuid()==uuid)
            return p;
    }
    FormPage *p = new FormPage(this);
    if (!uuid.isEmpty())
        p->setUuid(uuid);
    d->_formPages.append(p);
    return p;
}

/** Return the form (Form::FormMain*) with the uuid \e formUid or return zero. This function checks the central and all loaded subforms */
FormMain *FormManager::form(const QString &formUid) const
{
    for(int i=0; i < d->m_RootForms.count(); ++i) {
        Form::FormMain *form = d->m_RootForms.at(i);
        if (form->uuid()==formUid)
            return form;
        const QList<Form::FormMain*> &children = form->flattenFormMainChildren();
        for(int j=0; j < children.count(); ++j) {
            Form::FormMain *test = children.at(j);
            if (test->uuid()==formUid)
                return test;
        }
    }
    for(int i=0; i < d->m_SubFormsEmptyRoot.count(); ++i) {
        Form::FormMain *form = d->m_SubFormsEmptyRoot.at(i);
        if (form->uuid()==formUid)
            return form;
        const QList<Form::FormMain*> &children = form->flattenFormMainChildren();
        for(int j=0; j < children.count(); ++j) {
            Form::FormMain *test = children.at(j);
            if (test->uuid()==formUid)
                return test;
        }
    }
    return 0;
}

/**
 * Return all available empty root forms for the current patient.
 * \sa Form::FormManager::subFormsEmptyRoot(), Form::FormMain
 */
QList<FormMain *> FormManager::forms() const
{
    return d->m_RootForms;
}

/**  Return all available empty root sub-forms for the current patient. \sa Form::FormMain */
QList<FormMain *> FormManager::subFormsEmptyRoot() const
{
    return d->m_SubFormsEmptyRoot;
}

/**
 * Return the empty root forms loaded from the \e formUid by the Forms::IFormIO objects. \n
 * All these forms are stored in the plugin manager object pool and can be accessed using
 * Forms::FormManager::forms().
 */
QList<Form::FormMain *> FormManager::loadFormFile(const QString &formUid)
{
    return d->loadFormFile(formUid);
}

/**
 * Insert a sub-form to a form to the specified \e insertionPoint.
 * A signal is emitted before Form::FormMain are getting reparented.
 */
bool FormManager::insertSubForm(const SubFormInsertionPoint &insertionPoint)
{
    return d->insertSubForm(insertionPoint);
}

/** Load the generic patient file (and included subforms) and emit patientFormsLoaded() when finished. */
bool FormManager::loadPatientFile()
{
    return onCurrentPatientChanged();
}

bool FormManager::onCurrentPatientChanged()
{
    // Patient root form already loaded ?
    if (d->m_RootForms.isEmpty()) {
        if (d->getRootForm()) {
            LOG("Central patient file loaded");
            Q_EMIT patientFormsLoaded();
        } else {
            LOG_ERROR("Unable to load central patient file");
            return false;
        }
    } else {
        // Remove subforms from the root form (just reparent all forms to their initial empty root)
        QHashIterator<Form::FormMain *, Form::FormMain *> i(d->_formParents);
         while (i.hasNext()) {
             i.next();
             i.key()->setParent(i.value());
         }
    }

    // load subforms
    d->loadSubForms();

    return true;
}

/** Read the PMHx categories associated the \e formUidOrAbsPath form. */
bool FormManager::readPmhxCategories(const QString &formUuidOrAbsPath)
{
    return d->readPmhxCategories(formUuidOrAbsPath);
}

/**
 * Extract from the patient file form the empty root form corresponding to the \e modeUniqueName
 * or 0 if no form matches the \e modeUniqueName.
 * \sa loadPatientFile()
*/
Form::FormMain *FormManager::rootForm(const char *modeUniqueName) const
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

/**
 * Extract from the patient file form the empty root form corresponding to the identity form
 * or 0 if no form matches request.
*/
Form::FormMain *FormManager::identityRootForm() const
{
    QList<Form::FormMain *> forms;
    forms << d->m_RootForms;
    forms << d->m_SubFormsEmptyRoot;
    for(int i=0; i < forms.count(); ++i) {
        FormMain *root = forms.at(i);
        if (root->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool())
            return root;
        foreach(FormMain *form, root->flattenFormMainChildren()) {
            if (form->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool())
                return form;
        }
    }
    LOG_ERROR("No identity form found");
    return 0;
}

/**
 * Extract from the patient file form the empty root form corresponding to the identity form
 * or 0 if no form matches request.
*/
Form::FormMain *FormManager::identityRootFormDuplicate() const
{
    if (!ManageDuplicates)
        return 0;

    QList<Form::FormMain *> forms;
    forms << d->m_RootFormsDuplicates;
    forms << d->m_SubFormsEmptyRootDuplicates;
    for(int i=0; i < forms.count(); ++i) {
        FormMain *root = forms.at(i);
        if (root->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool())
            return root;
        foreach(FormMain *form, root->flattenFormMainChildren()) {
            if (form->spec()->value(FormItemSpec::Spec_IsIdentityForm).toBool())
                return form;
        }
    }
    LOG_ERROR("No identity form duplicate found");
    return 0;
}

/** Return one specific screenshot for the form \e formUid and nammed \e fileName. */
QPixmap FormManager::getScreenshot(const QString &formUid, const QString &fileName)
{
    if (formUid.isEmpty()) {
        LOG_ERROR("No formUid...");
        return QPixmap();
    }

    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    if (list.isEmpty()) {
        LOG_ERROR("No IFormIO loaded...");
        return QPixmap();
    }

    // Load root forms
    QPixmap pix;
    foreach(Form::IFormIO *io, list) {
        pix = io->screenShot(formUid, fileName);
        if (!pix.isNull())
            return pix;
    }
    return pix;
}

//FormTreeModel *FormManager::formTreeModel(const char* modeUniqueName)
//{
//}

/**
 * Return the unique Form::EpisodeModel linked to the patient form \e form.
 * Return zero if the form is not null.
 */
EpisodeModel *FormManager::episodeModel(Form::FormMain *form)
{
    if (!form)
        return 0;

    // Not in cache ?
    if (!d->_episodeModels.value(form, 0)) {
        // Create the model
        EpisodeModel *model = new EpisodeModel(form, this);
        model->initialize();
        d->_episodeModels.insert(form, model);
        // TODO connect rowCountChanged from episode to formtreemodel -> recompute the form label count
        return model;
    }
    return d->_episodeModels.value(form);
}

/**
 * Return the unique Form::EpisodeModel linked to the patient form identified by \e formUid.
 * Return zero if the form \e formUid is not available.
 */
EpisodeModel *FormManager::episodeModel(const QString &formUid)
{
    return episodeModel(form(formUid));
}


///** Execute all OnLoad scripts of the \e emptyRootForm */
//void FormManager::executeOnloadScript(Form::FormMain *emptyRootForm)
//{
//    scriptManager()->evaluate(emptyRootForm->scripts()->onLoadScript());
//    QList<Form::FormMain *> children = emptyRootForm->flattenFormMainChildren();
//    foreach(Form::FormMain *mainChild, children) {
//        scriptManager()->evaluate(mainChild->scripts()->onLoadScript());
//        foreach(Form::FormItem *item, mainChild->flattenFormItemChildren()) {
//            scriptManager()->evaluate(item->scripts()->onLoadScript());
//        }
//    }
//}

void FormManager::packChanged(const DataPack::Pack &pack)
{
    if ((pack.dataType() != DataPack::Pack::FormSubset) ||
            (pack.dataType() != DataPack::Pack::SubForms))
        return;

    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    if (list.isEmpty()) {
        LOG_ERROR("No IFormIO loaded...");
        return;
    }

    // Check form update
    foreach(Form::IFormIO *io, list) {
        io->checkForUpdates();
    }

    // Force patient files reloading
    if (!patient()->uuid().isEmpty())
        loadPatientFile();
}
