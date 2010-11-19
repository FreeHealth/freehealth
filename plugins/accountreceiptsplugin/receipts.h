/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.                  *
 *   pm.desombre@medsyn.fr                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef RECEIPTS_H
#define RECEIPTS_H

#include <accountreceiptsplugin/accountreceipts_exporter.h>
#include "findReceiptsValues.h"
#include "receiptsmanager.h"
#include "receiptsengine.h"
#include "receiptviewer.h"

#include <QWidget>


namespace Ui {
class ReceiptsDialog;
}

class ACCOUNTRECEIPTS_EXPORT ReceiptsGUI : public QWidget
{
    Q_OBJECT

public:
    ReceiptsGUI(QWidget *parent = 0);
    ~ReceiptsGUI();

    void initialize();
    void getPatientDatas(const QString &name,const QString &firstname,
                         const QString &uid,const QString &birthday);
private:

protected:
    void mousePressEvent(QMouseEvent *);
    void percentages();
    void fillComboBoxes(QComboBox *comboBox,const QStringList &withList, const QString &withTable);
    QStringList fillWithThesaurusList();

private slots:
    void save();
    void comboBoxThesaurus_changed(const QString &/*comboBoxItem*/);
    void getReceiptsLists();
    void plusFunction();
    void lessFunction();
    void clearAll();
    void showFreeWidgetsEntry(bool checkBoxchecked);
    void showFreeWidgetsValue(bool checkBoxchecked);
    void showReceiptsViewer();//to see account

private:
    Ui::ReceiptsDialog *ui;
    receiptsManager *m_rbm;
    receiptsEngine *m_recEng;
    ReceiptViewer * m_receiptsViewer;
    QWidget * m_parent;
    findReceiptsValues * m_receiptsValues;
    QMenu *m_menu;
    QAction *m_rightClic;
    QMultiHash <int,QString> m_hashPercentType,m_hashPercents ;
    int m_countMoreOrLess;
    double m_percentFactor;
    bool m_receiptsIsOn;
    QString m_name;
    QString m_firstname;
    QString m_uid;
    QString m_birthday;
    QString m_account_uid;
    QString m_patient_uid;
    QString m_user_uid;
    QString m_site_id;
    QString m_insurance_id;
    QHash<QString,QString> m_hashValuesChoosenFromFindValues;
    QHash<int,QString> paramsSelected();
    static ReceiptsGUI * d;
    void writeOnRegisterLabel();

};


#endif
