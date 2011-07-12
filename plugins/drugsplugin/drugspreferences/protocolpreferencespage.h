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
#ifndef PROTOCOLPREFERENCESPAGE_H
#define PROTOCOLPREFERENCESPAGE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QWidget>
#include <QStringListModel>

/**
 * \file protocolpreferencespage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 11 Nov 2010
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

}  // End namespace Internal


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

}  // End namespace DrugsWidget


#endif // PROTOCOLPREFERENCESPAGE_H
