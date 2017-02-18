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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef INTERACTIONSYNTHESISDIALOG_H
#define INTERACTIONSYNTHESISDIALOG_H

#include <QDialog>
QT_BEGIN_NAMESPACE
class QTableWidgetItem;
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file interactionsynthesisdialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 09 Mar 2011
*/

namespace DrugsDB {
class DrugsModel;
}

namespace DrugsWidget {
namespace Internal {
class InteractionSynthesisDialogPrivate;
}

namespace Ui {
    class InteractionSynthesisDialog;
}

class InteractionSynthesisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InteractionSynthesisDialog(DrugsDB::DrugsModel *drugModel, QWidget *parent = 0);
    ~InteractionSynthesisDialog();

protected Q_SLOTS:
//    void levelActivated(QAction *a);
    void interactionActivated(const QModelIndex &current, const QModelIndex &previous);
//    void interactorsActivated(QTableWidgetItem *item);
    void on_getBiblio_clicked();
    void showEbm(const QModelIndex &index);
    void print(QAction *action);
    void drugReportRequested();

protected:
    void changeEvent(QEvent *e);

private:
    Internal::InteractionSynthesisDialogPrivate *d;
};


}  // End namespace DrugsWidget

#endif // INTERACTIONSYNTHESISDIALOG_H
