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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DRUGSDATA_H
#define DRUGSDATA_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/idrug.h>

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QDate>
#include <QSet>
#include <QDebug>

/**
 * \file drugdata.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 25 Feb 2011
*/

namespace DrugsDB {
namespace Internal {
class DrugsDataPrivate;
class DrugsBase;
class DrugsBasePrivate;

class DRUGSBASE_EXPORT DrugsData : public IDrug
{
    friend class DrugsBase;
    friend class DrugsBasePrivate;

public:
     DrugsData();
     ~DrugsData();

     virtual bool hasPrescription() const;
     virtual void setPrescriptionValue(const int fieldref, const QVariant &value);
     virtual QVariant prescriptionValue(const int filedref) const;

     void smallDrugWarn() const;

protected:
     // setters
     void addRoute(const int routeId, const QString &lang, const QString &label);

private:
     DrugsDataPrivate *d;
};


class DRUGSBASE_EXPORT TextualDrugsData : public DrugsData
{
public:
    TextualDrugsData();
    void setDenomination(const QString &denomination);

    virtual void setPrescriptionValue(const int fieldref, const QVariant &value);
    virtual QVariant prescriptionValue(const int filedref) const;
};

}  // End Internal
}  // End DrugsDB

// Q_DECLARE_METATYPE( DrugsDB::DrugsData )
typedef QList<DrugsDB::Internal::DrugsData*> QDrugsList;

#endif  // DRUGSDATA_H
