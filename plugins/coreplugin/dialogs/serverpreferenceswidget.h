/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef COREPLUGIN_SERVERPREFERENCESWIDGET_H
#define COREPLUGIN_SERVERPREFERENCESWIDGET_H

#include <coreplugin/core_exporter.h>
#include <utils/database.h>

#include <QWidget>

namespace Core {
class ISettings;

namespace Internal {
class ServerPreferencesWidgetPrivate;

namespace Ui {
class ServerPreferencesWidget;
}  // namespace Ui
}  // namespace Internal

class CORE_EXPORT ServerPreferencesWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerPreferencesWidget)
    friend class Core::Internal::ServerPreferencesWidgetPrivate;

public:
    explicit ServerPreferencesWidget(QWidget *parent = 0);
    ~ServerPreferencesWidget();

    bool connectionSucceeded() const;
    void setUserLoginGroupTitle(const QString &trContext, const QString &untranslatedtext);

    void showUseDefaultAdminLogCheckbox(bool show);

    QString hostName() const;
    int port() const;
    QString login() const;
    QString password() const;
    Utils::Database::Grants grantsOnLastConnectedDatabase() const;

    static void writeDefaultSettings(Core::ISettings *s);

Q_SIGNALS:
    void hostConnectionChanged(bool coonected);
    void userConnectionChanged(bool coonected);

protected Q_SLOTS:
    void testHost();
    void testHost(const QString &hostName);
    void saveToSettings(Core::ISettings *s = 0);

private Q_SLOTS:
    void on_testMySQLButton_clicked();
    void toggleLogPass(bool state);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Internal::ServerPreferencesWidgetPrivate *d;
};

}  // End namespace Core

#endif // COREPLUGIN_SERVERPREFERENCESWIDGET_H
