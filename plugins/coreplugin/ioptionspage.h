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
#ifndef IOPTIONSPAGE_H
#define IOPTIONSPAGE_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/igenericpage.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

/**
 * \file ioptionspage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 12 Jul 2011
 * \class Core::IOptionsPage
 * \brief Represents an application preferences page. Derive objects from this interface and set it inside the PluginManager oject pool to get the page in the Core::SettingsDialog.
*/

namespace Core {

class CORE_EXPORT IOptionsPage : public IGenericPage
{
    Q_OBJECT
public:
    IOptionsPage(QObject *parent = 0) : IGenericPage(parent) {}
    virtual ~IOptionsPage() {}

    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString category() const = 0;

    virtual void resetToDefaults() = 0;
    virtual void checkSettingsValidity() = 0;
    virtual void applyChanges() = 0;
    virtual void finish() = 0;

    virtual QString helpPage() = 0;

    virtual QWidget *createPage(QWidget *parent = 0) = 0;
};

} // namespace Core

#endif // IOPTIONSPAGE_H
