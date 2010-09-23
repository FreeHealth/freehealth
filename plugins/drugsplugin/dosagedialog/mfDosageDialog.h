/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef DOSAGEDIALOG_H
#define DOSAGEDIALOG_H

// include Ui
#include "ui_mfDosageDialog.h"

/**
 * \file mfDosageDialog.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 26 Oct 2009
*/

namespace DrugsWidget {
namespace Internal {
class DosageDialogPrivate;


/**
 * \brief Dialog for dosage prescription based on a standard dosage set.
 * Before all, this dialog is a wrapper on the mfDrugsModel (not the mfDosageModel). The mfDrugsModel is a kind of proxy
 * that manages drugs (view only) / dosages (via mfDosageModel) / interactions (view only).
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

    void changeRow( const int drugRow , const int dosageRow );


private Q_SLOTS:
    void done( int r );
    void on_drugNameButton_clicked();
    void on_innButton_clicked();

private:
    DosageDialogPrivate *d;
};

} // namespace Internal
} // namespace DrugsWidget

#endif // DOSAGEDIALOG_H
