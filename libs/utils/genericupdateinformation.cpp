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
  \class Utils::GenericUpdateInformation
  Manage the information about update of items. You can translate the items to XML and vice-versa.
*/

#include "genericupdateinformation.h"
#include "versionnumber.h"

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QDomElement>
#include <QLocale>
#include <QDebug>

using namespace Utils;
using namespace Trans::ConstantTranslations;

GenericUpdateInformation::GenericUpdateInformation()
{
}

GenericUpdateInformation::~GenericUpdateInformation()
{
}

void GenericUpdateInformation::setText(const QString &text, const QString &lang)
{
    if (lang.isEmpty())
        m_TrText.insert(Trans::Constants::ALL_LANGUAGE, text);
    else
        m_TrText.insert(lang.toLower(), text);
}

/** Return the text of the update information translated to the language \e lang (if exists) or return the default text. */
QString GenericUpdateInformation::text(const QString lang) const
{
    if (lang.isEmpty()) {
        // Try QLocale
        QString l = QLocale().name().left(2).toLower();
        if (!m_TrText.value(l).isEmpty())
            return m_TrText.value(l);
        // Try ALL_LANGUAGE
        l = Trans::Constants::ALL_LANGUAGE;
        return m_TrText.value(l);
    }
    if (!m_TrText.value(lang).isEmpty())
        return m_TrText.value(lang);
    return m_TrText.value(Trans::Constants::ALL_LANGUAGE);
}

/** Used to sort containers */
bool GenericUpdateInformation::lessThan(const GenericUpdateInformation &one, const GenericUpdateInformation &two)
{
    return (one.fromVersion() < two.fromVersion());
}

bool GenericUpdateInformation::greaterThan(const GenericUpdateInformation &one, const GenericUpdateInformation &two)
{
    return (one.fromVersion() > two.fromVersion());
}

/** Return the information related to specified \e version and higher */
QList<GenericUpdateInformation> GenericUpdateInformation::updateInformationForVersion(const QList<GenericUpdateInformation> &list, const QString &version)
{
    Utils::VersionNumber v(version);
    return updateInformationForVersion(list, v);
}

/** Return the information related to specified \e version and higher */
QList<GenericUpdateInformation> GenericUpdateInformation::updateInformationForVersion(const QList<GenericUpdateInformation> &list, const Utils::VersionNumber &version)
{
    QList<GenericUpdateInformation> toReturn;
    foreach(const GenericUpdateInformation &u, list) {
        Utils::VersionNumber from(u.fromVersion());
        Utils::VersionNumber to(u.toVersion());
        if (version<=from || (version>from && version<to))
            toReturn << u;
    }
    qSort(toReturn.begin(), toReturn.end(), Utils::GenericUpdateInformation::lessThan);
    return toReturn;
}


namespace {
    const char* const TAG_SPEC_UPDATEINFO             = "updateinfo";
    const char* const TAG_SPEC_UPDATEINFOVERSION      = "updateversion";
    const char* const ATTRIB_UPDATEINFOVERSION_FROM   = "from";
    const char* const ATTRIB_UPDATEINFOVERSION_TO     = "to";
    const char* const ATTRIB_UPDATEINFOVERSION_DATE   = "date";
    const char* const TAG_SPEC_UPDATEINFOVERSIONTEXT  = "text";
    const char* const ATTRIB_LANGUAGE                 = "lang";
}

/** Read the update information xml block and return a list of items. */
QList<GenericUpdateInformation> GenericUpdateInformation::fromXml(const QDomElement &xml)
{
    // read update information
    QList<GenericUpdateInformation> toReturn;
    if (!xml.isNull() && xml.tagName().compare(::TAG_SPEC_UPDATEINFO, Qt::CaseInsensitive)==0) {
        QDomElement update = xml.firstChildElement(::TAG_SPEC_UPDATEINFOVERSION);
        while (!update.isNull()) {
            QDomElement updateText = update.firstChildElement(::TAG_SPEC_UPDATEINFOVERSIONTEXT);
            GenericUpdateInformation updateItem;
            updateItem.setFromVersion(update.attribute(::ATTRIB_UPDATEINFOVERSION_FROM));
            updateItem.setToVersion(update.attribute(::ATTRIB_UPDATEINFOVERSION_TO));
            updateItem.setIsoDate(update.attribute(::ATTRIB_UPDATEINFOVERSION_DATE));
            while (!updateText.isNull()) {
                updateItem.setText(updateText.text(), updateText.attribute(::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
                updateText = updateText.nextSiblingElement(::TAG_SPEC_UPDATEINFOVERSIONTEXT);
            }
            toReturn << updateItem;
            update = update.nextSiblingElement(::TAG_SPEC_UPDATEINFOVERSION);
        }
    }
    return toReturn;
}

/** Return the root tag name of update inforamtion xml block. */
QString GenericUpdateInformation::xmlTagName()
{
    return ::TAG_SPEC_UPDATEINFO;
}

bool GenericUpdateInformation::toDomElement(QDomElement *root, QDomDocument *document) const
{
    Q_ASSERT(root);
    Q_ASSERT(document);
    if (!root || !document)
        return false;
    if (root->tagName().compare(xmlTagName(), Qt::CaseInsensitive)!=0) {
        LOG_ERROR_FOR("GenericUpdateInformation",QString("Wrong root tag. Got %1, requiered %2").arg(root->tagName()).arg(xmlTagName()));
        return false;
    }
    // Create the version branch
    QDomElement e = document->createElement(TAG_SPEC_UPDATEINFOVERSION);
    root->appendChild(e);
    if (!m_From.isEmpty()) {
        e.setAttribute(::ATTRIB_UPDATEINFOVERSION_FROM, m_From);
    }
    if (!m_To.isEmpty()) {
        e.setAttribute(::ATTRIB_UPDATEINFOVERSION_TO, m_To);
    }
    if (!m_Date.isEmpty()) {
        e.setAttribute(::ATTRIB_UPDATEINFOVERSION_FROM, m_Date);
    }
    QHashIterator<QString, QString> i(m_TrText);
    while (i.hasNext()) {
        i.next();
        QDomElement et = document->createElement(::TAG_SPEC_UPDATEINFOVERSIONTEXT);
        e.appendChild(et);
        et.setAttribute(::ATTRIB_LANGUAGE, i.key());
        QDomText t = document->createTextNode(i.value());
        et.appendChild(t);
    }
    return true;
}

QString GenericUpdateInformation::toHtml() const
{
    return QString("%1 %2 %3 %4<br /><span style=\"margin-left:10px;\">%5</span><br />")
            .arg(tkTr(Trans::Constants::FROM)).arg(m_From).arg(tkTr(Trans::Constants::TO)).arg(m_To)
            .arg(text());
}

QDebug operator<<(QDebug dbg, const Utils::GenericUpdateInformation &c)
{
    QString tmp = "GenericUpdateInformation(";
    tmp += "f: " + c.fromVersion();
    tmp += "; t: " + c.toVersion();
    tmp += "; d: " + c.dateIso();
    tmp += "; text: " + c.text();
    tmp += ")";
    dbg.nospace() << tmp;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Utils::GenericUpdateInformation *c)
{
    if (!c) {
        dbg.nospace() << "GenericUpdateInformation(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}
