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
 *       %Author% <%AuthorEmail%>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef %PluginName:u%_INTERNAL_%PluginName:u%PREFERENCES_H
#define %PluginName:u%_INTERNAL_%PluginName:u%PREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QWidget>
#include <QPointer>

namespace Core {
class ISettings;
}

namespace %PluginName:c% {
namespace Internal {
namespace Ui {
class %PluginName:c%PreferencesWidget;
}

class %PluginName:c%PreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit %PluginName:c%PreferencesWidget(QWidget *parent = 0);
    ~%PluginName:c%PreferencesWidget();

    void setDataToUi();
    QString searchKeywords() const;

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private:
    Ui::%PluginName:c%PreferencesWidget *ui;
};


class %PluginName:c%PreferencesPage : public Core::IOptionsPage
{
public:
    %PluginName:c%PreferencesPage(QObject *parent = 0);
    ~%PluginName:c%PreferencesPage();

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

    static void writeDefaultSettings(Core::ISettings *s) {%PluginName:c%PreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::%PluginName:c%PreferencesWidget> m_Widget;
};


} // namespace Internal
} // namespace %PluginName:c%
#endif // %PluginName:u%_INTERNAL_%PluginName:u%PREFERENCES_H
