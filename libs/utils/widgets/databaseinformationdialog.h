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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_DATABASEINFORMATIONDIALOG_H
#define UTILS_DATABASEINFORMATIONDIALOG_H

#include <utils/global_exporter.h>
#include <QDialog>
QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file databaseinformationdialog.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 26 Jan 2013
*/

namespace Utils {
class Database;
namespace Internal {
class DatabaseInformationDialogPrivate;
} // namespace Internal

class UTILS_EXPORT DatabaseInformationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DatabaseInformationDialog(QWidget *parent = 0);
    ~DatabaseInformationDialog();

    void setTitle(const QString &title);
    bool setDatabase(const Utils::Database &database);

    QTreeWidget *getHeaderTreeWidget();
    QTreeWidget *getDescriptionTreeWidget();

public Q_SLOTS:
    bool saveContent();

private:
    Internal::DatabaseInformationDialogPrivate *d;
};

} // namespace Utils

#endif  // UTILS_DATABASEINFORMATIONDIALOG_H

