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
#ifndef ITHEME_H
#define ITHEME_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QIcon>

/**
 * \file itheme.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.10
 * \date 09 Aug 2009
*/

namespace Core {

class CORE_EXPORT ITheme : public QObject
{
    Q_OBJECT
public:
    enum IconSize {
        SmallIcon = 0, // 16x16 is the default
        MediumIcon,    // 32x32
        BigIcon        // 64x64
    };

    ITheme(QObject *parent=0) : QObject(parent) {}
    virtual ~ITheme() {}

    virtual QIcon icon( const QString & fileName, IconSize size = SmallIcon ) = 0;
    virtual QString iconFullPath( const QString &fileName, IconSize size = SmallIcon ) = 0;
    virtual QPixmap splashScreen( const QString &fileName ) = 0;
};

} // end Core

#endif // ITHEME_H
