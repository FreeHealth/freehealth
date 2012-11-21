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
#ifndef DRUGS_DRUGEXTRA_PREFERENCES
#define DRUGS_DRUGEXTRA_PREFERENCES

#include <coreplugin/ioptionspage.h>

#include <QPointer>

#include "ui_drugsextraoptionspage.h"

/**
 * \file drugextrapreferences.h
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

class DrugsExtraWidget : public QWidget, private Ui::DrugsExtraWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DrugsExtraWidget)

public:
    explicit DrugsExtraWidget(QWidget *parent = 0);
    void setDataToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);

protected:
    virtual void changeEvent(QEvent *e);
};

class DrugsExtraOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    DrugsExtraOptionsPage(QObject *parent = 0);
    ~DrugsExtraOptionsPage();

    QString displayName() const;
    QString category() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage();

    static void writeDefaultSettings(Core::ISettings *s)  {Internal::DrugsExtraWidget::writeDefaultSettings(s);}
    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::DrugsExtraWidget> m_Widget;
};

}  // End namespace Internal
}  // End namespace Drugs

#endif // DRUGS_DRUGEXTRA_PREFERENCES
