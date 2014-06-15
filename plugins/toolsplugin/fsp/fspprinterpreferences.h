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
 *       Eric Maeker <eric.maeker@gmail.com>                               *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_FSPPRINTER_PREFERENCES_H
#define TOOLS_INTERNAL_FSPPRINTER_PREFERENCES_H

#include <coreplugin/ioptionspage.h>

#include <QWidget>
#include <QPointer>

/**
 * \file fspprinterpreferences.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 23 Mar 2013
*/

namespace Core {
class ISettings;
}

namespace Tools {
namespace Internal {
namespace Ui {
class FspPrinterPreferencesWidget;
} // namespace Ui

class FspPrinterPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FspPrinterPreferencesWidget(QWidget *parent = 0);
    ~FspPrinterPreferencesWidget();

    void setDataToUi();
    QString searchKeywords() const;

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void printTest();
    void viewCerfa();

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private:
    Ui::FspPrinterPreferencesWidget *ui;
    QPixmap _background;
    double _xcoef, _ycoef;
};

class FspPrinterPreferencesPage : public Core::IOptionsPage
{
public:
    FspPrinterPreferencesPage(QObject *parent = 0);
    ~FspPrinterPreferencesPage();

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

    static void writeDefaultSettings(Core::ISettings *s) {FspPrinterPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::FspPrinterPreferencesWidget> m_Widget;
    QString m_searchKeywords;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_FSPPRINTER_PREFERENCES_H

