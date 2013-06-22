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
 *   Main Developpers:                                                     *
 *       Eric Maeker <e>                             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRCPLUGIN_INTERNAL_EDRCPREFERENCES_H
#define EDRCPLUGIN_INTERNAL_EDRCPREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QWidget>
#include <QPointer>

/**
 * \file edrcpreferences.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 22 Jun 2013
*/

namespace Core {
class ISettings;
}

namespace eDRC {
namespace Internal {
namespace Ui {
class EditorPreferencesWidget;
}

class EditorPreferencesWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit EditorPreferencesWidget(QWidget *parent = 0);
    ~EditorPreferencesWidget();
    
    void setDataToUi();
    QString searchKeywords() const;
    
    static void writeDefaultSettings(Core::ISettings *s);
    
public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    
private:
    void retranslateUi();
    void changeEvent(QEvent *e);
    
private:
    Ui::EditorPreferencesWidget *ui;
};


class EditorPreferencesPage : public Core::IOptionsPage
{
public:
    EditorPreferencesPage(QObject *parent = 0);
    ~EditorPreferencesPage();
    
    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;
    
    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();
    
    bool matches(const QString &s) const;
    
    QString helpPage() {return QString();}
    
    static void writeDefaultSettings(Core::ISettings *s) {EditorPreferencesWidget::writeDefaultSettings(s);}
    
    QWidget *createPage(QWidget *parent = 0);
    
private:
    QPointer<Internal::EditorPreferencesWidget> m_Widget;
    QString m_searchKeywords;
};


} // namespace Internal
} // namespace eDRC

#endif // EDRCPLUGIN_INTERNAL_EDRCPREFERENCES_H

