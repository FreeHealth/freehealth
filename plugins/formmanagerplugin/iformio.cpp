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
 * \class Form::FormIOQuery
 * Use this class to create specific queries in the Form::IFormIO engines.
 */

/*!
 * \fn void Form::FormIOQuery::setExcludeGenderSpecific(bool excludeGenderSpecific)
 * \brief Sets GenderSpecific filter for form searches.
 *
 * Some forms are gender specific, by setting this property to \e true, all form gender information will be checked
 * and excluded if the gender is different to the current patient gender. If set to \e false, gender is not checked.
 * By default, this property is set to false.
 */

/*!
 * \fn void Form::FormIOQuery::setIncludeUserDocumentForms(const bool includeUserDoc)
 * Includes users' form files stored in the UserCompleteFormsPath, UserSubFormsPath. \n
 * By default, this property is set to true.
 * \sa Form::FormIOQuery::includeUserDocumentForms()
 */


/**
 * \class Form::FormIODescription
 * Description of a form extracted from a Form::IFormIO engine. This is different than
 * Form::FormItemSpec. This description only handles information from the IO engine, not usable in
 * forms view/model computation.
 * The description contains informations about the Form name, category, author, version, FreeMedForms compatibility version,
 * country, and many other things. You can read this information very convenient through this FormIODescription class, using
 * the data() method with the right DataRepresentation parameters.
 */

/**
 * \class Form::IFormIO
 * This object interface is the in/out form object. It is used to read and create the patient files.\n
 * All Form::IFormIO engines are in charge of :
 * - correctly manage form uids in a persistent in time format
 * - reading and interpreting raw source form files
 * - retrieving all informations on Form::FormItem like specs, scripts, translations, full description...
 * - creating the forms QObject tree. The root parent is an empty Form::FormMain.
 * - preparing the forms widgets using the Form::IFormWidgetFactory objects stored in the
 *   plugin manager object pool
 * - store raw source into a specific user database (to ensure that anyone in the network can create the form)
 * The engines must not :\n
 * - create a cache of Form::FormMain objects. This cache is managed by the Form::FormManager
*/

/** \fn virtual QString Form::IFormIO::name() const
  Return the name of Form IO engine.
*/

/** \fn virtual void Form::IFormIO::muteUserWarnings(bool state);
  For debugging purpose, mute/unmute console warnings.
*/

/** \fn virtual  bool Form::IFormIO::canReadForms(const QString &uuidOrAbsPath) const
 * Return \e true if the IO engine can read the form identified by its uid (persistent in time).
 * \sa Form::FormIODescription::data(), Form::FormIODescription::UuidOrAbsPath
*/

/** \fn virtual FormIODescription *Form::IFormIO::readFileInformation(const QString &uuidOrAbsPath) const = 0;
  * If the form (identified by its persistent in time uuid) can be read by this object, return its
  * Form::FormIODescription or a null pointer in case of an error.
*/

/** \fn virtual QList<FormIODescription *> Form::IFormIO::getFormFileDescriptions(const FormIOQuery &query) const = 0;
  * If forms (identified by its persistent in time uuid) can be read by this object, return the
  * Form::FormIODescription list corresponding to the Form::FormIOQuery.
*/

/** \fn virtual QList<Form::FormMain *> Form::IFormIO::loadAllRootForms(const QString &uuidOrAbsPath = QString::null) const = 0;
  * Return the list of Form::FormMain * empty root object corresponding to the persistent in time
  * form uuid \e uuidOrAbsPath.\n
  * The uuid can be any of the following:
  * - a string uuid
  * - a absolute path to a file. The engine is in charge of computing the corresponding persistent in time uuid.
*/

/** \fn virtual bool Form::IFormIO::loadPmhCategories(const QString &uuidOrAbsPath) const = 0;
  * Load the categories to use in the PMH tree model and inform the PMH::PmhCategoryModel.
  * Return true in case of success, false in case of failure.
  * \sa PMH::PmhCore::pmhCategoryModel(), PMH::PmhCategoryModel
*/

/** \fn virtual QList<QPixmap> Form::IFormIO::screenShots(const QString &uuidOrAbsPath) const = 0;
  * Return all the screenshot linked to the form identified by the persistent in time uuid
  * \e uuidOrAbsPath
*/

/** \fn virtual QPixmap Form::IFormIO::screenShot(const QString &uuidOrAbsPath, const QString &name) const = 0;
  * Return the screenshot linked to the form identified by the persistent in time uuid
  * \e uuidOrAbsPath with a filename like \e name
*/

/** \fn virtual QString Form::IFormIO::extractFileToTmpPath(const QString &uuidOrAbsPath, const QString &name) const = 0;
 * Extract a specific file from the form uuid \e uuidOrAbsPath and the filename \e name.
 * The file is extracted to the temporary path and the absolute full path is returned.
 * In case of an error the return QString is empty.\n
 * This can be used with the "extra" files of the forms like:
 * - pdf files
 * - html files
 * - icon files
 */

/** \fn virtual bool Form::IFormIO::saveForm(const QString &uuidOrAbsPath) = 0;
  * Requires the IO plugin to save the form \e uuidOrAbsPath to its
  * internal database (if it manages a database). \n
  * Returns \e true if the process succeeded
*/

/** \fn virtual QString Form::IFormIO::lastError() const = 0;
  * Return the last error message of the engine.
*/

/** \fn virtual void Form::IFormIO::checkForUpdates() const = 0;
  * Check all stored form for any update. You can get all form that needs an update (with
  * all the update information) using the Form::IFormIO::availableUpdates() member.
*/

/** \fn virtual const QList<Form::FormIODescription> &Form::IFormIO::availableUpdates() const = 0;
 * Return the form description that needs an update. This list is built after calling the
 * Form::IFormIO::checkForUpdates().
 */

/** \fn virtual void toTreeWidget(QTreeWidget *tree, const QString &lang = Trans::Constants::ALL_LANGUAGE) const;
 * If the file is accessible to the IO device and is readable, read its description and populate a
 * QTreeWidget with it. Form::IFormIO::canReadFile() must be called first.
*/

#include "iformio.h"

#include <utils/versionnumber.h>
#include <utils/genericupdateinformation.h>
#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_msgerror.h>

#include <QDate>
#include <QDebug>

using namespace Form;
using namespace Trans::ConstantTranslations;

/**
 * Construct a default query:
 * complete forms,
 * do not get all available forms & descriptions from database
 * do not force local files reading,
 * do not exclude gender specific forms,
 * (include user doc)
 */
FormIOQuery::FormIOQuery() :
    m_type(CompleteForms),
    m_ForceFile(false),
    m_AllForms(false),
    m_AllDescr(false),
    m_ExcludeGenderSpecific(false),
    m_includeUserDoc(true)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  FormIODescription /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FormIODescription::FormIODescription() :
    Utils::GenericDescription()
{
    addNonTranslatableExtraData(GenderLimitation, "genderlimitation");
    setData(FromDatabase, false);
}

FormIODescription::~FormIODescription()
{
}

void FormIODescription::setIoFormReader(IFormIO *reader)
{
    m_reader=reader;
}

IFormIO *FormIODescription::reader() const
{
    return m_reader;
}

QVariant FormIODescription::data(const int ref, const QString &lang) const
{
    if (ref == TypeName) {
        if (data(IsCompleteForm).toBool()) {
            return QCoreApplication::translate("FormIODescription", "Complete form");
        } else if (data(IsSubForm).toBool()) {
            return QCoreApplication::translate("FormIODescription", "Sub-form");
        } else if (data(IsPage).toBool()) {
            return QCoreApplication::translate("FormIODescription", "Page only");
        }
        return QVariant();
    }
    return Utils::GenericDescription::data(ref, lang);
}

bool FormIODescription::setData(const int ref, const QVariant &value, const QString &lang)
{
    return Utils::GenericDescription::setData(ref, value, lang);
}

bool FormIODescription::hasScreenShots() const
{
    return data(HasScreenShot).toBool();
}

/** Returns all the screenshots available for the current language */
QList<QPixmap> FormIODescription::screenShots() const
{
    return m_reader->screenShots(data(UuidOrAbsPath).toString());
}

void FormIODescription::fillTreeWidget(QTreeWidget *tree) const
{
    // TODO: code here
    tree->clear();
    tree->setColumnCount(2);
    QFont bold;
    bold.setBold(true);

    QTreeWidgetItem *general = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::INFORMATION));
    general->setFont(0, bold);
    new QTreeWidgetItem(general, QStringList() << "Uuid" << data(FormIODescription::UuidOrAbsPath).toString());
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::AUTHOR) << data(FormIODescription::Author).toString());
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::LICENSE) << data(FormIODescription::LicenseName).toString());
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::DESCRIPTION) << data(FormIODescription::ShortDescription).toString());
    new QTreeWidgetItem(general, QStringList() << QString("Extracted from database") << data(FormIODescription::FromDatabase).toString());

    QTreeWidgetItem *version = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::VERSION));
    version->setFont(0, bold);
    new QTreeWidgetItem(version, QStringList() << tkTr(Trans::Constants::VERSION) << data(FormIODescription::Version).toString());
    new QTreeWidgetItem(version, QStringList() << QCoreApplication::translate("Forms", "Creation date") << data(FormIODescription::CreationDate).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)));
    if (!data(FormIODescription::LastModificationDate).isNull())
        new QTreeWidgetItem(version, QStringList() << QCoreApplication::translate("Forms", "Last modification date") << data(FormIODescription::LastModificationDate).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)));

    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}

QString FormIODescription::toHtml() const
{
    QString html;
    // Information
    html += QString("<p style=\"font-weight:bold;font-size:large;\">%1</p>")
            .arg(data(FormIODescription::ShortDescription).toString());
    // Author and version
    // FIXME: use tkTr(Trans::Constants::_1_COLON_2) "%1: %2" translation
    html += QString("<p style=\"margin-left:20px;font-size:small;color:darkgray\">"
                    "%1: %2<br/>"
                    "%3: %4<br/>"
                    "%5: %6<br/>"
                    "%7: %8<br/>"
                    "%9: %10</p>")
            .arg(tkTr(Trans::Constants::AUTHOR))
            .arg(data(FormIODescription::Author).toString())
            .arg(tkTr(Trans::Constants::VERSION))
            .arg(data(FormIODescription::Version).toString())
            .arg(QCoreApplication::translate("Forms", "Creation date"))
            .arg(data(FormIODescription::CreationDate).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)))
            .arg(QCoreApplication::translate("Forms", "Last modification date"))
            .arg(data(FormIODescription::LastModificationDate).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)))
            .arg(tkTr(Trans::Constants::LICENSE))
            .arg(data(FormIODescription::LicenseName).toString())
            ;
    // Long description
    const QString &hmltDescr = data(FormIODescription::HtmlDescription).toString();
    if (!hmltDescr.isEmpty())
    html += QString("<p style=\"margin:0px;\">%1</p><br/><br/>").arg(hmltDescr);
    if (hasScreenShots()) {
        html += QString("<span style=\"margin-left:20px;font-size:small;color:darkgray\">%1</span><br/><br/>")
                .arg(QCoreApplication::translate("Forms", "ScreenShots are available."));
    }
//    new QTreeWidgetItem(general, QStringList() << QString("Extracted from database") << data(FormIODescription::FromDatabase).toString());

    return html;
}

QDebug operator<<(QDebug dbg, const Form::FormIODescription &c)
{
    QString tmp = "FormIODescription(";
    QStringList attribs;
    if (c.data(Form::FormIODescription::IsCompleteForm).toBool())
        attribs << "isComplete";
    else if (c.data(Form::FormIODescription::IsSubForm).toBool())
        attribs << "isSub";
    else if (c.data(Form::FormIODescription::IsPage).toBool())
        attribs << "isPage";
    if (c.reader())
        attribs << "reader: " + c.reader()->name();
    attribs << "uid: " + c.data(Form::FormIODescription::UuidOrAbsPath).toString();
    attribs << "author: " + c.data(Form::FormIODescription::Author).toString();
    attribs << "country: " + c.data(Form::FormIODescription::Country).toString();
//    attribs << "languages: " + c.data(Form::FormIODescription::AvailableLanguages).toString();
    attribs << "v: " + c.data(Form::FormIODescription::Version).toString();
    attribs << "compat: " + c.data(Form::FormIODescription::EHRCompatVersion).toString();
    attribs << "cdate: " + c.data(Form::FormIODescription::CreationDate).toString();
    attribs << "ludate: " + c.data(Form::FormIODescription::LastModificationDate).toString();
    attribs << "cat: " + c.data(Form::FormIODescription::Category).toString();
    attribs << "spe: " + c.data(Form::FormIODescription::Specialties).toString();
    attribs << "license: " + c.data(Form::FormIODescription::LicenseName).toString();
    attribs << "icon: " + c.data(Form::FormIODescription::GeneralIcon).toString();
    attribs << "web: " + c.data(Form::FormIODescription::WebLink).toString();
    attribs << "shotpath: " + c.data(Form::FormIODescription::ScreenShotsPath).toString();
    attribs << c.data(Form::FormIODescription::ShortDescription).toString();

    dbg.nospace() << tmp + attribs.join("; ");
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Form::FormIODescription *c)
{
    if (!c) {
        dbg.nospace() << "FormIODescription(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}

