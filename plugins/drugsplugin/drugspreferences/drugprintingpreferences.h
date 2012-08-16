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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGS_DRUGPRINTING_PREFERENCES
#define DRUGS_DRUGPRINTING_PREFERENCES

#include <coreplugin/ioptionspage.h>

#include <QPointer>

#include "ui_drugsprintoptionspage.h"

/**
 * \file drugprintingpreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 16 Aug 2012
*/

namespace Core {
class ISettings;
}

namespace DrugsDB {
class IDrug;
}

namespace DrugsWidget {
namespace Internal {

class DrugsPrintWidget : public QWidget, private Ui::DrugsPrintWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugsPrintWidget)

public:
    explicit DrugsPrintWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void resetToDefaultFormatting();

private Q_SLOTS:
    void updateFormatting();

protected:
    virtual void changeEvent(QEvent *e);

private:
    DrugsDB::IDrug *drug;
};

class DrugsPrintOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugsPrintOptionsPage(QObject *parent = 0);
    ~DrugsPrintOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s) {Internal::DrugsPrintWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsPrintWidget> m_Widget;
};


}  // End namespace Internal
}  // End namespace Drugs

#endif // DRUGS_DRUGPRINTING_PREFERENCES
