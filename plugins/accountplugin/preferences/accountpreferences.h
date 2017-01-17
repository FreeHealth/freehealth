/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNT_PREFERENCES_H
#define ACCOUNT_PREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QString>

#include "ui_accountuseroptionspage.h"
#include "ui_accountdatabasedefaultswidget.h"

/**
 * \file accountpreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace Print {
class PrinterPreviewer;
class TextDocumentExtra;
}

namespace Account {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  AccountUserWidget  //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AccountUserWidget : public QWidget, private Ui::AccountUserWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AccountUserWidget)

public:
    explicit AccountUserWidget(QWidget *parent = 0);
    void setDataToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Print::TextDocumentExtra *header, *footer, *wm;
    Print::PrinterPreviewer *previewer;
};

}  // End Internal


class AccountUserOptionsPage : public Core::IOptionsPage
{
public:
    AccountUserOptionsPage(QObject *parent = 0);
    ~AccountUserOptionsPage();

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

    static void writeDefaultSettings(Core::ISettings *s) {Internal::AccountUserWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::AccountUserWidget> m_Widget;
};

}  // End namespace Account

namespace Account {
namespace Internal {

class AccountDatabaseDefaultsWidget : public QWidget, private Ui::AccountDatabaseDefaultsWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AccountDatabaseDefaultsWidget)

public:
    explicit AccountDatabaseDefaultsWidget(QWidget *parent = 0);
    void setDataToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

private Q_SLOTS:
    void on_createButton_clicked();
    //void on_datapackButton_clicked();

private:
    bool createDefaultsFor(const QString &filePrototype, const int tableRef);
    bool createMinimalsDefaults(const int tableRef);
    bool createMinimalDefaultsFor(const QString &connectionName,
                                  const int tableRef,
                                  const QStringList & valuesList);

protected:
    virtual void changeEvent(QEvent *e);

private:
};

}  // End Internal


class AccountDatabaseDefaultsPage : public Core::IOptionsPage
{
public:
    AccountDatabaseDefaultsPage(QObject *parent = 0);
    ~AccountDatabaseDefaultsPage();

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

    static void writeDefaultSettings(Core::ISettings *s) {Internal::AccountDatabaseDefaultsWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::AccountDatabaseDefaultsWidget> m_Widget;
};

}  // End namespace Account


#endif // ACCOUNT_PREFERENCES_H
