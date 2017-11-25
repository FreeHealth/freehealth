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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GENERICZIPCODESPAGE_H
#define GENERICZIPCODESPAGE_H


#include <coreplugin/ifullreleasestep.h>
#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>
#include "genericzipcodesstep.h"

namespace ZipCodes {

class GenericZipCodesPage : public Core::IToolPage
{
    Q_OBJECT
public:
    GenericZipCodesPage(QObject *parent = 0);

    virtual QString id() const {return "GenericZipCodesPage";}
    virtual QString name() const {return tr("ZipCodes: All (GeoNames.org)");}
    virtual QString category() const {return Core::Constants::CATEGORY_ZIPCODESDATABASE;}
    virtual QIcon icon() const {return QIcon();}

    virtual QWidget *createPage(QWidget *parent = 0);
};

} // end Zipcodes

#endif // GENERICZIPCODESPAGE_H
