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
#include "xmlformcontentreader.h"
#include "constants.h"
#include "xmliobase.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/genericupdateinformation.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformio.h>
#include <formmanagerplugin/formmanager.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorycore.h>

#include <QApplication>
#include <QDir>
#include <QTextCodec>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
#include <QScriptEngine>
#include <QScriptSyntaxCheckResult>

#include <QDebug>
enum {WarnTabOrder = false};

using namespace XmlForms;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline Internal::XmlIOBase *base() {return Internal::XmlIOBase::instance();}

static QString getNamespace(const Form::FormItem *item)
{
    QStringList ns;
    // Add all Form::FormMain Namespace
    Form::FormMain *form = item->parentFormMain();
    while (form) {
        if (form->useNameAsNSForSubItems())
            ns.prepend(form->uuid());
        form = form->parentFormMain();
    }
    if (ns.isEmpty())
        return QString::null;
    return ns.join("::") + "::";
}

XmlFormContentReader *XmlFormContentReader::m_Instance = 0;
XmlFormContentReader *XmlFormContentReader::instance()
{
    if (!m_Instance)
        m_Instance = new XmlFormContentReader;
    return m_Instance;
}

XmlFormContentReader::XmlFormContentReader() :
    m_Mute(false),
    m_Initialized(false)
{
}

bool XmlFormContentReader::initialize()
{
    if (m_Initialized)
        return true;
   m_ScriptsTypes.clear();
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONLOAD, Form::FormItemScripts::Script_OnLoad);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_POSTLOAD, Form::FormItemScripts::Script_PostLoad);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONDEMAND, Form::FormItemScripts::Script_OnDemand);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONVALUECHANGED, Form::FormItemScripts::Script_OnValueChanged);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONVALUEREQUIRED, Form::FormItemScripts::Script_OnValueRequired);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONDEPENDENCIESCHANGED, Form::FormItemScripts::Script_OnDependentValueChanged);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONCLICKED, Form::FormItemScripts::Script_OnClicked);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONTOGGLED, Form::FormItemScripts::Script_OnToggled);

   m_ValuesTypes.clear();
   m_ValuesTypes.insert(Constants::TAG_VALUE_UUID, Form::FormItemValues::Value_Uuid);
   m_ValuesTypes.insert(Constants::TAG_VALUE_NUMERICAL, Form::FormItemValues::Value_Numerical);
   m_ValuesTypes.insert(Constants::TAG_VALUE_SCRIPT, Form::FormItemValues::Value_Script);
   m_ValuesTypes.insert(Constants::TAG_VALUE_POSSIBLE, Form::FormItemValues::Value_Possible);
   m_ValuesTypes.insert(Constants::TAG_VALUE_DEPENDENCIES, Form::FormItemValues::Value_Dependency);
   m_ValuesTypes.insert(Constants::TAG_VALUE_PRINT, Form::FormItemValues::Value_Printing);
   m_ValuesTypes.insert(Constants::TAG_VALUE_DEFAULT, Form::FormItemValues::Value_Default);

   m_SpecsTypes.clear();
   m_SpecsTypes.insert(Constants::TAG_SPEC_PLUGINNAME, Form::FormItemSpec::Spec_Plugin);
   m_SpecsTypes.insert(Constants::TAG_SPEC_AUTHORS, Form::FormItemSpec::Spec_Author);
   m_SpecsTypes.insert(Constants::TAG_SPEC_CATEGORY, Form::FormItemSpec::Spec_Category);
   m_SpecsTypes.insert(Constants::TAG_SPEC_LICENSE, Form::FormItemSpec::Spec_License);
   m_SpecsTypes.insert(Constants::TAG_SPEC_CREATIONDATE, Form::FormItemSpec::Spec_CreationDate);
   m_SpecsTypes.insert(Constants::TAG_SPEC_BIBLIOGRAPHY, Form::FormItemSpec::Spec_Bibliography);
   m_SpecsTypes.insert(Constants::TAG_SPEC_DESCRIPTION, Form::FormItemSpec::Spec_Description);
   m_SpecsTypes.insert(Constants::TAG_SPEC_LABEL, Form::FormItemSpec::Spec_Label);
   m_SpecsTypes.insert(Constants::TAG_SPEC_EXTRALABEL, Form::FormItemSpec::Spec_ExtraLabel);
   m_SpecsTypes.insert(Constants::TAG_SPEC_VERSION, Form::FormItemSpec::Spec_Version);
   m_SpecsTypes.insert(Constants::TAG_SPEC_ICON, Form::FormItemSpec::Spec_IconFileName);
   m_SpecsTypes.insert(Constants::TAG_SPEC_TOOLTIP, Form::FormItemSpec::Spec_Tooltip);
   m_SpecsTypes.insert(Constants::TAG_SPEC_PLACEHOLDER, Form::FormItemSpec::Spec_PlaceHolder);
   m_SpecsTypes.insert(Constants::TAG_SPEC_PRIORITY, Form::FormItemSpec::Spec_Priority);
   m_SpecsTypes.insert(Constants::TAG_SPEC_HPRIM, Form::FormItemSpec::Spec_UseForHprimImportation);

   m_PatientData.clear();
   // use the Core::IPatient::enumToString()
   for(int i=0; i< Core::IPatient::NumberOfColumns; ++i) {
       m_PatientData.insert(patient()->enumToString(Core::IPatient::PatientDataRepresentation(i)).toLower(), i);
   }
   m_Initialized = true;
   return true;
}

XmlFormContentReader::~XmlFormContentReader()
{}

/** Clear internal cache */
void XmlFormContentReader::clearCache()
{
    m_DomDocFormCache.clear();
    m_ActualForm = 0;
}

/** Refresh the IFormWidgetFactories from the plugin manager */
void XmlFormContentReader::refreshPluginFactories()
{
    m_PlugsFactories.clear();
    foreach(Form::IFormWidgetFactory *fact, pluginManager()->getObjects<Form::IFormWidgetFactory>()) {
        foreach(const QString &widgetname, fact->providedWidgets()) {
           m_PlugsFactories.insert(widgetname,fact);
        }
    }
}

/** Return \e true is \e formAbsPath is already tested and present in cache */
bool XmlFormContentReader::isInCache(const QString &formAbsPath) const
{
    return m_DomDocFormCache.contains(formAbsPath);
}

/**
 * Return the cached readable QDomDocument from the cache system
 * or 0 if the \e formAbsPath is not present in cache.
 * \sa XmlForms::Internal::XmlFormName
 */
QDomDocument *XmlFormContentReader::fromCache(const QString &formAbsPath) const
{
    if (m_DomDocFormCache.contains(formAbsPath))
        return m_DomDocFormCache[formAbsPath];
    return 0;
}

/** Warn XML reading errors */
void XmlFormContentReader::warnXmlReadError(bool muteUserWarnings, const QString &file, const QString &msg, const int line, const int col) const
{
    QString m = Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file) + " ; " +
                  Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
                  .arg(msg).arg(line).arg(col);
    LOG_ERROR_FOR("XmlFormContentReader", m);
    m_Error.append(Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
                   .arg(msg).arg(line).arg(col));

    if (!muteUserWarnings)
        Utils::warningMessageBox(
            Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file),
            Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
            .arg(msg).arg(line).arg(col),"",qApp->applicationName());
}

/** Check if the xmlContent is conforming to the XML Form IO. formUid must be an absolute path if corresponding to a file. */
bool XmlFormContentReader::checkFileContent(const QString &formUidOrFullAbsPath, const QString &contents) const
{
//    qWarning() << "CHECK" << formUidOrFullAbsPath;
    bool ok = true;
    if (contents.isEmpty()) {
        if (!m_Mute)
            Utils::warningMessageBox(tkTr(Trans::Constants::FILE_1_ISEMPTY).arg(formUidOrFullAbsPath),"",qApp->applicationName());
        m_Error.append(tkTr(Trans::Constants::FILE_1_ISEMPTY).arg(formUidOrFullAbsPath));
        return false;
    }
    // Check XML files
    if (formUidOrFullAbsPath.endsWith(".xml", Qt::CaseInsensitive)) {
        if (contents.count("<"+QString(Constants::TAG_NEW_FORM)) != contents.count("</"+QString(Constants::TAG_NEW_FORM)+">")) {
            ok = false;
            m_Error.append(tkTr(Trans::Constants::XML_WRONG_NUMBER_OF_TAG_1).arg(Constants::TAG_NEW_FORM));
            LOG_ERROR_FOR("XmlFormContentReader", Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(formUidOrFullAbsPath));
        }
        if ((contents.count(QString("<%1>").arg(Constants::TAG_MAINXMLTAG)) != 1) ||
                (contents.count(QString("</%1>").arg(Constants::TAG_MAINXMLTAG)) != 1)) {
            m_Error.append(tkTr(Trans::Constants::XML_WRONG_NUMBER_OF_TAG_1).arg(Constants::TAG_NEW_FORM));
            ok = false;
        }

        // load document
        QString errorMsg;
        int errorLine, errorColumn;
        QDomDocument *doc = new QDomDocument;
        if (!doc->setContent(contents, &errorMsg, &errorLine, &errorColumn)) {
            warnXmlReadError(m_Mute, formUidOrFullAbsPath, errorMsg, errorLine, errorColumn);
            ok = false;
        }

        // Check doctype name
        if (doc->doctype().name().compare(Constants::DOCTYPE_NAME,Qt::CaseInsensitive)!=0) {
            const QString &error = tkTr(Trans::Constants::XML_DOCUMENT_TYPE_MISMATCH);
            m_Error << error;
            warnXmlReadError(m_Mute, formUidOrFullAbsPath, error);
            ok = false;
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if (ok)
            m_DomDocFormCache.insert(formUidOrFullAbsPath, doc);
    }
    // Check JS files
    else if (formUidOrFullAbsPath.endsWith(".js", Qt::CaseInsensitive)) {
        QScriptEngine engine;
        QScriptSyntaxCheckResult result = engine.checkSyntax(contents);
        if (result.state()==QScriptSyntaxCheckResult::Error) {
            QString msg = QString("Script error: (%1:%2) %3 -- in file %4")
                    .arg(result.errorLineNumber())
                    .arg(result.errorColumnNumber())
                    .arg(result.errorMessage())
                    .arg(formUidOrFullAbsPath);
            if (!m_Mute)
                Utils::warningMessageBox(msg, "", qApp->applicationName());
            m_Error.append(msg);
            ok = false;
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    return ok;
}

/**
 * When updating forms, users can add field equivalences from version to version.
 */
QMultiHash<QString, QString> XmlFormContentReader::readUuidEquivalence(const QDomDocument *doc) const
{
    QMultiHash<QString, QString> oldToNew;
    QDomElement item = doc->firstChildElement(Constants::TAG_MAINXMLTAG);
    item = item.firstChildElement(Constants::TAG_UUID_EQUIVALENCE_MAIN);
    item = item.firstChildElement(Constants::TAG_UUID_EQUIVALENCE_ITEM);
    while (!item.isNull()) {
        if (item.attribute(Constants::ATTRIB_OLD_UUID).isEmpty() || item.attribute(Constants::ATTRIB_NEW_UUID).isEmpty()) {
            LOG_ERROR_FOR("XmlFormContentReader", "Field equivalence missing old/new attributes");
            item = item.nextSiblingElement(Constants::TAG_UUID_EQUIVALENCE_ITEM);
            continue;
        }
        oldToNew.insertMulti(item.attribute(Constants::ATTRIB_OLD_UUID), item.attribute(Constants::ATTRIB_NEW_UUID));
        item = item.nextSiblingElement(Constants::TAG_UUID_EQUIVALENCE_ITEM);
    }
    return oldToNew;
}

/** Return the Form::FormIODescription according to the XML QDomElement \e xmlDescr. The \e xmlDescr must point to the first description tag of the document. */
Form::FormIODescription *XmlFormContentReader::readXmlDescription(const QDomElement &xmlDescr, const XmlFormName &form)
{
    Form::FormIODescription *ioDesc = new Form::FormIODescription;
    ioDesc->setRootTag(Constants::TAG_FORM_DESCRIPTION);
    ioDesc->fromDomElement(xmlDescr);
    ioDesc->setData(Form::FormIODescription::UuidOrAbsPath, form.uid);
    return ioDesc;
}

/** Return the Form description. Call this member just after the checkFormFileContent() so that the form xml content will be cached */
Form::FormIODescription *XmlFormContentReader::readFileInformation(const XmlFormName &form, const Form::FormIOQuery &query)
{
    Form::FormIODescription *toReturn = 0;
    // Get the QDomDocument from cache
    QDomDocument *doc = m_DomDocFormCache[form.absFileName];
    if (!doc) {
        LOG_ERROR_FOR("XmlFormContentReader", "No document in cache call canReadForm before. Form: " + form.uid);
        return toReturn;
    }

    // get from cache and read description
    QDomElement root = doc->documentElement();
    root = root.firstChildElement(Constants::TAG_FORM_DESCRIPTION);
    toReturn = readXmlDescription(root, form);

    // has screenshots?
    if (query.forceFileReading()) {
        // Get from local files
        QString shotPath = QString("%1/shots/").arg(form.absPath);
        QStringList lang;
        lang << QLocale().name().left(2).toLower() << "en" << "xx" << "all";
        bool found = false;
        foreach(const QString &l, lang) {
            if (QDir(shotPath + l).exists()) {
                found = true;
                shotPath = shotPath + l;
                break;
            }
        }
        if (found) {
            QDir dir(shotPath);
            qWarning() << "Trying to read shots" << dir.absolutePath();
            toReturn->setData(Form::FormIODescription::HasScreenShot, true);
            // foreach(const QFileInfo &file, dir.entryInfoList(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.gif")) {
            //     QPixmap pix(file.absoluteFilePath());
            //     toReturn->addScreenShot(file.absoluteFilePath().remove(shotPath), pix);
            // }
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    } else {
        // Get from database
        // qWarning() << "Trying to read shots from database";
        toReturn->setData(Form::FormIODescription::HasScreenShot, base()->hasScreenShots(form.uid));
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    return toReturn;
}

QList<Form::FormIODescription *> XmlFormContentReader::getFormFileDescriptions(const Form::FormIOQuery &query)
{
    Q_UNUSED(query);
    QList<Form::FormIODescription *> toReturn;

    // TODO: code here

//    QString startPath;
//    if (query.typeOfForms() & Form::FormIOQuery::UserForms) {
//        // TODO: manage user forms path and default path
//    } else {
//        // TODO: manage user forms path and default path
//    }
//    if (query.typeOfForms() & Form::FormIOQuery::CompleteForms) {
//        startPath = settings()->path(Core::ISettings::CompleteFormsPath);
//        getAllFormsFromDir(startPath, &toReturn);
//        for(int i = 0; i < toReturn.count(); ++i) {
//            toReturn.at(i)->setData(Form::FormIODescription::IsCompleteForm, true);
//        }
//    }
//    if (query.typeOfForms() & Form::FormIOQuery::SubForms) {
//        startPath = settings()->path(Core::ISettings::SubFormsPath);
//        getAllFormsFromDir(startPath, &toReturn);
//        for(int i = 0; i < toReturn.count(); ++i) {
//            toReturn.at(i)->setData(Form::FormIODescription::IsSubForm, true);
//        }
//    }
//    // TODO: Add IFormIO to descr && check all forms for params of Query
//    for(int i = 0; i < toReturn.count(); ++i) {
//        toReturn.at(i)->setIoFormReader(this);
//    }
    return toReturn;
}

bool XmlFormContentReader::loadForm(const XmlFormName &form, Form::FormMain *rootForm)
{
//    qWarning() << Q_FUNC_INFO << form.uid << form.absFileName << m_ActualForm;

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QDomDocument *doc = 0;
    if (!m_DomDocFormCache.keys().contains(form.absFileName)) {
        LOG_ERROR_FOR("XmlFormContentReader","Form not in cache: " + form.absFileName);
        return false;
    }
    doc = m_DomDocFormCache[form.absFileName];
    QDomElement root = doc->firstChildElement(Constants::TAG_MAINXMLTAG);
    QDomElement newForm = root.firstChildElement(Constants::TAG_NEW_FORM);
    QDomElement addFile = root.firstChildElement(Constants::TAG_ADDFILE);

    // in case of no rootForm is passed --> XML must start with a file inclusion or a newform tag
    if (!rootForm) {
        if (addFile.isNull() && newForm.isNull()) {
            warnXmlReadError(m_Mute, form.uid, tkTr(Trans::Constants::XML_WRONG_ROOT_TAG_1_2).arg(root.tagName()).arg(Constants::TAG_NEW_FORM));
            return false;
        }
    }
    m_ActualForm = rootForm;

    if (!loadElement(rootForm, root, form)) {
        LOG_ERROR_FOR("XmlFormContentReader", "Unable to load form: " + form.uid);
        return false;
    }

//    rootForm->createDebugPage();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
//    createWidgets(rootForm);
//    if (!setTabOrder(rootForm, ))

    // Manage uuid equivalence
    QMultiHash<QString, QString> oldToNew = readUuidEquivalence(doc);
    if (!oldToNew.isEmpty()) {
        QStringList newUids = oldToNew.values();
        newUids.removeDuplicates();
        foreach(Form::FormMain *main, rootForm->flattenedFormMainChildren()) {
            if (newUids.contains(main->uuid(), Qt::CaseInsensitive)) {
                main->spec()->setEquivalentUuid(oldToNew.keys(main->uuid()));
            }
            foreach(Form::FormItem *item, main->flattenedFormItemChildren()) {
                if (newUids.contains(item->uuid(), Qt::CaseInsensitive)) {
                    item->spec()->setEquivalentUuid(oldToNew.keys(item->uuid()));
                }
            }
        }
    }
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    return true;
}

bool XmlFormContentReader::loadElement(Form::FormItem *item, QDomElement &rootElement, const XmlFormName &form)
{
    bool descriptionPassed = false; // for speed improvements
    QDomElement element = rootElement.firstChildElement();
    while (!element.isNull()) {
        int i=0;

        // Do not proceed form description here
        if (!descriptionPassed) {
            if (element.tagName().compare(::Constants::TAG_FORM_DESCRIPTION, Qt::CaseInsensitive)==0) {
                descriptionPassed = true; // speed improvements
                element = element.nextSiblingElement();
                continue;
            }
        }

        // Create a nem FormItem ?
        i = Constants::createTags.indexOf(element.tagName());
        if (i != -1) {
            createElement(item, element, form);
            element = element.nextSiblingElement();
            continue;
        }

        // if there is no item defined then go nextSibling till we find a new form, item, page...

        if (!item) {
            element = element.nextSiblingElement();
            continue;
        }

        // Values ?
        if (element.tagName().compare(Constants::TAG_VALUE, Qt::CaseInsensitive)==0) {
            populateValues(item, element, form);
            element = element.nextSiblingElement();
            continue;
        }

        // Script ?
        if (element.tagName().compare(Constants::TAG_SCRIPT, Qt::CaseInsensitive)==0) {
            populateScripts(item, element, form);
            element = element.nextSiblingElement();
            continue;
        }

        // Spec ?
        i = m_SpecsTypes.value(element.tagName(), -1);
        if (i != -1) {
            QString lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
            item->spec()->setValue(m_SpecsTypes.value(element.tagName()), element.text(), lang);
            element = element.nextSiblingElement();
            continue;
        }

        // UiLink ?
        if (element.tagName().compare(Constants::TAG_UI_UILINK, Qt::CaseInsensitive)==0) {
            item->spec()->setValue(Form::FormItemSpec::Spec_UiLabel, element.attribute(Constants::ATTRIB_UI_UILINK_LABEL));
            item->spec()->setValue(Form::FormItemSpec::Spec_UiWidget, element.attribute(Constants::ATTRIB_UI_UILINK_WIDGET));
            item->spec()->setValue(Form::FormItemSpec::Spec_UiInsertIntoLayout, element.attribute(Constants::ATTRIB_UI_UILINK_INSERTINTOLAYOUT));
            element = element.nextSiblingElement();
            continue;
        }

        // Name/UUID ?
        if ((element.tagName().compare(Constants::ATTRIB_NAME, Qt::CaseInsensitive)==0) ||
            (element.tagName().compare(Constants::ATTRIB_UUID, Qt::CaseInsensitive)==0)) {
            QString uidNS = getNamespace(item);
            item->setUuid(uidNS + element.text());
            element = element.nextSiblingElement();
            continue;
        }

        // Patient Data Representation ?
        if (element.tagName().compare(Constants::TAG_DATAPATIENT, Qt::CaseInsensitive)==0) {
            i = m_PatientData.value(element.text().toLower(), -1);
            if (i != -1) {
                item->setPatientDataRepresentation(i);
            }
            element = element.nextSiblingElement();
            continue;
        }

    //             // optional?
//            if (element.hasAttribute(Constants::ATTRIB_OPTIONAL)) {
//                if (element.attribute(Constants::ATTRIB_OPTIONAL).compare("true",Qt::CaseInsensitive)==0)
//                    item->valueReferences()->setOptional(true);
//                else
//                    item->valueReferences()->setOptional(false);

        // Add a form file ?
        if (element.tagName().compare(Constants::TAG_ADDFILE, Qt::CaseInsensitive)==0) {
            if (!addFile(element, form, item)) {
                LOG_ERROR_FOR("XmlFormContentReader", QString("Unable to add file: %1; in form: %2")
                              .arg(element.text()).arg(form.absFileName));
            }
            element = element.nextSiblingElement();
            continue;
        }

        // Options
        if (element.tagName().compare(Constants::TAG_OPTIONS, Qt::CaseInsensitive)==0 ||
                element.tagName().compare(Constants::TAG_OPTION, Qt::CaseInsensitive)==0) {
            const QString &options = element.text();
            if (options.contains(Constants::TAG_OPTIONS_UNIQUE_EPISODE, Qt::CaseInsensitive)) {
                if (item==m_ActualForm)
                    m_ActualForm->setEpisodePossibilities(Form::FormMain::UniqueEpisode);
            } else if (options.contains(Constants::TAG_OPTIONS_NO_EPISODE, Qt::CaseInsensitive)) {
                if (item==m_ActualForm)
                    m_ActualForm->setEpisodePossibilities(Form::FormMain::NoEpisode);
            }
            if (options.contains(Constants::OPTION_USEFORMNAMEASNS, Qt::CaseInsensitive)) {
                if (item==m_ActualForm)
                    m_ActualForm->setUseNameAsNSForSubItems(true);
            }
            if (options.contains((Constants::TAG_OPTIONS_NO_HEADER),
                                 Qt::CaseInsensitive)) {
                if (item==m_ActualForm) {
                    m_ActualForm->setFormPrintHeader(Form::FormMain::NoHeader);
                }
            } else if (options.contains((Constants::TAG_OPTIONS_GENERIC_HEADER),
                                        Qt::CaseInsensitive)) {
                if (item==m_ActualForm) {
                    m_ActualForm->setFormPrintHeader(Form::FormMain::GenericHeader);
                }
            } else if (options.contains((Constants::TAG_OPTIONS_ADMINISTRATIVE_HEADER),
                                        Qt::CaseInsensitive)) {
                if (item==m_ActualForm) {
                    m_ActualForm->setFormPrintHeader(Form::FormMain::AdministrativeHeader);
                }
            } else if (options.contains((Constants::TAG_OPTIONS_PRESCRIPTION_HEADER),
                                        Qt::CaseInsensitive)) {
                if (item==m_ActualForm) {
                    m_ActualForm->setFormPrintHeader(Form::FormMain::PrescriptionHeader);
                }
            }
            item->addExtraData(element.tagName(), options);
            element = element.nextSiblingElement();
            continue;
        }

        // All others add them in extraData
        item->addExtraData(element.tagName(), element.text());
        // qWarning() << "°° XML adding other tag" << element.tagName() << element.text() << item->extraData();

        element = element.nextSiblingElement();
    }
    return true;
}

bool XmlFormContentReader::createElement(Form::FormItem *item, QDomElement &element, const XmlFormName &form)
{
//    qWarning() << "XmlFormIO create element" << m_ActualForm->useNameAsNSForSubItems() << m_ActualForm->uuid();
    // new item
    if (element.tagName().compare(Constants::TAG_NEW_ITEM, Qt::CaseInsensitive)==0) {
        if (item) {
            Form::FormItem *child = item->createChildItem();
            QString uidNS = getNamespace(child);
            // read attributes (type, uid/name, patient representation...)
            if (element.hasAttribute(Constants::ATTRIB_UUID))
                child->setUuid(uidNS + element.attribute(Constants::ATTRIB_UUID));

            if (element.hasAttribute(Constants::ATTRIB_NAME))
                child->setUuid(uidNS + element.attribute(Constants::ATTRIB_NAME));

            if (element.hasAttribute(Constants::ATTRIB_TYPE))
                child->spec()->setValue(Form::FormItemSpec::Spec_Plugin, element.attribute(Constants::ATTRIB_TYPE), Trans::Constants::ALL_LANGUAGE);

            if (element.hasAttribute(Constants::ATTRIB_PATIENTREPRESENTATION)) {
                int i = m_PatientData.value(element.attribute(Constants::ATTRIB_PATIENTREPRESENTATION).toLower(), -1);
                if (i != -1) {
                    child->setPatientDataRepresentation(i);
                }
            }

            if (element.hasAttribute(Constants::ATTRIB_UIFILE)) {
                QString content = base()->getFormContent(form.uid, XmlIOBase::UiFile, element.attribute(Constants::ATTRIB_UIFILE));
                child->spec()->setValue(Form::FormItemSpec::Spec_UiFileContent, content, Trans::Constants::ALL_LANGUAGE);
            }

            loadElement(child, element, form);
            return true;
        }
        else
            return false; // XML ERROR --> an item MUST have a parent
    }

    // new form
    if (element.tagName().compare(Constants::TAG_NEW_FORM, Qt::CaseInsensitive)==0) {
        // create a new form
        Form::FormMain *oldRootForm = m_ActualForm;
        m_ActualForm = m_ActualForm->createChildForm(element.firstChildElement(Constants::TAG_NAME).text());
        item = m_ActualForm;
        if (item) {
            QString uidNS = getNamespace(m_ActualForm);
            item->spec()->setValue(Form::FormItemSpec::Spec_Plugin, "form", Trans::Constants::ALL_LANGUAGE);
            // read attributes (type, uid/name, patient representation...)
            if (element.hasAttribute(Constants::ATTRIB_UUID))
                item->setUuid(uidNS + element.attribute(Constants::ATTRIB_UUID));

            if (element.hasAttribute(Constants::ATTRIB_NAME))
                item->setUuid(uidNS + element.attribute(Constants::ATTRIB_NAME));

            if (element.hasAttribute(Constants::ATTRIB_TYPE))
                item->spec()->setValue(Form::FormItemSpec::Spec_Plugin, element.attribute(Constants::ATTRIB_TYPE), Trans::Constants::ALL_LANGUAGE);

            if (element.hasAttribute(Constants::ATTRIB_ISIDENTITYFORM)) {
                const QString &att = element.attribute(Constants::ATTRIB_ISIDENTITYFORM);
                bool isIdentity =
                        (att.compare("yes", Qt::CaseInsensitive) == 0) ||
                        (att.compare("true", Qt::CaseInsensitive) == 0) ||
                        att == "1";
                item->spec()->setValue(Form::FormItemSpec::Spec_IsIdentityForm, isIdentity, Trans::Constants::ALL_LANGUAGE);
            }

            if (element.hasAttribute(Constants::ATTRIB_UIFILE)) {
                QString content = base()->getFormContent(form.uid, XmlIOBase::UiFile, element.attribute(Constants::ATTRIB_UIFILE));
                item->spec()->setValue(Form::FormItemSpec::Spec_UiFileContent, content, Trans::Constants::ALL_LANGUAGE);
            }

            // load all sub-items
            loadElement(item, element, form);

            // when the form is loaded -> create the formWidgets
            createFormWidget(m_ActualForm);

            // then set the taborder
            if (!setTabOrder(m_ActualForm, element)) {
                LOG_ERROR_FOR("XmlFormContentReader", "Unable to set tabsorder: " + form.uid);
            }

            // read specific form's data
            m_ActualForm = oldRootForm;
            return true;
        }
        else
            return false;
    }

    if (element.tagName().compare(Constants::TAG_NEW_PAGE, Qt::CaseInsensitive)==0) {
        // create a new page
        QString uuid = element.attribute(Constants::ATTRIB_UUID);
        if (uuid.isEmpty())
            uuid = element.attribute(Constants::ATTRIB_NAME);
        if (uuid.isEmpty())
            uuid = element.firstChildElement(Constants::TAG_NAME).text();
        if (uuid.isEmpty())
            LOG_ERROR_FOR("XMLIO", "No uuid specified for the FormPage");

        Form::FormPage *page = formManager().createFormPage(uuid);
        if (item) {
            item = page;
            loadElement(item, element, form);
            page->specLoaded();
            return true;
        } else {
            return false;
        }
    }

    return false;
}

/** Read the values of an element. */
bool XmlFormContentReader::populateValues(Form::FormItem *item, const QDomElement &root, const XmlFormName &form)
{
    Q_UNUSED(form);
    QDomElement element = root.firstChildElement();
    while (!element.isNull()) {
        const QString &lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
//        qWarning() << "Values" << root.tagName() << element.tagName() << lang;
        int id = element.attribute(Constants::ATTRIB_ID, 0).toInt();
        const QString &val = element.text();
        int type = m_ValuesTypes.value(element.tagName(), -1);
        if (type==Form::FormItemValues::Value_Printing) {
            // read file
            // TODO: ...
//            if (!element.attribute(Constants::ATTRIB_FILE).isEmpty()) {
//                QString content = readExtraFile(form, element.attribute(Constants::ATTRIB_FILE));
//                item->valueReferences()->setValue(type, id, content, lang);
//            } else {
                item->valueReferences()->setValue(type, id, val, lang);
//            }
        } else if (type != -1) {
            item->valueReferences()->setValue(type, id, val, lang);
        }
        element = element.nextSiblingElement();
    }
    return true;
}

/** Read the scripts of an element. */
bool XmlFormContentReader::populateScripts(Form::FormItem *item, const QDomElement &root, const XmlFormName &form)
{
    Q_UNUSED(form);
    QDomElement element = root.firstChildElement();
    QString lang = root.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE).left(2);
    while (!element.isNull()) {
        QString script = element.text();
        QString file = root.attribute(Constants::ATTRIB_FILE);
        // TODO: HERE
        int type = m_ScriptsTypes.value(element.tagName().toLower(), Form::FormItemScripts::Script_OnDemand);
        item->scripts()->setScript(type, script, lang);
        element = element.nextSiblingElement();
    }
    return true;
}

/** Read a new file. Called when matched XML tag \e file. */
bool XmlFormContentReader::addFile(const QDomElement &element, const XmlFormName &formName, Form::FormItem *item)
{
    XmlFormName form = formName;
    if (!item)
        item = m_ActualForm;
    // Get file content from database
    QString fileName = element.text();
    // Guess the type of file according to its extension
    int basetype = XmlIOBase::ExtraFiles;
    if (fileName.endsWith(".xml", Qt::CaseInsensitive)) {
        basetype = XmlIOBase::FullContent;
        if (fileName.startsWith("./")) {
            form.modeName = fileName;
            form.modeName = form.modeName.mid(2);
            form.modeName.chop(4);
        } else {
            form = XmlFormName(fileName);
            fileName = form.modeName;
        }
    } else if (fileName.endsWith(".js", Qt::CaseInsensitive)) {
        basetype = XmlIOBase::ScriptFile;
    } else if (fileName.endsWith(".ui", Qt::CaseInsensitive)) {
        basetype = XmlIOBase::UiFile;
    } else if (fileName.endsWith(".html", Qt::CaseInsensitive)) {
        basetype = XmlIOBase::HtmlFile;
    } else if (fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        basetype = XmlIOBase::PdfFile;
    }
    //    else if (fileName.endsWith(".qml", Qt::CaseInsensitive)) {
//        basetype = XmlIOBase::QmlContent;
//        }

    QString content = base()->getFormContent(form.uid, basetype, fileName);
    if (content.isEmpty()) {
        LOG_ERROR_FOR("XmlReader", "Content not in base??? " + form.uid + "; mode:" + form.modeName + ";filename:" + fileName);
        Q_ASSERT(false);
    }

    fileName = element.text();
    fileName = fileName.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath), Qt::CaseInsensitive);
    fileName = fileName.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath), Qt::CaseInsensitive);
    fileName = fileName.replace(Core::Constants::TAG_APPLICATION_LOCAL_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::LocalCompleteFormsPath), Qt::CaseInsensitive);
    fileName = fileName.replace(Core::Constants::TAG_APPLICATION_LOCAL_SUBFORMS_PATH, settings()->path(Core::ISettings::LocalSubFormsPath), Qt::CaseInsensitive);
    if (QFileInfo(fileName).isRelative())
        fileName.prepend(QFileInfo(form.absFileName).absolutePath() + QDir::separator());
    fileName = QDir::cleanPath(fileName);
//    QString content = Utils::readTextFile(fileName, Utils::DontWarnUser);

    // Check file content for script addition / printmask types
    const QString &type = element.attribute(Constants::ATTRIB_TYPE);
    if (type.compare(Constants::FILETYPE_SCRIPT, Qt::CaseInsensitive)==0) {
        if (checkFileContent(fileName, content)) {
            // Add script to the empty root FormMain
            Form::FormMain *parent = m_ActualForm;
            while (parent->parentFormMain()) {
                parent = parent->parentFormMain();
            }
            const QString &lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
            const QString &scriptType = element.attribute(Constants::ATTRIB_NAME, Trans::Constants::ALL_LANGUAGE);

//            qWarning()<< "\n\n" << lang << type << scriptType << parent->uuid();

            if (scriptType.compare(Constants::TAG_SCRIPT_ONLOAD, Qt::CaseInsensitive)==0)
                parent->scripts()->setScript(Form::FormItemScripts::Script_OnLoad, content, lang);
            else if (scriptType.compare(Constants::TAG_SCRIPT_POSTLOAD, Qt::CaseInsensitive)==0)
                parent->scripts()->setScript(Form::FormItemScripts::Script_PostLoad, content, lang);
            return true;
        }
    } else if (type.compare(Constants::FILETYPE_PRINTHTMLMASK, Qt::CaseInsensitive)==0) {
        const QString &lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
        if (item)
            item->spec()->setValue(Form::FormItemSpec::Spec_HtmlPrintMask, content, lang);
        return true;
    } else if (type.compare(Constants::FILETYPE_PRINTPLAINTEXTMASK, Qt::CaseInsensitive)==0) {
        const QString &lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
        if (item)
            item->spec()->setValue(Form::FormItemSpec::Spec_PlainTextPrintMask, content, lang);
        return true;
    } else if (type.compare(Constants::FILETYPE_EXPORTHTMLMASK, Qt::CaseInsensitive)==0) {
        const QString &lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
        if (item)
            item->spec()->setValue(Form::FormItemSpec::Spec_HtmlExportMask, content, lang);
        return true;
    } else if (type.compare(Constants::FILETYPE_EXPORTPLAINTEXTMASK, Qt::CaseInsensitive)==0) {
        const QString &lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
        if (item)
            item->spec()->setValue(Form::FormItemSpec::Spec_PlainTextExportMask, content, lang);
        return true;
    }

    // Check file content (for forms file)
    if (checkFileContent(fileName, content)) {
        if (!loadForm(fileName, m_ActualForm)) {
            LOG_ERROR_FOR("XmlReader", "Unable to add form file " + element.text());
            return false;
        } else {
            // TODO: code here
//            saveFormToDatabase(fileName, XmlIOBase::FullContent, content);
            return true;

        }
    }
    return true;
}

/** Create a formitemwidget for the \e item with the specified \e parent */
bool XmlFormContentReader::createItemWidget(Form::FormItem *item, QWidget *parent)
{
    if (!item)
        return false;

//    qWarning() << Q_FUNC_INFO;
//    qWarning() << item << item->uuid() << item->spec()->pluginName();
    QString requestedWidgetName = item->spec()->pluginName().toLower();
    Form::IFormWidgetFactory *factory = m_PlugsFactories.value(requestedWidgetName);
    Form::IFormWidget *w = 0;

    // does plugin was inform in the xml file ?
    if (requestedWidgetName.isEmpty()) {
        LOG_ERROR_FOR("XmlFormContentReader", "No plugin name for item: " + item->uuid());
        factory = m_PlugsFactories.value("helptext");
        w = factory->createWidget("helptext", item);
        item->spec()->setValue(Form::FormItemSpec::Spec_Label, QString("XML FORM ERROR: Requested widget does not exist for item '%1'").arg(item->uuid()));
        return false;
    }

    // does requested widget exist in the plugins ?
    if (!factory) {
        LOG_ERROR_FOR("XmlFormContentReader", QString("Form error in item '%1': Requested widget '%2' does not exist in plugin's widgets list.").arg(item->uuid()).arg(requestedWidgetName));
        // Add a default widget for the error log
        factory = m_PlugsFactories.value("helptext");
        item->spec()->setValue(Form::FormItemSpec::Spec_Label, QString("XML FORM ERROR: Requested widget does not exist for item '%1'").arg(item->uuid()));
        w = factory->createWidget("helptext", item);
        return false;
    }

    // get the widget
    w = factory->createWidget(requestedWidgetName, item, parent);
    if (w->isContainer()) {
        foreach(Form::FormItem *child, item->formItemChildren()) {
//            Form::IFormWidget *wchild = factory->createWidget(child->spec()->pluginName(),child,w);
//            w->addWidgetToContainer(wchild);
//            child->setFormWidget(wchild);
            createItemWidget(child, w);
        }
    }

    // FIXME: With QtUi forms we should not do this
    Form::IFormWidget *p = qobject_cast<Form::IFormWidget*>(parent);
    if (p)
        p->addWidgetToContainer(w);
    return true;
}

/** Create the formitemwidget for a main form (and also its children) */
bool XmlFormContentReader::createFormWidget(Form::FormMain *form)
{
    if (!createItemWidget(form, 0))
        return false;
    return true;
}

/** Create all the formitemwidgets for a main form */
bool XmlFormContentReader::createWidgets(const Form::FormMain *rootForm)
{
    // foreach FormMain children
    foreach(Form::FormMain *form, rootForm->flattenedFormMainChildren()) {
        // create the form
        createFormWidget(form);
    }
    return true;
}

bool XmlFormContentReader::setTabOrder(Form::FormMain *rootForm, const QDomElement &root)
{
    QDomElement tabs = root.firstChildElement(Constants::TAG_TABSTOPS);
    if (tabs.isNull())
        return true;
    if (WarnTabOrder)
        qWarning() << "SETABS" << rootForm->uuid();
//    QString ns = root.attribute("ns");
//    if (!ns.isEmpty())
//        ns.append("::");
    const QList<Form::FormItem *> &items = rootForm->flattenedFormItemChildren();
    Form::IFormWidget *first = 0;
    Form::IFormWidget *second = 0;
    QDomElement element = tabs.firstChildElement(Constants::TAG_TABSTOP);
    QString warn;
    while (!element.isNull()) {
        const QString &widgetName = element.text();
        foreach(Form::FormItem *item, items) {
            if (item->uuid().endsWith(widgetName)) {
                // get it
                if (!first) {
                    if (WarnTabOrder)
                        qWarning() << "---------------------------- first" << item->uuid();
                    first = item->formWidget();
                    first->setTabOrder(WarnTabOrder);
                    warn = "    first: " + item->uuid() + "\n";
                } else {
                    warn += "   second: " + item->uuid() + "\n";
                    second = item->formWidget();
                    if (WarnTabOrder) {
//                        qWarning() << QString("  setTabOrder\n" + warn);
                        qWarning() << "    " << first->lastTabWidget() << "\n    " << second->focusedWidget() << "\n\n";
                    }
                    QWidget::setTabOrder(first->lastTabWidget(), second->focusedWidget());
                    first = second;
                    first->setTabOrder(WarnTabOrder);
                    warn = "    first: " + item->uuid() + "\n";
                    second = 0;
                }
                break;
            }
        }
        element = element.nextSiblingElement(Constants::TAG_TABSTOP);
    }
    return true;
}
