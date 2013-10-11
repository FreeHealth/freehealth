/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Utils::GenericDescription
 * \note Unit-test available (see: tests/auto/auto.pro)
 */
// TODO: write code documentation

#include "genericdescription.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/versionnumber.h>
#include <utils/genericupdateinformation.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QString>
#include <QDir>

#include <QDebug>

using namespace Utils;
//using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {

const char * const XML_ROOT_TAG        = "FullDescription";

const char* const ATTRIB_LANG          = "lang";

const char* const TAG_FMF_COMPAT_VERSION   = "fmfv";
const char* const TAG_FD_COMPAT_VERSION   = "fdv";
const char* const TAG_FA_COMPAT_VERSION   = "fav";

const char* const TAG_UUID             = "uuid";
const char* const TAG_VERSION          = "version";
const char* const TAG_WEBLINK          = "weblink";
const char* const TAG_ICON             = "icon";
const char* const TAG_TOOLTIP          = "tooltip";
const char* const TAG_EXTRA            = "extra";
const char* const TAG_AUTHORS          = "authors";
const char* const TAG_COUNTRY          = "country";
const char* const TAG_URL              = "url";
const char* const TAG_VENDOR           = "vendor";
const char* const TAG_VALIDITY         = "validity";
const char* const TAG_CATEGORY         = "category";
const char* const TAG_LICENSE_NAME     = "license";
const char* const TAG_LICENSE_TERMS    = "licenseterms";
const char* const TAG_SPECIALTIES      = "specialties";
const char* const TAG_TYPENAME         = "typename";
const char* const TAG_CREATIONDATE     = "cdate";
const char* const TAG_LASTMODIFDATE    = "lmdate";
const char* const TAG_BIBLIOGRAPHY     = "bibliography";
const char* const TAG_DESCRIPTION      = "description";
const char* const TAG_HTMLDESCRIPTION  = "htmldescription";
const char* const TAG_HTMLSYNTHESIS    = "htmlsynthesis";
const char* const TAG_LABEL            = "label";
const char* const TAG_PLUGINNAME       = "type";
const char* const TAG_ABSFILENAME      = "file";

}

/** Create an empty valid object. \e rootTag defines the XML Root Tag for the description block, leave empty to use the default one. */
GenericDescription::GenericDescription(const QString &rootTag)
{
    m_RootTag = rootTag;
    if (m_RootTag.isEmpty())
        m_RootTag = XML_ROOT_TAG;
}

GenericDescription::~GenericDescription()
{
}

/** Define the XML Root Tag for the description block. */
void GenericDescription::setRootTag(const QString &rootTag)
{
    m_RootTag = rootTag;
    if (m_RootTag.isEmpty())
        m_RootTag = XML_ROOT_TAG;
}

/** Define the absolute path to the source file used to create this description. */
void GenericDescription::setSourceFileName(const QString &absFileName)
{
    m_SourceFileName = QDir::cleanPath(absFileName);
}

/** Return the absolute path to the source file used to create this description. */
QString GenericDescription::sourceFileName() const
{
    return m_SourceFileName;
}

/** Clear all data but rootTag */
void GenericDescription::clear()
{
    m_UpdateInfos.clear();
    m_TranslatableExtra.clear();
    m_NonTranslatableExtra.clear();
    m_Data.clear();
}

/** Return the data of the description. \sa addNonTranslatableExtraData(), addTranslatableExtraData() */
QVariant GenericDescription::data(const int ref, const QString &lang) const
{
    QVariant var;
    QString l = lang;
    if (lang.isEmpty()) {
        l = QLocale().name().left(2).toLower();
        if (m_Data.keys().contains(l))
            var = m_Data.value(l).value(ref);
        if (var.isNull()) {
            l = Trans::Constants::ALL_LANGUAGE;
            if (m_Data.keys().contains(l))
                var = m_Data.value(l).value(ref);
        }
    }
    if (var.isNull())
        if (m_Data.keys().contains(l))
            var = m_Data.value(l).value(ref);
    return var;
}

/**
 * Define the data of the description. \n
 * All QDateTime milliseconds are set to zero to ensure a perfect compatibility with
 * Qt::ISODate used in the XML transformation.
 * \sa addNonTranslatableExtraData(), addTranslatableExtraData()
 */
bool GenericDescription::setData(const int ref, const QVariant &value, const QString &lang)
{
    QString l = lang.toLower();
    if (lang.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    // Remove ms to any QDateTime
    if (value.canConvert(QVariant::DateTime)) {
        QDateTime dt = value.toDateTime();
        if (dt.time().msec() > 0) {
            dt = QDateTime(dt.date(), QTime(dt.time().hour(), dt.time().minute(), dt.time().second()));
            m_Data[l].insert(ref, dt);
        } else {
            m_Data[l].insert(ref, value);
        }
    } else {
        m_Data[l].insert(ref, value);
    }
    return true;
}

/** Return all languages with a translable value. */
QStringList GenericDescription::availableLanguages() const
{
    return m_Data.uniqueKeys();
}

/** Populate the description using a xml content \e xmlContent. */
bool GenericDescription::fromXmlContent(const QString &xmlContent)
{
    m_Data.clear();
    if (xmlContent.isEmpty())
        return true;

    QDomDocument doc;
    QString error;
    int line, col;
    if (!doc.setContent(xmlContent, &error, &line, &col)) {
        LOG_ERROR_FOR("GenericDescription", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
        return false;
    }
    QDomNodeList els = doc.elementsByTagName(m_RootTag);
    if (els.count() == 0) {
        LOG_ERROR_FOR("GenericDescription", "Wrong XML. No root tag: " + m_RootTag);
        return false;
    }
    QDomElement root = els.at(0).toElement();
    return fromDomElement(root);
}

/** Populate the description using a specific local file \e absFileName. */
bool GenericDescription::fromXmlFile(const QString &absFileName)
{
    // Read file content
    m_SourceFileName.clear();
    QString xml = Utils::readTextFile(absFileName, Utils::DontWarnUser);
    if (xml.isEmpty())
        return false;
    m_SourceFileName = QDir::cleanPath(absFileName);
    return fromXmlContent(xml);
}

bool GenericDescription::fromDomElement(const QDomElement &root)
{
    if (root.tagName().compare(m_RootTag, Qt::CaseInsensitive)!=0) {
        LOG_ERROR_FOR("GenericDescription", "Wrong XML. No root tag: " + m_RootTag);
        return false;
    }
    QHash<int, QString> elements;
    elements = nonTranslatableTagsDataReference();
    QHashIterator<int, QString> i(elements);
    while (i.hasNext()) {
        i.next();
        setData(i.key(), root.firstChildElement(i.value()).text());
    }

    elements = translatableTagsDataReference();
    i = elements;
    while (i.hasNext()) {
        i.next();
        QDomElement desc = root.firstChildElement(i.value());
        while (!desc.isNull()) {
            setData(i.key(), desc.text(), desc.attribute(::ATTRIB_LANG, Trans::Constants::ALL_LANGUAGE));
            desc = desc.nextSiblingElement(i.value());
        }
    }

    // read update information
    QDomElement update = root.firstChildElement(GenericUpdateInformation::xmlTagName());
    m_UpdateInfos.clear();
    m_UpdateInfos = Utils::GenericUpdateInformation::fromXml(update);
    return true;
}

/** Transform the actual description into XML. */
QString GenericDescription::toXml() const
{
    QDomDocument doc;
    // Create the main description tag
    QDomElement root = doc.createElement(m_RootTag);
    doc.appendChild(root);
    if (!toDomElement(&root, &doc)) {
        LOG_ERROR_FOR("GenericDescription", "Unable to generate XML code");
    }
    return doc.toString(2);
}

/** Transform the actual description into XML. */
bool GenericDescription::toDomElement(QDomElement *root, QDomDocument *doc) const
{
    Q_ASSERT(root);
    Q_ASSERT(doc);
    if (!root || !doc)
        return false;
    QDomElement toRoot = *root;
    if (root->tagName().compare(m_RootTag, Qt::CaseInsensitive) != 0) {
        QDomElement element = doc->createElement(m_RootTag);
        root->appendChild(element);
        toRoot = element;
    }
    QDomComment comment = doc->createComment("Non translatable values");
    toRoot.appendChild(comment);

    // Set non translatable items
    QHash<int, QString> elements;
    elements = nonTranslatableTagsDataReference();
    QHashIterator<int, QString> i(elements);
    while (i.hasNext()) {
        i.next();
        if (data(i.key()).toString().isEmpty())
            continue;
        QDomElement e = doc->createElement(i.value());
        toRoot.appendChild(e);
        e.setNodeValue(data(i.key()).toString());
        QDomText t = doc->createTextNode(data(i.key()).toString());
        e.appendChild(t);
    }

    // Set translatable contents
    comment = doc->createComment("Translatable values");
    toRoot.appendChild(comment);
    elements.clear();
    elements = translatableTagsDataReference();
    i = elements;
    while (i.hasNext()) {
        i.next();
        foreach(const QString &l, m_Data.keys()) {
            const QString &tmp = data(i.key(), l).toString();
            if (tmp.isEmpty())
                continue;
            QDomElement e = doc->createElement(i.value());
            e.setAttribute(ATTRIB_LANG, l);
            toRoot.appendChild(e);
            QDomText t = doc->createTextNode(tmp);
            e.appendChild(t);
        }
    }

    // Add update information
    if (m_UpdateInfos.count() > 0) {
        comment = doc->createComment("Update information");
        toRoot.appendChild(comment);
        QDomElement e = doc->createElement(GenericUpdateInformation::xmlTagName());
        toRoot.appendChild(e);
        for(int i=0; i < m_UpdateInfos.count(); ++i) {
            m_UpdateInfos.at(i).toDomElement(&e, doc);
        }
    }
    return true;
}

void GenericDescription::addUpdateInformation(const Utils::GenericUpdateInformation &updateInfo)
{
    m_UpdateInfos.append(updateInfo);
    qSort(m_UpdateInfos.begin(), m_UpdateInfos.end(), Utils::GenericUpdateInformation::greaterThan);
}

void GenericDescription::removeUpdateInformation(int index)
{
    if (index >= 0 && index < m_UpdateInfos.count()) {
        m_UpdateInfos.removeAt(index);
    }
}

void GenericDescription::insertUpdateInformation(int index, const Utils::GenericUpdateInformation &updateInfo)
{
    m_UpdateInfos.insert(index, updateInfo);
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformation() const
{
    return m_UpdateInfos;
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformationForVersion(const QString &version) const
{
    return Utils::GenericUpdateInformation::updateInformationForVersion(m_UpdateInfos, version);
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformationForVersion(const Utils::VersionNumber &version) const
{
    return Utils::GenericUpdateInformation::updateInformationForVersion(m_UpdateInfos, version);
}

QString GenericDescription::htmlUpdateInformationForVersion(const QString &version) const
{
    Utils::VersionNumber v(version);
    return htmlUpdateInformationForVersion(v);
}

QString GenericDescription::htmlUpdateInformationForVersion(const Utils::VersionNumber &version) const
{
    QString html;
    foreach(const Utils::GenericUpdateInformation &info, updateInformationForVersion(version)) {
        html += info.toHtml();
    }
    return html;
}

/** You can add untranslatable extra-data. \e ref must be greater than NonTranslatableExtraData and lower than TranslatableExtraData. */
void GenericDescription::addNonTranslatableExtraData(const int ref, const QString &tagName)
{
    Q_ASSERT(ref>NonTranslatableExtraData && ref<TranslatableExtraData);
    if (ref>NonTranslatableExtraData && ref<TranslatableExtraData) {
        m_NonTranslatableExtra.insert(ref, tagName);
    }
}

/** You can add translatable extra-data. \e ref must be greater than TranslatableExtraData. */
void GenericDescription::addTranslatableExtraData(const int ref, const QString &tagName)
{
    Q_ASSERT(ref>TranslatableExtraData);
    if (ref>TranslatableExtraData) {
        m_TranslatableExtra.insert(ref, tagName);
    }
}

///** Use inheritance to access to unknown QDomElement. This member is called each time a tagName element is unknown to the engine. */
//void GenericDescription::readUnknownElement(const QDomElement &element)
//{
//    // just add a warning
//    qWarning() << Q_FUNC_INFO << "trying to access to an unknown element: tag" << element.tagName();
//    qWarning() << "Use the Utils::GenericDescription inheritance to access to these data";
//}

void GenericDescription::fillTreeWidget(QTreeWidget *tree) const
{
    Q_UNUSED(tree);
}

/** Test equality between two descriptions. */
bool GenericDescription::operator==(const GenericDescription &other) const
{
    // First basic tests
    if (m_UpdateInfos.count() != other.m_UpdateInfos.count() ||
            m_RootTag != other.m_RootTag ||
            m_SourceFileName != other.m_SourceFileName ||
            m_TranslatableExtra.count() != other.m_TranslatableExtra.count() ||
            m_NonTranslatableExtra.count() != other.m_NonTranslatableExtra.count())
        return false;

    // Test extras
    if (m_TranslatableExtra != other.m_TranslatableExtra)
        return false;
    if (m_NonTranslatableExtra != other.m_NonTranslatableExtra)
        return false;

    // Test datas
    QStringList lang = m_Data.keys();
    QStringList other_lang = other.m_Data.keys();
    if (lang.count() != other_lang.count())
        return false;
    lang.sort();
    other_lang.sort();
    if (lang != other_lang)
        return false;

    foreach(const QString &l, lang) {
        const QHash<int, QVariant> &first = m_Data.value(l);
        const QHash<int, QVariant> &second = other.m_Data.value(l);
//         foreach(int id, first.keys()) {
//             if (first.value(id) != second.value(id))
//                 qWarning() << id << l << first.value(id) << second.value(id);
//         }
        if (first != second)
            return false;
    }

    // Test update infos /!\ lists must be sorted in the same order...
    // TODO: bool updatesAreEqual = (m_UpdateInfos == other.m_UpdateInfos);
    return true;
}

QHash<int, QString> GenericDescription::nonTranslatableTagsDataReference() const
{
    QHash<int, QString> elements;
    // get non translatable items
    elements.insert(GenericDescription::Uuid, TAG_UUID);
    elements.insert(GenericDescription::Version, TAG_VERSION);
    elements.insert(GenericDescription::Author, TAG_AUTHORS);
    elements.insert(GenericDescription::Country, TAG_COUNTRY);
    elements.insert(GenericDescription::URL, TAG_URL);
    elements.insert(GenericDescription::AbsFileName, TAG_ABSFILENAME);
    elements.insert(GenericDescription::Vendor, TAG_VENDOR);
    elements.insert(GenericDescription::Validity, TAG_VALIDITY);
    elements.insert(GenericDescription::FreeMedFormsCompatVersion, TAG_FMF_COMPAT_VERSION);
    elements.insert(GenericDescription::FreeDiamsCompatVersion, TAG_FD_COMPAT_VERSION);
    elements.insert(GenericDescription::FreeAccountCompatVersion, TAG_FA_COMPAT_VERSION);
    elements.insert(GenericDescription::CreationDate, TAG_CREATIONDATE);
    elements.insert(GenericDescription::LastModificationDate, TAG_LASTMODIFDATE);
    elements.insert(GenericDescription::GeneralIcon, TAG_ICON);
    // add extra tags
    QHashIterator<int, QString> i(m_NonTranslatableExtra);
    while (i.hasNext()) {
        i.next();
        elements.insert(i.key(), i.value());
    }
    return elements;
}

QHash<int, QString> GenericDescription::translatableTagsDataReference() const
{
    QHash<int, QString> elements;
    elements.insert(GenericDescription::Label, TAG_LABEL);
    elements.insert(GenericDescription::Category, TAG_CATEGORY);
    elements.insert(GenericDescription::ToolTip, TAG_TOOLTIP);
    elements.insert(GenericDescription::Specialties, TAG_SPECIALTIES);
    elements.insert(GenericDescription::TypeName, TAG_TYPENAME);
    elements.insert(GenericDescription::ShortDescription, TAG_DESCRIPTION);
    elements.insert(GenericDescription::HtmlDescription, TAG_HTMLDESCRIPTION);
    elements.insert(GenericDescription::HtmlSynthesis, TAG_HTMLSYNTHESIS);
    elements.insert(GenericDescription::LicenseName, TAG_LICENSE_NAME);
    elements.insert(GenericDescription::LicenseTerms, TAG_LICENSE_TERMS);
    elements.insert(GenericDescription::WebLink, TAG_WEBLINK);
    // add extra tags
    QHashIterator<int, QString> i(m_TranslatableExtra);
    while (i.hasNext()) {
        i.next();
        elements.insert(i.key(), i.value());
    }
    return elements;
}
