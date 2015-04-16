/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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

#include <QObject>
#include <QString>
#include <QWidget>

/**
 * \file ioptionspage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
 * \class Core::IOptionsPage
 * \brief Represents an application preferences page. Derive objects from this interface and set it
 * inside the PluginManager object pool to get the page into the Core::SettingsDialog.
*/

namespace Core {

class CORE_EXPORT IOptionsPage : public IGenericPage
{
    Q_OBJECT
public:
    IOptionsPage(QObject *parent = 0) : IGenericPage(parent) {}
    virtual ~IOptionsPage() {}

    // implemented in IGenericPage:
//    virtual QString id() const = 0;
//    virtual QString displayName() const = 0;
//    virtual QString category() const = 0;
//    virtual int sortIndex() const = 0;
//    virtual QWidget *createPage(QWidget *parent = 0) = 0;

    /*! Resets the current page to the defaults. */
    virtual void resetToDefaults() = 0;

    /** Check the validity of settings. You don't need to sync the settings. This will be automatically done. */
    virtual void checkSettingsValidity() = 0;

    /** Apply changes to the settings. You don't need to sync the settings. This will be automatically done. */
    virtual void apply() = 0;

    virtual void finish() = 0;

    virtual bool matches(const QString & searchKeyWord) const {Q_UNUSED(searchKeyWord); return false; }

    virtual QString helpPage() = 0;

};

} // namespace Core

#endif // IOPTIONSPAGE_H
