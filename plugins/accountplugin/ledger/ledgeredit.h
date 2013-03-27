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
#ifndef LEDGEREDIT_H
#define LEDGEREDIT_H
#include "mythread.h"

#include <QWidget>

namespace Ui{
    class LedgerEditWidget;
}

class LedgerEdit : public QWidget {
    Q_OBJECT
    public :
        LedgerEdit(QWidget * parent);
        ~LedgerEdit();
        void resizeLedgerEdit(QWidget * parent);

    private slots :
        void showLedger();
        void printLedger();
        void chosenDate(const QString & dateText);
        void fillInfoLabel(const QString & textFromThread);
        void inThread();
        void getDocument();
        void slotDeleteThread();
    signals :
        void deleteThread();
    private :
        Ui::LedgerEditWidget * ui;
        ProduceDoc    *m_myThread;
        QTextDocument *m_doc;
        int m_typeOfPaper;
        bool m_duplicata;
        QDate m_date;
        void changeEvent(QEvent * e);
};

#endif

