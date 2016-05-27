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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
#ifndef CORE_IGENERICPAGE_H
#define CORE_IGENERICPAGE_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QString>
#include <QWidget>
#include <QIcon>

/**
 * \file igenericpage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {

class CORE_EXPORT IGenericPage : public QObject
{
    Q_OBJECT
public:
    IGenericPage(QObject *parent = 0) : QObject(parent) {}
    virtual ~IGenericPage() {}

    virtual QString id() const = 0;
    virtual QString displayName() const = 0;
    virtual QString title() const = 0;
    virtual QIcon categoryIcon() const {return QIcon();}

    virtual QString category() const = 0;
    //TODO: make displayCategory pure virtual, implement in all implementing classes
    virtual QString displayCategory() const { return QString(); }

    virtual QWidget *createPage(QWidget *parent = 0) = 0;
    virtual int sortIndex() const = 0;

    static bool lessThan(IGenericPage *one, IGenericPage *two);
    static bool sortIndexLessThan(IGenericPage *one, IGenericPage *two);
};

} // namespace Core

#endif // CORE_IGENERICPAGE_H
