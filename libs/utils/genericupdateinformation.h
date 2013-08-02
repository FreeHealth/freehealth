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
#ifndef GENERICUPDATEINFORMATION_H
#define GENERICUPDATEINFORMATION_H

#include <utils/global_exporter.h>

#include <QDate>
#include <QHash>
#include <QString>
QT_BEGIN_NAMESPACE
class QDomElement;
class QDomDocument;
QT_END_NAMESPACE

/**
 * \file genericupdateinformation.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 25 Nov 2011
*/

namespace Utils {
class VersionNumber;

class UTILS_EXPORT GenericUpdateInformation
{
public:
    GenericUpdateInformation();
    virtual ~GenericUpdateInformation();

    void setFromVersion(const QString &version) {m_From = version;}
    void setToVersion(const QString &version) {m_To = version;}
    void setIsoDate(const QString &date) {m_Date = date;}
    void setAuthor(const QString &a) {m_Author = a;}
    void setText(const QString &text, const QString &lang = QString::null);

    QString fromVersion() const {return m_From;}
    QString toVersion() const {return m_To;}
    QString dateIso() const {return m_Date;}
    QString author() const {return m_Author;}
    QDate date() const {return QDate::fromString(m_Date, Qt::ISODate);}
    QString text(const QString lang = QString::null) const;

//    static bool lessThan(const GenericUpdateInformation *one, const GenericUpdateInformation *two);
    static bool lessThan(const GenericUpdateInformation &one, const GenericUpdateInformation &two);
    static bool greaterThan(const GenericUpdateInformation &one, const GenericUpdateInformation &two);

    static QList<GenericUpdateInformation> updateInformationForVersion(const QList<GenericUpdateInformation> &list, const QString &version);
    static QList<GenericUpdateInformation> updateInformationForVersion(const QList<GenericUpdateInformation> &list, const Utils::VersionNumber &version);

    static QList<GenericUpdateInformation> fromXml(const QDomElement &xml);
    static QString xmlTagName();
    bool toDomElement(QDomElement *root, QDomDocument *document) const;

    QString toHtml() const;

private:
    QString m_From, m_To, m_Date, m_Author;
    QHash<QString, QString> m_TrText;
};

}  // End namespace Utils

UTILS_EXPORT QDebug operator<<(QDebug dbg, const Utils::GenericUpdateInformation &u);
UTILS_EXPORT QDebug operator<<(QDebug dbg, const Utils::GenericUpdateInformation *u);

#endif // GENERICUPDATEINFORMATION_H
