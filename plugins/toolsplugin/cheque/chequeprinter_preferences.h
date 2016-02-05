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
 *  Main Developers:                                                       *
 *       Eric Maeker <>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_CHEQUEPRINTER_PREF_H
#define TOOLS_INTERNAL_CHEQUEPRINTER_PREF_H

#include <coreplugin/ioptionspage.h>

#include <QWidget>
#include <QPointer>
#include <QModelIndex>

/**
 * \file chequeprinter_preferences.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 16 Apr 2013
*/

namespace Core {
class ISettings;
}

namespace Tools {
namespace Internal {
class ChequePrintFormatModel;

namespace Ui {
class ChequePrinterPreferencesWidget;
}

class ChequePrinterPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChequePrinterPreferencesWidget(QWidget *parent = 0);
    ~ChequePrinterPreferencesWidget();

    void setDataToUi();
    QString searchKeywords() const;

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void onChequeFormatActivated(const QModelIndex &index);
    bool onPrintTestClicked();
    void saveToSettings(Core::ISettings *s = 0);

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private:
    ChequePrintFormatModel *_model;
    Ui::ChequePrinterPreferencesWidget *ui;
};


class ChequePrinterPreferencesPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    ChequePrinterPreferencesPage(QObject *parent = 0);
    ~ChequePrinterPreferencesPage();

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

    static void writeDefaultSettings(Core::ISettings *s) {ChequePrinterPreferencesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);

private:
    QPointer<Internal::ChequePrinterPreferencesWidget> m_Widget;
    QString m_searchKeywords;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_CHEQUEPRINTER_PREF_H
