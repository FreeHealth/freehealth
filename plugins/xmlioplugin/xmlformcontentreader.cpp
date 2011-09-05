/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "xmlformcontentreader.h"
#include "constants.h"
#include "xmliobase.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

//#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformio.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorycore.h>

#include <pmhplugin/pmhcore.h>
#include <pmhplugin/pmhcategorymodel.h>

#include <translationutils/constanttranslations.h>

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCodec>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>

using namespace XmlForms;
using namespace Internal;
using namespace Trans::ConstantTranslations;

//inline static Form::FormManager *formManager() { return Form::FormManager::instance(); }
inline static ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Category::CategoryCore *categoryCore() {return  Category::CategoryCore::instance();}
static inline PMH::PmhCore *pmhCore() {return PMH::PmhCore::instance();}
static inline Internal::XmlFormContentReader *reader() {return Internal::XmlFormContentReader::instance();}
static inline Internal::XmlIOBase *base() {return Internal::XmlIOBase::instance();}


XmlFormContentReader *XmlFormContentReader::m_Instance = 0;
XmlFormContentReader *XmlFormContentReader::instance()
{
    if (!m_Instance)
        m_Instance = new XmlFormContentReader;
    return m_Instance;
}

XmlFormContentReader::XmlFormContentReader()
{
   m_ScriptsTypes.clear();
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONLOAD, Form::FormItemScripts::Script_OnLoad);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_POSTLOAD, Form::FormItemScripts::Script_PostLoad);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONDEMAND, Form::FormItemScripts::Script_OnDemand);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONVALUECHANGED, Form::FormItemScripts::Script_OnValueChanged);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONVALUEREQUIERED, Form::FormItemScripts::Script_OnValueRequiered);
   m_ScriptsTypes.insert(Constants::TAG_SCRIPT_ONDEPENDENCIESCHANGED, Form::FormItemScripts::Script_OnDependentValueChanged);

   m_ValuesTypes.clear();
   m_ValuesTypes.insert(Constants::TAG_VALUE_UUID, Form::FormItemValues::Value_Uuid);
   m_ValuesTypes.insert(Constants::TAG_VALUE_NUMERICAL, Form::FormItemValues::Value_Numerical);
   m_ValuesTypes.insert(Constants::TAG_VALUE_SCRIPT, Form::FormItemValues::Value_Script);
   m_ValuesTypes.insert(Constants::TAG_VALUE_POSSIBLE, Form::FormItemValues::Value_Possible);
   m_ValuesTypes.insert(Constants::TAG_VALUE_DEPENDENCIES, Form::FormItemValues::Value_Dependency);
   m_ValuesTypes.insert(Constants::TAG_VALUE_PRINT, Form::FormItemValues::Value_Printing);

   m_SpecsTypes.clear();
   m_SpecsTypes.insert(Constants::TAG_SPEC_PLUGINNAME, Form::FormItemSpec::Spec_Plugin);
   m_SpecsTypes.insert(Constants::TAG_SPEC_AUTHORS, Form::FormItemSpec::Spec_Author);
   m_SpecsTypes.insert(Constants::TAG_SPEC_CATEGORY, Form::FormItemSpec::Spec_Category);
   m_SpecsTypes.insert(Constants::TAG_SPEC_LICENSE, Form::FormItemSpec::Spec_License);
   m_SpecsTypes.insert(Constants::TAG_SPEC_CREATIONDATE, Form::FormItemSpec::Spec_CreationDate);
   m_SpecsTypes.insert(Constants::TAG_SPEC_BIBLIOGRAPHY, Form::FormItemSpec::Spec_Bibliography);
   m_SpecsTypes.insert(Constants::TAG_SPEC_DESCRIPTION, Form::FormItemSpec::Spec_Description);
   m_SpecsTypes.insert(Constants::TAG_SPEC_LABEL, Form::FormItemSpec::Spec_Label);
   m_SpecsTypes.insert(Constants::TAG_SPEC_VERSION, Form::FormItemSpec::Spec_Version);
   m_SpecsTypes.insert(Constants::TAG_SPEC_ICON, Form::FormItemSpec::Spec_IconFileName);

   m_PatientDatas.clear();
   m_PatientDatas.insert(Constants::TAG_DATAPATIENT_DRUGSALLERGIES, Core::IPatient::DrugsInnAllergies);
   m_PatientDatas.insert(Constants::TAG_DATAPATIENT_DRUGSCHRONIC, Core::IPatient::DrugsChronicTherapeutics);
   m_PatientDatas.insert(Constants::TAG_DATAPATIENT_WEIGHT, Core::IPatient::Weight);
   m_PatientDatas.insert(Constants::TAG_DATAPATIENT_HEIGHT, Core::IPatient::Height);
}

XmlFormContentReader::~XmlFormContentReader()
{}

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

/** Return true is formUid is already tested and present in cache */
bool XmlFormContentReader::isInCache(const QString &formUid) const
{
    return m_ReadableForms.contains(formUid);
}

/** Return the cached readable QDomDocument from the cache system or 0 if the \e formuid is not present in cache. */
QDomDocument *XmlFormContentReader::fromCache(const QString &formUid) const
{
    if (m_DomDocFormCache.contains(formUid))
        return m_DomDocFormCache[formUid];
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

/** Check if the xmlContent is conform to the XML Form IO. formUid must be an absolute path if corresponding to a file. */
bool XmlFormContentReader::checkFormFileContent(const QString &formUidOrFullAbsPath, const QString &contents) const
{
    bool ok = true;
    if (contents.isEmpty()) {
        warnXmlReadError(m_Mute, formUidOrFullAbsPath, tkTr(Trans::Constants::FILE_1_ISEMPTY).arg(formUidOrFullAbsPath));
        m_Error.append(tkTr(Trans::Constants::FILE_1_ISEMPTY).arg(formUidOrFullAbsPath));
        return false;
    }
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

    /** \todo check version of the file */
//    if (!contents.contains(QString("<%1>").arg(Constants::TAG_SPEC_VERSION), Qt::CaseInsensitive)) {
//        const QString &error = tr("No version number defined");
//        warnXmlReadError(m_Mute, file, error);
//        return false;
//    } else {
//        int beg = contents.indexOf(QString("<%1>").arg(Constants::TAG_SPEC_VERSION)) + QString("<%1>").arg(Constants::TAG_SPEC_VERSION).length();
//        int end = contents.indexOf(QString("</%1>").arg(Constants::TAG_SPEC_VERSION));
//        QString version = contents.mid(beg, end-beg).simplified();
//    }
    if (ok)
        m_DomDocFormCache.insert(formUidOrFullAbsPath, doc);
    return ok;
}

static void setPathToDescription(QString path, Form::FormIODescription *desc)
{
    path.replace(settings()->path(Core::ISettings::CompleteFormsPath), Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH);
    path.replace(settings()->path(Core::ISettings::SubFormsPath), Core::Constants::TAG_APPLICATION_SUBFORMS_PATH);
    desc->setData(Form::FormIODescription::UuidOrAbsPath, path);
}

/** Return the Form::FormIODescription according to the XML QDomElement \e xmlDescr. The \e xmlDescr must point to the first description tag of the document. */
Form::FormIODescription *XmlFormContentReader::readXmlDescription(const QDomElement &xmlDescr, const QString &formUid)
{
    XmlFormName form(formUid);
    Form::FormIODescription *ioDesc = new Form::FormIODescription;
    QHash<int, QString> elements;
    // get non translatable items
    elements.insert(Form::FormIODescription::Version, Constants::TAG_SPEC_VERSION);
    elements.insert(Form::FormIODescription::Author, Constants::TAG_SPEC_AUTHORS);
    elements.insert(Form::FormIODescription::CreationDate, Constants::TAG_SPEC_CREATIONDATE);
    elements.insert(Form::FormIODescription::LastModificationDate, Constants::TAG_SPEC_LASTMODIFDATE);
    elements.insert(Form::FormIODescription::GeneralIcon, Constants::TAG_SPEC_ICON);
    elements.insert(Form::FormIODescription::WebLink, Constants::TAG_SPEC_WEBLINK);
    elements.insert(Form::FormIODescription::FreeMedFormsCompatVersion, Constants::TAG_SPEC_COMPTAVERSION);
    QHashIterator<int, QString> i(elements);
    while (i.hasNext()) {
        i.next();
        ioDesc->setData(i.key(), xmlDescr.firstChildElement(i.value()).text());
    }
    // get translatable contents
    elements.clear();
    elements.insert(Form::FormIODescription::Category, Constants::TAG_SPEC_CATEGORY);
    elements.insert(Form::FormIODescription::ShortDescription, Constants::TAG_SPEC_DESCRIPTION);
    elements.insert(Form::FormIODescription::License, Constants::TAG_SPEC_LICENSE);
    elements.insert(Form::FormIODescription::Specialties, Constants::TAG_SPEC_SPECIALTIES);
    i = elements;
    while (i.hasNext()) {
        i.next();
        QDomElement desc = xmlDescr.firstChildElement(i.value());
        while (!desc.isNull()) {
            ioDesc->setData(i.key(), desc.text(), desc.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
            desc = desc.nextSiblingElement(i.value());
        }
    }
    // get translatable files
    elements.clear();
    elements.insert(Form::FormIODescription::HtmlDescription, Constants::TAG_SPEC_HTMLDESCRIPTION);
    elements.insert(Form::FormIODescription::HtmlSynthesis, Constants::TAG_SPEC_HTMLSYNTHESIS);
    i = elements;
    while (i.hasNext()) {
        i.next();
        QDomElement desc = xmlDescr.firstChildElement(i.value());
        while (!desc.isNull()) {
            QString content = readExtraFile(form, desc.text());
            ioDesc->setData(i.key(), content, desc.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
            desc = desc.nextSiblingElement(i.value());
        }
    }
    setPathToDescription(formUid, ioDesc);
    return ioDesc;
}

/** Return the Form description. Call this member just after the checkFormFileContent() so that the form xml content will be cached */
Form::FormIODescription *XmlFormContentReader::readFileInformations(const QString &formUidOrFullAbsPath)
{
    // Get the QDomDocument from cache
    QDomDocument *doc = m_DomDocFormCache[formUidOrFullAbsPath];
    if (!doc)
        return 0;
    QDomElement root = doc->documentElement();
    root = root.firstChildElement(Constants::TAG_FORM_DESCRIPTION);
    return readXmlDescription(root, formUidOrFullAbsPath);
}

QList<Form::FormIODescription *> XmlFormContentReader::getFormFileDescriptions(const Form::FormIOQuery &query)
{
    QList<Form::FormIODescription *> toReturn;

    /** \todo code here */

//    QString startPath;
//    if (query.typeOfForms() & Form::FormIOQuery::UserForms) {
//        /** \todo manage user forms path and default path */
//    } else {
//        /** \todo manage user forms path and default path */
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
//    /** \todo Add IFormIO to descr && check all forms for params of Query */
//    for(int i = 0; i < toReturn.count(); ++i) {
//        toReturn.at(i)->setIoFormReader(this);
//    }
    return toReturn;
}

bool XmlFormContentReader::loadForm(const XmlFormName &form, Form::FormMain *rootForm)
{
//    qWarning() << Q_FUNC_INFO << form.uid << form.absFileName;

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
//        rootForm = createNewForm(newForm, m_ActualForm);
    }
    m_ActualForm = rootForm;

    if (!loadElement(rootForm, root, form)) {
        LOG_ERROR_FOR("XmlFormContentReader", "Unable to load form " + form.uid);
        return false;
    }

//    rootForm->createDebugPage();
    createWidgets(rootForm);
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
            if (element.tagName().compare(::Constants::TAG_FORM_DESCRIPTION)==0) {
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

        // Name ?
        if (element.tagName().compare("name", Qt::CaseInsensitive)==0) {
            item->setUuid(element.text());
            element = element.nextSiblingElement();
            continue;
        }

        // Patient Data Representation ?
        if (element.tagName().compare(Constants::TAG_DATAPATIENT, Qt::CaseInsensitive)==0) {
            i = m_PatientDatas.value(element.text(), -1);
            if (i != -1) {
                item->setPatientDataRepresentation(i);
            }
            element = element.nextSiblingElement();
            continue;
        }

    //             // optional?
//            if (element.hasAttribute(Constants::ATTRIB_OPTIONNAL)) {
//                if (element.attribute(Constants::ATTRIB_OPTIONNAL).compare("true",Qt::CaseInsensitive)==0)
//                    item->valueReferences()->setOptionnal(true);
//                else
//                    item->valueReferences()->setOptionnal(false);

        // Add a form file ?
        if (element.tagName().compare(Constants::TAG_ADDFILE, Qt::CaseInsensitive)==0) {
            QString fileName = element.text();
            if (QFileInfo(fileName).isRelative())
                fileName.prepend(QFileInfo(form.absFileName).absolutePath() + QDir::separator());
            fileName = QDir::cleanPath(fileName);
            QString content = Utils::readTextFile(fileName, Utils::DontWarnUser);
            if (checkFormFileContent(fileName, content)) {
                if (!loadForm(fileName, m_ActualForm)) {
                    LOG_ERROR_FOR("XmlReader", "Unable to add form file " + element.text());
                } else {
                    saveFormToDatabase(fileName, XmlIOBase::FullContent, content);
                }
            }
            element = element.nextSiblingElement();
            continue;
        }

        // Options
        if (element.tagName().compare(Constants::TAG_OPTIONS)==0) {
            if (element.text().contains(Constants::TAG_OPTIONS_UNIQUE_EPISODE)) {
                if (item==m_ActualForm)
                    m_ActualForm->setEpisodePossibilities(Form::FormMain::UniqueEpisode);
            } else if (element.text().contains(Constants::TAG_OPTIONS_NO_EPISODE)) {
                if (item==m_ActualForm)
                    m_ActualForm->setEpisodePossibilities(Form::FormMain::NoEpisode);
            } else {
                item->addExtraData(element.tagName(), element.text());
            }
            element = element.nextSiblingElement();
            continue;
        }

        // All others add them in extraData
//        qWarning() << "XML adding other tag" << element.tagName() << element.text();
        item->addExtraData(element.tagName(), element.text());

        element = element.nextSiblingElement();
    }
    return true;
}

bool XmlFormContentReader::createElement(Form::FormItem *item, QDomElement &element, const XmlFormName &form)
{
//    qWarning() << "XmlFormIO create element" << element.text();
    // new item
    if (element.tagName().compare(Constants::TAG_NEW_ITEM, Qt::CaseInsensitive)==0) {
        if (item) {
            Form::FormItem *child = item->createChildItem();
            // read attributes (type, uid/name, patient representation...)
            if (element.hasAttribute(Constants::ATTRIB_UUID))
                child->setUuid(element.attribute(Constants::ATTRIB_UUID));

            if (element.hasAttribute(Constants::ATTRIB_NAME))
                child->setUuid(element.attribute(Constants::ATTRIB_NAME));

            if (element.hasAttribute(Constants::ATTRIB_TYPE))
                child->spec()->setValue(Form::FormItemSpec::Spec_Plugin, element.attribute(Constants::ATTRIB_TYPE), Trans::Constants::ALL_LANGUAGE);

            if (element.hasAttribute(Constants::ATTRIB_PATIENTREPRESENTATION)) {
                int i = m_PatientDatas.value(element.attribute(Constants::ATTRIB_PATIENTREPRESENTATION), -1);
                if (i != -1) {
                    child->setPatientDataRepresentation(i);
                }
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
        /** \todo add Forms' parent */
        m_ActualForm = m_ActualForm->createChildForm(element.firstChildElement(Constants::TAG_NAME).text());
        item = m_ActualForm;
        if (item) {
            // read attributes (type, uid/name, patient representation...)
            if (element.hasAttribute(Constants::ATTRIB_UUID))
                item->setUuid(element.attribute(Constants::ATTRIB_UUID));

            if (element.hasAttribute(Constants::ATTRIB_NAME))
                item->setUuid(element.attribute(Constants::ATTRIB_NAME));

            if (element.hasAttribute(Constants::ATTRIB_TYPE))
                item->spec()->setValue(Form::FormItemSpec::Spec_Plugin, element.attribute(Constants::ATTRIB_TYPE), Trans::Constants::ALL_LANGUAGE);

            loadElement(item, element, form);
            // read specific form's datas
            m_ActualForm = oldRootForm;
            return true;
        }
        else
            return false;
    }

    if (element.tagName().compare(Constants::TAG_NEW_PAGE, Qt::CaseInsensitive)==0) {
        // create a new page
        item = item->createPage(element.firstChildElement(Constants::TAG_NAME).text());
        /** \todo add page to a form */
        if (item) {
            // read attributes (type, uid/name, patient representation...)
            if (element.hasAttribute(Constants::ATTRIB_UUID))
                item->setUuid(element.attribute(Constants::ATTRIB_UUID));

            if (element.hasAttribute(Constants::ATTRIB_NAME))
                item->setUuid(element.attribute(Constants::ATTRIB_NAME));

            if (element.hasAttribute(Constants::ATTRIB_TYPE))
                item->spec()->setValue(Form::FormItemSpec::Spec_Plugin, element.attribute(Constants::ATTRIB_TYPE), Trans::Constants::ALL_LANGUAGE);

            loadElement(item, element, form);
            // read specific page's datas
            return true;
        }
        else
            return false;
    }

    return false;
}

/** Read the values of an element. */
bool XmlFormContentReader::populateValues(Form::FormItem *item, const QDomElement &root, const XmlFormName &form)
{
    QDomElement element = root.firstChildElement();
    while (!element.isNull()) {
        const QString &lang = element.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE);
//        qWarning() << "Values" << root.tagName() << element.tagName() << lang;
        int id = element.attribute(Constants::ATTRIB_ID, 0).toInt();
        const QString &val = element.text();
        int type = m_ValuesTypes.value(element.tagName(), -1);
        if (type==Form::FormItemValues::Value_Printing) {
            // read file
            if (!element.attribute(Constants::ATTRIB_FILE).isEmpty()) {
                QString content = readExtraFile(form, element.attribute(Constants::ATTRIB_FILE));
                item->valueReferences()->setValue(type, id, content, lang);
            } else {
                item->valueReferences()->setValue(type, id, val, lang);
            }
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
        int type = m_ScriptsTypes.value(element.tagName(), Form::FormItemScripts::Script_OnDemand);
        item->scripts()->setScript(type, script, lang);
        element = element.nextSiblingElement();
    }
    return true;
}

bool XmlFormContentReader::createItemWidget(Form::FormItem *item, QWidget *parent)
{
//    qWarning() << Q_FUNC_INFO;
//    qWarning() << item << item->uuid() << item->spec()->pluginName();
    // does plugin was inform in the xml file ?
    if (item->spec()->pluginName().isEmpty()) {
        LOG_ERROR_FOR("XmlFormContentReader", "No plugin name for item: " + item->uuid());
        item->setFormWidget(0);
        return false;
    }

    // does asked widget exists in the plugins ?
    QString askedWidget = item->spec()->pluginName().toLower();
    if (!m_PlugsFactories.keys().contains(askedWidget)) {
        item->setFormWidget(0);
        LOG_ERROR_FOR("XmlFormContentReader", QString("Form error in item %1: Asked widget does not exists in plugins widgets list: %2").arg(item->uuid()).arg(askedWidget));
        return false;
    }

    // get the widget
    Form::IFormWidgetFactory *factory = m_PlugsFactories.value(askedWidget);
    Form::IFormWidget *w = 0;
    w = factory->createWidget(askedWidget,item);
    if (w->isContainer()) {
        foreach(Form::FormItem *child, item->formItemChildren()) {
//            Form::IFormWidget *wchild = factory->createWidget(child->spec()->pluginName(),child,w);
//            w->addWidgetToContainer(wchild);
//            child->setFormWidget(wchild);
            createItemWidget(child,w);
        }
    }
    item->setFormWidget(w);
    Form::IFormWidget *p = qobject_cast<Form::IFormWidget*>(parent);
    if (p)
        p->addWidgetToContainer(w);
    return true;
}

bool XmlFormContentReader::createFormWidget(Form::FormMain *form)
{
    // Create a new Widget with a vbox and put the label
//    QWidget *w = new QWidget();
    if (!createItemWidget(form, 0))
        return false;
    return true;
}

bool XmlFormContentReader::createWidgets(const Form::FormMain *rootForm)
{
    // foreach FormMain children
    foreach(Form::FormMain *form, rootForm->flattenFormMainChildren()) {
        // create the form
        createFormWidget(form);
    }
    return true;
}

bool XmlFormContentReader::loadPmhCategories(const QString &uuidOrAbsPath)
{
//    QString file = QFileInfo(uuidOrAbsPath).absolutePath() + "/pmhcategories.xml";
//    // replace path TAGs
//    file.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
//    file.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
//    file.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));

//    QDomDocument *doc = 0;
//    if (!reader()->isInCache(file)) {
//        if (!canReadForms(file)) {
//            LOG_ERROR("Unable to read Pmh Category file: " + file);
//            return false;
//        }
//    }
//    categoryCore()->removeAllExistingCategories("PMHx");
//    doc = reader()->fromCache(file);
//    Q_ASSERT(doc);
//    if (!doc) {
//        LOG_ERROR("No category document in XmlFormIO::loadPmhCategories("+file+")");
//        return false;
//    }
//    QDomElement root = doc->firstChildElement(Constants::TAG_MAINXMLTAG);
//    QDomElement element = root.firstChildElement(Constants::TAG_PMHX_CATEGORIES);
//    element = element.firstChildElement(::Constants::TAG_CATEGORY);
//    while (!element.isNull()) {
//        createCategory(element, 0);
//        element = element.nextSiblingElement(::Constants::TAG_CATEGORY);
//    }
//    pmhCore()->pmhCategoryModel()->refreshFromDatabase();
    return true;
}

bool XmlFormContentReader::createCategory(const QDomElement &element, Category::CategoryItem *parent)
{
//    // create the category
//    Category::CategoryItem *item = new Category::CategoryItem;
//    item->setData(Category::CategoryItem::DbOnly_Mime, "PMHx");
//    item->setData(Category::CategoryItem::ThemedIcon, element.attribute(::Constants::ATTRIB_ICON));

//    // read the labels
//    QDomElement label = element.firstChildElement(::Constants::TAG_SPEC_LABEL);
//    while (!label.isNull()) {
//        item->setLabel(label.text(), label.attribute(::Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
//        label = label.nextSiblingElement(::Constants::TAG_SPEC_LABEL);
//    }

//    // get ExtraTag content -> CategoryItem::ExtraXml
//    QDomElement extra = element.firstChildElement(::Constants::TAG_SPEC_EXTRA);
//    if (!extra.isNull()) {
//        item->setData(Category::CategoryItem::ExtraXml, extra.toDocument().toString(2));
//    }

//    // save to database
//    if (parent) {
//        parent->addChild(item);
//        item->setParent(parent);
//    }
//    categoryCore()->saveCategory(item);

//    // has children ?
//    QDomElement child = element.firstChildElement(::Constants::TAG_CATEGORY);
//    while (!child.isNull()) {
//        createCategory(child, item);
//        child = child.nextSiblingElement(::Constants::TAG_CATEGORY);
//    }
    return true;
}

/** Save the \e content of the form \e form to the database and return the used formUid. If the \e content is empty the form file is accessed */
QString XmlFormContentReader::saveFormToDatabase(const XmlFormName &form, const int type, const QString &content, const QString &modeName)
{
//    qWarning() << Q_FUNC_INFO << form.uid << modeName;
    // add form to database
    if (content.isEmpty()) {
        base()->saveContent(form.uid, Utils::readTextFile(form.absFileName, Utils::DontWarnUser), type, modeName);
    } else {
        base()->saveContent(form.uid, content, type, modeName);
    }
    return form.uid;
}

/** Get the content of a file from the database only */
QString XmlFormContentReader::getFileContentFromDatabase(const XmlFormName &form, const int type, const QString &fileNameOrModeName)
{
    return base()->getFormContent(form.uid, type, fileNameOrModeName);
}

/** Read an extra file and ensure that it will be saved into the database. */
QString XmlFormContentReader::readExtraFile(const XmlFormName &form, const QString &fileName)
{
    if (fileName.isEmpty())
        return QString();

    // try to get it from database
    QString content = getFileContentFromDatabase(form.uid, XmlIOBase::ExtraFiles, fileName);
    if (content.isEmpty()) {
        QString name = fileName;
        // not available in database -> read file, add to database
        if (QFileInfo(fileName).isRelative()) {
            QFileInfo info(form.absFileName);
            if (info.isDir())
                name.prepend(info.absoluteFilePath() + QDir::separator());
            else
                name.prepend(info.absolutePath() + QDir::separator());
        }
        content = Utils::readTextFile(name, Utils::DontWarnUser);
        saveFormToDatabase(form, XmlIOBase::ExtraFiles, content, fileName);
    }
    return content;
}
