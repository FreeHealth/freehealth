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
#ifndef AGENDAPREFERENCESPAGE_H
#define AGENDAPREFERENCESPAGE_H

#include <coreplugin/ioptionspage.h>

#include "ui_agendapreferencespage.h"

#include <QPointer>
#include <QObject>

/**
 * \file agendapreferencespage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace Agenda {
namespace Internal {

class AgendaPreferencesWidget : public QWidget, private Ui::AgendaPreferencesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AgendaPreferencesWidget)

public:
    explicit AgendaPreferencesWidget(QWidget *parent = 0);

    static void writeDefaultSettings(Core::ISettings *s);
    void setDataToUi();

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);
};


class AgendaPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    AgendaPreferencesPage(QObject *parent = 0);
    ~AgendaPreferencesPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {AgendaPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<AgendaPreferencesWidget> m_Widget;
};

}  // End Internal
}  // End Agenda


#endif // AGENDAPREFERENCESPAGE_H
