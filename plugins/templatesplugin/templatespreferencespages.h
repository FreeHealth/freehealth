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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TEMPLATESPREFERENCESPAGES_H
#define TEMPLATESPREFERENCESPAGES_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>

#include "ui_templatespreferenceswidget.h"

/**
 * \file templatespreferencespages.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace Templates {
namespace Internal {

class TemplatesPreferencesWidget : public QWidget, private Ui::TemplatesPreferencesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TemplatesPreferencesWidget)

public:
    explicit TemplatesPreferencesWidget(QWidget *parent = 0);
    void setDataToUi();

    static void writeDefaultSettings(Core::ISettings *s);
    static void appliFontToViews(const QFont &font);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);
};

class TemplatesPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT

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

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::TemplatesPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::TemplatesPreferencesWidget> m_Widget;
};

} // namespace Internal
} // namespace Templates

#endif // TEMPLATESPREFERENCESPAGES_H
