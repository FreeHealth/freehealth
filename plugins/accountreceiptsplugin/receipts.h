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

#include "ui_ReceiptsWidget.h"
#include "receiptsbasemanager.h"
#include "receiptsengine.h"
//#include <accountbaseplugin/accountbase.h>
#include <QtCore>
#include <QtGui>
#include <QtSql>

class receipts : public QWidget , public Ui::ReceiptsDialog{
  Q_OBJECT
  public :
    static receipts * getInstance(){
        if(NULL == d){
            d = new receipts;
            qDebug() << __FILE__ << QString::number(__LINE__) << " creating singleton receipts.";
            }
        else{
            qWarning() << __FILE__ << QString::number(__LINE__) << " singleton receipts already created";
            }
        return d;
        }
    static void kill(){
        if(NULL != d){
            delete d;
            d = NULL;
            }
        }
    void initialize();
    void getPatientDatas(QString&,QString&,QString&,QString&);
  private :
    receipts();
    ~receipts();
  protected :
    void mousePressEvent(QMouseEvent *);
    void percentages();
    void fillComboBoxes(QComboBox *, QStringList &  , QString & );
    QStringList fillWithCategoriesList();
    receiptsBaseManager * m_rbm;
    receiptsEngine * m_recEng;
    QMenu * m_menu;
    QAction * m_rightClic;
    QMultiHash <int,QString> m_hashPercentType,m_hashPercents ;
    int m_countMoreOrLess;
    double m_percentFactor;
  private :
    bool m_receiptsIsOn;
    QString m_name;
    QString m_firstname;
    QString m_uid;
    QString m_birthday;
    static receipts * d;
  
  private slots :
    void save();
    void comboBoxCategories_changed(const QString &);
    void plusFunction();
    void lessFunction();
    void clearAll();
    void showFreeWidgets(bool);
};


#endif
