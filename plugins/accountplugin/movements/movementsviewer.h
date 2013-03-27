/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef MOVEMENTSVIEWER_H
#define MOVEMENTSVIEWER_H

#include <accountbaseplugin/movementmodel.h>
#include <accountplugin/account_exporter.h>

#include <QWidget>

namespace Ui{
    class MovementsViewerWidget;
}

class ACCOUNT_EXPORT MovementsViewer : public QWidget
{
    Q_OBJECT
    public:
        MovementsViewer(QWidget * parent = 0);
        ~MovementsViewer();
    private Q_SLOTS:
        void recordMovement();
        void deleteMovement();
        void validMovement();
        void validAndRecord();
        void setMovementsComboBoxToolTips(int);
        void userIsChanged();
        void setYearIsChanged(int);
        void quitFreeaccout();
    private:
        bool showMovements();
        void fillMovementsComboBox();
        void fillYearComboBox();
        void fillBankComboBox();
        void changeEvent(QEvent * e);
        int m_valid ;
        Ui::MovementsViewerWidget * ui;

};

#endif

