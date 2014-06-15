/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "icdassociation.h"
#include "icddatabase.h"

#include <QDebug>


using namespace ICD;
using namespace Internal;

static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}

IcdAssociation::IcdAssociation(const QVariant &mainSID, const QVariant &associatedSID, const QString &dagCodeOfAssociation) :
        m_MainSID(mainSID), m_AssociatedSID(associatedSID), m_DagCode(dagCodeOfAssociation)
{
}

bool IcdAssociation::isValid() const
{
    return (!m_MainSID.isNull()) && (!m_AssociatedSID.isNull()) && (!m_DagCode.isEmpty());
}

QString IcdAssociation::mainCode() const
{
    return icdBase()->getIcdCode(m_MainSID).toString();
}

QString IcdAssociation::associatedCode() const
{
    return icdBase()->getIcdCode(m_AssociatedSID).toString();
}

QString IcdAssociation::mainCodeWithDagStar() const
{
    return icdBase()->getIcdCode(m_MainSID).toString() + m_MainDaget;
}

QString IcdAssociation::associatedCodeWithDagStar() const
{
    return icdBase()->getIcdCode(m_AssociatedSID).toString() + m_AssoDaget;
}

QString IcdAssociation::mainDaget() const
{
    return m_MainDaget;
}

QString IcdAssociation::associatedDaget() const
{
    return m_AssoDaget;
}

QString IcdAssociation::mainLabel() const
{
    return icdBase()->getSystemLabel(m_MainSID);
}

QString IcdAssociation::associatedLabel() const
{
    return icdBase()->getAssociatedLabel(m_MainSID, m_AssociatedSID);
}

bool IcdAssociation::mainIsDag() const
{
    return icdBase()->isDagetADag(m_DagCode);
}

bool IcdAssociation::associatedIsDag() const
{
    return (!icdBase()->isDagetADag(m_DagCode));
}

bool IcdAssociation::associationIsMandatory() const
{
    if (m_DagCode == "T" || m_DagCode == "G")
        return true;
    return false;
}
