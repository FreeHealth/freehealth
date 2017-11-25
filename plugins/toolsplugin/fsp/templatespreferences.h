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
 *   Main Developpers:                                                     *
 *       Eric Maeker <e>                             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_FSPPRINTER_PREFERENCES_H
#define TOOLS_INTERNAL_FSPPRINTER_PREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QWidget>
#include <QPointer>

/**
 * \file templatespreferences.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date
*/

namespace Core {
class ISettings;
}

namespace Templates {
namespace Internal {
namespace Ui {
class TemplatesPreferencesWidget;
}

class TemplatesPreferencesWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TemplatesPreferencesWidget(QWidget *parent = 0);
    ~TemplatesPreferencesWidget();
    
    void setDataToUi();
    QString searchKeywords() const;
    
    static void writeDefaultSettings(Core::ISettings *s);
    
public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    
private:
    void retranslateUi();
    void changeEvent(QEvent *e);
    
private:
    Ui::TemplatesPreferencesWidget *ui;
};


class TemplatesPreferencesPage : public Core::IOptionsPage
{
public:
    TemplatesPreferencesPage(QObject *parent = 0);
    ~TemplatesPreferencesPage();
    
    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;
    
    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();
    
    bool matches(QString &s) const;
    
    QString helpPage() {return QString();}
    
    static void writeDefaultSettings(Core::ISettings *s) {TemplatesPreferencesWidget::writeDefaultSettings(s);}
    
    QWidget *createPage(QWidget *parent = 0);
    
private:
    QPointer<Internal::TemplatesPreferencesWidget> m_Widget;
    QString m_searchKeywords;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_FSPPRINTER_PREFERENCES_H

