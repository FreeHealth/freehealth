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
#ifndef IABOUTPAGE_H
#define IABOUTPAGE_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/igenericpage.h>

#include <QObject>
#include <QString>
#include <QWidget>

/**
 * \file iaboutpage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 13 June 2010
 * \class Core::IAboutPage
 * \brief Derive object from this interface and set it inside the PluginManager object pool to get your page in the Core::AboutDialog.
*/

namespace Core {

class CORE_EXPORT IAboutPage : public Core::IGenericPage
{
    Q_OBJECT
public:
    IAboutPage(QObject *parent = 0) : Core::IGenericPage(parent) {}
    virtual ~IAboutPage() {}

    QString title() const {return displayName();}
};

} // namespace Core

#endif // IABOUTPAGE_H
