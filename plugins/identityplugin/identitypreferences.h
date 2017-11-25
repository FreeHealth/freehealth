/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Eric Maeker <eric.maeker@gmail.com>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IDENTITY_INTERNAL_IDENTITYPREFERENCES_H
#define IDENTITY_INTERNAL_IDENTITYPREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QWidget>
#include <QPointer>

namespace Core {
class ISettings;
}

namespace Identity {
namespace Internal {
namespace Ui {
class IdentityPreferencesWidget;
}

class IdentityPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IdentityPreferencesWidget(QWidget *parent = 0);
    ~IdentityPreferencesWidget();

    void setDataToUi();
    QString searchKeywords() const;

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private:
    Ui::IdentityPreferencesWidget *ui;
};


class IdentityPreferencesPage : public Core::IOptionsPage
{
public:
    IdentityPreferencesPage(QObject *parent = 0);
    ~IdentityPreferencesPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const {return name();}

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {IdentityPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::IdentityPreferencesWidget> m_Widget;
};


} // namespace Internal
} // namespace Identity
#endif // IDENTITY_INTERNAL_IDENTITYPREFERENCES_H

