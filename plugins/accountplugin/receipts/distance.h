/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef DISTANCE_H
#define DISTANCE_H

#include <accountplugin/account_exporter.h>

#include <QDialog>
#include <QTimer>
#include <QHash>

namespace Ui{
    class ChoiceDialog;
}

class ACCOUNT_EXPORT distance:public QDialog{
    Q_OBJECT
    public :
        distance(QWidget * parent);
        ~distance();
        double getDistanceNumber(const QString & data);
        int returnDistanceDialog();
        double m_percentValue;
    private slots :
        void value(double val);
        void valueUp();
        void valueDown();
        void valueUpStop();
        void valueDownStop();
        void quickPlus();
        void quickLess();
    private :
        Ui::ChoiceDialog * ui;
        QTimer * m_timerUp;
        QTimer * m_timerDown;
        double m_percent;
        int m_quickInt;
        QHash<int,QString> m_hashPercentages;
};

#endif

