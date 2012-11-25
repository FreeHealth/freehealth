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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSWIDGET_INTERNAL_DRUGENGINESPREFERENCES_H
#define DRUGSWIDGET_INTERNAL_DRUGENGINESPREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QWidget>

/**
 * \file drugenginespreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace DrugsWidget {
namespace Internal {

namespace Ui {
    class DrugEnginesPreferences;
}

class DrugEnginesPreferences : public QWidget
{
    Q_OBJECT

public:
    explicit DrugEnginesPreferences(QWidget *parent = 0);
    ~DrugEnginesPreferences();

    void setDataToUi();
    void saveToSettings(Core::ISettings *sets = 0);

    static void writeDefaultSettings(Core::ISettings *s);

private:
    Ui::DrugEnginesPreferences *ui;
};

class DrugEnginesPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugEnginesPreferencesPage(QObject *parent = 0);
    ~DrugEnginesPreferencesPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s)  {Internal::DrugEnginesPreferences::writeDefaultSettings(s);}
    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugEnginesPreferences> m_Widget;
};

} // namespace Internal
} // namespace DrugsWidget


#endif // DRUGSWIDGET_INTERNAL_DRUGENGINESPREFERENCES_H
