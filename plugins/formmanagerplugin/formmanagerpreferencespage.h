/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FORMMANAGERPREFERENCESPAGE_H
#define FORMMANAGERPREFERENCESPAGE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QObject>

/**
 * \file formmanagerpreferencespage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 09 June 2010
*/


namespace Core {
class ISettings;
}

namespace Form {
class FormFilesSelectorWidget;

namespace Internal {
namespace Ui {
class FormManagerPreferencesWidget;
}

class FormManagerPreferencesWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FormManagerPreferencesWidget)

public:
    explicit FormManagerPreferencesWidget(QWidget *parent = 0);
    ~FormManagerPreferencesWidget();

    static void writeDefaultSettings(Core::ISettings *) {}

public Q_SLOTS:
    void saveFormToBase();
    void saveToSettings(Core::ISettings *);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::FormManagerPreferencesWidget *ui;
};


class FormManagerPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    FormManagerPreferencesPage(QObject *parent = 0);
    ~FormManagerPreferencesPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return "parametrer.html";}

    static void writeDefaultSettings(Core::ISettings *s) {FormManagerPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<FormManagerPreferencesWidget> m_Widget;
};


}  // End Internal
}  // End Form


#endif // FORMMANAGERPREFERENCESPAGE_H
