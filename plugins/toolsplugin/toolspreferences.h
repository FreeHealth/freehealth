/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       Eric Maeker <eric.maeker@gmail.com>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_TOOLSPREFERENCES_H
#define TOOLS_INTERNAL_TOOLSPREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QWidget>
#include <QPointer>

namespace Core {
class ISettings;
}

namespace Tools {
namespace Internal {
namespace Ui {
class ToolsPreferencesWidget;
}

class ToolsPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolsPreferencesWidget(QWidget *parent = 0);
    ~ToolsPreferencesWidget();

    void setDataToUi();
    QString searchKeywords() const;

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private:
    Ui::ToolsPreferencesWidget *ui;
};


class ToolsPreferencesPage : public Core::IOptionsPage
{
public:
    ToolsPreferencesPage(QObject *parent = 0);
    ~ToolsPreferencesPage();

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

    static void writeDefaultSettings(Core::ISettings *s) {ToolsPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::ToolsPreferencesWidget> m_Widget;
};


} // namespace Internal
} // namespace Tools
#endif // TOOLS_INTERNAL_TOOLSPREFERENCES_H

