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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_PRESCRIPTIONTOKEN_H
#define DRUGSDB_PRESCRIPTIONTOKEN_H

#ifdef WITH_PAD
#include <drugsbaseplugin/drugsbase_exporter.h>
#include <coreplugin/ipadtools.h>
#include <QPointer>

/**
 * \file prescriptiontoken.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 18 Sept 2012
*/

namespace DrugsDB {
class DrugsModel;

class DRUGSBASE_EXPORT PrescriptionToken : public Core::IToken
{
public:
    PrescriptionToken(const QString &name, const int ref);
    ~PrescriptionToken();

    static void setPrescriptionModel(DrugsDB::DrugsModel *model) {_model = model;}
    static void setPrescriptionModelRow(int row) {_row = row;}

    QVariant testValue() const;
    QVariant value() const;

private:
    static QPointer<DrugsDB::DrugsModel> _model;
    static int _row;
    int _ref;
    bool _isRepeatedDailyScheme, _isDistributedDailyScheme, _isMeal;
};

} // namespace DrugsDB

#endif // WITH_PAD

#endif // DRUGSDB_PRESCRIPTIONTOKEN_H
