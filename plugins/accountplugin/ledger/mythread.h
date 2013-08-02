/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QTextDocument>
#include <QMutex>
#include <QThread>
#include <QDate>
#include <QTextTableFormat>
#include <QTextCursor>
#include <QTextTableCell>

class ProduceDoc: public QThread {
    Q_OBJECT
    enum typesEnum {
        RECEIPTS_TYPE = 1,
        MOVEMENTS_TYPE
        };
    enum vectorReceipts {
        DATE = 0,
        PATIENT_NAME,
        CASH,
        CHECKS,
        VISA,
        BANKING,
        ACTS
        };
    enum sumsEnum {
        SUMS_SUM = 0,
        SUMS_CASH,
        SUMS_CHECKS,
        SUMS_CREDITCARDS,
        SUMS_BANKING,
        SUMS_MaxParam
        };
    enum vectorMovements {
        VM_DATE = 0,
        VM_LABEL,
        VM_AMOUNT,
        VM_COMMENT,
        VM_DETAILS
        };
    enum placeInVector {
        PLACE_OF_DATE = 0,
        PLACE_OF_PATIENT_NAME,
        PLACE_OF_CASH
        };
    enum tableColumns {
        TABLE_DATE = 1,
        TABLE_NAME_OF_PATIENT,
        TABLE_SUMS,
        TABLE_NAME_OF_ACTS
        };
    public :
        ProduceDoc();
        ~ProduceDoc();
        int            m_fieldsToJoin;
        QString        m_tableFormatParameters;
        QString        m_tablesRecapParameters;
        QMutex         m_mutex;
        QStringList    m_typesReceipts;
        QStringList    m_typesMovements;
        QTextDocument* getTextDocument();
        void           dateChosen(QDate & date);
        void           nameChecked(bool checked);
    private :
        QDate           m_date;
        QDate           m_date1;
        QTextDocument * m_textDocument;
        bool            m_noNameChecked;
        void        run();
        void        organize();
        void        fillTable(QList<QVector<QString> > & tableau,
                              QTextTableFormat & tableFormatOrganized,
                              QTextCursor * cursorForFillFunction,
                              QString & thisMonth,
                              QStringList & listSums,
                              int choice,
                              const QString & totalMovementString);
        QStringList calculateReceipts(QString & dateBegin, QString & dateEnd);
        QStringList calculateMovements(QString & dateBegin, QString & dateEnd);


        QTextTableFormat myFormat(QTextTableFormat & tableFormat,QString & parametersForTableFormat);

        bool modele(QString & month,
                    QString & day1 ,
                    QString & lastDay ,
                    QTextTableFormat & tableFormatOrganized,
                    QTextCursor *cursorForFunction);

    signals :
        void outThread(const QString & text);
        //void started();
    private slots :
        void recupSlot(const QString & text);
};

#endif
