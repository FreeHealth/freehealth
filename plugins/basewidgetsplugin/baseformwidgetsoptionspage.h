/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 ***************************************************************************/
#ifndef BASEFORMWIDGETSOPTIONSPAGE_H
#define BASEFORMWIDGETSOPTIONSPAGE_H

#include <coreplugin/ioptionspage.h>
#include <QWidget>

#include <QPointer>

/**
 * \file baseformwidgetsoptionspage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace BaseWidgets {
namespace Internal {

namespace Ui {
    class BaseFormWidgetsOptionsPage;
}

class BaseFormSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    BaseFormSettingsWidget(QWidget *parent = 0);
    ~BaseFormSettingsWidget();

    void applyChanges();
    void resetToDefaults();

private:
    Ui::BaseFormWidgetsOptionsPage *m_ui;
};

class BaseFormWidgetsOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    BaseFormWidgetsOptionsPage(QObject *parent = 0);
    ~BaseFormWidgetsOptionsPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const {return displayName();}
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();

    /** \todo add help page */
    QString helpPage() {return QString();}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<BaseFormSettingsWidget> m_Widget;
};

} // End Internal
} // End BaseWidgets

#endif // BASEFORMWIDGETSOPTIONSPAGE_H
