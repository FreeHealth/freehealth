/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MAINWINDOWPREFERENCES_H
#define MAINWINDOWPREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <fmfmainwindowplugin/virtualdatabasepreferences.h>

#include <QPointer>
#include <QObject>

/**
 * \file mainwindowpreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace MainWin {
namespace Internal {

class VirtualPatientBasePage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    VirtualPatientBasePage(QObject *parent = 0);
    ~VirtualPatientBasePage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults() {VirtualDatabasePreferences::writeDefaultSettings();}
    void checkSettingsValidity() {}
    void apply() {}
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {VirtualDatabasePreferences::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<VirtualDatabasePreferences> m_Widget;
};


}  // End Internal
}  // End MainWin

#endif // MAINWINDOWPREFERENCES_H
