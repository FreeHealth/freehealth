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
#include "xmlformio.h"
#include "constants.h"
#include "xmlformcontentreader.h"
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

bool XmlFormIO::canReadForms(const QString &absFileName) const
{
//    qWarning() << Q_FUNC_INFO << absFileName;
    if (m_ReadableForms.keys().contains(absFileName)) {
        return m_ReadableForms.value(absFileName);
    }
    m_Error.clear();
    m_AbsFileName.clear();
    QString fileName = absFileName;
    // replace path TAGs
    fileName.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
    fileName.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
    fileName.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));
    if (m_ReadableForms.keys().contains(fileName)) {
        return m_ReadableForms.value(fileName);
    }

    QFileInfo formFile(absFileName);
    if (base()->isFormExists(formFile.path(), XmlIOBase::FullContent, formFile.baseName())) {
//        qWarning() << "READ FROM DATABASE";
        if (reader()->checkFormFileContent(formFile.path(), base()->getFormContent(formFile.path(), XmlIOBase::FullContent, formFile.baseName()))) {
            m_AbsFileName = fileName;
            m_ReadableForms.insert(formFile.path(), true);
            m_ReadableForms.insert(absFileName, true);
            return true;
        } else {
            Utils::warningMessageBox(tr("Invalid form in database."),
                                     tr("An invalid form was found in the database. Please contact your software administrator.\n"
                                        "Wrong form: %1\n"
                                        "Error: %2")
                                     .arg(absFileName)
                                     .arg(reader()->lastError()));
            return false;
        }
    } else {
//        qWarning() << "READ FROM FILE";
        // Correct filename ?
        if (QFileInfo(fileName).isDir()) {
            // try to read central.xml
            fileName = fileName + "/central.xml";
        }
        if (!QFileInfo(fileName).exists()) {
            m_Error.append(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName));
            return false;
        }
        if (QFileInfo(fileName).suffix().toLower()=="xml") {
            if (reader()->checkFormFileContent(fileName, Utils::readTextFile(fileName, Utils::DontWarnUser))) {  // use reader
                m_AbsFileName = fileName;
                m_ReadableForms.insert(fileName, true);
                m_ReadableForms.insert(absFileName, true);
                return true;
            } else {
                m_ReadableForms.insert(fileName, false);
                m_ReadableForms.insert(absFileName, false);
                return false;
            }
        } else {
            m_Error.append(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(fileName));
        }
    }
    m_ReadableForms.insert(fileName, false);
    m_ReadableForms.insert(absFileName, false);
    return false;
}

Form::FormIODescription *XmlFormIO::readFileInformations(const QString &uuidOrAbsPath)
{
    return reader()->readFileInformations(uuidOrAbsPath);
}

void XmlFormIO::getAllFormsFromDir(const QString &absPath, QList<Form::FormIODescription *> *list)
{
    QDir start(absPath);
    if (!start.exists()) {
        LOG_ERROR_FOR("XmlFormIO", tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(absPath) +
                  "\n" + tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg("File Form"));
        return;
    }
    // get all forms included in this path
    foreach(const QFileInfo &file, Utils::getFiles(start, "central.xml", true)) {
        const QString &fileName = file.absoluteFilePath();
        if (canReadForms(fileName))
            list->append(reader()->readFileInformations(fileName));
    }
}

QList<Form::FormIODescription *> XmlFormIO::getFormFileDescriptions(const Form::FormIOQuery &query)
{
    QList<Form::FormIODescription *> toReturn;
    QString startPath;
    if (query.typeOfForms() & Form::FormIOQuery::UserForms) {
        /** \todo manage user forms path and default path */
    } else {
        /** \todo manage user forms path and default path */
    }

    // Get from database
    toReturn = base()->getFormDescription(query);

    // Get from default form files
    if (query.typeOfForms() & Form::FormIOQuery::CompleteForms) {
        startPath = settings()->path(Core::ISettings::CompleteFormsPath);
        getAllFormsFromDir(startPath, &toReturn);
        for(int i = 0; i < toReturn.count(); ++i) {
            toReturn.at(i)->setData(Form::FormIODescription::IsCompleteForm, true);
        }
    }
    if (query.typeOfForms() & Form::FormIOQuery::SubForms) {
        startPath = settings()->path(Core::ISettings::SubFormsPath);
        getAllFormsFromDir(startPath, &toReturn);
        for(int i = 0; i < toReturn.count(); ++i) {
            toReturn.at(i)->setData(Form::FormIODescription::IsSubForm, true);
        }
    }
    /** \todo Add IFormIO to descr && check all forms for params of Query */
    for(int i = 0; i < toReturn.count(); ++i) {
        toReturn.at(i)->setIoFormReader(this);
    }
    return toReturn;
}

QList<Form::FormMain *> XmlFormIO::loadAllRootForms(const QString &uuidOrAbsPath)
{
    /** \todo code here
      * - Before read process, check if formUid exists in database, if not populate db with the
      *   local form file_s_ (all modes).
    */
    qWarning() << Q_FUNC_INFO << uuidOrAbsPath;
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
    if (!canReadForms(uuid))
        return toReturn;

    // replace path TAGs
    QString file = uuid;
    file.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
    file.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
    file.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::BundleResourcesPath));
    if (!file.endsWith("xml", Qt::CaseInsensitive))
        file += QDir::separator();
    // now we work with m_AbsFileName which is defined by canReadForm()
    QDir dir(QFileInfo(file).absolutePath());

    // Populate DB with all the files of this form if needed
    if (!base()->isFormExists(uuid)) {
        foreach(const QFileInfo &f, dir.entryInfoList(QStringList() << "*.xml", QDir::Files | QDir::Readable)) {
            QString modeName = f.baseName();
            reader()->saveFormToDatabase(uuid, Utils::readTextFile(f.absoluteFilePath(), Utils::DontWarnUser), modeName);
        }
    }

    QHash<QString, QString> mode_contents = base()->getAllFormFullContent(uuid);

    reader()->refreshPluginFactories();
    QHashIterator<QString, QString> it(mode_contents);
    while (it.hasNext()) {
        it.next();
        Form::FormMain *root = m_ActualForm = new Form::FormMain;
        root->setModeUniqueName(it.key());
        root->setUuid(uuidOrAbsPath);
        QString fakeFileName = QFileInfo(file).absolutePath() + "/" + it.key() + ".xml";

        if (!reader()->isInCache(fakeFileName)) {
            if (!reader()->checkFormFileContent(fakeFileName, it.value()))
                continue;
        }

        if (!reader()->loadForm(fakeFileName, root)) {
            LOG_ERROR(m_Error.join("\n"));
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

bool XmlFormIO::loadPmhCategories(const QString &uuidOrAbsPath)
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

bool XmlFormIO::createCategory(const QDomElement &element, Category::CategoryItem *parent, const QString &readingAbsPathFile)
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
        // if extra contains a reference to a form -> add it to the database
        QDomElement addFile = extra.firstChildElement(Constants::TAG_ADDFILE);
        if (!addFile.isNull()) {
            QString fileName = addFile.text();
            if (fileName.startsWith(".")) {
                QFileInfo file(fileName);
                file.setFile(QFileInfo(readingAbsPathFile).absolutePath() + QDir::separator() + file.fileName());
                fileName = file.absoluteFilePath();
            }
            // add form to database
            const QString &u = reader()->saveFormToDatabase(fileName);
            if (u.isEmpty())
                LOG_ERROR("Unable to save form to database");
            else
                addFile.toText().setData(u);
        }

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

