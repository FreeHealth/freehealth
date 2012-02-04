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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "xmlformio.h"
#include "constants.h"
#include "xmlformcontentreader.h"
#include "xmliobase.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/versionnumber.h>
#include <utils/genericupdateinformation.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

//#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorycore.h>

#include <pmhplugin/pmhcore.h>
#include <pmhplugin/pmhcategorymodel.h>

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCodec>
#include <QFileInfo>

using namespace XmlForms;
using namespace Internal;
using namespace Trans::ConstantTranslations;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  Inline static functions  //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline static ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Category::CategoryCore *categoryCore() {return  Category::CategoryCore::instance();}
static inline PMH::PmhCore *pmhCore() {return PMH::PmhCore::instance();}
static inline Internal::XmlFormContentReader *reader() {return Internal::XmlFormContentReader::instance();}
static inline Internal::XmlIOBase *base() {return Internal::XmlIOBase::instance();}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  XmlFormIO  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
XmlFormIO::XmlFormIO(QObject *parent) :
        IFormIO(parent), m_Mute(false)
{
    setObjectName("XmlFormIO");
}

XmlFormIO::~XmlFormIO()
{
}

QString XmlFormIO::lastestXmlVersion()
{
    return "0.3.0";
}

bool XmlFormIO::canReadForms(const QString &uuidOrAbsPath) const
{
    Form::FormIOQuery query;
    query.setFormUuid(uuidOrAbsPath);
    return canReadForms(query);
}

bool XmlFormIO::canReadForms(const Form::FormIOQuery &query) const
{
    XmlFormName form(query.formUuid());
//    qWarning() << "CanRead" << query.formUuid() << form.uid << form.absFileName;

    if (m_ReadableForms.keys().contains(form.absFileName)) {
        return m_ReadableForms.value(form.absFileName);
    }
    m_Error.clear();
    m_AbsFileName.clear();
    QFileInfo formFile(form.absFileName);

    QString content;
    if (!query.forceFileReading()) {
        // Get it from database (save it first if needed)
        if (!base()->isFormExists(form.uid, XmlIOBase::FullContent, form.modeName)) {
            base()->saveForm(form);
        }
        content = base()->getFormContent(form.uid, XmlIOBase::FullContent, form.modeName);
    } else {
        // Get from local file
        if (!formFile.exists()) {
            LOG_ERROR(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(formFile.absoluteFilePath()));
            m_Error.append(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(formFile.absoluteFilePath()));
            return false;
        }
        if (formFile.suffix().toLower()=="xml")
            content = Utils::readTextFile(formFile.absoluteFilePath(), Utils::DontWarnUser);
    }

    // check form content
    if (!reader()->checkFileContent(formFile.absoluteFilePath(), content)) {
        Utils::warningMessageBox(tr("Invalid file detected."),
                                 tr("An invalid file was found. Please contact your software administrator.\n"
                                    "Wrong file: %1\n"
                                    "Error: %2")
                                 .arg(form.absFileName)
                                 .arg(reader()->lastError()));
        m_ReadableForms.insert(form.absFileName, false);
        m_ReadableForms.insert(form.uid, false);
        return false;
    }

    // check included xml files too
    QDomDocument *doc = reader()->fromCache(formFile.absoluteFilePath());
    Q_ASSERT(doc);
    if (doc) {
        QDomNodeList list = doc->elementsByTagName("file");

        for(int i=0; i < list.count(); ++i) {
            const QDomNode &node = list.at(i);
            const QString &include = node.toElement().text();
            if (include.endsWith(".xml", Qt::CaseInsensitive)) {
                if (!canReadForms(include))
                    LOG_ERROR("unable to read included form: " + include);
            }
        }
    }

    m_AbsFileName = form.absFileName;
    m_ReadableForms.insert(form.uid, true);
    m_ReadableForms.insert(form.absFileName, true);
    return true;
}

bool XmlFormIO::canReadScripts(const Form::FormIOQuery &query) const
{
    XmlFormName form(query.formUuid());
//    qWarning() << Q_FUNC_INFO << query.formUuid() << form.uid << form.absFileName << form.modeName;

    if (m_ReadableForms.keys().contains(form.absFileName)) {
        return m_ReadableForms.value(form.absFileName);
    }
    m_Error.clear();
    m_AbsFileName.clear();
    QFileInfo scriptFile(form.absFileName);

    QString content;
    if (!query.forceFileReading()) {
        // Get it from database
        if (!base()->isFormExists(form.uid, XmlIOBase::ScriptFile, form.modeName)) {
            base()->saveForm(form);
        }
        content = base()->getFormContent(form.uid, XmlIOBase::ScriptFile, form.modeName);
    } else {
        if (!scriptFile.exists()) {
            LOG_ERROR(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(scriptFile.absoluteFilePath()));
            m_Error.append(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(scriptFile.absoluteFilePath()));
            return false;
        }
        if (scriptFile.suffix().toLower()=="js")
            content = Utils::readTextFile(scriptFile.absoluteFilePath(), Utils::DontWarnUser);
    }

    // check form content
    if (!reader()->checkFileContent(scriptFile.absoluteFilePath(), content)) {
        Utils::warningMessageBox(tr("Invalid file detected."),
                                 tr("An invalid file was found. Please contact your software administrator.\n"
                                    "Wrong file: %1\n"
                                    "Error: %2")
                                 .arg(form.absFileName)
                                 .arg(reader()->lastError()));
        m_ReadableScripts.insert(form.absFileName, false);
        return false;
    }
    m_AbsFileName = form.absFileName;
    m_ReadableScripts.insert(form.absFileName, true);
    return true;
}

Form::FormIODescription *XmlFormIO::readFileInformations(const QString &uuidOrAbsPath) const
{
    return reader()->readFileInformations(uuidOrAbsPath);
}

QList<Form::FormIODescription *> XmlFormIO::getFormFileDescriptions(const Form::FormIOQuery &query) const
{
    QList<Form::FormIODescription *> toReturn;
    QString startPath;
    QStringList includedUids;
    if (query.typeOfForms() & Form::FormIOQuery::UserForms) {
        /** \todo manage user forms path and default path */
    } else {
        /** \todo manage user forms path and default path */
    }

//    qWarning() << query.formUuid() << query.forceFileReading();

    if (!query.forceFileReading()) {
        // Get from database
        toReturn = base()->getFormDescription(query);
        if (!toReturn.isEmpty() && !query.getAllAvailableFormDescriptions())
            return toReturn;
        for(int i=0; i<toReturn.count(); ++i) {
            includedUids << toReturn.at(i)->data(Form::FormIODescription::UuidOrAbsPath).toString();
        }
    }

    // Get a specific form description
    if (!query.formUuid().isEmpty()) {
        XmlFormName form(query.formUuid());
        if (canReadForms(query)) {
            Form::FormIODescription *desc = reader()->readFileInformations(form.absFileName, query);
            if (desc) {
                desc->setData(Form::FormIODescription::IsCompleteForm, true);
                toReturn.append(desc);
                return toReturn;
            }
        }
        return toReturn;
    }

    // Get all form files
    if (query.typeOfForms() & Form::FormIOQuery::CompleteForms) {
        startPath = settings()->path(Core::ISettings::CompleteFormsPath);
        QDir start(startPath);
        // get all forms included in this path
        foreach(const QFileInfo &file, Utils::getFiles(start, "central.xml", Utils::Recursively)) {
            const QString &fileName = file.absoluteFilePath();
            XmlFormName form(fileName);
            if (includedUids.contains(form.uid)) {
                continue;
            }
            if (canReadForms(fileName)) {
                Form::FormIODescription *desc = reader()->readFileInformations(fileName);
                if (desc) {
                    desc->setData(Form::FormIODescription::IsCompleteForm, true);
                    toReturn.append(desc);
                }
            }
        }
    }
    if (query.typeOfForms() & Form::FormIOQuery::SubForms) {
        startPath = settings()->path(Core::ISettings::SubFormsPath);
        QDir start(startPath);
        foreach(const QFileInfo &file, Utils::getFiles(start, "central.xml", Utils::Recursively)) {
            const QString &fileName = file.absoluteFilePath();
            XmlFormName form(fileName);
            if (includedUids.contains(form.uid)) {
                qWarning() << "xxxxxxxxxxxx formAlredayIncluded" << form.uid;
                continue;
            }
            if (canReadForms(fileName)) {
                Form::FormIODescription *desc = reader()->readFileInformations(fileName);
                if (desc) {
                    desc->setData(Form::FormIODescription::IsSubForm, true);
                    toReturn.append(desc);
                }
            }
        }
    }
    /** \todo Add IFormIO to descr && check all forms for params of Query */
    for(int i = 0; i < toReturn.count(); ++i) {
        toReturn.at(i)->setIoFormReader((Form::IFormIO*)this);
    }
    return toReturn;
}

QList<Form::FormMain *> XmlFormIO::loadAllRootForms(const QString &uuidOrAbsPath) const
{
    XmlFormName form(uuidOrAbsPath);
//    qWarning() << Q_FUNC_INFO << uuidOrAbsPath << form.uid << form.absFileName;

    QList<Form::FormMain *> toReturn;
    QString uuid = uuidOrAbsPath;
    if (uuidOrAbsPath.isEmpty()) {
        if (m_AbsFileName.isEmpty()) {
            LOG_ERROR(tr("No form file name"));
            return toReturn;
        } else {
            uuid = m_AbsFileName;
        }
    }
    if (!canReadForms(form.uid)) {
        LOG_ERROR("Can not read form " + form.uid);
        return toReturn;
    }

    QFileInfo formFile(form.absFileName);
    QDir dir;
    // remove modeName for the fileName
    if (formFile.isDir())
        dir.setPath(formFile.absoluteFilePath());
    else
        dir.setPath(formFile.absolutePath());

    // Populate DB with all the files of this form if needed
    if (!base()->isFormExists(form.uid)) {
        base()->saveForm(form);
    }

    QHash<QString, QString> mode_contents = base()->getAllFormFullContent(form.uid);

    reader()->refreshPluginFactories();
    QHashIterator<QString, QString> it(mode_contents);
    while (it.hasNext()) {
        it.next();
        Form::FormMain *root = m_ActualForm = new Form::FormMain;
        root->setModeUniqueName(it.key());
        root->setUuid(form.uid);
        root->setIoFormReader((XmlFormIO*)this);
        QString fakeFileName;
        QFileInfo info(form.absFileName);
        if (formFile.isDir())
            fakeFileName = info.absoluteFilePath() + "/" + it.key() + ".xml";
        else
            fakeFileName = info.absolutePath() + "/" + it.key() + ".xml";

        if (!reader()->isInCache(fakeFileName)) {
            if (!reader()->checkFileContent(fakeFileName, it.value()))
                continue;
        }
        XmlFormName mode(form.uid);
        mode.absFileName = fakeFileName;
//        qWarning() << "MODE" << mode.absFileName << mode.uid;
        if (!reader()->loadForm(mode, root)) {
            LOG_ERROR("Form not readable: " + fakeFileName);
        } else {
            toReturn.append(root);
        }
        // Emit the formLoaded signal for all subForms loaded
        QList<Form::FormMain *> forms = root->flattenFormMainChildren();
        for(int i=0; i < forms.count(); ++i) {
            forms.at(i)->emitFormLoaded();
        }
    }
    return toReturn;
}

bool XmlFormIO::loadPmhCategories(const QString &uuidOrAbsPath) const
{
    QString file = uuidOrAbsPath;
    if (file.endsWith(".xml")) {
        file = file.left(file.lastIndexOf("/"));
    }
    file += "/pmhcategories.xml";

    // replace path TAGs
    file.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
    file.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
    file.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));

    QDomDocument *doc = 0;
    if (!reader()->isInCache(file)) {
        if (!canReadForms(file)) {
            LOG_ERROR("Unable to read Pmh Category file: " + file);
            return false;
        }
    }
    categoryCore()->removeAllExistingCategories("PMHx");
    doc = reader()->fromCache(file);
    Q_ASSERT(doc);
    if (!doc) {
        LOG_ERROR("No category document in XmlFormIO::loadPmhCategories("+file+")");
        return false;
    }
    QDomElement root = doc->firstChildElement(Constants::TAG_MAINXMLTAG);
    QDomElement element = root.firstChildElement(Constants::TAG_PMHX_CATEGORIES);
    element = element.firstChildElement(::Constants::TAG_CATEGORY);
    while (!element.isNull()) {
        createCategory(element, 0, file);
        element = element.nextSiblingElement(::Constants::TAG_CATEGORY);
    }
    pmhCore()->pmhCategoryModel()->refreshFromDatabase();
    LOG("Category created");
    return true;
}

bool XmlFormIO::createCategory(const QDomElement &element, Category::CategoryItem *parent, const QString &readingAbsPathFile) const
{
    // create the category
    Category::CategoryItem *item = new Category::CategoryItem;
    item->setData(Category::CategoryItem::DbOnly_Mime, "PMHx");
    item->setData(Category::CategoryItem::ThemedIcon, element.attribute(::Constants::ATTRIB_ICON));

    // read the labels
    QDomElement label = element.firstChildElement(::Constants::TAG_SPEC_LABEL);
    while (!label.isNull()) {
        item->setLabel(label.text(), label.attribute(::Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
        label = label.nextSiblingElement(::Constants::TAG_SPEC_LABEL);
    }

    // get ExtraTag content -> CategoryItem::ExtraXml
    QDomElement extra = element.firstChildElement(::Constants::TAG_SPEC_EXTRA);
    if (!extra.isNull()) {
//        QDomElement addFile = extra.firstChildElement(Constants::TAG_ADDFILE);
        // if extra contains a reference to a form -> add it to the database
//        if (!addFile.isNull()) {
//            QString fileName = addFile.text();
//            if (fileName.startsWith(".")) {
//                QFileInfo file(fileName);
//                file.setFile(QFileInfo(readingAbsPathFile).absolutePath() + QDir::separator() + file.fileName());
//                fileName = file.absoluteFilePath();
//            }
//            // add form to database
//            const QString &u = reader()->saveFormToDatabase(fileName, XmlIOBase::FullContent);
//            if (u.isEmpty())
//                LOG_ERROR("Unable to save form to database");
//            else
//                addFile.toText().setData(u);
//        }

        QString content;
        QTextStream s(&content);
        extra.save(s, 2);
        item->setData(Category::CategoryItem::ExtraXml, content);
    }

    // reparent item
    if (parent) {
        parent->addChild(item);
        item->setParent(parent);
    }
    categoryCore()->saveCategory(item);

    // has children ?
    QDomElement child = element.firstChildElement(::Constants::TAG_CATEGORY);
    while (!child.isNull()) {
        createCategory(child, item, readingAbsPathFile);
        child = child.nextSiblingElement(::Constants::TAG_CATEGORY);
    }
    return true;
}

QList<QPixmap> XmlFormIO::screenShots(const QString &uuidOrAbsPath) const
{
    QList<QPixmap> toReturn;
    return toReturn;
}

QPixmap XmlFormIO::screenShot(const QString &uuidOrAbsPath, const QString &name) const
{
    return base()->getScreenShot(uuidOrAbsPath, name);
}

/** Check the database form version and try to update them with the local files. */
bool XmlFormIO::checkDatabaseFormFileForUpdates()
{
    QList<Form::FormIODescription *> fromFiles;
    QList<Form::FormIODescription *> fromDb;
    LOG("Checking for form update");

    // get all available descriptions from database
    Form::FormIOQuery querydb;
    querydb.setGetAllAvailableFormDescriptions(true);
    fromDb = base()->getFormDescription(querydb);

    // Test all forms for an update and populate a list
    QList<XmlFormName> formsToUpdate;
    bool readError = false;
    QStringList msg;
    foreach(Form::FormIODescription *descDb, fromDb) {
        qDeleteAll(fromFiles);
        fromFiles.clear();
        Form::FormIOQuery query;
        query.setFormUuid(descDb->data(Form::FormIODescription::UuidOrAbsPath).toString());
        query.setForceFileReading(true);
        Utils::VersionNumber db(descDb->data(Form::FormIODescription::Version).toString());
        fromFiles = getFormFileDescriptions(query);

        foreach(Form::FormIODescription *descFile , fromFiles) {
            // check version number of forms
            Utils::VersionNumber file(descFile->data(Form::FormIODescription::Version).toString());
            if (file.versionString()=="test" || file>db) {
                // update database
                XmlFormName form(descFile->data(Form::FormIODescription::UuidOrAbsPath).toString());

                qWarning() << "UpdateDB:" << form.uid << form.modeName << form.absFileName;

                // Construct the detailled text of the user's question messagebox
                msg << tr("Form: ") + descFile->data(Form::FormIODescription::ShortDescription).toString() + "<br />";
                msg << tr("Database version: %1").arg(db.versionString()) + "<br />";
                foreach(const Utils::GenericUpdateInformation &u, descFile->updateInformationForVersion(db)) {
                    msg << Utils::firstLetterUpperCase(tkTr(Trans::Constants::FROM_1_TO_2).arg(u.fromVersion()).arg(u.toVersion())) + "<br>" + u.text();
                }
                if (!formsToUpdate.contains(form))
                    formsToUpdate << form;
            }
        }
    }

    if (!readError && !formsToUpdate.isEmpty()) {
        // Ask user for update
        bool yes = Utils::yesNoMessageBox(tr("Form update detected."),
                                          tr("A form update has been detected. Do you want to update the forms ?"),
                                          msg.join("<br><br>"));
        if (yes) {
            // Update all checked forms
            foreach(const XmlFormName &form, formsToUpdate) {
                if (!base()->saveForm(form)) { //, doc->toString(2), XmlIOBase::FullContent, file.baseName(), QDateTime::currentDateTime())) {
                    LOG_ERROR("Unable to update form database. Form: " + form.uid + " " + form.absFileName);
                } else {
                    LOG("Form updated: "  + form.uid + " " + form.absFileName);
                }
            }

//                foreach(const QFileInfo &file, formUidToUpdate.values(formUid)) {
//                    //                    qWarning() << "UPDATING" << file.baseName();
//                    QDomDocument *doc = reader()->fromCache(file.absoluteFilePath());
//                    if (!doc) {
//                        LOG_ERROR("Wrong cache");
//                        continue;
//                    }
//                    //                        qWarning() << "CACHE OK" << file.absoluteFilePath();
//                    XmlFormName form(formUid);
//                    if (!base()->saveForm(form)) { //, doc->toString(2), XmlIOBase::FullContent, file.baseName(), QDateTime::currentDateTime())) {
//                        LOG_ERROR("Unable to update form database. Form: " + formUid + " " + file.baseName());
//                    } else {
//                        LOG("Form updated: " + formUid + " " + file.baseName());
//                    }
//                    // save screenshots && scripts
////                    base()->saveScreenShots(file.absoluteFilePath(), formUid);
////                    base()->saveScriptFiles(file.absolutePath(), formUid);
//                }
//            }

        }
    }

    // Clear cache
    m_ReadableForms.clear();
    reader()->clearCache();

    // Clear pointers
    qDeleteAll(fromFiles);
    fromFiles.clear();
    qDeleteAll(fromDb);
    fromDb.clear();
    return true;
}
