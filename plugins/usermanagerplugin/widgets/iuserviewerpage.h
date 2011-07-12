/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef IUSERVIEWERPAGE_H
#define IUSERVIEWERPAGE_H

#include <usermanagerplugin/usermanager_exporter.h>

#include <QObject>
#include <QModelIndex>

/**
 * \file iuserviewerpage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 08 Jul 2011
*/

namespace UserPlugin {

class USER_EXPORT IUserViewerPage : public QObject
{
    Q_OBJECT
public:
    explicit IUserViewerPage(QObject *parent = 0);
    virtual ~IUserViewerPage() {}

    virtual QString uid() const = 0;
    virtual QString name() const = 0;
    virtual QStringList categoryTree() const = 0;

    virtual QWidget *createWidget(QWidget *parent) = 0;

    virtual void setUserIndex(const QModelIndex &index) = 0;
    virtual bool clear() = 0;
    virtual bool submit() = 0;
};

}  // End namespace UserPlugin

#endif // IUSERVIEWERPAGE_H
