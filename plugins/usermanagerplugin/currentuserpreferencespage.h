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
 ***************************************************************************/
#ifndef CURRENTUSERPREFERENCESPAGE_H
#define CURRENTUSERPREFERENCESPAGE_H

#include <coreplugin/ioptionspage.h>
#include "ui_currentuserpreferenceswidget.h"

#include <QPointer>
#include <QString>

/**
 * \file currentuserpreferencespage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace UserPlugin {
class UserViewer;

namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  CurrentUserPreferencesWidget  ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CurrentUserPreferencesWidget : public QWidget, private Ui::CurrentUserPreferencesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CurrentUserPreferencesWidget)

public:
    explicit CurrentUserPreferencesWidget(QWidget *parent = 0);
    void setDataToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);

private:
    UserPlugin::UserViewer *m_Viewer;
};

}  // End Internal


class CurrentUserPreferencesPage : public Core::IOptionsPage
{
public:
    CurrentUserPreferencesPage(QObject *parent = 0);
    ~CurrentUserPreferencesPage();

    QString displayName() const;
    QString category() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::CurrentUserPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::CurrentUserPreferencesWidget> m_Widget;
};

}  // End namespace UserPlugin

#endif // CURRENTUSERPREFERENCESPAGE_H
