/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *                                                                         *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Form::FormManager
 * This class manages all aspect of the patient's forms. It loads the patient
 * central form and all included forms using the Form::IFormIO objects. These forms
 * are used to create the patient forms and integrate them in the UI.\n
 * Due to the QObject copy protection, forms are always duplicated by the Form::IFormIO
 * readers, one collection is used for the user edition,
 * another collection is used for internal computations (the duplicates one).
 *
 * Management of Form::IFormIO:\n
 * - getting forms when patient is selected
 * - getting form updates at startup
 * - extracting screenshots and files from the 'form set'
 *
 * Form::FormManager also manages :
 * - a cache system to accelerate form access and construction
 * - keeps duplicates of each forms to use in specific computation (statistics...).
 * - allow creation of Form::FormPage (createFormPage())
 * - manages all Form::FormTreeModel (one per Form::FormMain empty root)
 * - manages datapack installation (form set datpacks)
 * - Core::IToken generation for each item
 *
 * \sa Form::IFormIO, Form::FormMain, Form::FormItem
 */

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
#include "formitemtoken.h"

#include "iformwidgetfactory.h"
#include "iformitemdata.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iscriptmanager.h>
#include <coreplugin/icommandline.h>
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
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_current.h>

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
#include <QTextDocument>

#include <QDebug>

enum { WarnFormCreation = false,
       ManageDuplicates = true,
       LogChronos       = true
     };

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::IPadTools *padTools()  { return Core::ICore::instance()->padTools(); }
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

namespace Form {
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
        _initialized(false),
        _forceFormLoading(false),
        _identityForm(0),
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

    // Returns all empty root forms of each collections
    QList<FormMain *> allEmptyRootForms() const
    {
        QList<FormMain*> roots;
        foreach(FormCollection *collection, _centralFormCollection)
            roots << collection->emptyRootForms();
        foreach(FormCollection *collection, _subFormCollection)
            roots << collection->emptyRootForms();
        return roots;
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
                // Load the forms once (for the main collection)
                QList<Form::FormMain *> list = io->loadAllRootForms(uid);

                // Check list for identity form
                if (!_identityForm) {
                    FormCollection *collection = new FormCollection;
                    collection->setEmptyRootForms(list);
                    _identityForm = collection->identityForm();
                    if (_identityForm) {
                        LOG_FOR(q, "Identity form detected: " + _identityForm->uuid());
                        // Reparent identity form and delete all other Form::FormMain
                        _identityForm->setParent(q);
                        list.removeAll(_identityForm);
                        qDeleteAll(list);
                        // Re-load the forms once (for the main collection)
                        list.clear();
                        list = io->loadAllRootForms(uid);
                    }
                    collection->setEmptyRootForms(QList<Form::FormMain *>());
                    delete collection;
                }

                // Create the main collection
                createModeFormCollections(list, type, false);
                list.clear();

                // Create its duplicate
                list = io->loadAllRootForms(uid);
                createModeFormCollections(list, type, true);
                LOG_FOR(q, QString("Form %1 loaded from reader %2")
                        .arg(uid)
                        .arg(io->name()));
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
            Q_ASSERT(false);
            return false;
        }

        LOG_FOR(q, "Loading patient form: " + absDirPath);
        // load central root forms, create cache and duplicates
        if (!loadFormCollection(absDirPath, CompleteForms)) {
            LOG_ERROR_FOR(q, "Unable to load main form: " + absDirPath);
            return false;
        }
        const FormCollection &main = extractFormCollectionFrom(_centralFormCollection, CompleteForms, absDirPath);
        if (main.isNull()) {
            LOG_ERROR_FOR(q, QString("Unable to load main form: %1. No collection loaded").arg(absDirPath));
            return false;
        }

        // load pmhx
        if (main.emptyRootForms().count() > 0) {
            main.emptyRootForms().at(0)->reader()->loadPmhCategories(absDirPath);
            return true;
        }
        LOG_FOR(q, "Patient form loaded : " + absDirPath);
        return false;
    }

    bool insertSubFormInModels(const SubFormInsertionPoint &insertionPoint)
    {
        if (!insertionPoint.isValid()) {
            LOG_ERROR_FOR(q, "Insertion point is not valid");
            return false;
        }
        // Load subform
        if (!loadFormCollection(insertionPoint.subFormUid(), SubForms)) {
            LOG_ERROR_FOR(q, "Unable to load subform: " + insertionPoint.subFormUid());
            return false;
        }

        // TODO: improve this code and manage modeuid
        // Find all FormTreeModel with the formUid && populate them
        if (insertionPoint.receiverUid() == Constants::ROOT_FORM_TAG) {
            // Add to the central mode
            QString mode = insertionPoint.modeUid();
            if (mode.isEmpty())
                mode = Core::Constants::MODE_PATIENT_FILE;
            FormTreeModel *model = getFormTreeModel(mode, ModeForms);
            if (model)
                return model->addSubForm(insertionPoint);
        } else {
            // Try to find the insertion point form
            QList<FormCollection *> colls;
            colls << _centralFormCollection.toList();
            colls << _subFormCollection.toList();
            foreach(FormCollection *coll, colls) {
                FormMain *receiver = coll->form(insertionPoint.receiverUid());
                if (receiver) {
                    QString mode = insertionPoint.modeUid();
                    if (mode.isEmpty())
                        mode = Core::Constants::MODE_PATIENT_FILE;
                    // update the formtreemodel
                    FormTreeModel *model = getFormTreeModel(mode, ModeForms); //coll->type() == FormCollection::CompleteForm ? CompleteForms : SubForms);
                    if (model)
                        return model->addSubForm(insertionPoint);
                }
            }
            LOG_ERROR_FOR(q, "Insertion point receiver not found");
            return false;
        }
        return false;
    }

    bool removeSubFormFromModels(const SubFormRemoval &remove)
    {
        if (!remove.isValid())
            return false;

//        // Find all FormTreeModel with the formUid && populate them
//        if (insertionPoint.receiverUid() == Constants::ROOT_FORM_TAG) {
//            // Add to the central mode
//            QString mode = insertionPoint.modeUid();
//            if (mode.isEmpty())
//                mode = Core::Constants::MODE_PATIENT_FILE;
//            FormTreeModel *model = getFormTreeModel(mode, ModeForms);
//            if (model)
//                model->addSubForm(insertionPoint);
//        } else {
//            foreach(FormCollection *collection, _centralFormCollection) {
//                if (collection->formUid() == insertionPoint.receiverUid()) {
//                    // update the formtreemodel
//                    FormTreeModel *model = getFormTreeModel(collection->formUid(), collection->type()==FormCollection::CompleteForm ? CompleteForms : SubForms );
//                    if (model)
//                        model->addSubForm(insertionPoint);
//                    break;
//                }
//            }
//        }
        return true;
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
            const SubFormInsertionPoint &sub = subs.at(i);
            insertSubFormInModels(sub);
        }
        return true;
    }

    bool readPmhxCategories(const QString &formUuidOrAbsPath)
    {
//        qWarning() << "readPmhxCategories";
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
                    LOG_ERROR_FOR(q, QString("Unable to create formtreemodel: %1").arg(uid));
                    return 0;
                } else {
                    model = new FormTreeModel(collection, q);
                }
            } else if (type == CompleteForms) {
                const FormCollection &collection = extractFormCollectionFrom(_centralFormCollection, type, uid);
                if (collection.isNull()) {
                    // Load the form
                    if (!loadFormCollection(uid, type)) {
                        LOG_ERROR_FOR(q, QString("Unable to create formtreemodel: %1").arg(uid));
                        return 0;
                    }
                    model = new FormTreeModel(extractFormCollectionFrom(_centralFormCollection, type, uid), q);
                } else {
                    model = new FormTreeModel(collection, q);
                }
            } else if (type == SubForms) {
                const FormCollection &collection = extractFormCollectionFrom(_subFormCollection, type, uid);
                if (collection.isNull()) {
                    // Load the form
                    if (!loadFormCollection(uid, type)) {
                        LOG_ERROR_FOR(q, QString("Unable to create formtreemodel: %1").arg(uid));
                        return 0;
                    }
                    model = new FormTreeModel(extractFormCollectionFrom(_subFormCollection, type, uid), q);
                } else {
                    model = new FormTreeModel(collection, q);
                }
            }
            model->initialize();
            _formTreeModels.insert(uid, model);
        }
        return model;
    }

    void createTokenNamespaces()
    {
#ifdef WITH_PAD
        // Create form token namespaces
        // Create and register namespaces
        Core::TokenNamespace formNs(Constants::TOKENFORM_NAMESPACE);
        formNs.setUntranslatedHumanReadableName(Trans::Constants::FORMS);
        // TODO: improve the help text & tooltip
        formNs.setUntranslatedHelpText(Trans::Constants::FORMS);
        formNs.setUntranslatedTooltip(Trans::Constants::FORMS);

        Core::TokenNamespace formLabelNs(Constants::TOKENFORM_LABEL_NAMESPACE);
        formLabelNs.setUntranslatedHumanReadableName(Trans::Constants::LABEL);
        Core::TokenNamespace formTooltipNs(Constants::TOKENFORM_TOOLTIP_NAMESPACE);
        formLabelNs.setUntranslatedHumanReadableName(Trans::Constants::TOOLTIP);

        Core::TokenNamespace formData(Constants::TOKENFORM_DATA_NAMESPACE);
        formData.setTrContext(Constants::FORM_TR_CONTEXT);
        formData.setUntranslatedHumanReadableName(Constants::TOKENFORM_DATA_NAMESPACE_TEXT);

        Core::TokenNamespace formDataPatient(Constants::TOKENFORM_PATIENTDATA_NAMESPACE);
        formDataPatient.setTrContext(Constants::FORM_TR_CONTEXT);
        formDataPatient.setUntranslatedHumanReadableName(Constants::TOKENFORM_PATIENTDATA_NAMESPACE_TEXT);

        Core::TokenNamespace formDataPrint(Constants::TOKENFORM_PRINTDATA_NAMESPACE);
        formDataPrint.setTrContext(Constants::FORM_TR_CONTEXT);
        formDataPrint.setUntranslatedHumanReadableName(Constants::TOKENFORM_PRINTDATA_NAMESPACE_TEXT);

        Core::TokenNamespace formDataItem(Constants::TOKENFORM_ITEMDATA_NAMESPACE);
        formDataItem.setTrContext(Constants::FORM_TR_CONTEXT);
        formDataItem.setUntranslatedHumanReadableName(Constants::TOKENFORM_ITEMDATA_NAMESPACE_TEXT);

        formData.addChild(formDataPatient);
        formData.addChild(formDataPrint);
        formData.addChild(formDataItem);

        formNs.addChild(formLabelNs);
        formNs.addChild(formTooltipNs);
        formNs.addChild(formData);

        if (padTools() && padTools()->tokenPool()) {
            padTools()->tokenPool()->registerNamespace(formNs);
        }
#endif
    }

public:
    bool _initialized;
    QVector<FormCollection *> _centralFormCollection, _centralFormDuplicateCollection, _subFormCollection, _subFormDuplicateCollection;
    FormCollection _nullFormCollection;
    QHash<QString, FormTreeModel *> _formTreeModels;
    bool _forceFormLoading;
    QVector<Form::FormPage *> _formPages;
    Form::FormMain *_identityForm;
    QMultiHash<Form::FormMain *, Core::IToken *> _tokens;

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
    if (d->_initialized)
        return true;

    d->createTokenNamespaces();
    //    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));

    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    connect(packManager(), SIGNAL(packRemoved(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    //    connect(packManager(), SIGNAL(packUpdated(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));

    d->_initialized = true;
    return d->_initialized;
}

/** Returns \e true if the core is initialized (with or without error) */
bool FormManager::isInitialized() const
{
    return d->_initialized;
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

/** Return the central patient file form uuid for the current patient */
QString FormManager::centralFormUid() const
{
    return episodeBase()->getGenericFormFile();
}

/**
 * Scans all loaded forms (complete and sub-forms) and extract all available modeUid
 * A mode is the same as a FormPage
 */
QStringList FormManager::availableModeUids() const
{
    return d->_formTreeModels.uniqueKeys();
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
    const QList<FormMain*> &roots = d->allEmptyRootForms();
    for(int i=0; i < roots.count(); ++i) {
        Form::FormMain *form = roots.at(i);
        if (form->uuid()==formUid)
            return form;
        const QList<Form::FormMain*> &children = form->flattenedFormMainChildren();
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
    return d->allEmptyRootForms();
}

/**
 * Return all available empty root forms (complete or subforms) for the current patient
 * using the duplicates forms (Forms are always duplicated, one is use for the user edition,
 * the other is used for internal computations - the duplicates one).
 * \sa Form::FormMain
 */
QList<FormMain *> FormManager::allDuplicatesEmptyRootForms() const
{
    QList<FormMain*> roots;
    foreach(FormCollection *collection, d->_centralFormDuplicateCollection)
        roots << collection->emptyRootForms();
    foreach(FormCollection *collection, d->_subFormDuplicateCollection)
        roots << collection->emptyRootForms();
    return roots;
}

/**
 * Insert a sub-form to a form to the specified \e insertionPoint.
 * A signal is emitted before Form::FormMain are getting reparented.
 */
bool FormManager::insertSubForm(const SubFormInsertionPoint &insertionPoint)
{
    bool ok = d->insertSubFormInModels(insertionPoint);
    if (!ok) {
        LOG_ERROR(tr("Unable to insert sub-form %1 into form %2")
                  .arg(insertionPoint.subFormUid())
                  .arg(insertionPoint.receiverUid()));
        return false;
    } else
    if (insertionPoint.emitInsertionSignal())
        Q_EMIT subFormLoaded(insertionPoint.subFormUid());
    return ok;
}

/** Remove a sub-form according to the \e subFormRemoval value */
bool FormManager::removeSubForm(const SubFormRemoval &subFormRemoval)
{
    qWarning() << subFormRemoval.modeUid() << subFormRemoval.receiverUid() << subFormRemoval.subFormUid();
    return true;
}

/** Load the generic patient file (and included subforms) and emit patientFormsLoaded() when finished. */
bool FormManager::loadPatientFile()
{
    d->_forceFormLoading = true;
    bool ok = onCurrentPatientChanged();
    d->_forceFormLoading = false;
    return ok;
}

/**
 * Reacts on current patient changed.
 * - Load central form if needed (because all patients have the same central form)
 * - Refresh all form tree models
 * - Load subforms into the form tree models
 * - Emits patientFormsLoaded() when done
*/
bool FormManager::onCurrentPatientChanged()
{
    if (!d->_forceFormLoading && patient()->uuid().isEmpty()) {
        LOG("No current patient.");
        return true;
    }

    QTime chrono;
    if (LogChronos)
        chrono.start();

    if (d->getMainFormCollection()) {
        LOG("Central patient file loaded");
    } else {
        LOG_ERROR("PatientChanged: Unable to load central patient file");
        return false;
    }
    if (LogChronos)
        Utils::Log::logTimeElapsed(chrono, objectName(), "onCurrentPatientChanged::get form collections");

    // reinitialize all formtreemodels
    foreach(FormTreeModel *model, d->_formTreeModels.values()) {
        model->refreshFormTree();
    }
    if (LogChronos)
        Utils::Log::logTimeElapsed(chrono, objectName(), "onCurrentPatientChanged::refresh formtreemodels");

    // load subforms
    d->loadPatientSubForms();
    if (LogChronos)
        Utils::Log::logTimeElapsed(chrono, objectName(), "onCurrentPatientChanged::load subforms");

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
    const QList<FormMain*> &roots = d->allEmptyRootForms();
    for(int i=0; i < roots.count(); ++i) {
        FormMain *root = roots.at(i);
        if (root->modeUniqueName().compare(QString(modeUniqueName), Qt::CaseInsensitive)==0) {
            return root;
        }
    }
    return 0;
}

/**
 * Return the specific identity form. You can populate it with any value without any
 * changes to the current patient (Core::IPatient) data. If no identity form is found
 * returns zero.
*/
Form::FormMain *FormManager::identityRootForm() const
{
    return d->_identityForm;
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

/**
 * Extract a specific file base on its \e fileName from the the form \e formUid
 * and return the absolute filepath of the extracted file.
*/
QString FormManager::extractFormFileToTmpPath(const QString &formUid, const QString &fileName)
{
    if (formUid.isEmpty()) {
        LOG_ERROR("No formUid...");
        return QString::null;
    }

    // get all form readers (IFormIO)
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    if (list.isEmpty()) {
        LOG_ERROR("No IFormIO loaded...");
        return QString::null;
    }

    // Load root forms
    QString output;
    foreach(Form::IFormIO *io, list) {
        output = io->extractFileToTmpPath(formUid, fileName);
        if (!output.isNull())
            return output;
    }
    return QString::null;
}

/**
 * Return the printable HTML resulting of a Form::FormMain
 * transformation (using the print mask if available)
 */
QString FormManager::formPrintHtmlOutput(Form::FormMain *formMain)
{
    QString out;

    // FIXME: same code for export/print, add a param to the member like an enumerator (Print/Export...)
    // FIXME: For each FormMain children get the HTML output

    // If a print mask is available, create tokens for the form
    if (!formMain->spec()->value(Form::FormItemSpec::Spec_HtmlPrintMask).toString().isEmpty()) {
        out = formMain->spec()->value(Form::FormItemSpec::Spec_HtmlPrintMask).toString();
        const QHash<QString, QVariant> &tokens = formToTokens(formMain);
        Utils::replaceTokens(out, tokens);
        patient()->replaceTokens(out);
        user()->replaceTokens(out);
#ifdef WITH_PAD
        out = padTools()->processHtml(out);
#endif
    } else {
        out = "<html><body>" + formMain->printableHtml(true) + "</body></html>";
    }
    return out;
}

/**
 * Return the printable HTML resulting of a Form::FormMain
 * transformation (using the print mask if available)
 */
QString FormManager::formExportHtmlOutput(Form::FormMain *formMain)
{
    QString out;
    // If a print mask is available, create tokens for the form
    if (!formMain->spec()->value(Form::FormItemSpec::Spec_HtmlExportMask).toString().isEmpty()) {
        // TODO: manage PadTools
        out = formMain->spec()->value(Form::FormItemSpec::Spec_HtmlExportMask).toString();
        const QHash<QString, QVariant> &tokens = formToTokens(formMain);
        Utils::replaceTokens(out, tokens);
        patient()->replaceTokens(out);
        user()->replaceTokens(out);
#ifdef WITH_PAD
        out = padTools()->processHtml(out);
#endif
    } else {
        out = "<html><body>" + formMain->printableHtml(true) + "</body></html>";
    }
    return out;
}

/**
 * Create a token for each FormItem of the \e form.
 * Also add its label (with a '.label' ending token)
 * and a tooltip label (with a '.tooltip' ending token)
 */
QHash<QString, QVariant> FormManager::formToTokens(Form::FormMain *form) const
{
    QHash<QString, QVariant> tokens;
#ifdef WITH_PAD
    // Removes all formitem token from the token pool
    // Include all formitems token from the formmain to the token pool
    if (d->_tokens.values(form).isEmpty()) {
        QVector<FormItemToken::ValueType> types;
        types << FormItemToken::FormItemLabel
              << FormItemToken::FormItemTooltip
              << FormItemToken::FormItemPatientModelValue
              << FormItemToken::FormItemPrintValue
              << FormItemToken::FormItemDataValue;
        foreach(Form::FormItem *item, form->flattenedFormItemChildren()) {
            for(int i = 0; i < types.count(); ++i) {
                if (FormItemToken::canManageValueType(item, types.at(i)))
                    d->_tokens.insertMulti(form, new FormItemToken(item, types.at(i)));
            }
        }
        padTools()->tokenPool()->addTokens(d->_tokens.values(form).toVector());
    }
#else
    // Create a token for each FormItem of the FormMain (label and value)
    foreach(FormItem *item, form->flattenedFormItemChildren()) {
        tokens.insert(item->uuid() + ".label", item->spec()->label());
        tokens.insert(item->uuid() + ".tooltip", item->spec()->tooltip());
        if (item->itemData()) {
            QString value = item->itemData()->data(0, Form::IFormItemData::PrintRole).toString();
            if (Qt::mightBeRichText(value)) {
                if (value.contains("<body") && value.contains("</body>")) {
                    // extract css
                    QString css = Utils::htmlTakeAllCssContent(value);
                    // remove body
                    value = Utils::htmlBodyContent(value);
                    // reinject css
                    value.prepend(css);
                }
                tokens.insert(item->uuid(), Utils::htmlReplaceParagraphWithDiv(value));
            } else {
                tokens.insert(item->uuid(), item->itemData()->data(0, Form::IFormItemData::PrintRole));
            }
        }
    }
#endif
    // Create tokens for episode data
    // date
    tokens.insert("EpisodeUserDate", QLocale().toString(form->itemData()->data(Form::IFormItemData::ID_EpisodeDateTime).toDate(), QLocale::ShortFormat));
    tokens.insert("EpisodeUserDateLong", QLocale().toString(form->itemData()->data(Form::IFormItemData::ID_EpisodeDateTime).toDate(), QLocale::LongFormat));
    // date & time
    QDateTime userDateTimeUtc(QDateTime::fromString(form->itemData()->data(Form::IFormItemData::ID_EpisodeDateTime).toString(), Qt::ISODate));
    QDateTime userDateTimeLocal(userDateTimeUtc.toLocalTime());
    tokens.insert("EpisodeUserDateTime", QLocale().toString(userDateTimeLocal, QLocale::ShortFormat));
    tokens.insert("EpisodeUserDateTimeLong", QLocale().toString(userDateTimeLocal, QLocale::LongFormat));

    tokens.insert("EpisodeUserLabel", form->itemData()->data(Form::IFormItemData::ID_EpisodeLabel));
    // Force the full name of the user
    QString userName = form->itemData()->data(Form::IFormItemData::ID_UserName).toString();
    if (userName == tkTr(Trans::Constants::YOU))
        userName = user()->value(Core::IUser::FullName).toString();
    else if (userName.isEmpty())
        userName = qApp->applicationName();
    tokens.insert("EpisodeUserName", userName);
    tokens.insert("EpisodeUserName.createdBy", tkTr(Trans::Constants::CREATED_BY_1).arg(userName));
    tokens.insert("EpisodePriority", form->itemData()->data(Form::IFormItemData::ID_Priority));
    tokens.insert("EpisodeFormLabel", form->spec()->label());
    Form::FormMain *parent = form;
    QStringList fullFormName;
    while (parent) {
        if (!parent->spec()->label().isEmpty())
            fullFormName.prepend(parent->spec()->label());
        parent = parent->formParent();
    }
    tokens.insert("EpisodeFullFormLabel", fullFormName.join(" / "));
    return tokens;
}

/**
 * Start the Form update checking.
 * Forms are stored into the user database to ensure their constant availability.
 * This function will check local files for an update of the form stored in the
 * user database. \n
 * \note If the version of form is defined to "test", it will \e always be considered as
 * an update. You can use this version naming for your form creation and testings.
 */
void FormManager::checkFormUpdates()
{
    if (!commandLine()->value(Core::ICommandLine::CheckFormUpdates).toBool())
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
        // No update ?
        if (io->availableUpdates().isEmpty())
            continue;

        // Has update. Construct the detailed text of the user's question messagebox
        QStringList msg;
        foreach(const Form::FormIODescription &descr, io->availableUpdates()) {
            QString html;
            html = QString("<b>%1</b><br />&nbsp;&nbsp;•&nbsp;%2<br />%3<br />")
                    .arg(tr("Form: ") + descr.data(Form::FormIODescription::ShortDescription).toString())
                    .arg(tr("New version: %1").arg(descr.data(Form::FormIODescription::Version).toString()))
                    .arg(tr("Database version: %1").arg(descr.data(Form::FormIODescription::UpdateAvailable_OldVersion).toString()))
                    ;
            foreach(const Utils::GenericUpdateInformation &u, descr.updateInformationForVersion(descr.data(Form::FormIODescription::UpdateAvailable_OldVersion).toString())) {
                html += "&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;" + Utils::firstLetterUpperCase(tkTr(Trans::Constants::FROM_1_TO_2).arg(u.fromVersion()).arg(u.toVersion())) + "&nbsp;:<br /> " + u.text() + "<br />";
            }
            msg << html;
        }

        // Ask user according to commandline params
        if (!commandLine()->value(Core::ICommandLine::AlwaysUpdateForms).toBool()) {
            if (!Utils::yesNoMessageBox(tr("Form update detected."),
                                       tr("A form update has been detected. Do you want to update the forms?"),
                                       msg.join("<br />")) == true)
                continue;
        }
        if (!io->updateForms()) {
            LOG_ERROR("Unable to update forms");
        } else {
            LOG("Forms updated");
        }
    }
}

/**
 * Reacts to DataPack installation or removal.
 */
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
        //TODO: manage user interaction
        io->updateForms();
    }

    // Force patient files reloading
    if (!patient()->uuid().isEmpty())
        loadPatientFile();
}
