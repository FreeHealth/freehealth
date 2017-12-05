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
#include <QTextCodec>
#include <QFileInfo>

using namespace XmlForms;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  Inline static functions  //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Internal::XmlFormContentReader *reader() {return Internal::XmlFormContentReader::instance();}
static inline Internal::XmlIOBase *base() {return Internal::XmlIOBase::instance();}

static inline XmlFormName &formName(const QString &uuid, QHash<QString, XmlFormName> &cache)
{
    XmlFormName form(uuid);
    if (!cache.contains(form.uid)) {
        cache.insert(form.uid, form);
    }
    return cache[form.uid];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  XmlFormIO  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
XmlFormIO::XmlFormIO(QObject *parent) :
    IFormIO(parent),
    m_Mute(false),
    alreadyCheckedForUpdates(false)
{
    setObjectName("XmlFormIO");
}

XmlFormIO::~XmlFormIO()
{
}

QString XmlFormIO::latestXmlVersion()
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
    XmlFormName &form = formName(query.formUuid(), m_FormNames);
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
        if (!base()->isFormExists(form, XmlIOBase::FullContent, form.modeName)) {
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
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // check form content
    if (!reader()->checkFileContent(formFile.absoluteFilePath(), content)) {
        LOG_ERROR(tr("Invalid form file detected: %1").arg(formFile.absoluteFilePath()));
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
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // check included xml files too
    QDomDocument *doc = reader()->fromCache(formFile.absoluteFilePath());
    Q_ASSERT(doc);
    if (doc) {
        QDomNodeList list = doc->elementsByTagName("file");

        for(int i=0; i < list.count(); ++i) {
            const QDomNode &node = list.at(i);
            const QString &include = node.toElement().text();
            if (include.endsWith(".xml", Qt::CaseInsensitive)) {
                Form::FormIOQuery queryInclude;
                queryInclude.setFormUuid(include);
                queryInclude.setForceFileReading(query.forceFileReading());
                if (!canReadForms(queryInclude))
                    LOG_ERROR("Unable to read included form: " + include);
            }
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }

    m_AbsFileName = form.absFileName;
    m_ReadableForms.insert(form.uid, true);
    m_ReadableForms.insert(form.absFileName, true);
    return true;
}

Form::FormIODescription *XmlFormIO::readFileInformation(const QString &uuidOrAbsPath) const
{
    return reader()->readFileInformation(uuidOrAbsPath);
}

// Check the descriptions according to the query settings. Warning: \e forms is modified
static void checkFormIODescription(QList<Form::FormIODescription *> &forms, const Form::FormIOQuery &query, const XmlFormIO *reader)
{
    // Check gender specific
    if (query.excludeGenderSpecific()) {
        QString patientGender = patient()->data(Core::IPatient::Gender).toString().toUpper(); // M F H
        for(int i = forms.count() - 1; i > 0 ; --i) {
            Form::FormIODescription *descr = forms.at(i);
            const QString &gender = descr->data(Form::FormIODescription::GenderLimitation).toString();
            if (!gender.isEmpty()) {
                if (gender.toUpper() != patientGender)
                    forms.removeAt(i);
            }
        }
    }
    // TODO: check all forms for params of Query, check forms versions, remove duplicates
    // Set IOReader to descriptions
    const Form::IFormIO *const_iformio = qobject_cast<const Form::IFormIO*>(reader);
    Form::IFormIO *iformio = const_cast<Form::IFormIO*>(const_iformio);
    for(int i = 0; i < forms.count(); ++i) {
        forms.at(i)->setIoFormReader(iformio);
    }
}

QList<Form::FormIODescription *> XmlFormIO::getFormFileDescriptions(const Form::FormIOQuery &query) const
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QList<Form::FormIODescription *> toReturn;
    QStringList includedUids;
//    qWarning() << query.formUuid() << query.forceFileReading();

    if (!query.forceFileReading()) {
        // Get from database
        toReturn = base()->getFormDescription(query);
        if (!toReturn.isEmpty() && !query.getAllAvailableFormDescriptions()) {
            checkFormIODescription(toReturn, query, this);
            return toReturn;
        }
        for(int i=0; i < toReturn.count(); ++i) {
            includedUids << toReturn.at(i)->data(Form::FormIODescription::UuidOrAbsPath).toString();
        }
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // Get a specific form description
    if (!query.formUuid().isEmpty()) {
        XmlFormName &form = formName(query.formUuid(), m_FormNames);
//        XmlFormName form(query.formUuid());
        if (canReadForms(query)) {
            Form::FormIODescription *desc = reader()->readFileInformation(form, query);
            if (desc) {
                desc->setData(Form::FormIODescription::IsCompleteForm, true);
                const Form::IFormIO *const_iformio = qobject_cast<const Form::IFormIO*>(this);
                Form::IFormIO *iformio = const_cast<Form::IFormIO*>(const_iformio);
                desc->setIoFormReader(iformio);
                toReturn.append(desc);
                return toReturn;
            }
        }
        return toReturn;
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // Get all form files
    if (query.typeOfForms() & Form::FormIOQuery::CompleteForms) {
        QStringList path;
        path << settings()->path(Core::ISettings::CompleteFormsPath);
        path << settings()->path(Core::ISettings::UserCompleteFormsPath);
        path << settings()->path(Core::ISettings::DataPackCompleteFormsInstallPath);
        foreach(const QString &startPath, path) {
            QDir start(startPath);
            // get all forms included in this path
            foreach(const QFileInfo &file, Utils::getFiles(start, "central.xml", Utils::Recursively)) {
                const QString &fileName = file.absoluteFilePath();
                XmlFormName &form = formName(fileName, m_FormNames);
                if (includedUids.contains(form.uid))
                    continue;

                if (canReadForms(fileName)) {
                    Form::FormIODescription *desc = reader()->readFileInformation(form);
                    if (desc) {
                        desc->setData(Form::FormIODescription::IsCompleteForm, true);
                        toReturn.append(desc);
                    }
                }
            }
        }
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (query.typeOfForms() & Form::FormIOQuery::SubForms) {
        QStringList path;
        path << settings()->path(Core::ISettings::SubFormsPath);
        path << settings()->path(Core::ISettings::UserSubFormsPath);
        path << settings()->path(Core::ISettings::DataPackSubFormsInstallPath);
        foreach(const QString &startPath, path) {
            QDir start(startPath);
            foreach(const QFileInfo &file, Utils::getFiles(start, "central.xml", Utils::Recursively)) {
                const QString &fileName = file.absoluteFilePath();
                XmlFormName &form = formName(fileName, m_FormNames);
                if (includedUids.contains(form.uid))
                    continue;

                if (canReadForms(fileName)) {
                    Form::FormIODescription *desc = reader()->readFileInformation(form);
                    if (desc) {
                        desc->setData(Form::FormIODescription::IsSubForm, true);
                        toReturn.append(desc);
                    }
                }
            }
        }
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    checkFormIODescription(toReturn, query, this);
    return toReturn;
}

QList<Form::FormMain *> XmlFormIO::loadAllRootForms(const QString &uuidOrAbsPath) const
{
    XmlFormName &form = formName(uuidOrAbsPath, m_FormNames);
//    XmlFormName form(uuidOrAbsPath);
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
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
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
    if (!base()->isFormExists(form)) {
        base()->saveForm(form);
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QHash<QString, QString> mode_contents = base()->getAllFormFullContent(form.uid);

    reader()->refreshPluginFactories();

    QHashIterator<QString, QString> it(mode_contents);
    while (it.hasNext()) {
        it.next();
        Form::FormMain *root = m_ActualForm = new Form::FormMain;
        root->setModeUniqueName(it.key());
        root->setUuid(form.uid);
        root->setIoFormReader(const_cast<XmlFormIO*>(this));
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
        mode.modeName = it.key();
//        qWarning() << "MODE" << mode.absFileName << mode.uid;
        if (!reader()->loadForm(mode, root)) {
            LOG_ERROR("Form not readable: " + fakeFileName);
        } else {
            toReturn.append(root);
        }
        // Emit the formLoaded signal for all subForms loaded
        QList<Form::FormMain *> forms = root->flattenedFormMainChildren();
        for(int i=0; i < forms.count(); ++i) {
            forms.at(i)->emitFormLoaded();
        }
    }
    return toReturn;
}

bool XmlFormIO::loadPmhCategories(const QString &uuidOrAbsPath) const
{
    Q_UNUSED(uuidOrAbsPath);
    // TODO: remove dead code
//    XmlFormName &form = formName(uuidOrAbsPath, m_FormNames);
//    WARN_FUNC << uuidOrAbsPath << form;
//    pmhCore()->pmhCategoryModel()->setRootFormUid(form.uid);
//    pmhCore()->pmhCategoryModel()->refreshFromDatabase();
//    LOG("--------------------------- PMH Category retrieved");
    return true;
}

QList<QPixmap> XmlFormIO::screenShots(const QString &uuidOrAbsPath) const
{
    return base()->getScreenShots(uuidOrAbsPath).values();
}

QPixmap XmlFormIO::screenShot(const QString &uuidOrAbsPath, const QString &name) const
{
    return base()->getScreenShot(uuidOrAbsPath, name);
}

QString XmlFormIO::extractFileToTmpPath(const QString &uuidOrAbsPath, const QString &name) const
{
    // Get the extension of the requested file
    QFileInfo info(name);
    int type = XmlIOBase::ExtraFiles;
    if (info.suffix().compare("pdf", Qt::CaseInsensitive) == 0) {
        type = XmlIOBase::PdfFile;
    } else if ((info.suffix().compare("html", Qt::CaseInsensitive) == 0)
               || (info.suffix().compare("htm", Qt::CaseInsensitive) == 0)) {
        type = XmlIOBase::HtmlFile;
    }

    // Get the file content
    QString content = base()->getFormContent(uuidOrAbsPath, type, name);
    if (content.isEmpty())
        return QString::null;

    // Create output absolutefilepath
    QString output = settings()->path(Core::ISettings::ApplicationTempPath) + QDir::separator();
    output += XmlIOBase::createUid() + "." + info.suffix();

    // Save the content to a tmp path
    if (type == XmlIOBase::PdfFile) {
        QByteArray save = QByteArray::fromBase64(content.toAscii());
        QFile file(output);
        if (!file.open(QFile::WriteOnly)) {
            LOG_ERROR("Unable to open file: " + output);
            return QString::null;
        }
        file.write(save);
        file.close();
    } else {
        if (!Utils::saveStringToFile(content, output, Utils::Overwrite, Utils::DontWarnUser)) {
            LOG_ERROR("Unable to open file: " + output);
            return QString::null;
        }
    }
    return output;
}

bool XmlFormIO::saveForm(const QString &uuidOrAbsPath)
{
    XmlFormName form(uuidOrAbsPath);
    return base()->saveForm(form);
}

void XmlFormIO::checkForUpdates() const
{
    reader()->clearCache();
    checkDatabaseFormFileForUpdates();
    alreadyCheckedForUpdates = true;
}

const QList<Form::FormIODescription> &XmlFormIO::availableUpdates() const
{
    if (!alreadyCheckedForUpdates)
        checkForUpdates();
    return m_FormUpdatesList;
}

bool XmlFormIO::updateForms()
{
    if (!alreadyCheckedForUpdates)
        checkForUpdates();
    if (formsToUpdate.empty())
        return false;

    foreach(XmlFormName form, formsToUpdate) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if (!base()->saveForm(form)) {
            LOG_ERROR("Unable to update form database. Form: " + form.uid + " " + form.absFileName);
        } else {
            LOG("Form updated: "  + form.uid + " " + form.absFileName);
        }
    }
    return true;
}

/** Check the database form version and try to update them with the local files. */
bool XmlFormIO::checkDatabaseFormFileForUpdates() const
{
    m_FormUpdatesList.clear();
    QList<Form::FormIODescription *> fileDescriptionList;
    QList<Form::FormIODescription *> dbDescriptionList;
    LOG("Checking for form update");

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // get all available descriptions from database
    Form::FormIOQuery querydb;
    querydb.setGetAllAvailableFormDescriptions(true);
    dbDescriptionList = base()->getFormDescription(querydb);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Test all database forms for an update and populate a list
    // iterate through all FormIO descriptions in database
    QStringList checkedFormUid;
    foreach(Form::FormIODescription *dbDescription, dbDescriptionList) {
        QString dbFormUid = dbDescription->data(Form::FormIODescription::UuidOrAbsPath).toString();
        if (checkedFormUid.contains(dbFormUid))
            continue;
        checkedFormUid.append(dbFormUid);

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        qDeleteAll(fileDescriptionList);
        fileDescriptionList.clear();

        Form::FormIOQuery query;
        query.setFormUuid(dbFormUid);
        query.setForceFileReading(true);

        // get current version number of a form in database
        Utils::VersionNumber dbVersion(dbDescription->data(Form::FormIODescription::Version).toString());

        // get file description list of that form and iterate through this list
        fileDescriptionList = getFormFileDescriptions(query);
        foreach(Form::FormIODescription *fileDescription, fileDescriptionList) {
            // check version number of forms in the file
            Utils::VersionNumber fileVersion(fileDescription->data(Form::FormIODescription::Version).toString());

            if (fileVersion.versionString() == "test" || fileVersion > dbVersion) {
                // update database
                XmlFormName &form = formName(fileDescription->data(Form::FormIODescription::UuidOrAbsPath).toString(), m_FormNames);
                if (!formsToUpdate.contains(form)) {
                    formsToUpdate << form;
                    fileDescription->setData(Form::FormIODescription::UpdateAvailable_OldVersion, dbVersion.versionString());
                    m_FormUpdatesList << Form::FormIODescription(*fileDescription);
                }
            }
        }
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Clear cache
    m_ReadableForms.clear();
    reader()->clearCache();

    // Clear pointers
    qDeleteAll(fileDescriptionList);
    fileDescriptionList.clear();
    qDeleteAll(dbDescriptionList);
    dbDescriptionList.clear();
    return true;
}
