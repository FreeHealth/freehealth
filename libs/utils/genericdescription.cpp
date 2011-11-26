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

#include <utils/global.h>
#include <utils/genericupdateinformation.h>
#include <translationutils/multilingualclasstemplate.h>

#include <QDomDocument>
#include <QDomElement>

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
};
}  // End namespace Internal
}  // End namespace Utils


GenericDescription::GenericDescription(const QString &rootTag) :
    d(new GenericDescriptionPrivate),
    m_RootTag(rootTag)
{
    if (m_RootTag.isEmpty())
        m_RootTag = XML_ROOT_TAG;
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
    return false;
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


//XmlFormName form(formUid);
//Form::FormIODescription *ioDesc = new Form::FormIODescription;
//QHash<int, QString> elements;
//// get non translatable items
//elements.insert(Form::FormIODescription::Version, Constants::TAG_SPEC_VERSION);
//elements.insert(Form::FormIODescription::Author, Constants::TAG_SPEC_AUTHORS);
//elements.insert(Form::FormIODescription::CreationDate, Constants::TAG_SPEC_CREATIONDATE);
//elements.insert(Form::FormIODescription::LastModificationDate, Constants::TAG_SPEC_LASTMODIFDATE);
//elements.insert(Form::FormIODescription::GeneralIcon, Constants::TAG_SPEC_ICON);
//elements.insert(Form::FormIODescription::WebLink, Constants::TAG_SPEC_WEBLINK);
//elements.insert(Form::FormIODescription::FreeMedFormsCompatVersion, Constants::TAG_SPEC_COMPTAVERSION);
//QHashIterator<int, QString> i(elements);
//while (i.hasNext()) {
//    i.next();
//    ioDesc->setData(i.key(), xmlDescr.firstChildElement(i.value()).text());
//}
//// get translatable contents
//elements.clear();
//elements.insert(Form::FormIODescription::Category, Constants::TAG_SPEC_CATEGORY);
//elements.insert(Form::FormIODescription::ShortDescription, Constants::TAG_SPEC_DESCRIPTION);
//elements.insert(Form::FormIODescription::License, Constants::TAG_SPEC_LICENSE);
//elements.insert(Form::FormIODescription::Specialties, Constants::TAG_SPEC_SPECIALTIES);
//i = elements;
//while (i.hasNext()) {
//    i.next();
//    QDomElement desc = xmlDescr.firstChildElement(i.value());
//    while (!desc.isNull()) {
//        ioDesc->setData(i.key(), desc.text(), desc.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
//        desc = desc.nextSiblingElement(i.value());
//    }
//}
//// get translatable files
//elements.clear();
//elements.insert(Form::FormIODescription::HtmlDescription, Constants::TAG_SPEC_HTMLDESCRIPTION);
//elements.insert(Form::FormIODescription::HtmlSynthesis, Constants::TAG_SPEC_HTMLSYNTHESIS);
//i = elements;
//while (i.hasNext()) {
//    i.next();
//    QDomElement desc = xmlDescr.firstChildElement(i.value());
//    while (!desc.isNull()) {
//        QString content = readExtraFile(form, desc.text());
//        ioDesc->setData(i.key(), content, desc.attribute(Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
//        desc = desc.nextSiblingElement(i.value());
//    }
//}
//// read update informations
//QDomElement update = xmlDescr.firstChildElement(Constants::TAG_SPEC_UPDATEINFO);
//ioDesc->addUpdateInformation(Utils::GenericUpdateInformation::fromXml(update));
//setPathToDescription(formUid, ioDesc);


/** Transform the actual description into XML. */
QString GenericDescription::toXml() const
{
    QDomDocument doc;
    QHash<int, QString> elements;

    // Create the main description tag
    QDomElement root = doc.createElement(m_RootTag);
    doc.appendChild(root);

    QDomComment comment = doc.createComment("Non translatable values");
    root.appendChild(comment);

    // Set non translatable items
    elements.insert(Version, TAG_VERSION);
    elements.insert(Author, TAG_AUTHORS);
    elements.insert(Vendor, TAG_VENDOR);
    elements.insert(Validity, TAG_VALIDITY);
    elements.insert(FreeMedFormsCompatVersion, TAG_FMF_COMPAT_VERSION);
    elements.insert(FreeDiamsCompatVersion, TAG_FD_COMPAT_VERSION);
    elements.insert(FreeAccountCompatVersion, TAG_FA_COMPAT_VERSION);
    elements.insert(CreationDate, TAG_CREATIONDATE);
    elements.insert(LastModificationDate, TAG_LASTMODIFDATE);
    elements.insert(GeneralIcon, TAG_ICON);
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
    elements.insert(Category, TAG_CATEGORY);
    elements.insert(ShortDescription, TAG_DESCRIPTION);
    elements.insert(HtmlDescription, TAG_HTMLDESCRIPTION);
    elements.insert(HtmlSynthesis, TAG_HTMLSYNTHESIS);
    elements.insert(GlobalLicense, TAG_LICENSE);
    elements.insert(WebLink, TAG_WEBLINK);
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
    return doc.toString(2);
}

void GenericDescription::addUpdateInformation(Utils::GenericUpdateInformation updateInfo)
{
    m_UpdateInfos.append(updateInfo);
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformation() const
{
    return m_UpdateInfos;
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformationForVersion(const QString &version) const
{}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformationForVersion(const Utils::VersionNumber &version) const
{}

void GenericDescription::toTreeWidget(QTreeWidget *tree) const
{}
