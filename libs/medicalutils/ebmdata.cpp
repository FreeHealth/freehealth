/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class MedicalUtils::EbmData
  \brief Provide access to Evidence Based Medicine data (publications).
*/

#include "ebmdata.h"

#include <QString>
#include <QDebug>

using namespace MedicalUtils;

EbmData::EbmData()
{
}

EbmData::~EbmData()
{
}

//EbmData::EbmData(const EbmData &other)
//{
//    m_Id = other.id();
//    m_Link = other.link();
//    m_Ref = other.references();
//    m_Abstract = other.abstract();
//}

//EbmData &EbmData::operator=(const EbmData &other)
//{
//    EbmData data(other);
//    return data;
//}

void EbmData::setAbstract(const QString &plainText)
{
    m_Abstract = plainText;
}

void EbmData::setReferences(const QString &plainText)
{
    m_Ref = plainText;
}

void EbmData::setLink(const QString &url)
{
    m_Link=url;
}

void EbmData::warn() const
{
    qWarning() << QString("EbmData(%1)\n%2\n%3").arg(m_Link).arg(m_Ref).arg(m_Abstract);
}
