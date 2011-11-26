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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "genericdescription.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/genericupdateinformation.h>
#include <translationutils/multilingualclasstemplate.h>

#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QString>

#include <QDebug>

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {

const char * const XML_ROOT_TAG        = "FullDescription";

const char* const ATTRIB_LANG          = "lang";

const char* const TAG_FMF_COMPAT_VERSION   = "fmfv";
const char* const TAG_FD_COMPAT_VERSION   = "fdv";
const char* const TAG_FA_COMPAT_VERSION   = "fav";

const char* const TAG_VERSION          = "version";
const char* const TAG_WEBLINK          = "weblink";
const char* const TAG_ICON             = "icon";
const char* const TAG_TOOLTIP          = "tooltip";
const char* const TAG_EXTRA            = "extra";
const char* const TAG_AUTHORS          = "authors";
const char* const TAG_VENDOR           = "vendor";
const char* const TAG_VALIDITY         = "validity";
const char* const TAG_CATEGORY         = "category";
const char* const TAG_LICENSE          = "licence";
const char* const TAG_SPECIALTIES      = "specialties";
const char* const TAG_CREATIONDATE     = "cdate";
const char* const TAG_LASTMODIFDATE    = "lmdate";
const char* const TAG_BIBLIOGRAPHY     = "bibliography";
const char* const TAG_DESCRIPTION      = "description";
const char* const TAG_HTMLDESCRIPTION  = "htmldescription";
const char* const TAG_HTMLSYNTHESIS    = "htmlsynthesis";
const char* const TAG_LABEL            = "label";
const char* const TAG_PLUGINNAME       = "type";

}

namespace Utils {
namespace Internal {
class DescriptionBook
{
public:
    QHash<int, QVariant> m_Datas;
};

class GenericDescriptionPrivate : public Trans::MultiLingualClass<DescriptionBook>
{
public:
    GenericDescriptionPrivate() {}
    ~GenericDescriptionPrivate() {}

    QHash<int, QString> nonTranslatableTagsDataReference()
    {
        QHash<int, QString> elements;
        // get non translatable items
        elements.insert(GenericDescription::Version, TAG_VERSION);
        elements.insert(GenericDescription::Author, TAG_AUTHORS);
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

    QHash<int, QString> translatableTagsDataReference()
    {
        QHash<int, QString> elements;
        elements.insert(GenericDescription::Category, TAG_CATEGORY);
        elements.insert(GenericDescription::ShortDescription, TAG_DESCRIPTION);
        elements.insert(GenericDescription::HtmlDescription, TAG_HTMLDESCRIPTION);
        elements.insert(GenericDescription::HtmlSynthesis, TAG_HTMLSYNTHESIS);
        elements.insert(GenericDescription::GlobalLicense, TAG_LICENSE);
        elements.insert(GenericDescription::WebLink, TAG_WEBLINK);
        // add extra tags
        QHashIterator<int, QString> i(m_TranslatableExtra);
        while (i.hasNext()) {
            i.next();
            elements.insert(i.key(), i.value());
        }
        return elements;
    }

    QList<Utils::GenericUpdateInformation> m_UpdateInfos;
    QString m_RootTag;
    QHash<int, QString> m_TranslatableExtra, m_NonTranslatableExtra;

};
}  // End namespace Internal
}  // End namespace Utils


GenericDescription::GenericDescription(const QString &rootTag) :
    d(new GenericDescriptionPrivate)
{
    if (d->m_RootTag.isEmpty())
        d->m_RootTag = XML_ROOT_TAG;
}

GenericDescription::~GenericDescription()
{
    if (d)
        delete d;
    d = 0;
}

QVariant GenericDescription::data(const int ref, const QString &lang) const
{
    QString l = lang;
    if (lang.isEmpty()) {
        l = QLocale().name().left(2);
        DescriptionBook *book = d->getLanguage(l);
        if (book) {
            QVariant val = book->m_Datas.value(ref, QVariant());
            if (!val.isNull())
                return val;
        }
        l = Trans::Constants::ALL_LANGUAGE;
        book = d->getLanguage(l);
        if (book) {
            QVariant val = book->m_Datas.value(ref, QVariant());
            if (!val.isNull())
                return val;
        }
    }
    DescriptionBook *book = d->getLanguage(l);
    if (book) {
        QVariant val = book->m_Datas.value(ref, QVariant());
        if (!val.isNull())
            return val;
    }
    return QVariant();
}

bool GenericDescription::setData(const int ref, const QVariant &value, const QString &lang)
{
    QString l = lang;
    if (lang.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    DescriptionBook *book = d->createLanguage(l);
    book->m_Datas.insert(ref, value);
    return true;
}

/** Populate the description using a xml content \e xmlContent. */
bool GenericDescription::fromXmlContent(const QString &xmlContent)
{
    d->clear();
    if (xmlContent.isEmpty())
        return true;

    QDomDocument doc;
    if (!doc.setContent(xmlContent)) {
        LOG_ERROR_FOR("GenericDescription", "Wrong XML");
        return false;
    }
    QDomNodeList els = doc.elementsByTagName(d->m_RootTag);
    if (els.count() == 0) {
        LOG_ERROR_FOR("GenericDescription", "Wrong XML. No root tag: " + d->m_RootTag);
        return false;
    }
    QDomElement root = els.at(0).toElement();

    QHash<int, QString> elements;
    elements = d->nonTranslatableTagsDataReference();
    QHashIterator<int, QString> i(elements);
    while (i.hasNext()) {
        i.next();
        setData(i.key(), root.firstChildElement(i.value()).text());
    }

    elements = d->translatableTagsDataReference();
    i = elements;
    while (i.hasNext()) {
        i.next();
        QDomElement desc = root.firstChildElement(i.value());
        while (!desc.isNull()) {
            setData(i.key(), desc.text(), desc.attribute(::ATTRIB_LANG, Trans::Constants::ALL_LANGUAGE));
            desc = desc.nextSiblingElement(i.value());
        }
    }

    // read update informations
    QDomElement update = root.firstChildElement(GenericUpdateInformation::xmlTagName());
    d->m_UpdateInfos.clear();
    d->m_UpdateInfos = Utils::GenericUpdateInformation::fromXml(update);
    return true;
}

/** Populate the description using a specific local file \e absFileName. */
bool GenericDescription::fromXmlFile(const QString &absFileName)
{
    // Read file content
    QString xml = Utils::readTextFile(absFileName, Utils::DontWarnUser);
    if (xml.isEmpty())
        return false;
    return fromXmlContent(xml);
}


/** Transform the actual description into XML. */
QString GenericDescription::toXml() const
{
    QDomDocument doc;
    QHash<int, QString> elements;

    // Create the main description tag
    QDomElement root = doc.createElement(d->m_RootTag);
    doc.appendChild(root);

    QDomComment comment = doc.createComment("Non translatable values");
    root.appendChild(comment);

    // Set non translatable items
    elements = d->nonTranslatableTagsDataReference();
    QHashIterator<int, QString> i(elements);
    while (i.hasNext()) {
        i.next();
        QDomElement e = doc.createElement(i.value());
        root.appendChild(e);
        e.setNodeValue(data(i.key()).toString());
        QDomText t = doc.createTextNode(data(i.key()).toString());
        e.appendChild(t);
    }

    // Set translatable contents
    comment = doc.createComment("Translatable values");
    root.appendChild(comment);
    elements.clear();
    elements = d->translatableTagsDataReference();
    i = elements;
    while (i.hasNext()) {
        i.next();
        foreach(const QString &l, d->languages()) {
            const QString &tmp = data(i.key(), l).toString();
            if (tmp.isEmpty())
                continue;
            QDomElement e = doc.createElement(i.value());
            e.setAttribute(ATTRIB_LANG, l);
            root.appendChild(e);
            QDomText t = doc.createTextNode(tmp);
            e.appendChild(t);
        }
    }

    // Add update informations
    if (d->m_UpdateInfos.count() > 0) {
        comment = doc.createComment("Update information");
        root.appendChild(comment);
        QDomElement e = doc.createElement(GenericUpdateInformation::xmlTagName());
        root.appendChild(e);
        for(int i=0; i < d->m_UpdateInfos.count(); ++i) {
            d->m_UpdateInfos.at(i).toDomElement(&e, &doc);
        }
    }
    return doc.toString(2);
}

void GenericDescription::addUpdateInformation(Utils::GenericUpdateInformation updateInfo)
{
    d->m_UpdateInfos.append(updateInfo);
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformation() const
{
    return d->m_UpdateInfos;
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformationForVersion(const QString &version) const
{}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformationForVersion(const Utils::VersionNumber &version) const
{}

/** You can add untranslatable extra-data. \e ref must be greated than GenericDescription::NonTranslatableExtraData and lower than GenericDescription::TranslatableExtraData. */
void GenericDescription::addNonTranslatableExtraData(const int ref, const QString &tagName)
{
    Q_ASSERT(ref>NonTranslatableExtraData && ref<TranslatableExtraData);
    if (ref>NonTranslatableExtraData && ref<TranslatableExtraData) {
        d->m_NonTranslatableExtra.insert(ref, tagName);
    }
}

/** You can add translatable extra-data. \e ref must be greated than GenericDescription::TranslatableExtraData. */
void GenericDescription::addTranslatableExtraData(const int ref, const QString &tagName)
{
    Q_ASSERT(ref>TranslatableExtraData);
    if (ref>TranslatableExtraData) {
        d->m_TranslatableExtra.insert(ref, tagName);
    }
}

void GenericDescription::toTreeWidget(QTreeWidget *tree) const
{}
