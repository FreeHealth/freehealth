/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DOSAGEVIEWER_H
#define DOSAGEVIEWER_H

#include <QWidget>

/** \todo aggregate the ui in the private part */
#include "ui_mfDosageViewer.h"

/**
 * \file mfDosageViewer.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.2.1
 * \date 26 Oct 2009
*/

namespace DrugsDB {
namespace Internal {
class DosageModel;
}
}

namespace DrugsWidget {
namespace Internal {
class DosageViewerPrivate;
class DosageModel;

class DosageViewer : public QWidget, public Ui::DosageViewer
{
    Q_OBJECT
    Q_DISABLE_COPY(DosageViewer);

public:
    explicit DosageViewer(QWidget *parent);
    ~DosageViewer();

    void setDosageModel(DrugsDB::Internal::DosageModel *model);
    void useDrugsModel(const QVariant &drugId, const int drugRow);

public Q_SLOTS:
    void done(int r);
    void commitToModel();
    void changeCurrentRow(const int dosageRow);
    void changeCurrentRow(const QModelIndex &current, const QModelIndex &previous); 

Q_SIGNALS:
    void protocolDataChanged();

private:
    void resizeEvent(QResizeEvent * event);

private Q_SLOTS:
    void on_fromToIntakesCheck_stateChanged(int state);
    void on_fromToDurationCheck_stateChanged(int state);
    void on_intakesFromSpin_valueChanged(double d);
    void on_durationFromSpin_valueChanged(double d);
    void on_userformsButton_clicked();
    void on_dosageForAllInnCheck_stateChanged(int state);
    void on_aldCheck_stateChanged(int state);
    void on_monographButton_clicked();
    void on_tabWidget_currentChanged(int);
    void onDailySchemeModelDataChanged(const QModelIndex &index);

private:
    DosageViewerPrivate *d;
};

}  // End Internal
}  // End DrugsWidget

#endif // DOSAGEVIEWER_H
