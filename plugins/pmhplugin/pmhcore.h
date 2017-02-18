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
 *  Authors:                                                               *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *                                                                         *
 ***************************************************************************/
#pragma once

#include <pmhplugin/pmh_exporter.h>

#include <QObject>

/**
 * \file pmhcore.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 25 July 2012
*/

namespace PMH {
namespace Internal {
class PmhCorePrivate;
}
class PmhCategoryModel;


class PMH_EXPORT PmhCore : public QObject
{
    Q_OBJECT
    explicit PmhCore(QObject *parent = 0);

public:
    static PmhCore *instance(QObject *parent = 0);
    ~PmhCore();

    PmhCategoryModel *pmhCategoryModel() const;

private Q_SLOTS:
    void onPatientFormsLoaded();

private:
    static PmhCore *m_Instance;
    Internal::PmhCorePrivate *d;
};

}  // End PMH
