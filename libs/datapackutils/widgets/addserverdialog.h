/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_ADDSERVERDIALOG_H
#define DATAPACK_ADDSERVERDIALOG_H

#include <datapackutils/datapack_exporter.h>

#include <QDialog>

namespace DataPack {
class Server;

namespace Internal {
namespace Ui {
    class AddServerDialog;
}
}  // End namespace Internal

class DATAPACK_EXPORT AddServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddServerDialog(QWidget *parent = 0);
    ~AddServerDialog();

    void setServer(const Server &server);

public Q_SLOTS:
    void submitTo(Server *server);

private Q_SLOTS:
    void on_serverType_currentIndexChanged(int index);
    void on_selectPath_clicked();

private:
    Internal::Ui::AddServerDialog *ui;
};


} // namespace DataPack


#endif // DATAPACK_ADDSERVERDIALOG_H
