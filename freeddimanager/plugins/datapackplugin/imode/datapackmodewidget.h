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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DRUGSDB_INTERNAL_DataPackModeWidget_H
#define DDIMANAGER_DRUGSDB_INTERNAL_DataPackModeWidget_H

#include <QWidget>

/**
 * \file datapackmodewidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 25 May 2014
*/

namespace DataPackPlugin {
namespace Internal {
class DataPackModeWidgetPrivate;

class DataPackModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataPackModeWidget(QWidget *parent = 0);
    ~DataPackModeWidget();

private:
    Internal::DataPackModeWidgetPrivate *d;
};

} // namespace Internal
} // namespace DrugsDb

#endif // DDIMANAGER_DRUGSDB_INTERNAL_DataPackModeWidget_H

