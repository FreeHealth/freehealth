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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEMEDFORMSCORE_PLUGIN_SQLITEDATABASEPATHPREFERENCES_H
#define FREEMEDFORMSCORE_PLUGIN_SQLITEDATABASEPATHPREFERENCES_H

#include <coreplugin/ioptionspage.h>
#include <QPointer>
#include <QObject>

/**
 * \file sqlitedatabasepathpreferences.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 22 Jul 2014
*/

namespace Core {
class ISettings;
namespace Internal {
namespace Ui {
class SqliteDatabasePathWidget;
}

class SqliteDatabasePathWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SqliteDatabasePathWidget)

public:
    explicit SqliteDatabasePathWidget(QWidget *parent = 0);
    ~SqliteDatabasePathWidget();

    static void writeDefaultSettings(Core::ISettings * = 0) {}

public Q_SLOTS:
    void saveToSettings(Core::ISettings *);

private Q_SLOTS:
    void onMoveDatabaseRequested();

private:
    Ui::SqliteDatabasePathWidget *ui;
};

class SqliteDatabasePathPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    SqliteDatabasePathPage(QObject *parent = 0);
    ~SqliteDatabasePathPage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults() {SqliteDatabasePathWidget::writeDefaultSettings();}
    void checkSettingsValidity() {}
    void apply() {}
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {SqliteDatabasePathWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<SqliteDatabasePathWidget> m_Widget;
};
}  // End Internal
}  // End Core

#endif // FREEMEDFORMSCORE_PLUGIN_SQLITEDATABASEPATHPREFERENCES_H
