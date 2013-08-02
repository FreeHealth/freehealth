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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "episodedata.h"

#include <QDateTime>

#include <QDebug>

using namespace Form;
using namespace Internal;

EpisodeValidationData::EpisodeValidationData()
{
    m_Data.insert(EpisodeId, -1);
    m_Data.insert(ValidationId, -1);
    m_Modified = false;
}

EpisodeValidationData::~EpisodeValidationData()
{}

void EpisodeValidationData::setData(int ref, const QVariant &value)
{
    m_Data.insert(ref, value);
    m_Modified = true;
}

QVariant EpisodeValidationData::data(int ref) const
{
    return m_Data.value(ref);
}


EpisodeModificationData::EpisodeModificationData()
{
    m_Data.insert(EpisodeId, -1);
    m_Data.insert(ModificationId, -1);
    m_Modified = false;
}

EpisodeModificationData::~EpisodeModificationData()
{}

void EpisodeModificationData::setData(int ref, const QVariant &value)
{
    m_Data.insert(ref, value);
    m_Modified = true;
}

QVariant EpisodeModificationData::data(int ref) const
{
    return m_Data.value(ref);
}



EpisodeData::EpisodeData()
{
    m_Data.insert(Id, -1);
    m_Data.insert(ContentId, -1);
    m_Data.insert(ValidationId, -1);
    m_Data.insert(IsValid, 1);
    m_Data.insert(IsXmlContentPopulated, false);
    m_Modified = false;
}

EpisodeData::~EpisodeData()
{
}

void EpisodeData::setData(int ref, const QVariant &value)
{
    if (value==m_Data.value(ref))
        return;
    m_Data.insert(ref, value);
    if (ref==Id) {
        for(int i = 0; i < m_Validation.count(); ++i) {
            m_Validation[i].setData(EpisodeValidationData::EpisodeId, value);
        }
        for(int i = 0; i < m_Modification.count(); ++i) {
            m_Modification[i].setData(EpisodeModificationData::EpisodeId, value);
        }
    }
    m_Modified = true;
}

QVariant EpisodeData::data(int ref) const
{
    return m_Data.value(ref);
}

/** Add a validation record to the episode. */
void EpisodeData::addEpisodeValidation(EpisodeValidationData &validation)
{
    validation.setData(EpisodeValidationData::EpisodeId, m_Data.value(Id));
    m_Validation.append(validation) ;
}

/** Add modification records to the episode. The LastModificationDate is automatically updated. */
void EpisodeData::addEpisodeModification(EpisodeModificationData &modification)
{
    modification.setData(EpisodeModificationData::EpisodeId, m_Data.value(Id));
    m_Modification.append(modification);
    // manage LastModificationDate
    if (data(LastModificationDate).isNull()) {
        setData(LastModificationDate, modification.data(EpisodeModificationData::Date));
    } else {
        if (modification.data(EpisodeModificationData::Date).toDateTime() > data(LastModificationDate).toDateTime())
            setData(LastModificationDate, modification.data(EpisodeModificationData::Date));
    }
}

bool EpisodeData::lessThanByUserDate(EpisodeData *one, EpisodeData *two)
{
    return one->data(UserDate).toDateTime() < two->data(UserDate).toDateTime();
}
