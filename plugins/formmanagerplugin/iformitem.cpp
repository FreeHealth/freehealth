/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 * \class Form::FormItem
 * This object represents one element of the patient file form. The forms are a
 * tree of QObjects.
 * The root parent is an empty Form::FormMain item with some information about
 * the Form::IFormIO reader, the form internal information.\n
 *
 * Each Form::FormItem contains:
 * - one Form::FormItemSpec with a description of the item (or the form for the
 *   empty root object):
 *   - author, version, license...
 *   - these data are fully translatable and can be used in any form
 *     model/view/controller
 * - one Form::FormItemScripts book of JavaScript of the item
 * - one Form::FormItemValues book of different types of values of the item
 * - you can set its Form::IFormItemData for the database access and widget data
 *   management
 * - you can set its Form::IFormWidget for a more specialized graphical user
 *   interface
 *
 * In your own Form::IFormIO engine, you may want to create children of item,
 * you can use:
 * - createChildForm() to create a new Form::FormMain
 * - createChildItem() to create a new Form::FormItem
 * - createPage() to create a new Form::FormPage
 *
 * You can get all of its first level children by calling
 * Form::FormItem::formItemChildren() and the full list of its children
 * (flattened) using Form::FormItem::flattenedFormItemChildren().
 *
 * You can define extra data (options) with the addExtraData(), extraData() and
 * clearExtraData().
 *
 * You can define the Core::IPatient representation of this Form::FormItem with
 * setPatientDataRepresentation(), patientDataRepresentation(). The patient data
 * representation is used in the Core::IPatient model wrapper to find requested
 * values that are not stored in the patient database.
 */
// TODO: change Form::FormItem pointers to references distributed by the
// Form::FormManager? like
// getItem(Form::FormMainIndex &index, const QString &uid)?

// TODO: - Display options and printing??
// TODO: - Rights options / authorized users
// TODO: - Rule base
// TODO: - 1 QWidget history
// TODO: - add a checkValueIntegrity() method, based on the rules of the item to
// verify its accuracy
// TODO: move multiple inline void findChildren(FormItem *item) to member
// function of FormItem class, as qFindChildren is deprecated

/**
 * \class Form::FormPage
 * Not used.
 */

/**
 * \class Form::FormMain
 * A Form::FormMain represents a root item of a form. Usually,
 * the Form::FormMain is created by the Form::IFormIO engines.
 * The first item returned by the Form::IFormIO when loading files is empty
 * and represents the root of the form (it does only handle some basic
 * information).\n
 *
 * Object tree management:\n
 * When you need to create a Form::FormMain as child of another one, use the
 * createChildForm() member.
 * Get all its children (including sub-trees) using
 * Form::FormMain::flattenedFormMainChildren()
 * or formMainChild() if you want a specific child.\n
 *
 * Managing episode behavior:\n
 * Forms can be populated by episodes. Episodes are mainly created by the user
 * when he wants to save some data using a specific form.\n
 * The Form::EpisodeModel and the Form::FormTreeModel manage three types of forms:
 * - Forms without any episode (just like a category)
 * - Forms with only one episode
 * - Forms with multiple episodes
 * You can define the episode possibilities using
 * Form::FormMain::setEpisodePossibilities() and
 * Form::FormMain::episodePossibilities().\n
 *
 * Form pointer accessing: \n
 * All Form::FormMain pointers are created by the Form::IFormIO engines and then
 * managed by Form::FormManager. You should never:
 * - delete a Form::FormMain pointer
 * - reparent it outside the internal part of the Form plugin
 * - create a cache of theses pointers, the Form::FormManager already manages
 *   this cache
 */
// TODO: change Form::FormMain pointers to references distributed by
// Form::FormManager?


/**
 * \class Form::FormItemSpec
 * Contains descriptions of a Form::FormItem (like author name, compatibility
 * version, form version, label, tooltip, license...).\n
 * You should never delete a Form::FormItemSpec pointer outside the internal
 * part of the Form plugin.
 */

/** \enum Form::FormItemSpec::SpecData
 * Use this enum to populate or get data from/to the object.
 * \sa Form::FormItemSpec::setValue(), Form::FormItemSpec::value()
 *
 * \var Form::FormItemSpec::Spec_Uuid
 * Unique persistent in time UUID of the item.
 *
 * \var Form::FormItemSpec::Spec_Author
 * Name of the author(s).
 *
 * \var Form::FormItemSpec::Spec_License
 * License type (abbreviation).
 *
 * \var Form::FormItemSpec::Spec_Category
 * Translatable category of the item.
 *
 * \var Form::FormItemSpec::Spec_CreationDate
 * Date of creation.
 *
 * \var Form::FormItemSpec::Spec_LastModified
 * Date of last modification.
 *
 * \var Form::FormItemSpec::Spec_Version
 * Version of the item.
 *
 * \var Form::FormItemSpec::Spec_Description
 * Translatable description of the item (usually HTML long description).
 *
 * \var Form::FormItemSpec::Spec_Bibliography
 * Translatable bibliography of the item (non-interpreted string).
 *
 * \var Form::FormItemSpec::Spec_Label
 * Translatable label of the item.
 *
 * \var Form::FormItemSpec::Spec_ExtraLabel
 * Translatable extra-label of the item. This extra-string is only used by some
 * plugins.
 *
 * \var Form::FormItemSpec::Spec_Priority
 * Priority of the item (usually an integer).\n
 * For eg, used to place Form::FormPage in the mainwindow tabbar.
 *
 * \var Form::FormItemSpec::Spec_Plugin
 * Name of the Form::IFormWidget plugin used to create the widget.
 *
 * \var Form::FormItemSpec::Spec_UiFileContent
 * Content of the QtUi file to use for the widget generation. Uses QtUiLoader.\n
 * \sa Form::IFormWidget, Form::IFormWidgetFactory
 *
 * \var Form::FormItemSpec::Spec_UiLabel
 * Used during the widget generation, define the Qt UI label to use.\n
 * Only used with a Qt UI file (Form::FormItemSpec::Spec_UiFileContent).
 * \sa Form::IFormWidget, Form::IFormWidgetFactory
 *
 * \var Form::FormItemSpec::Spec_UiWidget
 * Used during widget generation, define the Qt widget to use.\n
 * Only used with a Qt UI file (Form::FormItemSpec::Spec_UiFileContent).
 * \sa Form::IFormWidget, Form::IFormWidgetFactory
 *
 * \var Form::FormItemSpec::Spec_UiInsertIntoLayout
 * Used during widget generation, define the layout where to insert the widget.\n
 * Only used with a Qt UI file (Form::FormItemSpec::Spec_UiFileContent).
 * \sa Form::IFormWidget, Form::IFormWidgetFactory
 *
 * \var Form::FormItemSpec::Spec_HtmlPrintMask
 * Printing HTML mask of the form (using tokens)
 *
 * \var Form::FormItemSpec::Spec_PlainTextPrintMask
 * Printing plain text mask of the form (using tokens)
 *
 * \var Form::FormItemSpec::Spec_HtmlExportMask
 * Export mask of the form (using tokens) in HTML
 *
 * \var Form::FormItemSpec::Spec_PlainTextExportMask
 * Export mask of the form (using tokens) in plain text
 *
 * \var Form::FormItemSpec::Spec_IconFileName
 * Translatable icon file name (can contain path tag).
 * \sa Core::Constants::TAG_APPLICATION_THEME_PATH
 *
 * \var Form::FormItemSpec::Spec_Tooltip
 * Translatable tooltip of the item.
 *
 * \var Form::FormItemSpec::Spec_PlaceHolder
 * Translatable placeHolder (for line edits for eg)
 *
 * \var Form::FormItemSpec::Spec_IsIdentityForm
 * Boolean value. By default, set to false.\n
 * Only set for Form::FormMain. \n
 * Is true if the form owns the identity information/widgets.
 */

// TODO: change Form::FormItemSpec pointers to references


/**
 * \class Form::FormItemScripts
 * Stores the scripts associated with the Form::FormItem.
 * \todo Documentation
 */
// TODO: change Form::FormItemScripts pointers to references distributed by the
//Form::FormManager?
//like getScript(Form::FormMainIndex &index, const ScriptType &type)?

/**
  \class Form::FormItemValues
  \todo Documentation
*/
// TODO: change Form::FormItemValues pointers to references distributed by
//the Form::FormManager?
//like getValues(Form::FormMainIndex &index, const ValueType &type)?

#include "iformitem.h"
#include "formcore.h"
#include "formplaceholder.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/modemanager/imode.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/global.h>
#include <utils/log.h>

#include <translationutils/multilingualclasstemplate.h>
#include <translationutils/constanttranslations.h>

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QGridLayout>
#include <QHeaderView>
#include <QMap>

#include <QDebug>

using namespace Form;
using namespace Form::Internal;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

#ifdef DEBUG
enum {WarnFormCreation=false};
#else
enum {WarnFormCreation=false};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  FormItemScripts  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ScriptsBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l) const
    {
        new QTreeWidgetItem(l, QStringList() << "OnLoad" << m_Scripts.value(0) );
        new QTreeWidgetItem(l, QStringList() << "PostLoad" << m_Scripts.value(1) );
        new QTreeWidgetItem(l, QStringList() << "OnDemand" << m_Scripts.value(2) );
        new QTreeWidgetItem(l, QStringList() << "OnValueChanged" << m_Scripts.value(3) );
        new QTreeWidgetItem(l, QStringList() << "OnValueRequired" << m_Scripts.value(4) );
        new QTreeWidgetItem(l, QStringList() << "OnDependentValueChanged" << m_Scripts.value(5) );
        new QTreeWidgetItem(l, QStringList() << "OnToggled" << m_Scripts.value(6) );
    }

    QHash<int, QString> m_Scripts;
    QHash<int, int> m_States;
};

namespace Form {
namespace Internal {
class FormItemScriptsPrivate : public Trans::MultiLingualClass<ScriptsBook>
{
public:
    FormItemScriptsPrivate() {}
    ~FormItemScriptsPrivate() {}

    QString categoryForTreeWiget() const {return QString("Scripts");}
};

} // End Internal
} // End Form

FormItemScripts::FormItemScripts(
        const QString &lang,
        const QString &onLoad,
        const QString &postLoad,
        const QString &onDemand,
        const QString &onValChanged,
        const QString &onValRequired,
        const QString &onDependentValuesChanged,
        const QString &onToggled ) :
        d(new FormItemScriptsPrivate)
{
    ScriptsBook *s = d->createLanguage(lang.left(2));
    s->m_Scripts.insert(Script_OnLoad, onLoad);
    s->m_Scripts.insert(Script_PostLoad, postLoad);
    s->m_Scripts.insert(Script_OnDemand, onDemand);
    s->m_Scripts.insert(Script_OnValueChanged, onValChanged);
    s->m_Scripts.insert(Script_OnValueRequired, onValRequired);
    s->m_Scripts.insert(Script_OnDependentValueChanged, onDependentValuesChanged);
    s->m_Scripts.insert(Script_OnToggled, onToggled);
}

FormItemScripts::~FormItemScripts()
{
    if (d) delete d;
    d=0;
}

void FormItemScripts::setScript(const int type, const QString &script, const QString &lang)
{
//    WARN_FUNC << type << script << lang;
    ScriptsBook *s = 0;
    if (d->hasLanguage(lang))
        s = d->getLanguage(lang);
    else
        s = d->createLanguage(lang);
    s->m_Scripts.insert(type,script);
}

QString FormItemScripts::script(const int type, const QString &lang) const
{
    ScriptsBook *s = d->getLanguage(lang);
    if (!s) {
        s = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!s) {
            s = d->getLanguage("en"); // TODO check this, default language is xx not en
            if (!s)
                return QString::null;
        }
    }
    return s->m_Scripts.value(type);
}

void FormItemScripts::toTreeWidget(QTreeWidgetItem *tree)
{
    d->toTreeWidget(tree);
}

void FormItemScripts::warn() const
{
    ScriptsBook *s = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
    Utils::quickDebugDialog(
            QStringList()
            << QString("Script_OnLoad\n") + s->m_Scripts.value(Script_OnLoad)
            << QString("Script_PostLoad\n") + s->m_Scripts.value(Script_PostLoad)
            << QString("Script_OnDemand\n") + s->m_Scripts.value(Script_OnDemand)
            << QString("Script_OnValueChanged\n") + s->m_Scripts.value(Script_OnValueChanged)
            << QString("Script_OnValueRequired\n") + s->m_Scripts.value(Script_OnValueRequired)
            << QString("Script_OnDependentValueChanged\n") + s->m_Scripts.value(Script_OnDependentValueChanged)
            );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  FormItemValueReferences  ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Form {
namespace Internal {

class ValuesBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *l) const
    {
        QTreeWidgetItem *i = 0;
        i = new QTreeWidgetItem(l, QStringList() << "Possibles");
        foreach(int id, m_Possible.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Possible.value(id).toString() );
        }

        i = new QTreeWidgetItem(l, QStringList() << "Numerical");
        foreach(int id, m_Numerical.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Numerical.value(id).toString() );
        }

        i = new QTreeWidgetItem(l, QStringList() << "Script");
        foreach(int id, m_Script.keys()) {
            new QTreeWidgetItem(i, QStringList() << QString::number(id) << m_Script.value(id).toString() );
        }
    }

public:
    QMap<int, QVariant> m_Possible;
    QMap<int, QVariant> m_Script;
    QMap<int, QVariant> m_Numerical;
    QMap<int, QVariant> m_Printing;
    QMap<int, QVariant> m_Uuid;
    QVariant m_Default;
};

class FormItemValuesPrivate : public Trans::MultiLingualClass<ValuesBook>
{
public:
    FormItemValuesPrivate() {}
    ~FormItemValuesPrivate() {}

    QString categoryForTreeWiget() const {return QString("Value References");}
public:
    bool m_optional;
};
}  // End Internal
}  // End Form

FormItemValues::FormItemValues() :
    d(new Form::Internal::FormItemValuesPrivate)
{
}

FormItemValues::~FormItemValues()
{
    if (d) delete d;
    d=0;
}

/**
  \brief Defines a value \e val for one \e type at one specified \e id in a
  list, for one \e language.
  \e language must be a valid local language name in 2 chars encoding.
*/
void FormItemValues::setValue(int type, const int id, const QVariant &val, const QString &language)
{
    QString l = language;
    if (language.isEmpty() || (type==Value_Uuid))
        l = Trans::Constants::ALL_LANGUAGE;
    ValuesBook *values = d->createLanguage(l);
//    qWarning() << "SETVALUE" << l << language << values << type << id << val;
    switch (type)
    {
    case Value_Uuid :      values->m_Uuid.insert(id,val); break;
    case Value_Numerical : values->m_Numerical.insert(id,val); break;
    case Value_Script :    values->m_Script.insert(id,val); break;
    case Value_Possible :  values->m_Possible.insert(id,val); break;
    case Value_Printing:   values->m_Printing.insert(id,val); break;
    case Value_Default :   values->m_Default = val; break;
    case Value_Dependency : break;
    }
}

/** Defines the default value content using \e val for a specific language \e lang. */
void FormItemValues::setDefaultValue(const QVariant &val, const QString &lang)
{
    QString l = lang;
    if (lang.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    ValuesBook *values = d->createLanguage(l);
    values->m_Default = val;
}

/** Returns the default value content for a specific language \e lang. */
QVariant FormItemValues::defaultValue(const QString &lang) const
{
    QString l = lang;
    if (lang.isEmpty())
        l = QLocale().name();
    ValuesBook *values = d->getLanguage(l);
    QVariant ret;
    if (values)
        ret = values->m_Default;
    if (ret.isNull() && l==QLocale().name()) {
        values = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (values)
            return values->m_Default;
    }
    return QVariant();
}

//void FormItemValues::setSelectedValue(const QVariant &val)
//{
//    d->m_SelectedValue = val;
//}
//
//QVariant FormItemValues::selectedValue() const
//{
//    return d->m_SelectedValue;
//}

bool FormItemValues::isOptional() const
{
    return d->m_optional;
}

void FormItemValues::setOptional(bool state)
{
    d->m_optional = state;
}

/** \brief Returns a QStringList of values asked sorted by id number. \e typeOfValue represents the enumerator. */
QStringList FormItemValues::values(const int typeOfValues) const
{
    if (typeOfValues==Value_Uuid) {
        // return the ALL_LANGUAGE uuids
        ValuesBook *values = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!values)
            return QStringList();
        QStringList list;
        foreach(const QVariant &uuid, values->m_Uuid.values()) {
            list.append(uuid.toString());
        }
        return list;
    } else if (typeOfValues==Value_Default) {
        return QStringList() << defaultValue().toString();
    } else if (typeOfValues==Value_Dependency) {
        return QStringList();
    }
    ValuesBook *values = d->getLanguage(QLocale().name());
    QMap<int, QVariant> map;
    if (values) {
        switch (typeOfValues)
        {
        case Value_Numerical : map = values->m_Numerical; break;
        case Value_Script :    map = values->m_Script; break;
        case Value_Possible :  map = values->m_Possible; break;
        case Value_Printing :  map = values->m_Printing; break;
        }
    }
    if (map.isEmpty()) {
        values = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!values) {
            return QStringList();
        }
        switch (typeOfValues)
        {
        case Value_Numerical : map = values->m_Numerical; break;
        case Value_Script :    map = values->m_Script; break;
        case Value_Possible :  map = values->m_Possible; break;
        case Value_Printing :  map = values->m_Printing; break;
        }
    }
    QStringList list;
    foreach(int i, map.keys()) {
        list.append(map.value(i).toString());
    }
    return list;
}

/** Used for debugging purpose */
void FormItemValues::toTreeWidget(QTreeWidgetItem *tree) const
{
    d->toTreeWidget(tree);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  FormItem  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Form {
namespace Internal {
class FormItemPrivate
{
public:
    FormItemPrivate(FormItem *parent) :
        m_Spec(new FormItemSpec),
        m_Scripts(new FormItemScripts),
        m_Values(new FormItemValues),
        m_FormWidget(0),
        m_ItemData(0),
        m_PatientData(-1),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~FormItemPrivate()
    {
        if (m_Scripts) {
            delete m_Scripts;
            m_Scripts = 0;
        }
        if (m_Spec) {
            delete m_Spec;
            m_Spec = 0;
        }
        if (m_Values) {
            delete m_Values;
            m_Values = 0;
        }
        if (m_ItemData) {
            delete m_ItemData;
            m_ItemData = 0;
        }
//        if (m_FormWidget) {
//            delete m_FormWidget;
//            m_FormWidget = 0;
//        }
    }

public:
    Form::FormItemSpec *m_Spec;
    Form::FormItemScripts *m_Scripts;
    Form::FormItemValues *m_Values;
    QPointer<Form::IFormWidget> m_FormWidget;
    Form::IFormItemData *m_ItemData;
    QHash<QString, QString> m_ExtraData;
    int m_PatientData;

private:
    FormItem *q;
};
}
}

FormItem::FormItem(QObject *parent) :
    QObject(parent),
    d_ifi(new Internal::FormItemPrivate(this))
{}

FormItem::~FormItem()
{
    if (d_ifi) {
        delete d_ifi;
        d_ifi = 0;
    }
}

QString FormItem::uuid() const
{
    return d_ifi->m_Spec->uuid();
}

void FormItem::setUuid(const QString &uuid)
{
    d_ifi->m_Spec->setUuid(uuid);
}

Form::FormItemSpec *FormItem::spec() const {return d_ifi->m_Spec;}
Form::FormItemScripts *FormItem::scripts() const {return d_ifi->m_Scripts;}
Form::FormItemValues *FormItem::valueReferences() const {return d_ifi->m_Values;}

// Access to database values. Pointer will not be deleted
void FormItem::setItemData(Form::IFormItemData *data) {d_ifi->m_ItemData = data;}
Form::IFormItemData *FormItem::itemData() {return d_ifi->m_ItemData;}

// Access to the user's widget
void FormItem::setFormWidget(Form::IFormWidget *w) {d_ifi->m_FormWidget=w;}
IFormWidget *FormItem::formWidget() {return d_ifi->m_FormWidget;}

// Access to the FormItem tree
FormMain *FormItem::createChildForm(const QString &uuid) {Q_UNUSED(uuid); return 0;}
FormPage *FormItem::createPage(const QString &uuid) {Q_UNUSED(uuid); return 0;}

/*! Returns the FormItem extra data as QHash (keys are all lowercase) */
QHash<QString,QString> FormItem::extraData() const {return d_ifi->m_ExtraData;}

/*! Clears the internal extra data of the FormItem */
void FormItem::clearExtraData() {d_ifi->m_ExtraData.clear();}

/**
 * Define a patient data representation. The reference is taken from
 * Core::IPatient::PatientDataRepresentation
 */
void FormItem::setPatientDataRepresentation(const int ref) {d_ifi->m_PatientData = ref;}

/**
 * Return the patient data representation of the item.
 * The reference is taken from Core::IPatient::PatientDataRepresentation.
 */
int FormItem::patientDataRepresentation() const {return d_ifi->m_PatientData;}

/*!
 * \brief Adds user defined extra data to the FormItem.
 *
 * The data in \e value is saved as \e key-value pairs using a QHash<QString, QString>. If the key
 * already exists, the \e value is appended to that \e key with a ";" as delimiter.
 * Keys are \b lowered.
 * \sa extraData(), clearExtraData()
 */
void FormItem::addExtraData(const QString &key, const QString &value)
{
    if (d_ifi->m_ExtraData.keys().indexOf(key.toLower()) != -1) {
        QString add = d_ifi->m_ExtraData.value(key.toLower()) + ";" + value;
        d_ifi->m_ExtraData.insert(key.toLower(), add);
    } else {
        d_ifi->m_ExtraData.insert(key.toLower(), value);
    }
}

/** Returns all options defined for this item. */
QStringList FormItem::getOptions() const
{
    QStringList l;
    l = d_ifi->m_ExtraData.value("options").split(";", QString::SkipEmptyParts);
    l += d_ifi->m_ExtraData.value("option").split(";", QString::SkipEmptyParts);
    return l;
}

/** Creates a child Form::FormItem using the specified \e uuid. */
FormItem *FormItem::createChildItem(const QString &uuid)
{
    Form::FormItem *i = new Form::FormItem(this);
    if (!uuid.isEmpty())
        i->setUuid(uuid);
    return i;
}

void FormItem::languageChanged()
{
    qWarning() << "FormItem language changed" << uuid();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  FormPage  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormPage::FormPage(QObject *parent):
    Form::FormItem(parent),
    _mode(new Core::IMode(this)),
    _placeHolder(0),
    _inPool(false)
{
    if (spec())
        setObjectName("Form::FormMode::" + spec()->uuid());
    else
        setObjectName("Form::FormMode");

    _placeHolder = new Form::FormPlaceHolder;
    _placeHolder->setObjectName("BaseWidget::Mode::FormPlaceHolder");

    if (spec())
        _mode->setId(spec()->uuid().toUtf8());
    _mode->setPatientBarVisibility(true);
    _mode->setEnabledOnlyWithCurrentPatient(true);
    _mode->setWidget(_placeHolder);
    languageChanged();

    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onPatientFormsLoaded()));
//    qWarning() << "FormPage" << objectName() << spec()->value(Form::FormItemSpec::Spec_Priority).toInt();
}

FormPage::~FormPage()
{
    if (_inPool)
        pluginManager()->removeObject(_mode);
}

void FormPage::onPatientFormsLoaded()
{
    Form::FormTreeModel *model = formManager().formTreeModelForMode(spec()->uuid());
    _mode->setPriority(Core::Constants::P_MODE_PATIENT_FILE + spec()->value(Form::FormItemSpec::Spec_Priority).toInt());
    if (!model) {
        if (_inPool)
            pluginManager()->removeObject(_mode);
        _inPool = false;
    } else {
        if (!_inPool)
            pluginManager()->addObject(_mode);
        _inPool = true;
    }
    _placeHolder->setFormTreeModel(model);
}

void FormPage::languageChanged()
{
//    qWarning() << "FormPage language changed" << uuid() << spec()->value(Form::FormItemSpec::Spec_Priority).toInt();
    _mode->setDisplayName(spec()->label());
    QString icon = spec()->iconFileName();
    icon.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::BigPixmapPath));
    _mode->setIcon(QIcon(icon));
    _mode->setPriority(spec()->value(Form::FormItemSpec::Spec_Priority).toInt());
    // TODO: move the extradata inside the spec to have a fully translated extra-values
}

void FormPage::specLoaded()
{
    languageChanged();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  FormMain  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormMain::FormMain(QObject *parent) :
    FormItem(parent),
    m_Reader(0),
    m_DebugPage(0),
    m_Episode(MultiEpisode),
    m_UseNameAsNSForSubItems(false)
{
}

FormMain::~FormMain()
{
// TODO: this is buggy
//    if (m_DebugPage)
//        ExtensionSystem::PluginManager::instance()->removeObject(m_DebugPage);
}

/** Defines the Form::IFormIO reader of the Form::FormMain empty root parent. */
void FormMain::setIoFormReader(IFormIO *reader)
{
    if (rootFormParent()==this) {
        m_Reader=reader;
    } else {
        rootFormParent()->setIoFormReader(reader);
    }
}

/** Returns the Form::IFormIO reader of the Form::FormMain empty root parent. */
IFormIO *FormMain::reader() const
{
    if (rootFormParent()==(FormMain*)this) {
        return m_Reader;
    }
    return rootFormParent()->reader();
}

/**
 * \fn bool Form::FormMain::isEmptyRootForm() const
 * Return true is the Form::FormMain is an empty root form.
 * This kind of form is the only one who owns a link
 * to the Form::IFormIO reader. \sa Form::FormMain::reader().
 */


/** \brief Create and return a new FormMain as children of this FormMain. */
FormMain *FormMain::createChildForm(const QString &uuid)
{
    FormMain *f = new FormMain(this);
    if (!uuid.isEmpty())
        f->setUuid(uuid);
    if (WarnFormCreation)
        LOG("FormManager Creating Form: " + uuid + " has child of " + this->uuid());
    return f;
}

/** \brief Currently unused */
FormPage *FormMain::createPage(const QString &uuid)
{
    FormPage *p = new FormPage(this);
    if (!uuid.isEmpty())
        p->setUuid(uuid);
    return p;
}

void FormMain::languageChanged()
{
    qWarning() << "FormMain language changed" << uuid();
}

/** Clear the form and all its items. This should set the items to their default value. */
void FormMain::clear()
{
    if (itemData())
        itemData()->clear();
    foreach(FormItem *it, this->flattenedFormItemChildren()) {
        if (it->itemData())
            it->itemData()->clear();
    }
}

/** Returns the printable html for the form using all its items. */
QString FormMain::printableHtml(bool withValues) const
{
    // TODO: code here: print a form with/wo item content
    FormMain *f = (FormMain*)this;
    if (f->formWidget())
        return f->formWidget()->printableHtml(withValues);
    else
        LOG_ERROR("No formWidget in form: " + uuid());
    return QString();
}

/** For debugging purpose. */
void FormMain::createDebugPage()
{
    m_DebugPage = new FormMainDebugPage(this, this);
    ExtensionSystem::PluginManager::instance()->addObject(m_DebugPage);
}

static inline void itemToTree(FormItem *item, QTreeWidgetItem *tree)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(tree, QStringList() << item->spec()->pluginName() << item->spec()->label());
    QFont bold;
    bold.setBold(true);
    i->setFont(0,bold);
    item->valueReferences()->toTreeWidget(i);
    item->scripts()->toTreeWidget(i);
    foreach(FormItem *c, item->formItemChildren()) {
        itemToTree(c, i);
    }
}

/** For debugging purpose. */
void FormMain::toTreeWidget(QTreeWidget *tree)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(tree, QStringList() << tr("Form: ") + spec()->label());
    QFont bold;
    bold.setBold(true);
    i->setFont(0,bold);
    spec()->toTreeWidget(i);
    valueReferences()->toTreeWidget(i);
    scripts()->toTreeWidget(i);
    foreach(FormItem *c, formItemChildren()) {
        itemToTree(c, i);
    }
}

/** Emit the signal Form::FormMain::formLoaded(). */
void FormMain::emitFormLoaded()
{
    Q_EMIT formLoaded();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  FormItemSpec  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpecsBook {
public:
    void toTreeWidgetItem(QTreeWidgetItem *i) const
    {
        new QTreeWidgetItem(i, QStringList() << "Authors" << m_Specs.value(Form::FormItemSpec::Spec_Author).toString() );
        new QTreeWidgetItem(i, QStringList() << "License" << m_Specs.value(Form::FormItemSpec::Spec_License).toString() );
        new QTreeWidgetItem(i, QStringList() << "version" << m_Specs.value(Form::FormItemSpec::Spec_Version).toString() );
        new QTreeWidgetItem(i, QStringList() << "References" << m_Specs.value(Form::FormItemSpec::Spec_Bibliography).toString() );
        new QTreeWidgetItem(i, QStringList() << "Description" << m_Specs.value(Form::FormItemSpec::Spec_Description).toString() );
        new QTreeWidgetItem(i, QStringList() << "Category" << m_Specs.value(Form::FormItemSpec::Spec_Category).toString() );
        new QTreeWidgetItem(i, QStringList() << "Creation date" << m_Specs.value(Form::FormItemSpec::Spec_CreationDate).toString() );
        new QTreeWidgetItem(i, QStringList() << "Last modification" << m_Specs.value(Form::FormItemSpec::Spec_LastModified).toString() );
        new QTreeWidgetItem(i, QStringList() << "Plugin Name" << m_Specs.value(Form::FormItemSpec::Spec_Plugin).toString() );
        new QTreeWidgetItem(i, QStringList() << "Icon filename" << m_Specs.value(Form::FormItemSpec::Spec_IconFileName).toString() );
    }

    QHash<int, QVariant> m_Specs;
};

namespace Form {
namespace Internal {
class FormItemSpecPrivate : public Trans::MultiLingualClass<SpecsBook>
{
public:
    FormItemSpecPrivate() {}
    ~FormItemSpecPrivate() {}

    QString categoryForTreeWiget() const {return QString("Specs");}

    QString m_Uuid;
    QStringList m_EquivalentUuid;

};

} // End Internal
} // End Form

/**
 * Creates an empty valid object. Some default values are defined:
 * - Spec_IsIdentityForm : false
 * - Spec_UseForHprimImportation : false
 */
FormItemSpec::FormItemSpec() :
    d(new Form::Internal::FormItemSpecPrivate)
{
    setValue(Spec_IsIdentityForm, false);
    setValue(Spec_UseForHprimImportation, false);
}

FormItemSpec::~FormItemSpec()
{
    if (d) delete d;
    d=0;
}

/**
  \brief Defines a value \e val for one \e type at one specified \e id in a list, for one \e language.
  \e language must be a valid local language name in 2 chars encoding.
*/
void FormItemSpec::setValue(int type, const QVariant &val, const QString &language)
{
//    qWarning() << "SPEC" << type << val << language;
    if (type == Spec_Uuid) {
        d->m_Uuid = val.toString();
        return;
    }
    QString l = language;
    if (language.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    SpecsBook *values = d->createLanguage(l);
    values->m_Specs.insert(type,val);
}

QVariant FormItemSpec::value(const int type, const QString &lang) const
{
    if (type == Spec_Uuid) {
        return d->m_Uuid;
    }
    QString l = lang;
    if (lang.isEmpty())
        l = QLocale().name().left(2);
    SpecsBook *values = d->getLanguage(l);
    if (!values)
        return QString();
    QVariant val = values->m_Specs.value(type);
    if (val.isNull() && (l.compare(Trans::Constants::ALL_LANGUAGE)!=0)) {
        val = value(type, Trans::Constants::ALL_LANGUAGE);
    }
    return val;
}

/** Defines the FreeMedForms persistent in time UUID of the item. */
void FormItemSpec::setUuid(const QString &uuid)
{
    d->m_Uuid = uuid;
}

/** Returns the FreeMedForms persistent in time UUID of the item. This UUID is used for database accesses. */
QString FormItemSpec::uuid() const
{
    return d->m_Uuid;
}

/** Defines the FreeMedForms equivalence in persistent in time UUID for the item. Equivalence UUID is used to keep data correspondance when a form was updated and uuid of item changed. */
void FormItemSpec::setEquivalentUuid(const QStringList &list)
{
    d->m_EquivalentUuid = list;
    d->m_EquivalentUuid.removeDuplicates();
    d->m_EquivalentUuid.removeAll("");
}

/** Returns the FreeMedForms equivalence in persistent in time UUID for the item. \sa FormItemIdentifier::setEquivalentUuid */
QStringList FormItemSpec::equivalentUuid() const
{
    return d->m_EquivalentUuid;
}

void FormItemSpec::toTreeWidget(QTreeWidgetItem *tree) const
{
    d->toTreeWidget(tree);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  FormMainDebugPage  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormMainDebugPage::FormMainDebugPage(FormMain *form, QObject *parent) :
    IDebugPage(parent), m_Form(form)
{
    setObjectName("FormMainDebugPage_" + m_Form->uuid());
    m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    tree->setColumnCount(2);
    layout->addWidget(tree);
}

FormMainDebugPage::~FormMainDebugPage()
{
    if (m_Widget)
        delete m_Widget;
}

QString FormMainDebugPage::displayName() const
{ return objectName(); }

QString FormMainDebugPage::category() const
{ return tr("Forms: ") + m_Form->spec()->label(); }

int FormMainDebugPage::sortIndex() const
{ return 20; }

void FormMainDebugPage::refreshContents()
{
    tree->clear();
    m_Form->toTreeWidget(tree);
    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}

QWidget *FormMainDebugPage::createPage(QWidget *parent)
{
    Q_UNUSED(parent);
    return m_Widget;
}

//QDebug operator<<(QDebug dbg, const Form::FormItem &c)
//{
//    dbg.nospace() << "Form::FormItem("
//                  << &c
//                  << "; Uuid:" << c.uuid()
//                  << ")";
//    return dbg.space();
//}

//QDebug operator<<(QDebug dbg, Form::FormItem *c)
//{
//    if (!c) {
//        dbg.nospace() << "Form::FormItem(0x0)";
//        return dbg.space();
//    }
//    return operator<<(dbg, *c);
//}

//QDebug operator<<(QDebug dbg, const Form::FormMain &c)
//{
//    dbg.nospace() << "Form::FormMain("
//                  << &c
//                  << "; Uuid:" << c.uuid()
//                  << ")";
//    return dbg.space();
//}

//QDebug operator<<(QDebug dbg, Form::FormMain *c)
//{
//    if (!c) {
//        dbg.nospace() << "Form::FormMain(0x0)";
//        return dbg.space();
//    }
//    return operator<<(dbg, *c);
//}
