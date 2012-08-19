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
#ifndef PROTOCOLPREFERENCESPAGE_H
#define PROTOCOLPREFERENCESPAGE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QWidget>
#include <QStringListModel>

/**
 * \file protocolpreferencespage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace DrugsDB {
class DatabaseInfos;
}

namespace Core {
class ISettings;
}

namespace DrugsWidget {
namespace Internal {
namespace Ui {
class ProtocolPreferencesWidget;
}

class ProtocolPreferencesWidget : public QWidget
{
    Q_OBJECT
public:
    ProtocolPreferencesWidget(QWidget *parent = 0);
    ~ProtocolPreferencesWidget();

    void setDatasToUi();
    static void writeDefaultSettings(Core::ISettings *s = 0);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ProtocolPreferencesWidget *ui;
    QHash<QString, QString> m_ButtonChoices;
};

class ProtocolPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    ProtocolPreferencesPage(QObject *parent = 0);
    ~ProtocolPreferencesPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {Internal::ProtocolPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::ProtocolPreferencesWidget> m_Widget;
};

}  // End namespace Internal
}  // End namespace DrugsWidget

#endif // PROTOCOLPREFERENCESPAGE_H
