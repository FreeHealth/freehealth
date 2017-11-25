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
 *  Main Developers:                                                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_TESTINGS_APP_SERVERCREATIONDIALOG_H
#define DATAPACK_TESTINGS_APP_SERVERCREATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ServerCreationDialog;
}

namespace DataPack {
class PackCreationModel;
}

class ServerCreationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerCreationDialog(QWidget *parent = 0);
    ~ServerCreationDialog();

private Q_SLOTS:
    void screenNewPath();
    void onCheckedToConsole();

private:
    Ui::ServerCreationDialog *ui;
    DataPack::PackCreationModel *_packCreationModel;
};

#endif // DATAPACK_TESTINGS_APP_SERVERCREATIONDIALOG_H
