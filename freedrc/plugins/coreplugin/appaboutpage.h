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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef APPABOUTPAGE_H
#define APPABOUTPAGE_H

#include <coreplugin/iaboutpage.h>

namespace Core {
namespace Internal {

class AppAboutPage : public IAboutPage
{
    Q_OBJECT
public:
    AppAboutPage(QObject *parent);
    ~AppAboutPage();

    QString id() const {return objectName();}
    QString displayName() const;
    QString category() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent = 0);
};

class CommandLineAboutPage : public IAboutPage
{
    Q_OBJECT

public:
    CommandLineAboutPage(QObject *parent) : IAboutPage(parent) {}
    ~CommandLineAboutPage() {}

    QString id() const {return objectName();}
    QString displayName() const;
    QString category() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent = 0);
};

} // End Internal
} // End Core

#endif // APPABOUTPAGE_H
