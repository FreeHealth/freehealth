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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <coreplugin/core_exporter.h>

#include <QDialog>
QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QUrl;
QT_END_NAMESPACE

/**
 * \file helpdialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 17 Oct 2010
*/

namespace Core {
namespace Internal {
class HelpDialogPrivate;
}

class CORE_EXPORT HelpDialog : public QDialog
{
    Q_OBJECT
public:
    static void showPage(const QString &page);
    static void showIndex();

protected:
    explicit HelpDialog( const QString &page = QString::null, QWidget *parent = 0 );
    void changeEvent(QEvent *event);

private Q_SLOTS:
    void updateWindowTitle();
    void fullScreen();
    void treeActivated(QTreeWidgetItem *item);
//    void changePage(const QUrl &url);

private:
    Internal::HelpDialogPrivate *d;
};

}  // End Core

#endif // HELPDIALOG_H
