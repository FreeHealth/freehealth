/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef VIRTUALDATABASECREATOR_H
#define VIRTUALDATABASECREATOR_H

#include <coreplugin/ioptionspage.h>

#include <QPointer>
#include <QString>

#include <QWidget>


/**
 * \file virtualdatabasecreator.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.1.0
 * \date 02 Oct 2010
*/

namespace Core {
class ISettings;
}

namespace Account {
namespace Internal {

namespace Ui {
class VirtualDatabaseCreator;
}  // End namespace Ui

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  VirtualDatabaseCreator  ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
class VirtualDatabaseCreator : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(VirtualDatabaseCreator)

public:
    explicit VirtualDatabaseCreator(QWidget *parent = 0);
    ~VirtualDatabaseCreator();
    void setDatasToUi();

    static void writeDefaultSettings(Core::ISettings *s);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_populate_clicked();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::VirtualDatabaseCreator *ui;
};

}  // End Internal


class VirtualDatabaseCreatorPage : public Core::IOptionsPage
{
public:
    VirtualDatabaseCreatorPage(QObject *parent = 0);
    ~VirtualDatabaseCreatorPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::VirtualDatabaseCreator::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);


private:
    QPointer<Internal::VirtualDatabaseCreator> m_Widget;
};

}  // End namespace Account


#endif // VIRTUALDATABASECREATOR_H
