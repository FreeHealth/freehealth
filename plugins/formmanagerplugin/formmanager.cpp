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
#include "formcollection.h"
#include "formtreemodel.h"

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
#include <QProgressDialog>

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
    enum FormType {
        CompleteForms,
        ModeForms,
        SubForms
    };

    FormManagerPrivate(FormManager *parent) :
        q(parent)
    {}

    ~FormManagerPrivate()
    {
        // FormTreeModel are deleted by QObject mecanism
        qDeleteAll(_centralFormCollection);
        qDeleteAll(_subFormCollection);
        // TODO: segfaulting on deletion
//        qDeleteAll(_centralFormDuplicateCollection);
//        qDeleteAll(_subFormDuplicateCollection);
    }

    const FormCollection &extractFormCollectionFrom(const QVector<FormCollection *> &collections, FormType type, const QString &uid)
    {
        for(int i=0; i < collections.count(); ++i) {
            FormCollection *coll = collections.at(i);
            if (type == CompleteForms && coll->type()==FormCollection::CompleteForm && coll->formUid() == uid) {
                return *coll;
            } else if (type == ModeForms && coll->type()==FormCollection::CompleteForm && coll->modeUid() == uid) {
                return *coll;
            } else if (type == SubForms && coll->type()==FormCollection::SubForm && coll->formUid() == uid) {
                return *coll;
            }
        }
        return _nullFormCollection;
    }

    bool isCollectionLoaded(const QString &uid, FormType type)
    {
        if (type==CompleteForms)
            return !extractFormCollectionFrom(_centralFormCollection, type, uid).isNull();
        return !extractFormCollectionFrom(_subFormCollection, type, uid).isNull();
    }

    void createModeFormCollections(const QList<Form::FormMain *> &list, FormType type, bool isDuplicate)
    {
        foreach(Form::FormMain *form, list) {
            // Check if a collection with the mode is already existing
            FormCollection *collection = new FormCollection;
            collection->setEmptyRootForms(QList<Form::FormMain *>() << form);
            collection->setDuplicates(isDuplicate);
            if (type==CompleteForms) {
                collection->setType(FormCollection::CompleteForm);
                if (isDuplicate)
                    _centralFormDuplicateCollection << collection;
                else
                    _centralFormCollection << collection;
            } else {
                collection->setType(FormCollection::SubForm);
                if (isDuplicate)
                    _subFormDuplicateCollection << collection;
                else
                    _subFormCollection << collection;
            }
        }
    }

    // Loads a form and create its collection, creates duplicates too
    bool loadFormCollection(const QString &uid, FormType type)
    {
        if (uid.isEmpty()) {
            LOG_ERROR_FOR(q, "No uid to load...");
            return false;
        }

        // Check from cache
        if (isCollectionLoaded(uid, type))
            return true;

        // Not in cache -> ask IFormIO plugins
        QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
        if (list.isEmpty()) {
            LOG_ERROR_FOR(q, "No IFormIO loaded...");
            return false;
        }

        // Load forms
        foreach(Form::IFormIO *io, list) {
            if (io->canReadForms(uid)) {
                // Create the main collection
                QList<Form::FormMain *> list = io->loadAllRootForms(uid);
                createModeFormCollections(list, type, false);
                list.clear();

                // Create its duplicate
                list = io->loadAllRootForms(uid);
                createModeFormCollections(list, type, true);
                return true;
            }
        }
        return false;
    }

    bool getMainFormCollection()
    {
        // get form general form absPath from episodeBase
        QString absDirPath = episodeBase()->getGenericFormFile();
        if (absDirPath.isEmpty()) {
            // TODO: code here: manage no patient form file recorded in episodebase
            LOG_ERROR_FOR(q, "No patient central form defined");
            return false;
        }

        // load central root forms, create cache and duplicates
        if (!loadFormCollection(absDirPath, CompleteForms)) {
            LOG_ERROR_FOR(q, "Unable to load main form: " + absDirPath);
            return false;
        }
        const FormCollection &main = extractFormCollectionFrom(_centralFormCollection, CompleteForms, absDirPath);
        if (main.isNull()) {
            LOG_ERROR_FOR(q, "Unable to load main form: " + absDirPath);
            return false;
        }

        // load pmhx
        if (main.emptyRootForms().count() > 0) {
            main.emptyRootForms().at(0)->reader()->loadPmhCategories(absDirPath);
            return true;
        }
        return false;
    }

    bool loadPatientSubForms()
    {
        // get sub-forms from database
        const QVector<SubFormInsertionPoint> &subs = episodeBase()->getSubFormFiles();

//        qWarning() << "SUB" << subs;

        if (subs.isEmpty())
            return true;

        // clear all FormTreeModel from subForms
        foreach(FormTreeModel *model, _formTreeModels.values())
            model->clearSubForms();

        // Repopulate FormTreeModel with subforms
        for(int i=0; i < subs.count(); ++i) {
            const SubFormInsertionPoint &sub = subs.at(0);

            // Load subform
            if (!loadFormCollection(sub.subFormUid(), SubForms)) {
                LOG_ERROR_FOR(q, "Unable to load subform: " + sub.subFormUid());
                return false;
            }

            // Find all FormTreeModel with the formUid && populate them
            if (sub.receiverUid() == Constants::ROOT_FORM_TAG) {
                // Add to the central mode
                FormTreeModel *model = getFormTreeModel(Core::Constants::MODE_PATIENT_FILE, ModeForms);
//                qWarning() << model;
                if (model)
                    model->addSubForm(sub);
            } else {
                foreach(FormCollection *collection, _centralFormCollection) {
                    if (collection->formUid() == sub.receiverUid()) {
                        // update the formtreemodel
                        FormTreeModel *model = getFormTreeModel(collection->formUid(), collection->type()==FormCollection::CompleteForm ? CompleteForms : SubForms );
//                        qWarning() << model;
                        if (model)
                            model->addSubForm(sub);
                        break;
                    }
                }
            }
        }
        return true;
    }

    bool readPmhxCategories(const QString &formUuidOrAbsPath)
    {
        qWarning() << "readPmhxCategories";
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

    FormTreeModel *getFormTreeModel(const QString &uid, FormType type)
    {
        FormTreeModel *model = _formTreeModels.value(uid, 0);
        if (!model) {
            // Not already available -> create it (&& load form if needed)
            if (type == ModeForms) {
                const FormCollection &collection = extractFormCollectionFrom(_centralFormCollection, type, uid);
                if (collection.isNull()) {
                    LOG_ERROR_FOR(q, "unable to create formtreemodel");
                    return 0;
                } else {
                    model = new FormTreeModel(collection, q);
                }
            } else if (type == CompleteForms) {
                const FormCollection &collection = extractFormCollectionFrom(_centralFormCollection, type, uid);
                if (collection.isNull()) {
                    // Load the form
                    if (!loadFormCollection(uid, type)) {
                        LOG_ERROR_FOR(q, "unable to create formtreemodel");
                        return 0;
                    }
                    model = new FormTreeModel(extractFormCollectionFrom(_centralFormCollection, type, uid), q);
                } else {
                    model = new FormTreeModel(collection, q);
                }
            } else {
                const FormCollection &collection = extractFormCollectionFrom(_subFormCollection, type, uid);
                if (collection.isNull()) {
                    // Load the form
                    if (!loadFormCollection(uid, type)) {
                        LOG_ERROR_FOR(q, "unable to create formtreemodel");
                        return 0;
                    }
                    model = new FormTreeModel(extractFormCollectionFrom(_subFormCollection, type, uid), q);
                } else {
                    model = new FormTreeModel(collection, q);
                }
            }
            _formTreeModels.insert(uid, model);
        }
        return model;
    }

public:
    QVector<FormCollection *> _centralFormCollection, _centralFormDuplicateCollection, _subFormCollection, _subFormDuplicateCollection;
    FormCollection _nullFormCollection;
    QHash<QString, FormTreeModel *> _formTreeModels;
    // OLD
    QVector<Form::FormPage *> _formPages;
    QList<Form::FormMain *> m_RootForms, m_RootFormsDuplicates, m_SubFormsEmptyRoot, m_SubFormsEmptyRootDuplicates;
    QHash<Form::FormMain *, Form::FormMain *> _formParents; // keep the formMain parent in cache (K=form to reparent, V=emptyrootform)

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

/** Return the Form::FormCollection to use for the mode uid \e modeUid */
const FormCollection &FormManager::centralFormCollection(const QString &modeUId) const
{
    return d->extractFormCollectionFrom(d->_centralFormCollection, FormManagerPrivate::ModeForms, modeUId);
}

/** Return the Form::FormCollection corresponding to the subform \e subFormUid */
const FormCollection &FormManager::subFormCollection(const QString &subFormUid) const
{
    return d->extractFormCollectionFrom(d->_subFormCollection, FormManagerPrivate::SubForms, subFormUid);
}

/**
 * Return the duplicate Form::FormCollection to use for the mode uid \e modeUid.
 * Duplicate collection can be used to make computation without perturbing the user experience and the ui.
 */
const FormCollection &FormManager::centralFormDuplicateCollection(const QString &modeUId) const
{
    return d->extractFormCollectionFrom(d->_centralFormDuplicateCollection, FormManagerPrivate::ModeForms, modeUId);
}

/**
 * Return the Form::FormCollection corresponding to the subform \e subFormUid
 * Duplicate collection can be used to make computation without perturbing the user experience and the ui.
 */
const FormCollection &FormManager::subFormDuplicateCollection(const QString &subFormUid) const
{
    return d->extractFormCollectionFrom(d->_subFormDuplicateCollection, FormManagerPrivate::SubForms, subFormUid);
}

/** Return the Form::FormTreeModel corresponding to the mode \e modeUid */
FormTreeModel *FormManager::formTreeModelForMode(const QString &modeUid)
{
    return d->getFormTreeModel(modeUid, FormManagerPrivate::ModeForms);
}

/** Return the Form::FormTreeModel corresponding to the complete form uid \e formUid */
FormTreeModel *FormManager::formTreeModelForCompleteForm(const QString &formUid)
{
    return d->getFormTreeModel(formUid, FormManagerPrivate::CompleteForms);
}

/** Return the Form::FormTreeModel corresponding to the subForm \e surFormUid */
FormTreeModel *FormManager::formTreeModelForSubForm(const QString &subFormUid)
{
    return d->getFormTreeModel(subFormUid, FormManagerPrivate::SubForms);
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
    const QList<FormMain*> &roots = allEmptyRootForms();
    for(int i=0; i < roots.count(); ++i) {
        Form::FormMain *form = roots.at(i);
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
 * Return all available empty root forms (complete or subforms) for the current patient.
 * \sa Form::FormMain
 */
QList<FormMain *> FormManager::allEmptyRootForms() const
{
    QList<FormMain*> roots;
    foreach(FormCollection *collection, d->_centralFormCollection)
        roots << collection->emptyRootForms();
    foreach(FormCollection *collection, d->_subFormCollection)
        roots << collection->emptyRootForms();
    return roots;
}

/**
 * Insert a sub-form to a form to the specified \e insertionPoint.
 * A signal is emitted before Form::FormMain are getting reparented.
 */
bool FormManager::insertSubForm(const SubFormInsertionPoint &insertionPoint)
{
//    return d->insertSubForm(insertionPoint);
    return true;
}

/** Load the generic patient file (and included subforms) and emit patientFormsLoaded() when finished. */
bool FormManager::loadPatientFile()
{
    return onCurrentPatientChanged();
}

bool FormManager::onCurrentPatientChanged()
{
    if (d->getMainFormCollection()) {
        LOG("Central patient file loaded");
    } else {
        LOG_ERROR("Unable to load central patient file");
        return false;
    }

    // reinitialize all formtreemodels
    foreach(FormTreeModel *model, d->_formTreeModels.values()) {
        model->refreshFormTree();
    }

    // load subforms
    d->loadPatientSubForms();

    Q_EMIT patientFormsLoaded();
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
    const QList<FormMain*> &roots = allEmptyRootForms();
    for(int i=0; i < roots.count(); ++i) {
        FormMain *root = roots.at(i);
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
    foreach(FormCollection *collection, d->_centralFormCollection) {
        FormMain *identity = collection->identityForm();
        if (identity)
            return identity;
    }
    foreach(FormCollection *collection, d->_subFormCollection) {
        FormMain *identity = collection->identityForm();
        if (identity)
            return identity;
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
    foreach(FormCollection *collection, d->_centralFormDuplicateCollection) {
        FormMain *identity = collection->identityForm();
        if (identity)
            return identity;
    }
    foreach(FormCollection *collection, d->_subFormDuplicateCollection) {
        FormMain *identity = collection->identityForm();
        if (identity)
            return identity;
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

void FormManager::checkFormUpdates()
{
    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    if (list.isEmpty()) {
        LOG_ERROR("No IFormIO loaded...");
        return;
    }

//    QProgressDialog dlg(qApp->activeWindow());
//    dlg.setModal(true);
//    dlg.exec();
    // Check form update
    foreach(Form::IFormIO *io, list) {
        io->checkForUpdates();
    }
}

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
