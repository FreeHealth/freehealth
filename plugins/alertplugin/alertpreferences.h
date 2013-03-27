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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERT_INTERNAL_ALERTPREFERENCES_H
#define ALERT_INTERNAL_ALERTPREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QWidget>
#include <QPointer>

/**
 * \file alertpreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace Alert {
namespace Internal {
namespace Ui {
class AlertPreferencesWidget;
}

class AlertPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlertPreferencesWidget(QWidget *parent = 0);
    ~AlertPreferencesWidget();

    void setDataToUi();

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private:
    Ui::AlertPreferencesWidget *ui;
};


class AlertPreferencesPage : public Core::IOptionsPage
{
public:
    AlertPreferencesPage(QObject *parent = 0);
    ~AlertPreferencesPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const {return displayName();}
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {AlertPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::AlertPreferencesWidget> m_Widget;
};


} // namespace Internal
} // namespace Alert
#endif // ALERT_INTERNAL_ALERTPREFERENCES_H
