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
#ifndef DOSAGEDIALOG_H
#define DOSAGEDIALOG_H

// include Ui
#include "ui_dosagedialog.h"

/**
 * \file dosagedialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 12 Oct 2010
*/

namespace DrugsWidget {
namespace Internal {
class DosageDialogPrivate;


/**
 * \brief Dialog for dosage prescription based on a standard dosage set.
 * Before all, this dialog is a wrapper on the DrugsDB::DrugsModel (not the DrugsDB::DosageModel). The mfDrugsModel is a kind of proxy
 * that manages drugs (view only) / dosages (via DrugsDB::DosageModel) / interactions (view only).
 * If you want to create a new dosage, you must create a new row onto the model BEFORE.
 * If you want to edit or modify a dosage, you must inform the dialog of the row and the CIS of the drug.
 \ingroup freediams drugswidget
*/
class DosageDialog : public QDialog, public Ui::DosageDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(DosageDialog);

public:
    explicit DosageDialog(QWidget *parent);
    ~DosageDialog();

    void changeRow(const QVariant &drugUid, const int dosageRow);


private Q_SLOTS:
    void done(int r);
    void on_drugNameButton_clicked();
    void on_innButton_clicked();
    void updatePosologicSentence(const QModelIndex &, const QModelIndex &);

private:
    DosageDialogPrivate *d;
};

} // namespace Internal
} // namespace DrugsWidget

#endif // DOSAGEDIALOG_H
