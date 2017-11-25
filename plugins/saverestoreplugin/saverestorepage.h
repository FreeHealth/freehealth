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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SAVERESTORE_PAGE_H
#define SAVERESTORE_PAGE_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QString>

#include "ui_saverestorepage.h"

/**
 * \file saverestorepage.h
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

namespace SaveRestore {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  SaveRestoreWidget  //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SaveRestoreWidget : public QWidget, private Ui::SaveRestoreWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SaveRestoreWidget)

public:
    explicit SaveRestoreWidget(QWidget *parent = 0);
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_saveButton_clicked();
    bool on_restoreButton_clicked();
    void on_importButton_clicked();
    void on_exportButton_clicked();

protected:
    virtual void changeEvent(QEvent *e);
};

}  // End Internal


class SaveRestorePage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    SaveRestorePage(QObject *parent = 0);
    ~SaveRestorePage();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void apply();
    void finish();

    QString helpPage() {return "saverestore.html";}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::SaveRestoreWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::SaveRestoreWidget> m_Widget;
};

}  // End namespace SaveRestore

#endif // SAVERESTORE_PAGE_H
