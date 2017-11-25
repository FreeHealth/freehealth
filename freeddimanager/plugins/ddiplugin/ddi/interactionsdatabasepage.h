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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef INTERACTIONSDATABASEPAGE_H
#define INTERACTIONSDATABASEPAGE_H

#include <coreplugin/itoolpage.h>
#include <QAbstractItemModel>

namespace IAMDb {

class InteractionsDatabaseCreatorPage : public Core::IToolPage
{
    Q_OBJECT

public:
    InteractionsDatabaseCreatorPage(QObject *parent = 0) : Core::IToolPage(parent) {}
    ~InteractionsDatabaseCreatorPage() {}

    virtual QString id() const {return "InteractionsDatabaseCreatorPage";}
    virtual QString name() const {return "Create the interactions database";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};

namespace Ui {
class InteractionDatabaseCreator;
}

class InteractionDatabaseCreatorPrivate;

class InteractionDatabaseCreator : public QWidget
{
    Q_OBJECT
public:
    InteractionDatabaseCreator(QWidget *parent = 0);
    ~InteractionDatabaseCreator();

protected Q_SLOTS:
    void on_recreateRoutes_clicked();
    void on_checkDatas_clicked();
    void on_createAndSave_clicked();

private:
    Ui::InteractionDatabaseCreator *ui;
    InteractionDatabaseCreatorPrivate *d;
};




namespace Ui {
class InteractionDatabaseChecker;
}

class InteractionDatabaseCheckerPrivate;

class InteractionDatabaseChecker : public QWidget
{
    Q_OBJECT
public:
    InteractionDatabaseChecker(QWidget *parent = 0);
    ~InteractionDatabaseChecker();

protected Q_SLOTS:
    void on_check_clicked();

private:
    Ui::InteractionDatabaseChecker *ui;
    InteractionDatabaseCheckerPrivate *d;
};


} // namespace IAMDb

#endif // INTERACTIONSDATABASEPAGE_H
