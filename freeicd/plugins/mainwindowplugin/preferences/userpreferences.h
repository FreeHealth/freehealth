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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEICD_USER_PREFERENCES_H
#define FREEICD_USER_PREFERENCES_H

#include <coreplugin/ioptionspage.h>
#include "ui_useroptionspage.h"

#include <QPointer>
#include <QString>

/**
 * \file userpreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.1
 * \date 24 Nov 2010
*/

namespace Core {
class ISettings;
}

namespace Print {
class PrinterPreviewer;
class TextDocumentExtra;
}

namespace ICD {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  IcdUserWidget  ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IcdUserWidget : public QWidget, private Ui::IcdUserWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(IcdUserWidget)

public:
    explicit IcdUserWidget(QWidget *parent = 0);
    void setDatasToUi();

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


class IcdUserOptionsPage : public Core::IOptionsPage
{
public:
    IcdUserOptionsPage(QObject *parent = 0);
    ~IcdUserOptionsPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::IcdUserWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::IcdUserWidget> m_Widget;
};

}  // End namespace ICD

#endif // FREEICD_USER_PREFERENCES_H
