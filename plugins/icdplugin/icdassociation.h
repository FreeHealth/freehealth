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
 ***************************************************************************/
#ifndef ICDASSOCIATION_H
#define ICDASSOCIATION_H

#include <icdplugin/icd_exporter.h>

#include <QString>
#include <QVariant>

namespace ICD {
class IcdDatabase;
namespace Internal {

class ICD_EXPORT IcdAssociation {
    friend class ICD::IcdDatabase;

public:
    IcdAssociation(){}
    IcdAssociation(const QVariant &mainSID, const QVariant &associatedSID, const QString &dagCodeOfAssociation);

    bool isValid() const;

    QVariant mainSid() const {return m_MainSID;}
    QVariant associatedSid() const {return m_AssociatedSID;}
    QString dagCode() const {return m_DagCode;}

    QString mainCode() const;
    QString associatedCode() const;
    QString mainCodeWithDagStar() const;
    QString associatedCodeWithDagStar() const;
    QString mainDaget() const;
    QString associatedDaget() const;

    QString mainLabel() const;
    QString associatedLabel() const;

    bool mainIsDag() const;
    bool associatedIsDag() const;
    bool associationIsMandatory() const;

protected:
    void setMainHumanReadableDaget(const QString &daget) {m_MainDaget = daget;}
    void setAssociatedHumanReadableDaget(const QString &daget) {m_AssoDaget = daget;}

private:
    QVariant m_MainSID, m_AssociatedSID;
    QString m_DagCode;
    QString m_MainDaget, m_AssoDaget;
};

}  // End namespace Internal
}  // End namespace ICD


#endif // ICDASSOCIATION_H
