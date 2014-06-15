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
#ifndef FORMMANAGERPREFERENCESPAGE_H
#define FORMMANAGERPREFERENCESPAGE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QObject>

/**
 * \file formmanagerpreferencespage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace Form {
class FormFilesSelectorWidget;

namespace Internal {
namespace Ui {
class FormPreferencesFileSelectorWidget;
}

class FormPreferencesFileSelectorWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FormPreferencesFileSelectorWidget)

public:
    explicit FormPreferencesFileSelectorWidget(QWidget *parent = 0);
    ~FormPreferencesFileSelectorWidget();

    static void writeDefaultSettings(Core::ISettings *) {}

public Q_SLOTS:
    void saveFormToBase();
    void saveToSettings(Core::ISettings *);

//protected:
//    virtual void changeEvent(QEvent *e);

private:
    Ui::FormPreferencesFileSelectorWidget *ui;
};


class FormPreferencesFileSelectorPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    FormPreferencesFileSelectorPage(QObject *parent = 0);
    ~FormPreferencesFileSelectorPage();

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

    static void writeDefaultSettings(Core::ISettings *s) {FormPreferencesFileSelectorWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<FormPreferencesFileSelectorWidget> m_Widget;
};


}  // End Internal
}  // End Form


namespace Form {
namespace Internal {
namespace Ui {
class FormPreferencesWidget;
}

class FormPreferencesWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FormPreferencesWidget)

public:
    explicit FormPreferencesWidget(QWidget *parent = 0);
    ~FormPreferencesWidget();

    static void writeDefaultSettings(Core::ISettings *) {}
    void setupUiData();

public Q_SLOTS:
    void saveToSettings(Core::ISettings *);

//protected:
//    virtual void changeEvent(QEvent *e);

private:
    Ui::FormPreferencesWidget *ui;
};


class FormPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    FormPreferencesPage(QObject *parent = 0);
    ~FormPreferencesPage();

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

    static void writeDefaultSettings(Core::ISettings *s) {FormPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<FormPreferencesWidget> m_Widget;
    QFont _defaultFormFont;
    QFont _defaultEpisodeFont;
};


}  // End Internal
}  // End Form

#endif // FORMMANAGERPREFERENCESPAGE_H
